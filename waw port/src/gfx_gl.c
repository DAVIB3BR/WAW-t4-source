/*
 * gfx_gl.c - backend OpenGL do porte.
 *
 * Desktop: tenta contexto 4.5 core -> 4.3 core -> 3.3 core -> 2.1 compat.
 * ES:      tenta OpenGL ES 3.x (mesmo codigo com shader GLSL ES).
 * Todas as funcoes entram por SDL_GL_GetProcAddress, entao nao ha dependencia
 * de headers/libGL no build - o binario roda em qualquer maquina com um
 * driver de video instalado.
 */
#include "gfx.h"
#include "sdl2_dyn.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
static void *gl_dlopen(const char *n) { return (void *)LoadLibraryA(n); }
static void gl_dlclose(void *l) { if (l) FreeLibrary((HMODULE)l); }
#else
#include <dlfcn.h>
static void *gl_dlopen(const char *n) { return dlopen(n, RTLD_NOW | RTLD_LOCAL); }
static void gl_dlclose(void *l) { if (l) dlclose(l); }
#endif

/* tipos minimos do GL ( GLsizei=GLint=int, GLuint=unsigned, etc.) */
typedef unsigned int GLenum_t;
typedef unsigned int GLuint_t;
typedef unsigned int GLbitfield_t;
typedef int GLint_t;
typedef int GLsizei_t;
typedef char GLchar_t;
typedef float GLfloat_t;
typedef unsigned char GLboolean_t;

#define GL_VENDOR   0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION  0x1F02
#define GL_COLOR_BUFFER_BIT 0x4000
#define GL_DEPTH_BUFFER_BIT 0x0100
#define GL_TRIANGLES        0x0004
#define GL_FLOAT            0x1406
#define GL_ARRAY_BUFFER     0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_DYNAMIC_DRAW     0x88E8
#define GL_UNSIGNED_SHORT   0x1403
#define GL_UNSIGNED_BYTE    0x1401
#define GL_RGBA             0x1908
#define GL_DEPTH_TEST       0x0B71
#define GL_LEQUAL           0x0203

/* ------------------------------------------------------------- lib check */

int t4_gfx_gl_lib_check(char *info, size_t cap)
{
    static const char *cands[] = {
#if defined(__APPLE__)
        "/System/Library/Frameworks/OpenGL.framework/OpenGL",
        "libGL.dylib",
#elif defined(_WIN32)
        "opengl32.dll",
#else
        "libGL.so.1", "libGL.so",
#endif
        NULL
    };
    int i;
    for (i = 0; cands[i]; i++) {
        void *lib = gl_dlopen(cands[i]);
        if (lib) {
            snprintf(info, cap, "disponivel (%s)", cands[i]);
            gl_dlclose(lib);
            return 0;
        }
    }
    snprintf(info, cap, "missing (no OpenGL driver)");
    return -1;
}

/* ------------------------------------------------------ carregamento GL */

typedef const unsigned char *(*PFN_glGetString)(GLenum_t);
typedef void (*PFN_glClearColor)(GLfloat_t, GLfloat_t, GLfloat_t, GLfloat_t);
typedef void (*PFN_glClear)(GLbitfield_t);
typedef void (*PFN_glViewport)(GLint_t, GLint_t, GLsizei_t, GLsizei_t);
/* modernos */
typedef GLuint_t (*PFN_glCreateShader)(GLenum_t);
typedef void (*PFN_glShaderSource)(GLuint_t, GLsizei_t, const GLchar_t *const *, const GLint_t *);
typedef void (*PFN_glCompileShader)(GLuint_t);
typedef void (*PFN_glGetShaderiv)(GLuint_t, GLenum_t, GLint_t *);
typedef void (*PFN_glGetShaderInfoLog)(GLuint_t, GLsizei_t, GLsizei_t *, GLchar_t *);
typedef GLuint_t (*PFN_glCreateProgram)(void);
typedef void (*PFN_glAttachShader)(GLuint_t, GLuint_t);
typedef void (*PFN_glLinkProgram)(GLuint_t);
typedef void (*PFN_glGetProgramiv)(GLuint_t, GLenum_t, GLint_t *);
typedef void (*PFN_glGetProgramInfoLog)(GLuint_t, GLsizei_t, GLsizei_t *, GLchar_t *);
typedef void (*PFN_glUseProgram)(GLuint_t);
typedef void (*PFN_glDeleteShader)(GLuint_t);
typedef void (*PFN_glGenVertexArrays)(GLsizei_t, GLuint_t *);
typedef void (*PFN_glBindVertexArray)(GLuint_t);
typedef void (*PFN_glGenBuffers)(GLsizei_t, GLuint_t *);
typedef void (*PFN_glBindBuffer)(GLenum_t, GLuint_t);
typedef void (*PFN_glBufferData)(GLenum_t, long, const void *, GLenum_t);
typedef void (*PFN_glVertexAttribPointer)(GLuint_t, GLint_t, GLenum_t, GLboolean_t, GLsizei_t, const void *);
typedef void (*PFN_glEnableVertexAttribArray)(GLuint_t);
typedef void (*PFN_glDrawArrays)(GLenum_t, GLint_t, GLsizei_t);
typedef GLint_t (*PFN_glGetUniformLocation)(GLuint_t, const GLchar_t *);
typedef void (*PFN_glUniform1f)(GLint_t, GLfloat_t);
/* mundo */
typedef void (*PFN_glDrawElements)(GLenum_t, GLsizei_t, GLenum_t, const void *);
typedef void (*PFN_glUniformMatrix4fv)(GLint_t, GLsizei_t, GLboolean_t, const GLfloat_t *);
typedef void (*PFN_glReadPixels)(GLint_t, GLint_t, GLsizei_t, GLsizei_t, GLenum_t, GLenum_t, void *);
typedef void (*PFN_glEnable)(GLenum_t);
typedef void (*PFN_glDisable)(GLenum_t);
typedef void (*PFN_glDepthFunc)(GLenum_t);
typedef GLint_t (*PFN_glGetAttribLocation)(GLuint_t, const GLchar_t *);
typedef void (*PFN_glPolygonMode)(GLenum_t, GLenum_t);
/* legado (2.1) */
typedef void (*PFN_glBegin)(GLenum_t);
typedef void (*PFN_glEnd)(void);
typedef void (*PFN_glColor3f)(GLfloat_t, GLfloat_t, GLfloat_t);
typedef void (*PFN_glVertex2f)(GLfloat_t, GLfloat_t);

struct gl_api {
    PFN_glGetString GetString;
    PFN_glClearColor ClearColor;
    PFN_glClear Clear;
    PFN_glViewport Viewport;
    /* modernos */
    PFN_glCreateShader CreateShader;
    PFN_glShaderSource ShaderSource;
    PFN_glCompileShader CompileShader;
    PFN_glGetShaderiv GetShaderiv;
    PFN_glGetShaderInfoLog GetShaderInfoLog;
    PFN_glCreateProgram CreateProgram;
    PFN_glAttachShader AttachShader;
    PFN_glLinkProgram LinkProgram;
    PFN_glGetProgramiv GetProgramiv;
    PFN_glGetProgramInfoLog GetProgramInfoLog;
    PFN_glUseProgram UseProgram;
    PFN_glDeleteShader DeleteShader;
    PFN_glGenVertexArrays GenVertexArrays;
    PFN_glBindVertexArray BindVertexArray;
    PFN_glGenBuffers GenBuffers;
    PFN_glBindBuffer BindBuffer;
    PFN_glBufferData BufferData;
    PFN_glVertexAttribPointer VertexAttribPointer;
    PFN_glEnableVertexAttribArray EnableVertexAttribArray;
    PFN_glDrawArrays DrawArrays;
    PFN_glGetUniformLocation GetUniformLocation;
    PFN_glUniform1f Uniform1f;
    /* mundo (soft: podem ficar NULL no caminho legado) */
    PFN_glDrawElements DrawElements;
    PFN_glUniformMatrix4fv UniformMatrix4fv;
    PFN_glReadPixels ReadPixels;
    PFN_glEnable Enable;
    PFN_glDisable Disable;
    PFN_glDepthFunc DepthFunc;
    PFN_glGetAttribLocation GetAttribLocation;
    PFN_glPolygonMode PolygonMode;
    /* legado */
    PFN_glBegin Begin;
    PFN_glEnd End;
    PFN_glColor3f Color3f;
    PFN_glVertex2f Vertex2f;
};

#define LOAD(gl, name)                                                     \
    do {                                                                   \
        gl->name = (void *)t4_SDL_GL_GetProcAddress("gl" #name);           \
        if (!gl->name) {                                                   \
            if (misscap > 4) snprintf(miss, misscap, "gl" #name);          \
            return -1;                                                     \
        }                                                                  \
    } while (0)

static int gl_load_common(struct gl_api *gl, char *miss, size_t misscap)
{
    snprintf(miss, misscap, "?");
    LOAD(gl, GetString);
    LOAD(gl, ClearColor);
    LOAD(gl, Clear);
    LOAD(gl, Viewport);
    return 0;
}

static int gl_load_modern(struct gl_api *gl, char *miss, size_t misscap)
{
    LOAD(gl, CreateShader);
    LOAD(gl, ShaderSource);
    LOAD(gl, CompileShader);
    LOAD(gl, GetShaderiv);
    LOAD(gl, GetShaderInfoLog);
    LOAD(gl, CreateProgram);
    LOAD(gl, AttachShader);
    LOAD(gl, LinkProgram);
    LOAD(gl, GetProgramiv);
    LOAD(gl, GetProgramInfoLog);
    LOAD(gl, UseProgram);
    LOAD(gl, DeleteShader);
    LOAD(gl, GenVertexArrays);
    LOAD(gl, BindVertexArray);
    LOAD(gl, GenBuffers);
    LOAD(gl, BindBuffer);
    LOAD(gl, BufferData);
    LOAD(gl, VertexAttribPointer);
    LOAD(gl, EnableVertexAttribArray);
    LOAD(gl, DrawArrays);
    LOAD(gl, GetUniformLocation);
    LOAD(gl, Uniform1f);
    return 0;
}

static int gl_load_legacy(struct gl_api *gl, char *miss, size_t misscap)
{
    LOAD(gl, Begin);
    LOAD(gl, End);
    LOAD(gl, Color3f);
    LOAD(gl, Vertex2f);
    return 0;
}

/* simbolos do renderizador de mundo; ausentes ficam NULL (caminho legado
 * nao os usa) */
static void gl_load_world_syms(struct gl_api *gl)
{
    gl->DrawElements = (void *)t4_SDL_GL_GetProcAddress("glDrawElements");
    gl->UniformMatrix4fv = (void *)t4_SDL_GL_GetProcAddress("glUniformMatrix4fv");
    gl->ReadPixels = (void *)t4_SDL_GL_GetProcAddress("glReadPixels");
    gl->Enable = (void *)t4_SDL_GL_GetProcAddress("glEnable");
    gl->Disable = (void *)t4_SDL_GL_GetProcAddress("glDisable");
    gl->DepthFunc = (void *)t4_SDL_GL_GetProcAddress("glDepthFunc");
    gl->GetAttribLocation = (void *)t4_SDL_GL_GetProcAddress("glGetAttribLocation");
    gl->PolygonMode = (void *)t4_SDL_GL_GetProcAddress("glPolygonMode");
}

#define SHADER_COMPILE 0x8B81
#define SHADER_LINK    0x8B82

static GLuint_t gl_build_program(struct gl_api *gl, const char *vs,
                                 const char *fs, char *err, size_t errcap)
{
    GLuint_t v, f, prog;
    GLint_t ok = 0;

    v = gl->CreateShader(0x8B31); /* GL_VERTEX_SHADER */
    gl->ShaderSource(v, 1, &vs, NULL);
    gl->CompileShader(v);
    gl->GetShaderiv(v, SHADER_COMPILE, &ok);
    if (!ok) {
        char log[512];
        gl->GetShaderInfoLog(v, sizeof(log), NULL, log);
        snprintf(err, errcap, "vertex shader: %s", log);
        gl->DeleteShader(v);
        return 0;
    }
    f = gl->CreateShader(0x8B30); /* GL_FRAGMENT_SHADER */
    gl->ShaderSource(f, 1, &fs, NULL);
    gl->CompileShader(f);
    gl->GetShaderiv(f, SHADER_COMPILE, &ok);
    if (!ok) {
        char log[512];
        gl->GetShaderInfoLog(f, sizeof(log), NULL, log);
        snprintf(err, errcap, "fragment shader: %s", log);
        gl->DeleteShader(v);
        gl->DeleteShader(f);
        return 0;
    }
    prog = gl->CreateProgram();
    gl->AttachShader(prog, v);
    gl->AttachShader(prog, f);
    gl->LinkProgram(prog);
    gl->GetProgramiv(prog, SHADER_LINK, &ok);
    gl->DeleteShader(v);
    gl->DeleteShader(f);
    if (!ok) {
        char log[512];
        gl->GetProgramInfoLog(prog, sizeof(log), NULL, log);
        snprintf(err, errcap, "link: %s", log);
        return 0;
    }
    return prog;
}

/* ------------------------------------------------------------------ run */

static int pump_quit(void)
{
    T4_SDLEvent ev;
    int quit = 0;
    while (t4_SDL_PollEvent(&ev)) {
        if (ev.type == T4_SDL_QUIT) quit = 1;
        if (ev.type == T4_SDL_KEYDOWN && ev.raw[16] == T4_SDLK_ESCAPE) quit = 1;
    }
    return quit;
}

int t4_gfx_gl_run(int w, int h, int seconds, int es_mode,
                  char *info, size_t info_cap)
{
    /* cadeia de tentativas: versao, minor, perfil (-1 = sem mascara) */
    static const struct { int maj, min, prof; } tries_desktop[] = {
        { 4, 5, 1 }, { 4, 3, 1 }, { 3, 3, 1 }, { 2, 1, -1 }, { 0, 0, 0 }
    };
    static const struct { int maj, min, prof; } tries_es[] = {
        { 3, 0, 4 }, { 0, 0, 0 }
    };
    const char *verstr;
    int maj = 0, min = 0, i, modern, running = 1;
    char miss[64];
    char serr[512];
    T4_SDL_Window *win = NULL;
    T4_SDL_GLContext ctx = NULL;
    struct gl_api gl;
    const char *profile_name = es_mode ? "es" : "compat";

    memset(&gl, 0, sizeof(gl));
    snprintf(info, info_cap, "?");

    for (i = 0; (es_mode ? tries_es[i].maj : tries_desktop[i].maj); i++) {
        int majr = es_mode ? tries_es[i].maj : tries_desktop[i].maj;
        int minr = es_mode ? tries_es[i].min : tries_desktop[i].min;
        int prof = es_mode ? tries_es[i].prof : tries_desktop[i].prof;

        t4_SDL_GL_SetAttribute(T4_SDL_GL_DOUBLEBUFFER, 1);
        t4_SDL_GL_SetAttribute(T4_SDL_GL_CONTEXT_MAJOR_VERSION, majr);
        t4_SDL_GL_SetAttribute(T4_SDL_GL_CONTEXT_MINOR_VERSION, minr);
        if (prof >= 0)
            t4_SDL_GL_SetAttribute(T4_SDL_GL_CONTEXT_PROFILE_MASK, (unsigned)prof);
        else
            t4_SDL_GL_SetAttribute(T4_SDL_GL_CONTEXT_PROFILE_MASK, 0);

        win = t4_SDL_CreateWindow("t4port", T4_SDL_WINDOWPOS_CENTERED,
                                  T4_SDL_WINDOWPOS_CENTERED, w, h,
                                  T4_SDL_WINDOW_OPENGL);
        if (!win) {
            snprintf(miss, sizeof(miss), "CreateWindow: %s", t4_SDL_GetError());
            continue;
        }
        ctx = t4_SDL_GL_CreateContext(win);
        if (!ctx) {
            t4_SDL_DestroyWindow(win);
            win = NULL;
            continue;
        }
        break; /* contexto criado */
    }
    if (!ctx) {
        snprintf(serr, sizeof(serr), "OpenGL: nenhum contexto criado (%s)",
                 win ? "sem contexto" : miss);
        snprintf(info, info_cap, "%s", serr);
        return -1;
    }

    /* versao real do contexto */
    if (gl_load_common(&gl, miss, sizeof(miss)) != 0) {
        snprintf(serr, sizeof(serr), "OpenGL: missing basic symbol: %s", miss);
        goto fail;
    }
    verstr = (const char *)gl.GetString(GL_VERSION);
    if (!verstr) verstr = "0.0";
    {
        /* a string pode vir como "OpenGL ES 3.2 ..." - pula ate o digito */
        const char *dig = strpbrk(verstr, "0123456789");
        if (dig) sscanf(dig, "%d.%d", &maj, &min);
    }

    modern = es_mode ? (maj >= 3) : (maj > 3 || (maj == 3 && min >= 3));
    if (es_mode && !modern) {
        snprintf(serr, sizeof(serr), "OpenGL ES 3.x necessario (contexto %s)", verstr);
        goto fail;
    }
    profile_name = es_mode ? "es" : (modern ? "core" : "compat");

    {
        const unsigned char *ren = gl.GetString(GL_RENDERER);
        snprintf(info, info_cap, "OpenGL %s (%s) - %s", verstr, profile_name,
                 ren ? (const char *)ren : "?");
    }

    if (modern) {
        static const char vs_glsl[] =
            "#version 330 core\n"
            "in vec2 aPos;\n"
            "in vec3 aCol;\n"
            "uniform float uAngle;\n"
            "out vec3 vCol;\n"
            "void main() {\n"
            "  float c = cos(uAngle), s = sin(uAngle);\n"
            "  gl_Position = vec4(aPos.x*c - aPos.y*s, aPos.x*s + aPos.y*c, 0.0, 1.0);\n"
            "  vCol = aCol;\n"
            "}\n";
        static const char fs_glsl[] =
            "#version 330 core\n"
            "in vec3 vCol;\n"
            "out vec4 fragColor;\n"
            "void main() { fragColor = vec4(vCol, 1.0); }\n";
        static const char vs_es[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 aPos;\n"
            "in vec3 aCol;\n"
            "uniform float uAngle;\n"
            "out vec3 vCol;\n"
            "void main() {\n"
            "  float c = cos(uAngle), s = sin(uAngle);\n"
            "  gl_Position = vec4(aPos.x*c - aPos.y*s, aPos.x*s + aPos.y*c, 0.0, 1.0);\n"
            "  vCol = aCol;\n"
            "}\n";
        static const char fs_es[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec3 vCol;\n"
            "out vec4 fragColor;\n"
            "void main() { fragColor = vec4(vCol, 1.0); }\n";
        GLuint_t prog, vao, vbo;
        GLint_t u_angle;
        GLuint_t loc_pos, loc_col;
        float verts[15];
        char serr2[512];
        uint32_t t0;

        if (gl_load_modern(&gl, miss, sizeof(miss)) != 0) {
            snprintf(serr, sizeof(serr), "OpenGL: missing modern symbol: %s", miss);
            goto fail;
        }
        prog = gl_build_program(&gl, es_mode ? vs_es : vs_glsl,
                                es_mode ? fs_es : fs_glsl,
                                serr2, sizeof(serr2));
        if (!prog) {
            snprintf(serr, sizeof(serr), "OpenGL: shaders: %s", serr2);
            goto fail;
        }
        u_angle = gl.GetUniformLocation(prog, "uAngle");

        gl.GenVertexArrays(1, &vao);
        gl.BindVertexArray(vao);
        gl.GenBuffers(1, &vbo);
        gl.BindBuffer(GL_ARRAY_BUFFER, vbo);
        gl.BufferData(GL_ARRAY_BUFFER, (long)sizeof(verts), NULL, GL_DYNAMIC_DRAW);

        loc_pos = 0; /* layout padrao: aPos=0, aCol=1 (nome != local; usamos o
                      * atributo pelo indice do shader) */
        {
            /* descobre os locais pelos nomes */
            typedef GLint_t (*PFN_geta)(GLuint_t, const GLchar_t *);
            PFN_geta geta = (void *)t4_SDL_GL_GetProcAddress("glGetAttribLocation");
            if (geta) {
                loc_pos = (GLuint_t)geta(prog, "aPos");
                loc_col = (GLuint_t)geta(prog, "aCol");
            } else {
                loc_pos = 0; loc_col = 1;
            }
        }
        gl.EnableVertexAttribArray(loc_pos);
        gl.VertexAttribPointer(loc_pos, 2, GL_FLOAT, (GLboolean_t)0, 5 * sizeof(float), (const void *)0);
        gl.EnableVertexAttribArray(loc_col);
        gl.VertexAttribPointer(loc_col, 3, GL_FLOAT, (GLboolean_t)0, 5 * sizeof(float),
                               (const void *)(2 * sizeof(float)));

        t4_SDL_GL_SetSwapInterval(1);
        printf("janela OpenGL ativa (%s) - ESC ou fechar para sair\n", info);
        fflush(stdout);

        t0 = t4_SDL_GetTicks();
        while (running) {
            float ang, cs, sn;
            int k;
            if (pump_quit()) running = 0;
            if (seconds > 0 && (int)(t4_SDL_GetTicks() - t0) >= seconds * 1000)
                running = 0;

            ang = (float)(t4_SDL_GetTicks() - t0) / 1000.0f;
            cs = cosf(ang); sn = sinf(ang);
            /* triangulo com vertices rotacionados no proprio vertex shader */
            (void)cs; (void)sn; (void)k;
            verts[0] =  0.0f; verts[1] =  0.65f; verts[2] = 1.f; verts[3] = 0.25f; verts[4] = 0.2f;
            verts[5] = -0.6f; verts[6] = -0.4f; verts[7] = 0.2f; verts[8] = 1.f;   verts[9] = 0.3f;
            verts[10] = 0.6f; verts[11] = -0.4f; verts[12] = 0.2f; verts[13] = 0.4f; verts[14] = 1.f;

            gl.Viewport(0, 0, w, h);
            gl.ClearColor(0.06f, 0.07f, 0.10f, 1.0f);
            gl.Clear(GL_COLOR_BUFFER_BIT);

            gl.UseProgram(prog);
            gl.Uniform1f(u_angle, ang);
            gl.BindVertexArray(vao);
            gl.BindBuffer(GL_ARRAY_BUFFER, vbo);
            gl.BufferData(GL_ARRAY_BUFFER, (long)sizeof(verts), verts, GL_DYNAMIC_DRAW);
            gl.DrawArrays(GL_TRIANGLES, 0, 3);

            t4_SDL_GL_SwapWindow(win);
        }
        (void)loc_pos; (void)loc_col;
        return 0;
    }

    /* caminho legado: GL 2.1 compat (immediate mode) */
    {
        uint32_t t0;
        if (gl_load_legacy(&gl, miss, sizeof(miss)) != 0) {
            snprintf(serr, sizeof(serr), "OpenGL: missing legacy symbol: %s", miss);
            goto fail;
        }
        t4_SDL_GL_SetSwapInterval(1);
        printf("janela OpenGL 2.1 ativa - ESC ou fechar para sair\n");
        fflush(stdout);

        t0 = t4_SDL_GetTicks();
        while (running) {
            float ang, cs, sn, x, y;
            int k;
            static const float base[3][5] = {
                {  0.0f,  0.65f, 1.f, 0.25f, 0.2f },
                { -0.6f, -0.4f, 0.2f, 1.f,  0.3f },
                {  0.6f, -0.4f, 0.2f, 0.4f, 1.f }
            };
            if (pump_quit()) running = 0;
            if (seconds > 0 && (int)(t4_SDL_GetTicks() - t0) >= seconds * 1000)
                running = 0;

            ang = (float)(t4_SDL_GetTicks() - t0) / 1000.0f;
            cs = cosf(ang); sn = sinf(ang);

            gl.Viewport(0, 0, w, h);
            gl.ClearColor(0.06f, 0.07f, 0.10f, 1.0f);
            gl.Clear(GL_COLOR_BUFFER_BIT);

            gl.Begin(GL_TRIANGLES);
            for (k = 0; k < 3; k++) {
                x = base[k][0] * cs - base[k][1] * sn;
                y = base[k][0] * sn + base[k][1] * cs;
                gl.Color3f(base[k][2], base[k][3], base[k][4]);
                gl.Vertex2f(x, y);
            }
            gl.End();

            t4_SDL_GL_SwapWindow(win);
        }
        return 0;
    }

fail:
    if (ctx) t4_SDL_GL_DeleteContext(ctx);
    if (win) t4_SDL_DestroyWindow(win);
    snprintf(info, info_cap, "%s", serr);
    return -1;
}

/* ------------------------------------------------- renderizador de mundo */

#include <math.h>
#include "png.h"

static void w4_mul(float r[16], const float a[16], const float b[16])
{
    int c, rr, k;
    for (c = 0; c < 4; c++)
        for (rr = 0; rr < 4; rr++) {
            float s = 0.0f;
            for (k = 0; k < 4; k++)
                s += a[k * 4 + rr] * b[c * 4 + k];
            r[c * 4 + rr] = s;
        }
}

static void w4_persp(float m[16], float fovy_deg, float aspect, float zn, float zf)
{
    float f = 1.0f / tanf(fovy_deg * 3.14159265f / 360.0f);
    memset(m, 0, 16 * sizeof(float));
    m[0] = f / aspect;
    m[5] = f;
    m[10] = (zf + zn) / (zn - zf);
    m[11] = -1.0f;
    m[14] = (2.0f * zf * zn) / (zn - zf);
}

static void v3_sub(float o[3], const float a[3], const float b[3])
{
    o[0] = a[0] - b[0]; o[1] = a[1] - b[1]; o[2] = a[2] - b[2];
}

static float v3_dot(const float a[3], const float b[3])
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

static void v3_cross(float o[3], const float a[3], const float b[3])
{
    o[0] = a[1] * b[2] - a[2] * b[1];
    o[1] = a[2] * b[0] - a[0] * b[2];
    o[2] = a[0] * b[1] - a[1] * b[0];
}

static void v3_norm(float a[3])
{
    float l = sqrtf(v3_dot(a, a));
    if (l > 1e-12f) { a[0] /= l; a[1] /= l; a[2] /= l; }
}

static void w4_look(float m[16], const float eye[3], const float ctr[3],
                    const float up0[3])
{
    float f[3], s[3], u[3], up[3];
    up[0] = up0[0]; up[1] = up0[1]; up[2] = up0[2];
    v3_sub(f, ctr, eye);
    v3_norm(f);
    v3_cross(s, f, up);
    v3_norm(s);
    v3_cross(u, s, f);
    memset(m, 0, 16 * sizeof(float));
    m[0] = s[0]; m[4] = s[1]; m[8] = s[2];
    m[1] = u[0]; m[5] = u[1]; m[9] = u[2];
    m[2] = -f[0]; m[6] = -f[1]; m[10] = -f[2];
    m[12] = -v3_dot(s, eye);
    m[13] = -v3_dot(u, eye);
    m[14] = v3_dot(f, eye);
    m[15] = 1.0f;
}

int t4_gfx_gl_world_run(int w, int h, int seconds, int es_mode,
                        const float *pos, const float *nrm, size_t nverts,
                        const uint16_t *idx, size_t nidx,
                        const float mins[3], const float maxs[3],
                        const char *shot_png, int wire,
                        char *info, size_t info_cap)
{
    static const struct { int maj, min, prof; } tries_desktop[] = {
        { 4, 5, 1 }, { 4, 3, 1 }, { 3, 3, 1 }, { 0, 0, 0 }
    };
    static const struct { int maj, min, prof; } tries_es[] = {
        { 3, 0, 4 }, { 0, 0, 0 }
    };
    static const char vs_glsl[] =
        "#version 330 core\n"
        "in vec3 aPos;\n"
        "in vec3 aNrm;\n"
        "uniform mat4 uMvp;\n"
        "out vec3 vNrm;\n"
        "void main() {\n"
        "  gl_Position = uMvp * vec4(aPos, 1.0);\n"
        "  vNrm = aNrm;\n"
        "}\n";
    static const char fs_glsl[] =
        "#version 330 core\n"
        "in vec3 vNrm;\n"
        "out vec4 fragColor;\n"
        "uniform vec3 uLight;\n"
        "void main() {\n"
        "  vec3 n = normalize(vNrm);\n"
        "  float d = max(dot(n, uLight), 0.0);\n"
        "  vec3 c = vec3(0.30, 0.34, 0.40) + vec3(1.0, 0.95, 0.85) * d;\n"
        "  fragColor = vec4(c, 1.0);\n"
        "}\n";
    static const char vs_es[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec3 aPos;\n"
        "in vec3 aNrm;\n"
        "uniform mat4 uMvp;\n"
        "out vec3 vNrm;\n"
        "void main() {\n"
        "  gl_Position = uMvp * vec4(aPos, 1.0);\n"
        "  vNrm = aNrm;\n"
        "}\n";
    static const char fs_es[] =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec3 vNrm;\n"
        "out vec4 fragColor;\n"
        "uniform vec3 uLight;\n"
        "void main() {\n"
        "  vec3 n = normalize(vNrm);\n"
        "  float d = max(dot(n, uLight), 0.0);\n"
        "  vec3 c = vec3(0.30, 0.34, 0.40) + vec3(1.0, 0.95, 0.85) * d;\n"
        "  fragColor = vec4(c, 1.0);\n"
        "}\n";
    const char *verstr;
    int maj = 0, min = 0, i, running = 1;
    char miss[64];
    char serr[512];
    T4_SDL_Window *win = NULL;
    T4_SDL_GLContext ctx = NULL;
    struct gl_api gl;
    GLuint_t prog, vao, vbo_pos, vbo_nrm, ibo;
    GLint_t u_mvp, u_light;
    float ctr[3], ext[3], center[3], eye[3], up[3], light[3];
    float mvp[16], proj[16], view[16];
    float radius, zn, zf, aspect;
    uint32_t t0;
    int frame = 0, shot_done = 0;

    memset(&gl, 0, sizeof(gl));
    snprintf(info, info_cap, "?");
    if (!pos || !idx || !nverts || !nidx) {
        snprintf(info, info_cap, "world: empty arrays");
        return -1;
    }
    /* SDL obrigatorio: os ponteiros t4_SDL_* so existem apos o load
     * (chamar direto sem isto era SIGSEGV em 0x0 no primeiro uso) */
    if (t4_sdl_load() != 0) {
        snprintf(info, info_cap, "SDL2 indisponivel: %s", t4_sdl_error());
        return -1;
    }
    if (t4_SDL_Init(T4_SDL_INIT_VIDEO) != 0) {
        snprintf(info, info_cap, "SDL_Init(VIDEO): %s", t4_SDL_GetError());
        t4_sdl_unload();
        return -1;
    }

    for (i = 0; (es_mode ? tries_es[i].maj : tries_desktop[i].maj); i++) {
        int majr = es_mode ? tries_es[i].maj : tries_desktop[i].maj;
        int minr = es_mode ? tries_es[i].min : tries_desktop[i].min;
        int prof = es_mode ? tries_es[i].prof : tries_desktop[i].prof;

        t4_SDL_GL_SetAttribute(T4_SDL_GL_DOUBLEBUFFER, 1);
        t4_SDL_GL_SetAttribute(T4_SDL_GL_CONTEXT_MAJOR_VERSION, majr);
        t4_SDL_GL_SetAttribute(T4_SDL_GL_CONTEXT_MINOR_VERSION, minr);
        if (prof >= 0)
            t4_SDL_GL_SetAttribute(T4_SDL_GL_CONTEXT_PROFILE_MASK, (unsigned)prof);
        else
            t4_SDL_GL_SetAttribute(T4_SDL_GL_CONTEXT_PROFILE_MASK, 0);

        win = t4_SDL_CreateWindow("t4port - GfxWorld (nativo)", T4_SDL_WINDOWPOS_CENTERED,
                                  T4_SDL_WINDOWPOS_CENTERED, w, h,
                                  T4_SDL_WINDOW_OPENGL);
        if (!win) {
            snprintf(miss, sizeof(miss), "CreateWindow: %s", t4_SDL_GetError());
            continue;
        }
        ctx = t4_SDL_GL_CreateContext(win);
        if (!ctx) {
            t4_SDL_DestroyWindow(win);
            win = NULL;
            continue;
        }
        break;
    }
    if (!ctx) {
        snprintf(serr, sizeof(serr), "world: no OpenGL context (%s)",
                 win ? "sem contexto" : miss);
        snprintf(info, info_cap, "%s", serr);
        t4_SDL_Quit();
        t4_sdl_unload();
        return -1;
    }
    if (gl_load_common(&gl, miss, sizeof(miss)) != 0) {
        snprintf(serr, sizeof(serr), "world: missing basic symbol: %s", miss);
        goto fail;
    }
    verstr = (const char *)gl.GetString(GL_VERSION);
    if (!verstr) verstr = "0.0";
    {
        const char *dig = strpbrk(verstr, "0123456789");
        if (dig) sscanf(dig, "%d.%d", &maj, &min);
    }
    if (!(es_mode ? (maj >= 3) : (maj > 3 || (maj == 3 && min >= 3)))) {
        snprintf(serr, sizeof(serr), "world: OpenGL 3.3+ or ES 3.x required (has %s)", verstr);
        goto fail;
    }
    if (gl_load_modern(&gl, miss, sizeof(miss)) != 0) {
        snprintf(serr, sizeof(serr), "world: missing modern symbol: %s", miss);
        goto fail;
    }
    gl_load_world_syms(&gl);
    if (!gl.DrawElements || !gl.UniformMatrix4fv || !gl.ReadPixels ||
        !gl.Enable || !gl.DepthFunc || !gl.GetAttribLocation) {
        snprintf(serr, sizeof(serr), "world: driver lacks essential symbols");
        goto fail;
    }
    {
        char serr2[512];
        prog = gl_build_program(&gl, es_mode ? vs_es : vs_glsl,
                                es_mode ? fs_es : fs_glsl,
                                serr2, sizeof(serr2));
        if (!prog) {
            snprintf(serr, sizeof(serr), "world: shaders: %s", serr2);
            goto fail;
        }
    }
    u_mvp = gl.GetUniformLocation(prog, "uMvp");
    u_light = gl.GetUniformLocation(prog, "uLight");

    gl.GenVertexArrays(1, &vao);
    gl.BindVertexArray(vao);
    gl.GenBuffers(1, &vbo_pos);
    gl.BindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    gl.BufferData(GL_ARRAY_BUFFER, (long)(nverts * 3 * sizeof(float)), pos, 0x88E4 /*STATIC_DRAW*/);
    gl.VertexAttribPointer((GLuint_t)gl.GetAttribLocation(prog, "aPos"), 3, GL_FLOAT,
                           (GLboolean_t)0, 3 * sizeof(float), (const void *)0);
    gl.EnableVertexAttribArray((GLuint_t)gl.GetAttribLocation(prog, "aPos"));
    gl.GenBuffers(1, &vbo_nrm);
    gl.BindBuffer(GL_ARRAY_BUFFER, vbo_nrm);
    gl.BufferData(GL_ARRAY_BUFFER, (long)(nverts * 3 * sizeof(float)), nrm, 0x88E4);
    gl.VertexAttribPointer((GLuint_t)gl.GetAttribLocation(prog, "aNrm"), 3, GL_FLOAT,
                           (GLboolean_t)0, 3 * sizeof(float), (const void *)0);
    gl.EnableVertexAttribArray((GLuint_t)gl.GetAttribLocation(prog, "aNrm"));
    gl.GenBuffers(1, &ibo);
    gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, (long)(nidx * sizeof(uint16_t)), idx, 0x88E4);

    /* camera: centro e raio a partir dos limites do mundo */
    for (i = 0; i < 3; i++) {
        ctr[i] = (mins[i] + maxs[i]) * 0.5f;
        ext[i] = maxs[i] - mins[i];
    }
    radius = ext[0];
    if (ext[1] > radius) radius = ext[1];
    if (ext[2] > radius) radius = ext[2];
    if (radius < 1.0f) radius = 1.0f;
    zn = radius * 0.02f;
    zf = radius * 40.0f;
    up[0] = 0.0f; up[1] = 0.0f; up[2] = 1.0f;
    light[0] = 0.38f; light[1] = -0.55f; light[2] = 0.74f;
    v3_norm(light);

    gl.Enable(GL_DEPTH_TEST);
    gl.DepthFunc(GL_LEQUAL);
    if (wire && gl.PolygonMode)
        gl.PolygonMode(0x0408 /*GL_FRONT_AND_BACK*/, 0x1B01 /*GL_LINE*/);

    t4_SDL_GL_SetSwapInterval(1);
    t0 = t4_SDL_GetTicks();
    printf("mundo 3D nativo: %zu vertices, %zu indices (%zu triangulos)\n",
           nverts, nidx, nidx / 3);
    printf("limites: [%.1f %.1f %.1f] .. [%.1f %.1f %.1f]  - ESC ou fechar para sair\n",
           mins[0], mins[1], mins[2], maxs[0], maxs[1], maxs[2]);
    fflush(stdout);

    while (running) {
        float ang, dur;
        uint32_t ticks = t4_SDL_GetTicks();
        if (pump_quit()) running = 0;
        if (seconds > 0 && (int)(ticks - t0) >= seconds * 1000)
            running = 0;
        dur = (float)(ticks - t0) * 0.00012f;
        ang = dur;

        aspect = (h > 0) ? (float)w / (float)h : 1.0f;
        w4_persp(proj, 55.0f, aspect, zn, zf);

        center[0] = ctr[0] + cosf(ang) * radius * 1.35f;
        center[1] = ctr[1] + sinf(ang) * radius * 1.35f;
        center[2] = ctr[2] + radius * 0.85f;
        eye[0] = ctr[0]; eye[1] = ctr[1]; eye[2] = ctr[2];
        w4_look(view, center, eye, up);
        w4_mul(mvp, proj, view);

        gl.Viewport(0, 0, w, h);
        gl.ClearColor(0.05f, 0.07f, 0.11f, 1.0f);
        gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gl.UseProgram(prog);
        gl.UniformMatrix4fv(u_mvp, 1, (GLboolean_t)0, mvp);
        if (u_light >= 0) {
            typedef void (*PFN_u3f)(GLint_t, GLfloat_t, GLfloat_t, GLfloat_t);
            static PFN_u3f u3f;
            if (!u3f) u3f = (void *)t4_SDL_GL_GetProcAddress("glUniform3f");
            if (u3f) u3f(u_light, light[0], light[1], light[2]);
        }
        gl.BindVertexArray(vao);
        gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        gl.DrawElements(GL_TRIANGLES, (GLsizei_t)nidx, GL_UNSIGNED_SHORT, (const void *)0);
        t4_SDL_GL_SwapWindow(win);

        frame++;
        if (shot_png && !shot_done && frame == 4) {
            size_t npix = (size_t)w * (size_t)h;
            uint8_t *rgba = (uint8_t *)malloc(npix * 4);
            if (rgba && gl.ReadPixels) {
                uint8_t *flip = (uint8_t *)malloc(npix * 4);
                gl.ReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
                if (flip) {
                    int y;
                    for (y = 0; y < h; y++)
                        memcpy(flip + (size_t)y * (size_t)w * 4,
                               rgba + (size_t)(h - 1 - y) * (size_t)w * 4,
                               (size_t)w * 4);
                    if (t4_write_png(shot_png, flip, w, h) == 0)
                        printf("screenshot: %s\n", shot_png);
                    else
                        printf("screenshot: failed to write %s\n", shot_png);
                    free(flip);
                }
                free(rgba);
                shot_done = 1;
                fflush(stdout);
            }
        }
    }
    snprintf(info, info_cap, "mundo nativo: %zu verts, %zu tris - OpenGL %s%s",
             nverts, nidx / 3, verstr, es_mode ? " ES" : "");
    if (ctx) t4_SDL_GL_DeleteContext(ctx);
    if (win) t4_SDL_DestroyWindow(win);
    t4_SDL_Quit();
    t4_sdl_unload();
    return 0;

fail:
    if (ctx) t4_SDL_GL_DeleteContext(ctx);
    if (win) t4_SDL_DestroyWindow(win);
    snprintf(info, info_cap, "%s", serr);
    t4_SDL_Quit();
    t4_sdl_unload();
    return -1;
}
