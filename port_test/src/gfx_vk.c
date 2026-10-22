/*
 * gfx_vk.c - backend Vulkan do porte.
 *
 * Carrega o loader (libvulkan.so.1 / vulkan-1.dll / libMoltenVK.dylib) em
 * tempo de execucao e declara as structs Vulkan na mao (layout natural C,
 * igual ao ABI da especificacao) para nao depender de vulkan.h no build.
 * Escopo desta fase: instancia -> superficie SDL2 -> swapchain -> render
 * pass com clear animado -> present. Triangulo/shaders (SPIR-V) entram na
 * proxima fase do renderer.
 */
#include "gfx.h"
#include "sdl2_dyn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
static void *vk_dlopen(const char *n) { return (void *)LoadLibraryA(n); }
static void *vk_dlsym(void *l, const char *n)
{ return (void *)GetProcAddress((HMODULE)l, n); }
static void vk_dlclose(void *l) { if (l) FreeLibrary((HMODULE)l); }
#else
#include <dlfcn.h>
static void *vk_dlopen(const char *n) { return dlopen(n, RTLD_NOW | RTLD_LOCAL); }
static void *vk_dlsym(void *l, const char *n) { return dlsym(l, n); }
static void vk_dlclose(void *l) { if (l) dlclose(l); }
#endif

/* ------------------------------------------------------------- handles */
typedef void    *vk_disp_t;   /* dispatchable  (ponteiro)     */
typedef uint64_t vk_nd_t;     /* non-dispatchable (64 bits)   */

typedef int32_t VkResult;
#define VK_SUCCESS                     0
#define VK_TIMEOUT                     2
#define VK_ERROR_INCOMPATIBLE_DRIVER  (-9)
#define VK_SUBOPTIMAL_KHR             1000001003
#define VK_ERROR_OUT_OF_DATE_KHR      (-1000001004)

#define VK_STRUCTURE_TYPE_APPLICATION_INFO               0
#define VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO           1
#define VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO       2
#define VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO             3
#define VK_STRUCTURE_TYPE_SUBMIT_INFO                    4
#define VK_STRUCTURE_TYPE_FENCE_CREATE_INFO              8
#define VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO          9
#define VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO        15
#define VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO       37
#define VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO       38
#define VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO      39
#define VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO  40
#define VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO     42
#define VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO        43
#define VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR  1000000001
#define VK_STRUCTURE_TYPE_PRESENT_INFO_KHR           1000000002

#define VK_QUEUE_GRAPHICS_BIT                0x1
#define VK_FORMAT_B8G8R8A8_UNORM             44
#define VK_COLOR_SPACE_SRGB_NONLINEAR_KHR    0
#define VK_PRESENT_MODE_FIFO_KHR             1
#define VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT  0x10
#define VK_SHARING_MODE_EXCLUSIVE            0
#define VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR    0x1
#define VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR 0x1
#define VK_IMAGE_LAYOUT_UNDEFINED            0
#define VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL 2
#define VK_IMAGE_LAYOUT_PRESENT_SRC_KHR      1000001002
#define VK_ATTACHMENT_LOAD_OP_CLEAR          1
#define VK_ATTACHMENT_STORE_OP_STORE         0
#define VK_ATTACHMENT_STENCIL_DONT_CARE      2
#define VK_IMAGE_ASPECT_COLOR_BIT            0x1
#define VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT 0x80
#define VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT 0x10
#define VK_COMMAND_BUFFER_LEVEL_PRIMARY      0
#define VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT 0x2
#define VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT 0x1
#define VK_FENCE_CREATE_SIGNALED_BIT         0x1
#define VK_PIPELINE_BIND_POINT_GRAPHICS      0
#define VK_SAMPLE_COUNT_1_BIT                0x1
#define VK_SUBPASS_EXTERNAL                  0xFFFFFFFFu
#define VK_API_VERSION_1_0                   ((1u << 22) | (0u << 12))

/* -------------------------------------------------------------- structs */

typedef struct { uint32_t width, height; } VkExtent2D;

typedef struct {
    uint32_t sType; const void *pNext;
    const char *pApplicationName; uint32_t applicationVersion;
    const char *pEngineName; uint32_t engineVersion; uint32_t apiVersion;
} VkApplicationInfo;

typedef struct {
    uint32_t sType; const void *pNext; uint32_t flags;
    const VkApplicationInfo *pApplicationInfo;
    uint32_t enabledLayerCount; const char *const *ppEnabledLayerNames;
    uint32_t enabledExtensionCount; const char *const *ppEnabledExtensionNames;
} VkInstanceCreateInfo;

typedef struct {
    uint32_t sType; const void *pNext; uint32_t flags;
    uint32_t queueFamilyIndex; uint32_t queueCount; const float *pQueuePriorities;
} VkDeviceQueueCreateInfo;

typedef struct {
    uint32_t sType; const void *pNext; uint32_t flags;
    uint32_t queueCreateInfoCount; const VkDeviceQueueCreateInfo *pQueueCreateInfos;
    uint32_t enabledLayerCount; const char *const *ppEnabledLayerNames;
    uint32_t enabledExtensionCount; const char *const *ppEnabledExtensionNames;
    const void *pEnabledFeatures;
} VkDeviceCreateInfo;

typedef struct { uint32_t format, colorSpace; } VkSurfaceFormatKHR;

typedef struct {
    uint32_t minImageCount, maxImageCount;
    VkExtent2D currentExtent, minImageExtent, maxImageExtent;
    uint32_t maxImageArrayLayers, supportedTransforms, currentTransform;
    uint32_t supportedCompositeAlpha, supportedUsageFlags;
} VkSurfaceCapabilitiesKHR;

typedef struct {
    uint32_t sType; const void *pNext; uint32_t flags;
    vk_nd_t surface;
    uint32_t minImageCount, imageFormat, imageColorSpace;
    VkExtent2D imageExtent;
    uint32_t imageArrayLayers, imageUsage, imageSharingMode;
    uint32_t queueFamilyIndexCount; const uint32_t *pQueueFamilyIndices;
    uint32_t preTransform, compositeAlpha, presentMode;
    int32_t clipped; vk_nd_t oldSwapchain;
} VkSwapchainCreateInfoKHR;

typedef struct { uint32_t r, g, b, a; } VkComponentMapping;
typedef struct {
    uint32_t aspectMask, baseMipLevel, levelCount, baseArrayLayer, layerCount;
} VkImageSubresourceRange;

typedef struct {
    uint32_t sType; const void *pNext; uint32_t flags; vk_nd_t image;
    uint32_t viewType, format; VkComponentMapping components;
    VkImageSubresourceRange subresourceRange;
} VkImageViewCreateInfo;

typedef struct {
    uint32_t flags, format, samples, loadOp, storeOp;
    uint32_t stencilLoadOp, stencilStoreOp, initialLayout, finalLayout;
} VkAttachmentDescription;

typedef struct { uint32_t attachment, layout; } VkAttachmentReference;

typedef struct {
    uint32_t flags; uint32_t pipelineBindPoint;
    uint32_t inputAttachmentCount; const VkAttachmentReference *pInputAttachments;
    uint32_t colorAttachmentCount; const VkAttachmentReference *pColorAttachments;
    const VkAttachmentReference *pResolveAttachments;
    const VkAttachmentReference *depthStencilAttachment;
    uint32_t preserveAttachmentCount; const uint32_t *pPreserveAttachments;
} VkSubpassDescription;

typedef struct {
    uint32_t srcSubpass, dstSubpass;
    uint32_t srcStageMask, dstStageMask, srcAccessMask, dstAccessMask;
    uint32_t dependencyFlags;
} VkSubpassDependency;

typedef struct {
    uint32_t sType; const void *pNext; uint32_t flags;
    uint32_t attachmentCount; const VkAttachmentDescription *pAttachments;
    uint32_t subpassCount; const VkSubpassDescription *pSubpasses;
    uint32_t dependencyCount; const VkSubpassDependency *pDependencies;
} VkRenderPassCreateInfo;

typedef struct {
    uint32_t sType; const void *pNext; uint32_t flags;
    vk_nd_t renderPass; uint32_t attachmentCount; const vk_nd_t *pAttachments;
    uint32_t width, height, layers;
} VkFramebufferCreateInfo;

typedef struct {
    uint32_t sType; const void *pNext; uint32_t flags; uint32_t queueFamilyIndex;
} VkCommandPoolCreateInfo;

typedef struct {
    uint32_t sType; const void *pNext;
    vk_nd_t commandPool; uint32_t level; uint32_t commandBufferCount;
} VkCommandBufferAllocateInfo;

typedef struct {
    uint32_t sType; const void *pNext; uint32_t flags; const void *pInheritanceInfo;
} VkCommandBufferBeginInfo;

typedef struct { uint32_t sType; const void *pNext; uint32_t flags; } VkFenceCreateInfo;
typedef struct { uint32_t sType; const void *pNext; uint32_t flags; } VkSemaphoreCreateInfo;

typedef union {
    float float32[4]; int32_t int32[4]; uint32_t uint32[4];
} VkClearValue;

typedef struct { int32_t x, y; VkExtent2D extent; } VkRect2D;

typedef struct {
    uint32_t sType; const void *pNext;
    vk_nd_t renderPass; vk_nd_t framebuffer; VkRect2D renderArea;
    uint32_t clearValueCount; const VkClearValue *pClearValues;
} VkRenderPassBeginInfo;

typedef struct {
    uint32_t sType; const void *pNext;
    uint32_t waitSemaphoreCount; const vk_nd_t *pWaitSemaphores;
    const uint32_t *pWaitDstStageMask;
    uint32_t commandBufferCount; vk_disp_t const *pCommandBuffers;
    uint32_t signalSemaphoreCount; const vk_nd_t *pSignalSemaphores;
} VkSubmitInfo;

typedef struct {
    uint32_t sType; const void *pNext;
    uint32_t waitSemaphoreCount; const vk_nd_t *pWaitSemaphores;
    uint32_t swapchainCount; const vk_nd_t *pSwapchains;
    const uint32_t *pImageIndices; VkResult *pResults;
} VkPresentInfoKHR;

typedef struct { uint32_t queueFlags, queueCount, timestampValidBits,
                 minImageTransferExtent[3]; } VkQueueFamilyProperties;
_Static_assert(sizeof(VkQueueFamilyProperties) == 24, "VkQueueFamilyProperties");

/* o ABI usa alinhamento natural; os asserts pegam qualquer divergencia */
_Static_assert(sizeof(VkApplicationInfo) == 48, "VkApplicationInfo");
_Static_assert(sizeof(VkInstanceCreateInfo) == 64, "VkInstanceCreateInfo");
_Static_assert(sizeof(VkDeviceQueueCreateInfo) == 40, "VkDeviceQueueCreateInfo");
_Static_assert(sizeof(VkDeviceCreateInfo) == 72, "VkDeviceCreateInfo");
_Static_assert(sizeof(VkSurfaceCapabilitiesKHR) == 52, "VkSurfaceCapabilitiesKHR");
_Static_assert(sizeof(VkSwapchainCreateInfoKHR) == 104, "VkSwapchainCreateInfoKHR");
_Static_assert(sizeof(VkImageViewCreateInfo) == 80, "VkImageViewCreateInfo");
_Static_assert(sizeof(VkSubpassDescription) == 72, "VkSubpassDescription");
_Static_assert(sizeof(VkSubpassDependency) == 28, "VkSubpassDependency");
_Static_assert(sizeof(VkRenderPassCreateInfo) == 64, "VkRenderPassCreateInfo");
_Static_assert(sizeof(VkFramebufferCreateInfo) == 64, "VkFramebufferCreateInfo");
_Static_assert(sizeof(VkCommandPoolCreateInfo) == 24, "VkCommandPoolCreateInfo");
_Static_assert(sizeof(VkCommandBufferAllocateInfo) == 32, "VkCommandBufferAllocateInfo");
_Static_assert(sizeof(VkRenderPassBeginInfo) == 64, "VkRenderPassBeginInfo");
_Static_assert(sizeof(VkSubmitInfo) == 72, "VkSubmitInfo");
_Static_assert(sizeof(VkPresentInfoKHR) == 64, "VkPresentInfoKHR");

/* ------------------------------------------------------- ponteiros vk */

typedef void *(*PFN_vkGetInstanceProcAddr)(vk_disp_t, const char *);
typedef void *(*PFN_vkGetDeviceProcAddr)(vk_disp_t, const char *);

typedef VkResult (*PFN_vkCreateInstance)(const VkInstanceCreateInfo *, const void *, vk_disp_t *);
typedef VkResult (*PFN_vkEnumerateInstanceExtensionProperties)(const char *, uint32_t *, void *);
typedef void (*PFN_vkDestroyInstance)(vk_disp_t, const void *);
typedef VkResult (*PFN_vkEnumeratePhysicalDevices)(vk_disp_t, uint32_t *, vk_disp_t *);
typedef void (*PFN_vkGetPhysicalDeviceProperties)(vk_disp_t, void *);
typedef void (*PFN_vkGetPhysicalDeviceQueueFamilyProperties)(vk_disp_t, uint32_t *, VkQueueFamilyProperties *);
typedef VkResult (*PFN_vkGetPhysicalDeviceSurfaceSupportKHR)(vk_disp_t, uint32_t, vk_nd_t, int32_t *);
typedef VkResult (*PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)(vk_disp_t, vk_nd_t, VkSurfaceCapabilitiesKHR *);
typedef VkResult (*PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)(vk_disp_t, vk_nd_t, uint32_t *, VkSurfaceFormatKHR *);
typedef VkResult (*PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)(vk_disp_t, vk_nd_t, uint32_t *, uint32_t *);
typedef VkResult (*PFN_vkCreateDevice)(vk_disp_t, const VkDeviceCreateInfo *, const void *, vk_disp_t *);
typedef void (*PFN_vkDestroySurfaceKHR)(vk_disp_t, vk_nd_t, const void *);
typedef void (*PFN_vkDestroyDevice)(vk_disp_t, const void *);
typedef void (*PFN_vkGetDeviceQueue)(vk_disp_t, uint32_t, uint32_t, vk_disp_t *);
typedef VkResult (*PFN_vkCreateSwapchainKHR)(vk_disp_t, const VkSwapchainCreateInfoKHR *, const void *, vk_nd_t *);
typedef void (*PFN_vkDestroySwapchainKHR)(vk_disp_t, vk_nd_t, const void *);
typedef VkResult (*PFN_vkGetSwapchainImagesKHR)(vk_disp_t, vk_nd_t, uint32_t *, vk_nd_t *);
typedef VkResult (*PFN_vkCreateImageView)(vk_disp_t, const VkImageViewCreateInfo *, const void *, vk_nd_t *);
typedef void (*PFN_vkDestroyImageView)(vk_disp_t, vk_nd_t, const void *);
typedef VkResult (*PFN_vkCreateRenderPass)(vk_disp_t, const VkRenderPassCreateInfo *, const void *, vk_nd_t *);
typedef void (*PFN_vkDestroyRenderPass)(vk_disp_t, vk_nd_t, const void *);
typedef VkResult (*PFN_vkCreateFramebuffer)(vk_disp_t, const VkFramebufferCreateInfo *, const void *, vk_nd_t *);
typedef void (*PFN_vkDestroyFramebuffer)(vk_disp_t, vk_nd_t, const void *);
typedef VkResult (*PFN_vkCreateCommandPool)(vk_disp_t, const VkCommandPoolCreateInfo *, const void *, vk_nd_t *);
typedef void (*PFN_vkDestroyCommandPool)(vk_disp_t, vk_nd_t, const void *);
typedef VkResult (*PFN_vkAllocateCommandBuffers)(vk_disp_t, const VkCommandBufferAllocateInfo *, vk_disp_t *);
typedef VkResult (*PFN_vkCreateFence)(vk_disp_t, const VkFenceCreateInfo *, const void *, vk_nd_t *);
typedef void (*PFN_vkDestroyFence)(vk_disp_t, vk_nd_t, const void *);
typedef VkResult (*PFN_vkCreateSemaphore)(vk_disp_t, const VkSemaphoreCreateInfo *, const void *, vk_nd_t *);
typedef void (*PFN_vkDestroySemaphore)(vk_disp_t, vk_nd_t, const void *);
typedef VkResult (*PFN_vkBeginCommandBuffer)(vk_disp_t, const VkCommandBufferBeginInfo *);
typedef VkResult (*PFN_vkEndCommandBuffer)(vk_disp_t);
typedef void (*PFN_vkCmdBeginRenderPass)(vk_disp_t, const VkRenderPassBeginInfo *, uint32_t);
typedef void (*PFN_vkCmdEndRenderPass)(vk_disp_t);
typedef VkResult (*PFN_vkQueueSubmit)(vk_disp_t, uint32_t, const VkSubmitInfo *, vk_nd_t);
typedef VkResult (*PFN_vkQueuePresentKHR)(vk_disp_t, const VkPresentInfoKHR *);
typedef VkResult (*PFN_vkQueueWaitIdle)(vk_disp_t);
typedef VkResult (*PFN_vkDeviceWaitIdle)(vk_disp_t);
typedef VkResult (*PFN_vkWaitForFences)(vk_disp_t, uint32_t, const vk_nd_t *, int32_t, uint64_t);
typedef VkResult (*PFN_vkAcquireNextImageKHR)(vk_disp_t, vk_nd_t, uint64_t, vk_nd_t, vk_nd_t, uint32_t *);

/* ------------------------------------------------------------- loader */

static void *vk_open_loader(void)
{
    static const char *cands[] = {
#if defined(__APPLE__)
        "libvulkan.1.dylib", "libMoltenVK.dylib", "libvulkan.dylib",
#elif defined(_WIN32)
        "vulkan-1.dll",
#else
        "libvulkan.so.1", "libvulkan.so",
#endif
        NULL
    };
    int i;
    for (i = 0; cands[i]; i++) {
        void *lib = vk_dlopen(cands[i]);
        if (lib) return lib;
    }
    return NULL;
}

int t4_gfx_vk_loader_check(char *info, size_t cap)
{
    void *lib = vk_open_loader();
    PFN_vkGetInstanceProcAddr gipa;
    PFN_vkEnumerateInstanceExtensionProperties eeip;
    VkResult r;
    uint32_t n = 0;

    if (!lib) {
        snprintf(info, cap, "loader missing (libvulkan)");
        return -1;
    }
    gipa = (PFN_vkGetInstanceProcAddr)vk_dlsym(lib, "vkGetInstanceProcAddr");
    if (!gipa) {
        snprintf(info, cap, "loader sem vkGetInstanceProcAddr");
        vk_dlclose(lib);
        return -1;
    }
    eeip = (PFN_vkEnumerateInstanceExtensionProperties)
        gipa(NULL, "vkEnumerateInstanceExtensionProperties");
    if (!eeip) {
        snprintf(info, cap, "loader sem vkEnumerateInstanceExtensionProperties");
        vk_dlclose(lib);
        return -1;
    }
    r = eeip(NULL, &n, NULL);
    vk_dlclose(lib);
    if (r == VK_ERROR_INCOMPATIBLE_DRIVER) {
        snprintf(info, cap, "loader ok, driver (ICD) AUSENTE");
        return -1;
    }
    if (r != VK_SUCCESS) {
        snprintf(info, cap, "loader: erro %d ao listar extensoes", (int)r);
        return -1;
    }
    snprintf(info, cap, "loader + driver ICD presentes (%u extensoes)", n);
    return 0;
}

/* ----------------------------------------------------------------- run */

#include <math.h>

static int vk_pump_quit(void)
{
    T4_SDLEvent ev;
    int quit = 0;
    while (t4_SDL_PollEvent(&ev)) {
        if (ev.type == T4_SDL_QUIT) quit = 1;
        if (ev.type == T4_SDL_KEYDOWN && ev.raw[16] == T4_SDLK_ESCAPE) quit = 1;
    }
    return quit;
}

int t4_gfx_vk_run(int w, int h, int seconds, char *info, size_t info_cap)
{
    void *loader = NULL;
    PFN_vkGetInstanceProcAddr gipa = NULL;
    PFN_vkGetDeviceProcAddr gipa_dev = NULL;
    PFN_vkCreateInstance pCreateInstance = NULL;
    PFN_vkEnumerateInstanceExtensionProperties pEnumInstExt = NULL;
    PFN_vkDestroyInstance pDestroyInstance = NULL;
    PFN_vkEnumeratePhysicalDevices pEnumPhys = NULL;
    PFN_vkGetPhysicalDeviceProperties pGetProps = NULL;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties pGetQProps = NULL;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR pGetSupport = NULL;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR pGetCaps = NULL;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR pGetFormats = NULL;
    PFN_vkCreateDevice pCreateDevice = NULL;
    PFN_vkDestroySurfaceKHR pDestroySurface = NULL;
    PFN_vkDestroyDevice pDestroyDevice = NULL;
    PFN_vkGetDeviceQueue pGetQueue = NULL;
    PFN_vkCreateSwapchainKHR pCreateSwap = NULL;
    PFN_vkDestroySwapchainKHR pDestroySwap = NULL;
    PFN_vkGetSwapchainImagesKHR pGetSwapImgs = NULL;
    PFN_vkCreateImageView pCreateView = NULL;
    PFN_vkDestroyImageView pDestroyView = NULL;
    PFN_vkCreateRenderPass pCreateRP = NULL;
    PFN_vkDestroyRenderPass pDestroyRP = NULL;
    PFN_vkCreateFramebuffer pCreateFB = NULL;
    PFN_vkDestroyFramebuffer pDestroyFB = NULL;
    PFN_vkCreateCommandPool pCreatePool = NULL;
    PFN_vkDestroyCommandPool pDestroyPool = NULL;
    PFN_vkAllocateCommandBuffers pAllocCmds = NULL;
    PFN_vkCreateFence pCreateFence = NULL;
    PFN_vkDestroyFence pDestroyFence = NULL;
    PFN_vkCreateSemaphore pCreateSem = NULL;
    PFN_vkDestroySemaphore pDestroySem = NULL;
    PFN_vkBeginCommandBuffer pBeginCmd = NULL;
    PFN_vkEndCommandBuffer pEndCmd = NULL;
    PFN_vkCmdBeginRenderPass pCmdBeginRP = NULL;
    PFN_vkCmdEndRenderPass pCmdEndRP = NULL;
    PFN_vkQueueSubmit pQueueSubmit = NULL;
    PFN_vkQueuePresentKHR pQueuePresent = NULL;
    PFN_vkQueueWaitIdle pQueueWaitIdle = NULL;
    PFN_vkDeviceWaitIdle pDeviceWaitIdle = NULL;
    PFN_vkWaitForFences pWaitFences = NULL;
    PFN_vkAcquireNextImageKHR pAcquire = NULL;

    vk_disp_t inst = NULL, phys = NULL, dev = NULL, queue = NULL;
    vk_nd_t surface = 0, swapchain = 0, rp = 0, pool = 0;
    vk_nd_t sem_avail = 0, sem_done = 0, fence = 0;
    vk_nd_t views[16], fbs[16], imgs[16];
    vk_disp_t cmds[1];
    VkExtent2D g_extent;
    uint32_t n_imgs = 0, qfam = 0, i;
    char err[256];
    char devname[64];
    uint32_t running = 1;
    uint32_t t0;

    (void)h;
    snprintf(info, info_cap, "?");
    err[0] = '\0';

    loader = vk_open_loader();
    if (!loader) {
        snprintf(err, sizeof(err), "Vulkan: loader missing (libvulkan)");
        goto fail;
    }
    gipa = (PFN_vkGetInstanceProcAddr)vk_dlsym(loader, "vkGetInstanceProcAddr");
    if (!gipa) { snprintf(err, sizeof(err), "Vulkan: sem vkGetInstanceProcAddr"); goto fail; }

    pCreateInstance = (void *)gipa(NULL, "vkCreateInstance");
    pEnumInstExt = (void *)gipa(NULL, "vkEnumerateInstanceExtensionProperties");
    if (!pCreateInstance || !pEnumInstExt) {
        snprintf(err, sizeof(err), "Vulkan: loader incompleto");
        goto fail;
    }

    /* superficie via SDL2: exige janela com flag VULKAN */
    {
        T4_SDL_Window *win;
        const char *exts[16];
        unsigned next = 16;
        VkApplicationInfo app;
        VkInstanceCreateInfo ici;
        VkResult r;

        win = t4_SDL_CreateWindow("t4port", T4_SDL_WINDOWPOS_CENTERED,
                                  T4_SDL_WINDOWPOS_CENTERED, w, h,
                                  T4_SDL_WINDOW_VULKAN);
        if (!win) {
            snprintf(err, sizeof(err), "Vulkan: janela: %s", t4_SDL_GetError());
            goto fail;
        }
        if (!t4_SDL_Vulkan_LoadLibrary || !t4_SDL_Vulkan_GetInstanceExtensions ||
            !t4_SDL_Vulkan_CreateSurface) {
            snprintf(err, sizeof(err), "Vulkan: SDL2 sem suporte Vulkan (>= 2.0.6)");
            goto fail;
        }
        if (t4_SDL_Vulkan_LoadLibrary(NULL) != 0) {
            snprintf(err, sizeof(err), "Vulkan: SDL did not load the loader: %s",
                     t4_SDL_GetError());
            goto fail;
        }
        if (!t4_SDL_Vulkan_GetInstanceExtensions(win, &next, exts) || next == 0) {
            snprintf(err, sizeof(err), "Vulkan: extensoes de superficie: %s",
                     t4_SDL_GetError());
            goto fail;
        }

        memset(&app, 0, sizeof(app));
        app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app.pApplicationName = "t4port";
        app.applicationVersion = 1;
        app.pEngineName = "t4port";
        app.engineVersion = 1;
        app.apiVersion = VK_API_VERSION_1_0;
        memset(&ici, 0, sizeof(ici));
        ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        ici.pApplicationInfo = &app;
        ici.enabledExtensionCount = next;
        ici.ppEnabledExtensionNames = exts;

        r = pCreateInstance(&ici, NULL, &inst);
        if (r != VK_SUCCESS) {
            snprintf(err, sizeof(err), "Vulkan: vkCreateInstance: %d", (int)r);
            goto fail;
        }

        pDestroySurface = (void *)gipa(inst, "vkDestroySurfaceKHR");
        pEnumPhys = (void *)gipa(inst, "vkEnumeratePhysicalDevices");
        pGetProps = (void *)gipa(inst, "vkGetPhysicalDeviceProperties");
        pGetQProps = (void *)gipa(inst, "vkGetPhysicalDeviceQueueFamilyProperties");
        pGetSupport = (void *)gipa(inst, "vkGetPhysicalDeviceSurfaceSupportKHR");
        pGetCaps = (void *)gipa(inst, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        pGetFormats = (void *)gipa(inst, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        pCreateDevice = (void *)gipa(inst, "vkCreateDevice");
        if (!pDestroySurface || !pEnumPhys || !pGetQProps || !pGetSupport ||
            !pGetCaps || !pGetFormats || !pCreateDevice) {
            snprintf(err, sizeof(err), "Vulkan: VK_KHR_surface unavailable");
            goto fail;
        }
        if (t4_SDL_Vulkan_CreateSurface(win, inst, &surface) == 0) {
            snprintf(err, sizeof(err), "Vulkan: CreateSurface: %s", t4_SDL_GetError());
            goto fail;
        }
    }

    /* dispositivo + fila com graphics + present */
    {
        uint32_t ndev = 0, d;
        int found = 0;
        VkResult r = pEnumPhys(inst, &ndev, NULL);
        if (r != VK_SUCCESS || ndev == 0) {
            snprintf(err, sizeof(err), "Vulkan: nenhum dispositivo de video");
            goto fail;
        }
        if (ndev > 16) ndev = 16;
        {
            vk_disp_t devs[16];
            uint32_t best_score = 0;
            uint32_t pref_vendor = 0;
            const char *pref = getenv("T4_GPU");
            if (pref && strcmp(pref, "nvidia") == 0) pref_vendor = 0x10de;
            else if (pref && strcmp(pref, "amd") == 0) pref_vendor = 0x1002;
            else if (pref && strcmp(pref, "intel") == 0) pref_vendor = 0x8086;

            pEnumPhys(inst, &ndev, devs);
            for (d = 0; d < ndev; d++) {
                uint32_t nq = 0, q;
                VkQueueFamilyProperties qp[16];
                uint32_t qpick = 0, score = 0;
                int okq = 0;
                pGetQProps(devs[d], &nq, NULL);
                if (nq > 16) nq = 16;
                pGetQProps(devs[d], &nq, qp);
                for (q = 0; q < nq; q++) {
                    int32_t present = 0;
                    if (!(qp[q].queueFlags & VK_QUEUE_GRAPHICS_BIT)) continue;
                    if (pGetSupport(devs[d], q, surface, &present) != VK_SUCCESS)
                        continue;
                    if (!present) continue;
                    okq = 1;
                    qpick = q;
                    break;
                }
                if (!okq) continue;
                /* pontuacao: T4_GPU > placa discreta > integrada; vence o
                 * maior (desempate: primeiro device = estavel). Vale para
                 * qualquer placa: vendorID@8 (0x10de/0x1002/0x8086) e
                 * deviceType@16 (0 out,1 int,2 discreta,3 virt,4 cpu) de
                 * VkPhysicalDeviceProperties. */
                if (pGetProps) {
                    unsigned char pb[4096];
                    uint32_t vendor = 0, dtype = 3;
                    memset(pb, 0, sizeof(pb));
                    pGetProps(devs[d], pb);
                    memcpy(&vendor, pb + 8, 4);
                    memcpy(&dtype, pb + 16, 4);
                    switch (dtype) {
                    case 2: score = 3; break;        /* discreta */
                    case 1: score = 1; break;        /* integrada */
                    case 0: score = 2; break;        /* other */
                    default: score = 0; break;       /* virtual/cpu */
                    }
                    if (pref_vendor && vendor == pref_vendor) score += 8;
                }
                if (!found || score > best_score) {
                    phys = devs[d];
                    qfam = qpick;
                    best_score = score;
                    found = 1;
                }
            }
            if (!found) {
                snprintf(err, sizeof(err), "Vulkan: sem fila graphics+present");
                goto fail;
            }
        }

        devname[0] = '\0';
        if (pGetProps) {
            unsigned char propbuf[4096];      /* VkPhysicalDeviceProperties */
            memset(propbuf, 0, sizeof(propbuf));
            pGetProps(phys, propbuf);
            /* deviceName fica no offset 20 (5 x u32 antes) */
            memcpy(devname, propbuf + 20, 63);
            devname[63] = '\0';
        }

        {
            float prio = 1.0f;
            uint32_t qf = qfam;
            const char *deext[] = { "VK_KHR_swapchain" };
            VkDeviceQueueCreateInfo dq;
            VkDeviceCreateInfo dci;
            VkResult rr;
            memset(&dq, 0, sizeof(dq));
            dq.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            dq.queueFamilyIndex = qf;
            dq.queueCount = 1;
            dq.pQueuePriorities = &prio;
            memset(&dci, 0, sizeof(dci));
            dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            dci.queueCreateInfoCount = 1;
            dci.pQueueCreateInfos = &dq;
            dci.enabledExtensionCount = 1;
            dci.ppEnabledExtensionNames = deext;
            rr = pCreateDevice(phys, &dci, NULL, &dev);
            if (rr != VK_SUCCESS) {
                snprintf(err, sizeof(err), "Vulkan: vkCreateDevice: %d", (int)rr);
                goto fail;
            }
        }

        pDestroyDevice = (void *)gipa(inst, "vkDestroyDevice");
        gipa_dev = (PFN_vkGetDeviceProcAddr)gipa(inst, "vkGetDeviceProcAddr");
        if (!gipa_dev) { snprintf(err, sizeof(err), "Vulkan: sem vkGetDeviceProcAddr"); goto fail; }

        pGetQueue = (void *)gipa_dev(dev, "vkGetDeviceQueue");
        pCreateSwap = (void *)gipa_dev(dev, "vkCreateSwapchainKHR");
        pDestroySwap = (void *)gipa_dev(dev, "vkDestroySwapchainKHR");
        pGetSwapImgs = (void *)gipa_dev(dev, "vkGetSwapchainImagesKHR");
        pCreateView = (void *)gipa_dev(dev, "vkCreateImageView");
        pDestroyView = (void *)gipa_dev(dev, "vkDestroyImageView");
        pCreateRP = (void *)gipa_dev(dev, "vkCreateRenderPass");
        pDestroyRP = (void *)gipa_dev(dev, "vkDestroyRenderPass");
        pCreateFB = (void *)gipa_dev(dev, "vkCreateFramebuffer");
        pDestroyFB = (void *)gipa_dev(dev, "vkDestroyFramebuffer");
        pCreatePool = (void *)gipa_dev(dev, "vkCreateCommandPool");
        pDestroyPool = (void *)gipa_dev(dev, "vkDestroyCommandPool");
        pAllocCmds = (void *)gipa_dev(dev, "vkAllocateCommandBuffers");
        pCreateFence = (void *)gipa_dev(dev, "vkCreateFence");
        pDestroyFence = (void *)gipa_dev(dev, "vkDestroyFence");
        pCreateSem = (void *)gipa_dev(dev, "vkCreateSemaphore");
        pDestroySem = (void *)gipa_dev(dev, "vkDestroySemaphore");
        pBeginCmd = (void *)gipa_dev(dev, "vkBeginCommandBuffer");
        pEndCmd = (void *)gipa_dev(dev, "vkEndCommandBuffer");
        pCmdBeginRP = (void *)gipa_dev(dev, "vkCmdBeginRenderPass");
        pCmdEndRP = (void *)gipa_dev(dev, "vkCmdEndRenderPass");
        pQueueSubmit = (void *)gipa_dev(dev, "vkQueueSubmit");
        pQueuePresent = (void *)gipa_dev(dev, "vkQueuePresentKHR");
        pQueueWaitIdle = (void *)gipa_dev(dev, "vkQueueWaitIdle");
        pDeviceWaitIdle = (void *)gipa_dev(dev, "vkDeviceWaitIdle");
        pWaitFences = (void *)gipa_dev(dev, "vkWaitForFences");
        pAcquire = (void *)gipa_dev(dev, "vkAcquireNextImageKHR");
        if (!pGetQueue || !pCreateSwap || !pGetSwapImgs || !pCreateView ||
            !pCreateRP || !pCreateFB || !pCreatePool || !pAllocCmds ||
            !pCreateFence || !pCreateSem || !pBeginCmd || !pEndCmd ||
            !pCmdBeginRP || !pCmdEndRP || !pQueueSubmit || !pQueuePresent ||
            !pWaitFences || !pAcquire) {
            snprintf(err, sizeof(err), "Vulkan: driver sem VK_KHR_swapchain");
            goto fail;
        }
        pGetQueue(dev, qfam, 0, &queue);
    }

    /* swapchain */
    {
        VkSurfaceCapabilitiesKHR caps;
        VkSurfaceFormatKHR fmts[32];
        uint32_t nfmt = 0, fi;
        VkSwapchainCreateInfoKHR sci;
        VkResult r;
        uint32_t chosen = 0;

        if (pGetCaps(phys, surface, &caps) != VK_SUCCESS) {
            snprintf(err, sizeof(err), "Vulkan: surface capabilities failed");
            goto fail;
        }
        nfmt = 32;
        if (pGetFormats(phys, surface, &nfmt, fmts) != VK_SUCCESS || nfmt == 0) {
            snprintf(err, sizeof(err), "Vulkan: sem formatos de superficie");
            goto fail;
        }
        for (fi = 0; fi < nfmt; fi++)
            if (fmts[fi].format == VK_FORMAT_B8G8R8A8_UNORM) { chosen = fi; break; }

        memset(&sci, 0, sizeof(sci));
        sci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        sci.surface = surface;
        sci.minImageCount = caps.minImageCount + 1;
        if (caps.maxImageCount && sci.minImageCount > caps.maxImageCount)
            sci.minImageCount = caps.maxImageCount;
        sci.imageFormat = fmts[chosen].format;
        sci.imageColorSpace = fmts[chosen].colorSpace;
        sci.imageExtent = caps.currentExtent;
        if (sci.imageExtent.width == 0xFFFFFFFFu) {
            sci.imageExtent.width = (uint32_t)w;
            sci.imageExtent.height = (uint32_t)h;
        }
        sci.imageArrayLayers = 1;
        sci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        sci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        sci.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        sci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        sci.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        sci.clipped = 1;
        g_extent = sci.imageExtent;

        r = pCreateSwap(dev, &sci, NULL, &swapchain);
        if (r != VK_SUCCESS) {
            snprintf(err, sizeof(err), "Vulkan: vkCreateSwapchainKHR: %d", (int)r);
            goto fail;
        }
        n_imgs = 16;
        pGetSwapImgs(dev, swapchain, &n_imgs, imgs);
        if (n_imgs == 0 || n_imgs > 16) {
            snprintf(err, sizeof(err), "Vulkan: invalid swapchain images");
            goto fail;
        }

        /* render pass:1 cor, clear animado */
        {
            VkAttachmentDescription att;
            VkAttachmentReference ref;
            VkSubpassDescription sub;
            VkSubpassDependency dep;
            VkRenderPassCreateInfo rpc;
            memset(&att, 0, sizeof(att));
            att.format = sci.imageFormat;
            att.samples = VK_SAMPLE_COUNT_1_BIT;
            att.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            att.stencilLoadOp = VK_ATTACHMENT_STENCIL_DONT_CARE;
            att.stencilStoreOp = VK_ATTACHMENT_STENCIL_DONT_CARE;
            att.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            att.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            ref.attachment = 0;
            ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            memset(&sub, 0, sizeof(sub));
            sub.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            sub.colorAttachmentCount = 1;
            sub.pColorAttachments = &ref;
            memset(&dep, 0, sizeof(dep));
            dep.srcSubpass = VK_SUBPASS_EXTERNAL;
            dep.dstSubpass = 0;
            dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            memset(&rpc, 0, sizeof(rpc));
            rpc.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            rpc.attachmentCount = 1;
            rpc.pAttachments = &att;
            rpc.subpassCount = 1;
            rpc.pSubpasses = &sub;
            rpc.dependencyCount = 1;
            rpc.pDependencies = &dep;
            r = pCreateRP(dev, &rpc, NULL, &rp);
            if (r != VK_SUCCESS) {
                snprintf(err, sizeof(err), "Vulkan: render pass: %d", (int)r);
                goto fail;
            }
        }

        for (i = 0; i < n_imgs; i++) {
            VkImageViewCreateInfo ivci;
            memset(&ivci, 0, sizeof(ivci));
            ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            ivci.image = imgs[i];
            ivci.viewType = 1; /* 2D */
            ivci.format = sci.imageFormat;
            ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            ivci.subresourceRange.levelCount = 1;
            ivci.subresourceRange.layerCount = 1;
            if (pCreateView(dev, &ivci, NULL, &views[i]) != VK_SUCCESS) {
                snprintf(err, sizeof(err), "Vulkan: image view %u", i);
                goto fail;
            }
            {
                VkFramebufferCreateInfo fci;
                memset(&fci, 0, sizeof(fci));
                fci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                fci.renderPass = rp;
                fci.attachmentCount = 1;
                fci.pAttachments = &views[i];
                fci.width = sci.imageExtent.width;
                fci.height = sci.imageExtent.height;
                fci.layers = 1;
                if (pCreateFB(dev, &fci, NULL, &fbs[i]) != VK_SUCCESS) {
                    snprintf(err, sizeof(err), "Vulkan: framebuffer %u", i);
                    goto fail;
                }
            }
        }

        /* comandos, semaforos, fence */
        {
            VkCommandPoolCreateInfo cpci;
            VkCommandBufferAllocateInfo cbai;
            VkFenceCreateInfo fci;
            VkSemaphoreCreateInfo sci2;
            memset(&cpci, 0, sizeof(cpci));
            cpci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            cpci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            cpci.queueFamilyIndex = qfam;
            if (pCreatePool(dev, &cpci, NULL, &pool) != VK_SUCCESS) {
                snprintf(err, sizeof(err), "Vulkan: command pool");
                goto fail;
            }
            memset(&cbai, 0, sizeof(cbai));
            cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cbai.commandPool = pool;
            cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cbai.commandBufferCount = 1;
            if (pAllocCmds(dev, &cbai, cmds) != VK_SUCCESS) {
                snprintf(err, sizeof(err), "Vulkan: command buffer");
                goto fail;
            }
            memset(&fci, 0, sizeof(fci));
            fci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fci.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            if (pCreateFence(dev, &fci, NULL, &fence) != VK_SUCCESS) {
                snprintf(err, sizeof(err), "Vulkan: fence");
                goto fail;
            }
            memset(&sci2, 0, sizeof(sci2));
            sci2.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            if (pCreateSem(dev, &sci2, NULL, &sem_avail) != VK_SUCCESS ||
                pCreateSem(dev, &sci2, NULL, &sem_done) != VK_SUCCESS) {
                snprintf(err, sizeof(err), "Vulkan: semaforos");
                goto fail;
            }
        }
    }

    snprintf(info, info_cap, "Vulkan 1.0 - %s", devname[0] ? devname : "GPU");
    printf("janela Vulkan ativa (%s) - ESC ou fechar para sair\n", info);
    fflush(stdout);

    t0 = t4_SDL_GetTicks();
    while (running) {
        uint32_t idx = 0;
        VkResult ar;
        VkCommandBufferBeginInfo cbbi;
        VkRenderPassBeginInfo rpbi;
        VkClearValue clearv;
        VkSubmitInfo sub;
        VkPresentInfoKHR pres;
        uint32_t stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        float tt = (float)(t4_SDL_GetTicks() - t0) / 1000.0f;

        if (vk_pump_quit()) running = 0;
        if (seconds > 0 && (int)(t4_SDL_GetTicks() - t0) >= seconds * 1000)
            running = 0;
        if (!running) break;

        pWaitFences(dev, 1, &fence, 1 /*VK_TRUE*/, (uint64_t)-1);
        pQueueWaitIdle(queue);

        ar = pAcquire(dev, swapchain, (uint64_t)-1, sem_avail, 0, &idx);
        if (ar == VK_ERROR_OUT_OF_DATE_KHR) break;
        if (ar != VK_SUCCESS && ar != VK_SUBOPTIMAL_KHR) {
            snprintf(err, sizeof(err), "Vulkan: acquire: %d", (int)ar);
            goto fail;
        }

        memset(&cbbi, 0, sizeof(cbbi));
        cbbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cbbi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        pBeginCmd(cmds[0], &cbbi);

        clearv.float32[0] = 0.05f + 0.05f * (0.5f + 0.5f * (float)sin(tt));
        clearv.float32[1] = 0.06f + 0.04f * (0.5f + 0.5f * (float)sin(tt * 0.7f + 2.0f));
        clearv.float32[2] = 0.10f + 0.05f * (0.5f + 0.5f * (float)sin(tt * 1.3f + 4.0f));
        clearv.float32[3] = 1.0f;

        memset(&rpbi, 0, sizeof(rpbi));
        rpbi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rpbi.renderPass = rp;
        rpbi.framebuffer = fbs[idx];
        rpbi.renderArea.extent = g_extent;
        rpbi.clearValueCount = 1;
        rpbi.pClearValues = &clearv;
        pCmdBeginRP(cmds[0], &rpbi, 0 /* INLINE */);
        pCmdEndRP(cmds[0]);
        pEndCmd(cmds[0]);

        memset(&sub, 0, sizeof(sub));
        sub.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        sub.waitSemaphoreCount = 1;
        sub.pWaitSemaphores = &sem_avail;
        sub.pWaitDstStageMask = &stage;
        sub.commandBufferCount = 1;
        sub.pCommandBuffers = cmds;
        sub.signalSemaphoreCount = 1;
        sub.pSignalSemaphores = &sem_done;
        pQueueSubmit(queue, 1, &sub, fence);

        memset(&pres, 0, sizeof(pres));
        pres.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        pres.waitSemaphoreCount = 1;
        pres.pWaitSemaphores = &sem_done;
        pres.swapchainCount = 1;
        pres.pSwapchains = &swapchain;
        pres.pImageIndices = &idx;
        pQueuePresent(queue, &pres);
        pQueueWaitIdle(queue);
    }

    if (pDeviceWaitIdle) pDeviceWaitIdle(dev);

    /* limpeza */
    if (pDestroySem) {
        pDestroySem(dev, sem_avail, NULL);
        pDestroySem(dev, sem_done, NULL);
    }
    if (pDestroyFence) pDestroyFence(dev, fence, NULL);
    if (pDestroyPool) pDestroyPool(dev, pool, NULL);
    for (i = 0; i < n_imgs; i++) {
        if (pDestroyFB) pDestroyFB(dev, fbs[i], NULL);
        if (pDestroyView) pDestroyView(dev, views[i], NULL);
    }
    if (pDestroyRP) pDestroyRP(dev, rp, NULL);
    if (pDestroySwap) pDestroySwap(dev, swapchain, NULL);
    if (pDestroyDevice) pDestroyDevice(dev, NULL);
    if (pDestroySurface) pDestroySurface(inst, surface, NULL);
    if (pDestroyInstance) {
        pDestroyInstance = (void *)gipa(inst, "vkDestroyInstance");
        if (pDestroyInstance) pDestroyInstance(inst, NULL);
    }
    vk_dlclose(loader);
    return 0;

fail:
    snprintf(info, info_cap, "%s", err);
    if (loader) vk_dlclose(loader);
    return -1;
}
