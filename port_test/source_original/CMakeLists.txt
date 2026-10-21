cmake_minimum_required(VERSION 3.16)
project(t4port VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

option(T4PORT_WERROR "Tratar avisos como erros" OFF)
# T4PORT_SDL2: auto    -> linka a SDL2 se pkg-config a achar, senao dlopen
#              linked  -> exige SDL2 de desenvolvimento
#              dynamic -> nunca linka (usa dlopen na .so runtime)
set(T4PORT_SDL2 "auto" CACHE STRING "auto|linked|dynamic")

find_package(ZLIB REQUIRED)
find_package(PkgConfig QUIET)

set(T4PORT_SDL2_LINKED OFF)
if(NOT T4PORT_SDL2 STREQUAL "dynamic")
  if(T4PORT_SDL2 STREQUAL "linked")
    pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
  else()
    pkg_check_modules(SDL2 IMPORTED_TARGET sdl2)
  endif()
  if(SDL2_FOUND)
    set(T4PORT_SDL2_LINKED ON)
    message(STATUS "SDL2: linkada (${SDL2_LIBRARIES})")
  else()
    message(STATUS "SDL2: headers ausentes -> binding dinamico via dlopen")
  endif()
endif()

add_executable(t4port
  src/main.c
  src/iwd.c
  src/ff.c
  src/gsc.c
  src/iwi.c
  src/mods.c
  src/net.c
  src/platform.c
  src/png.c
  src/sdl2_dyn.c
)

target_include_directories(t4port PRIVATE src)
target_link_libraries(t4port PRIVATE ZLIB::ZLIB m ${CMAKE_DL_LIBS})

if(T4PORT_SDL2_LINKED)
  target_compile_definitions(t4port PRIVATE T4PORT_SDL2_LINKED=1)
  target_link_libraries(t4port PRIVATE PkgConfig::SDL2)
endif()

target_compile_options(t4port PRIVATE
  $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wno-unused-parameter>
)
if(T4PORT_WERROR)
  target_compile_options(t4port PRIVATE -Werror)
endif()

install(TARGETS t4port RUNTIME DESTINATION bin)
install(PROGRAMS packaging/linux/t4port.sh DESTINATION bin)
install(FILES packaging/linux/com.t4port.engine.desktop
        DESTINATION share/applications)
foreach(sz IN ITEMS 512 256 128 64 48 32)
  install(FILES packaging/linux/icons/${sz}x${sz}/com.t4port.engine.png
          DESTINATION share/icons/hicolor/${sz}x${sz}/apps)
endforeach()
