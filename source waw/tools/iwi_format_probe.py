#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
iwi_format_probe.py - identifica o que cada codigo de formato IWI realmente e,
usando MATEMATICA de tamanho (nao suposicao).

Para cada textura, compara o payload real com o esperado por hipotese:
  - bloco de  8 bytes (BC1/DXT1)      com 0, 1 ou cadeia completa de mips
  - bloco de 16 bytes (BC2/BC3/BC5)   idem
  - 4 bytes/pixel (BGRA8)             idem
  - 2 bytes/pixel (RGB565/LA8)        idem
  - 1 byte /pixel (L8/A8)             idem

Sai uma tabela codigo -> hipotese vencedora e a taxa de acerto.

Uso:
  python3 iwi_format_probe.py /home/davi/Plutonium/pluto_t4_full_game
"""
from __future__ import print_function

import collections
import os
import struct
import sys
import zipfile

HEADER = 28

HYPOTHESES = [
    ("BC1 (DXT1), bloco 8B", lambda w, h: ((w + 3) // 4) * ((h + 3) // 4) * 8),
    ("BC2/BC3/BC5, bloco 16B", lambda w, h: ((w + 3) // 4) * ((h + 3) // 4) * 16),
    ("BGRA8, 4B/pixel", lambda w, h: w * h * 4),
    ("RGB565/LA8, 2B/pixel", lambda w, h: w * h * 2),
    ("L8/A8, 1B/pixel", lambda w, h: w * h),
]


def chain_sum(size_fn, w, h, limit=64):
    """Soma dos niveis 0..N de uma cadeia de mips completa (ate 1x1)."""
    total = 0
    cw, ch = w, h
    for _ in range(limit):
        total += size_fn(cw, ch)
        if cw == 1 and ch == 1:
            break
        cw = max(1, cw // 2)
        ch = max(1, ch // 2)
    return total


def probe(root):
    main_dir = os.path.join(root, "main")
    archives = sorted(f for f in os.listdir(main_dir) if f.endswith(".iwd"))

    # stats[fmt][hipotese] = {"no_mips": n, "full_chain": n, "total": n}
    stats = collections.defaultdict(lambda: collections.defaultdict(
        lambda: collections.Counter()))

    for arch in archives:
        zf = zipfile.ZipFile(os.path.join(main_dir, arch))
        for info in zf.infolist():
            if not info.filename.lower().endswith(".iwi"):
                continue
            with zf.open(info) as fp:
                hdr = fp.read(HEADER)
            if len(hdr) < HEADER or hdr[:3] != b"IWi":
                continue
            fmt = hdr[4]
            w, h = struct.unpack_from("<HHH", hdr, 6)[0], struct.unpack_from("<HHH", hdr, 8)[0]
            payload = info.file_size - HEADER
            st = stats[fmt]
            for name, fn in HYPOTHESES:
                st[name]["total"] += 1
                if payload == fn(w, h):
                    st[name]["no_mips"] += 1
                elif payload == chain_sum(fn, w, h):
                    st[name]["full_chain"] += 1
                elif payload > fn(w, h):
                    st[name]["entre_cadeias"] += 1
        zf.close()

    print("== hipotese vencedora por codigo de formato ==")
    print("%-6s %8s  %-26s %s" % ("fmt", "qtd", "hipotese", "acerto (sem mip / mip)"))
    for fmt in sorted(stats):
        st = stats[fmt]
        total = st[HYPOTHESES[0][0]]["total"]
        best = None
        best_score = -1
        for name, _ in HYPOTHESES:
            c = st[name]
            score = c["no_mips"] + c["full_chain"]
            if score > best_score:
                best_score = score
                best = name
        c = st[best]
        print("%-6d %8d  %-26s %d/%d sem-mip + %d/%d mip"
              % (fmt, total, best, c["no_mips"], total, c["full_chain"], total))
    print()
    print("== detalhe (todos os candidatos) ==")
    for fmt in sorted(stats):
        st = stats[fmt]
        total = st[HYPOTHESES[0][0]]["total"]
        print("fmt %d (%d arquivos):" % (fmt, total))
        for name, _ in HYPOTHESES:
            c = st[name]
            print("   %-26s sem-mip=%-6d mip-completa=%-6d payload>1nivel=%d"
                  % (name, c["no_mips"], c["full_chain"], c.get("entre_cadeias", 0)))
    return 0


if __name__ == "__main__":
    root = sys.argv[1] if len(sys.argv) > 1 else \
        os.path.expanduser("~/Plutonium/pluto_t4_full_game")
    if not os.path.isdir(os.path.join(root, "main")):
        print("diretorio nao encontrado: %s" % root, file=sys.stderr)
        sys.exit(2)
    sys.exit(probe(root))
