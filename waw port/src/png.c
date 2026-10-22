/*
 * png.c - encoder PNG basico: 8 bits por canal, RGBA, filtro 0 por linha,
 * um unico bloco IDAT. Suficiente para exportar texturas com qualquer visualizador.
 */
#include "png.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

static void put32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24); p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);  p[3] = (uint8_t)v;
}

static int chunk(FILE *fp, const char *type, const uint8_t *data, size_t len)
{
    uint8_t hdr[8], crcb[4];
    uint32_t crc;
    put32(hdr, (uint32_t)len);
    memcpy(hdr + 4, type, 4);
    if (fwrite(hdr, 1, 8, fp) != 8) return -1;
    if (len && fwrite(data, 1, len, fp) != len) return -1;

    crc = (uint32_t)crc32(0, Z_NULL, 0);
    crc = (uint32_t)crc32(crc, (const Bytef *)type, 4);
    if (len) crc = (uint32_t)crc32(crc, data, (uInt)len);
    put32(crcb, crc);
    if (fwrite(crcb, 1, 4, fp) != 4) return -1;
    return 0;
}

int t4_write_png(const char *path, const uint8_t *rgba, int w, int h)
{
    FILE *fp;
    static const uint8_t sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    uint8_t ihdr[13];
    uint8_t *raw = NULL, *comp = NULL;
    uLongf raw_len, comp_len;
    int y, rc = -1;

    if (w <= 0 || h <= 0) return -1;

    fp = fopen(path, "wb");
    if (!fp) return -1;
    if (fwrite(sig, 1, 8, fp) != 8) goto done;

    put32(ihdr, (uint32_t)w);
    put32(ihdr + 4, (uint32_t)h);
    ihdr[8] = 8;   /* bit depth */
    ihdr[9] = 6;   /* color type RGBA */
    ihdr[10] = 0; ihdr[11] = 0; ihdr[12] = 0;
    if (chunk(fp, "IHDR", ihdr, 13) < 0) goto done;

    raw_len = (uLong)((size_t)h * ((size_t)w * 4 + 1));
    raw = (uint8_t *)malloc(raw_len);
    if (!raw) goto done;
    for (y = 0; y < h; y++) {
        uint8_t *row = raw + (size_t)y * ((size_t)w * 4 + 1);
        row[0] = 0; /* filtro None */
        memcpy(row + 1, rgba + (size_t)y * w * 4, (size_t)w * 4);
    }

    comp_len = compressBound(raw_len);
    comp = (uint8_t *)malloc(comp_len);
    if (!comp) goto done;
    if (compress2(comp, &comp_len, raw, raw_len, 6) != Z_OK) goto done;

    if (chunk(fp, "IDAT", comp, (size_t)comp_len) < 0) goto done;
    if (chunk(fp, "IEND", NULL, 0) < 0) goto done;
    rc = 0;

done:
    free(raw);
    free(comp);
    fclose(fp);
    if (rc != 0) remove(path);
    return rc;
}
