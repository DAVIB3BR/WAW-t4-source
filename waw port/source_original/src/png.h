/*
 * png.h - escrita minima de PNG (RGBA8) usando so zlib + crc32.
 */
#ifndef T4_PNG_H
#define T4_PNG_H

#include <stdint.h>

int t4_write_png(const char *path, const uint8_t *rgba, int w, int h);

#endif
