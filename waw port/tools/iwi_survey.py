#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
iwi_survey.py - varre TODOS os .iwi dentro de main/*.iwd e confere se o
cabecalho de 28 bytes descoberto vale para o conjunto inteiro.

Le apenas os primeiros 28 bytes de cada entrada (stream), entao e rapido mesmo
com 8,2 GB de arquivos.

Saidas:
  - histograma de formatos (11 = DXT1, 12 = DXT5, outros = ainda nao suportados)
  - contagem de divergencias entre o u32[4] de tamanho e o tamanho real
  - distribuicao de dimensoes e de mipmaps (deduzida do payload)

Uso:
  python3 iwi_survey.py /home/davi/Plutonium/pluto_t4_full_game
"""
from __future__ import print_function

import collections
import os
import struct
import sys
import zipfile

HEADER = 28
FORMAT_NAMES = {
    11: "DXT1/BC1 (8 B/bloco)",
    12: "DXT5 ou DXT3/BC2-BC3 (16 B/bloco)",
}


def block_size(fmt):
    if fmt == 11:
        return 8
    if fmt == 12:
        return 16
    return None


def mips_do_payload(payload_len, w, h, bs):
    """Deduz quantos niveis cabem no payload (base + mip chain)."""
    total = 0
    n = 0
    cw, ch = w, h
    while True:
        bw = max(1, (cw + 3) // 4)
        bh = max(1, (ch + 3) // 4)
        need = bw * bh * bs
        if total + need > payload_len:
            break
        total += need
        n += 1
        if cw == 1 and ch == 1:
            break
        cw = max(1, cw // 2)
        ch = max(1, ch // 2)
        if n > 32:
            break
    return n, total


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else \
        os.path.expanduser("~/Plutonium/pluto_t4_full_game")
    main_dir = os.path.join(root, "main")
    if not os.path.isdir(main_dir):
        print("diretorio nao encontrado: %s" % main_dir, file=sys.stderr)
        return 2

    fmt_hist = collections.Counter()
    ver_hist = collections.Counter()
    flag_hist = collections.Counter()
    dim_hist = collections.Counter()
    mip_hist = collections.Counter()
    size_mismatch = 0
    bad_magic = 0
    short_payload = 0
    total = 0
    total_bytes = 0

    archives = sorted(f for f in os.listdir(main_dir) if f.endswith(".iwd"))
    for arch in archives:
        path = os.path.join(main_dir, arch)
        try:
            zf = zipfile.ZipFile(path)
        except Exception as exc:
            print("falha abrindo %s: %s" % (arch, exc), file=sys.stderr)
            continue
        for info in zf.infolist():
            if info.filename.lower().endswith(".iwi"):
                total += 1
                total_bytes += info.file_size
                with zf.open(info) as fp:
                    hdr = fp.read(HEADER)
                if len(hdr) < HEADER or hdr[:3] != b"IWi":
                    bad_magic += 1
                    continue
                version = hdr[3]
                fmt = hdr[4]
                flags = hdr[5]
                w, h, f0a = struct.unpack_from("<HHH", hdr, 6)
                size_field = struct.unpack_from("<I", hdr, 12)[0]

                ver_hist[version] += 1
                fmt_hist[fmt] += 1
                flag_hist[flags] += 1
                dim_hist[(w, h)] += 1
                if size_field != info.file_size:
                    size_mismatch += 1

                bs = block_size(fmt)
                if bs:
                    payload = info.file_size - HEADER
                    n_mips, used = mips_do_payload(payload, w, h, bs)
                    mip_hist[n_mips] += 1
                    if used > payload:
                        short_payload += 1
        zf.close()

    print("== levantamento IWI ==")
    print("arquivos .iwd analisados : %d" % len(archives))
    print("texturas .iwi            : %d  (%.1f MB)" % (total, total_bytes / 1048576.0))
    print("magic invalido           : %d" % bad_magic)
    print("u32 de tamanho divergente: %d   (0 = cabecalho confere em 100%%)" % size_mismatch)
    print("payload curto p/ DXT     : %d" % short_payload)
    print()
    print("formatos (codigo -> qtd):")
    for fmt, n in sorted(fmt_hist.items()):
        name = FORMAT_NAMES.get(fmt, "NAO SUPORTADO AINDA")
        print("  %-4d %8d   %s" % (fmt, n, name))
    print()
    print("versoes: %s" % dict(ver_hist))
    print("flags  : %s" % dict(flag_hist))
    print()
    print("mips deduzidos do payload: %s" % dict(sorted(mip_hist.items())))
    print()
    print("dimensoes mais comuns:")
    for (w, h), n in dim_hist.most_common(12):
        print("  %5dx%-5d %8d" % (w, h, n))
    print("dimensoes distintas: %d" % len(dim_hist))
    return 0


if __name__ == "__main__":
    sys.exit(main())
