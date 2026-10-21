#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Decodifica amostras IWI como BC3 (DXT5) e BC5 para descobrir o que
o formato 13 realmente e (DXT3/DXT5/BC5 nao se distinguem por tamanho)."""
import os, struct, sys, zipfile, zlib

HEADER = 28

def write_png(path, px, w, h):
    raw = b''.join(b'\x00' + bytes(px[y*w*4:(y+1)*w*4]) for y in range(h))
    def chunk(t, d):
        c = zlib.crc32(t + d) & 0xffffffff
        return struct.pack('>I', len(d)) + t + d + struct.pack('>I', c)
    out = b'\x89PNG\r\n\x1a\n'
    out += chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 6, 0, 0, 0))
    out += chunk(b'IDAT', zlib.compress(raw, 6))
    out += chunk(b'IEND', b'')
    open(path, 'wb').write(out)

def rgb565(c):
    return (((c >> 11) & 31) * 255 + 15) // 31, (((c >> 5) & 63) * 255 + 31) // 63, ((c & 31) * 255 + 15) // 31

def bc1_color(blk, i):
    c0, c1 = struct.unpack_from('<HH', blk, i)
    idx = struct.unpack_from('<I', blk, i + 4)[0]
    pal = [rgb565(c0), rgb565(c1)]
    if c0 > c1:
        pal.append(tuple((2*a+b)//3 for a, b in zip(pal[0], pal[1])))
        pal.append(tuple((a+2*b)//3 for a, b in zip(pal[0], pal[1])))
    else:
        pal.append(tuple((a+b)//2 for a, b in zip(pal[0], pal[1])))
        pal.append((0, 0, 0))
    return [pal[(idx >> (2*p)) & 3] for p in range(16)]

def alpha_dxt5(blk):
    a0, a1 = blk[0], blk[1]
    bits = 0
    for i in range(6):
        bits |= blk[2+i] << (8*i)
    a = [a0, a1]
    if a0 > a1:
        a += [((7-i)*a0 + i*a1)//7 for i in range(1, 7)]
    else:
        a += [((5-i)*a0 + i*a1)//5 for i in range(1, 5)] + [0, 255]
    return [a[(bits >> (3*p)) & 7] for p in range(16)]

def bc5_channels(blk):
    """Retorna (R[16], G[16]) - blocos de 3 bits por pixel para cada canal."""
    def one(base):
        v0, v1 = blk[base], blk[base+1]
        bits = 0
        for i in range(6):
            bits |= blk[base+2+i] << (8*i)
        t = [v0, v1]
        if v0 > v1:
            t += [((7-i)*v0 + i*v1)//7 for i in range(1, 7)]
        else:
            t += [((5-i)*v0 + i*v1)//5 for i in range(1, 5)] + [0, 255]
        return [t[(bits >> (3*p)) & 7] for p in range(16)]
    return one(0), one(8)

def decode(data, mode, w, h, out_png):
    payload = data[HEADER:]
    bs = 8 if mode == 'dxt1' else 16
    bw, bh = (w+3)//4, (h+3)//4
    if len(payload) < bw*bh*bs:
        print('payload curto para %s: %d < %d' % (mode, len(payload), bw*bh*bs))
        return
    px = bytearray(w*h*4)
    for by in range(bh):
        for bx in range(bw):
            off = (by*bw+bx)*bs
            blk = payload[off:off+bs]
            if mode == 'dxt1':
                cols = bc1_color(blk, 0)
                al = [c[3] if len(c) > 3 else 255 for c in cols]
                al = [255]*16
            else:
                cols = bc1_color(blk, 8)
                if mode == 'dxt5':
                    al = alpha_dxt5(blk)
                else:
                    r, g = bc5_channels(blk)
                    al = [255]*16
            for p in range(16):
                x, y = bx*4 + p % 4, by*4 + p // 4
                if x >= w or y >= h: continue
                o = (y*w+x)*4
                if mode == 'dxt5':
                    px[o], px[o+1], px[o+2], px[o+3] = cols[p][0], cols[p][1], cols[p][2], al[p]
                elif mode == 'bc5':
                    px[o], px[o+1], px[o+2], px[o+3] = r[p], g[p], 128, 255
                else:
                    px[o], px[o+1], px[o+2], px[o+3] = cols[p][0], cols[p][1], cols[p][2], 255
    write_png(out_png, px, w, h)
    print('gerou', out_png, w, 'x', h)

def find_samples(root, formats_wanted, n=1):
    main_dir = os.path.join(root, 'main')
    found = {}
    for arch in sorted(os.listdir(main_dir)):
        if not arch.endswith('.iwd'): continue
        zf = zipfile.ZipFile(os.path.join(main_dir, arch))
        for info in zf.infolist():
            if not info.filename.lower().endswith('.iwi'): continue
            with zf.open(info) as fp:
                hdr = fp.read(HEADER)
            if len(hdr) < HEADER: continue
            fmt = hdr[4]
            if fmt in formats_wanted and fmt not in found:
                w, h = struct.unpack_from('<HHH', hdr, 6)[0], struct.unpack_from('<HHH', hdr, 8)[0]
                if w < 64 or h < 64:      # amostra grande o bastante para julgar visualmente
                    continue
                found[fmt] = (arch, info.filename, w, h)
                if len(found) == len(formats_wanted):
                    zf.close()
                    return found
        zf.close()
    return found

if __name__ == '__main__':
    root = sys.argv[1]
    samples = find_samples(root, {11, 12, 13})
    for fmt, (arch, name, w, h) in sorted(samples.items()):
        zf = zipfile.ZipFile(os.path.join(root, 'main', arch))
        data = zf.read(name)
        zf.close()
        print('fmt %d: %s/%s %dx%d' % (fmt, arch, name, w, h))
        if fmt == 11:
            decode(data, 'dxt5', w, h, '/tmp/fmt%d.png' % fmt)
        else:
            decode(data, 'dxt5', w, h, '/tmp/fmt%d_as_dxt5.png' % fmt)
            decode(data, 'bc5', w, h, '/tmp/fmt%d_as_bc5.png' % fmt)
