#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
iwitool.py - leitor de headers .IWI e conversor IWI -> PNG.

Texturas .IWI do Call of Duty: World at War (motor Treyarch T4) dentro dos
pacotes .IWD (ZIP).  Reengenharia empirica: ver IWI_FORMAT.md (ao lado).

Suporta:
  fmt  1 = BGRA8888 (A8R8G8B8)          4 bytes/px
  fmt  2 = BGR888  (R8G8B8)             3 bytes/px
  fmt  3 = ARGB4444 (A4R4G4B4, u16 LE)  2 bytes/px
  fmt  4 = A8 (mascara de alfa)         1 byte/px  -> PNG grayscale
  fmt 11 = DXT1 / BC1                   8 bytes/bloco
  fmt 12 = DXT3 / BC2                  16 bytes/bloco
  fmt 13 = DXT5 / BC3                  16 bytes/bloco

NAO suporta (sai com codigo 2, sem inventar nada):
  fmt 6, 7, 8, 9  - payload de tamanho variavel, sem codec identificado
  fmt desconhecidos, cubemaps (flags & 0x04), arquivos com versao != 6
  (nao observados nesta amostra, mas recusados de qualquer forma)

Codigos de saida:
  0 = ok
  1 = erro de uso / arquivo / estrutura inesperada
  2 = formato nao suportado

Uso:
  python3 iwitool.py arquivo.iwi -o saida.png
  python3 iwitool.py arquivo.iwi --list
  python3 iwitool.py arquivo.iwi -o saida.png --mip 1   # um nivel abaixo do topo

So stdlib: argparse, struct, zlib, sys, os.
Compativel com Python 3.6 (sem f-string de debug, sem walrus).
"""

import argparse
import os
import struct
import sys
import zlib

HEADER_SIZE = 28

# ---------------------------------------------------------------------------
# Tabela de formatos (0x00 - 0x0D). Ids 11/12/13 confirmados empiricamente e
# contra o leitor IWI do projeto DevIL (src-IL/src-il/il_iwi.cpp).
# ---------------------------------------------------------------------------
FMT_BGRA8888 = 1
FMT_BGR888 = 2
FMT_ARGB4444 = 3
FMT_A8 = 4
FMT_DXT1 = 11
FMT_DXT3 = 12
FMT_DXT5 = 13

FORMAT_NAMES = {
    0: "desconhecido (nao observado)",
    1: "BGRA8888 / A8R8G8B8",
    2: "BGR888 / R8G8B8",
    3: "ARGB4444 / A4R4G4B4",
    4: "A8 (mascara de alfa)",
    5: "desconhecido (nao observado)",
    6: "desconhecido (payload de tamanho variavel)",
    7: "desconhecido (DevIL chama 0x07 de JPG; payload WaW nao e JPEG)",
    8: "desconhecido (payload de tamanho variavel)",
    9: "desconhecido (payload de tamanho variavel)",
    10: "desconhecido (nao observado)",
    11: "DXT1 / BC1",
    12: "DXT3 / BC2",
    13: "DXT5 / BC3",
}

# formatos que esta ferramenta sabe decodificar
SUPPORTED_FORMATS = (1, 2, 3, 4, 11, 12, 13)
BYTES_PER_PIXEL = {1: 4, 2: 3, 3: 2, 4: 1}       # formatos "cru" (por pixel)
BLOCK_BYTES = {11: 8, 12: 16, 13: 16}            # formatos de blocos

# bits da byte de flags (offset 0x05)
FLAG_NOMIPS_MASK = 0x03     # (flags & 3) == 3  -> sem mipmaps (1 unico nivel)
FLAG_NOMIPS_VALUE = 0x03
FLAG_CUBEMAP = 0x04         # 6 cadeias de mips (cubemap) no payload
FLAG_MEANING_HINT = "0x01=?, 0x02/0x03=sem-mipmaps, 0x04=cubemap, 0x20/0x40/0x80=?"


class IwiError(Exception):
    """Erro de estrutura do arquivo (nao de formato nao-suportado)."""


class UnsupportedFormat(Exception):
    """Formato/variante que esta ferramenta nao decodifica (exit 2)."""


# ---------------------------------------------------------------------------
# Header
# ---------------------------------------------------------------------------
def parse_header(data):
    """Converte os 28 primeiros bytes num dicionario de campos."""
    if len(data) < HEADER_SIZE:
        raise IwiError("arquivo com %d bytes, menor que o header de %d"
                       % (len(data), HEADER_SIZE))
    if data[0:3] != b"IWi":
        raise IwiError("magic invalido: %r (esperado b'IWi')" % (data[0:3],))

    version = data[3]
    fmt = data[4]
    flags = data[5]
    width, height, depth = struct.unpack_from("<HHH", data, 6)
    lump0, lump1, lump2, lump3 = struct.unpack_from("<IIII", data, 12)

    if version != 6:
        raise UnsupportedFormat(
            "versao de header %d (byte 0x03); so a v6 ('IWi\\x06') foi "
            "verificada nesta amostra" % version)
    if width == 0 or height == 0:
        raise IwiError("dimensao zerada (%dx%d)" % (width, height))

    return {
        "version": version,
        "format": fmt,
        "flags": flags,
        "width": width,
        "height": height,
        "depth": depth,
        "lumps": (lump0, lump1, lump2, lump3),
        "nomips": (flags & FLAG_NOMIPS_MASK) == FLAG_NOMIPS_VALUE,
        "cubemap": (flags & FLAG_CUBEMAP) != 0,
    }


def level_sizes(width, height, stride):
    """Tamanho em bytes de cada nivel, do MAIOR (indice 0) ao menor.

    'stride' = bytes por pixel (formatos cru) ou bytes por bloco 4x4 (DXT).
    A cadeia vai ate 1x1 (tambem chamando 2x2 -> 1x1).
    """
    sizes = []
    tw, th = width, height
    while True:
        if stride > 4:      # formato de bloco: blocos 4x4 (minimo 1)
            n = max(1, (tw + 3) // 4) * max(1, (th + 3) // 4)
        else:               # formato cru: pixels
            n = tw * th
        sizes.append(n * stride)
        if tw == 1 and th == 1:
            break
        tw = max(1, tw // 2)
        th = max(1, th // 2)
    return sizes


def mip_count(hdr):
    """Quantidade de niveis realmente gravados no arquivo."""
    if hdr["nomips"]:
        return 1
    return len(level_sizes(hdr["width"], hdr["height"], 1))


def top_level_offset(hdr, body_len):
    """Offset DENTRO do corpo (apos o header) do nivel 0 (maior resolucao).

    Os mipmaps sao gravados do MENOR para o MAIOR: o nivel top e o ultimo
    bloco do arquivo.  Para arquivos sem mipmaps e simplesmente 0.
    """
    if hdr["nomips"]:
        return 0
    sizes = level_sizes(hdr["width"], hdr["height"], _stride(hdr))
    return body_len - sizes[0]


def _stride(hdr):
    fmt = hdr["format"]
    if fmt in BLOCK_BYTES:
        return BLOCK_BYTES[fmt]
    if fmt in BYTES_PER_PIXEL:
        return BYTES_PER_PIXEL[fmt]
    raise UnsupportedFormat(
        "formato %d (%s): payload nao identificado"
        % (fmt, FORMAT_NAMES.get(fmt, "id invalido")))


def validate_layout(hdr, body_len):
    """Confere se o corpo tem exatamente o tamanho esperado pelo header."""
    if hdr["cubemap"]:
        raise UnsupportedFormat(
            "cubemap (flags 0x%02x): payload com 6 cadeias de mips; ordem "
            "das faces nao verificada - conversao nao suportada" % hdr["flags"])
    stride = _stride(hdr)          # lanca UnsupportedFormat se formato ruim
    sizes = level_sizes(hdr["width"], hdr["height"], stride)
    if hdr["nomips"]:
        sizes = sizes[:1]
    expected = sum(sizes)
    if body_len != expected:
        raise IwiError(
            "tamanho de dados incoerente: header indica %d bytes "
            "(%s %dx%d, %d nivel(is)), arquivo tem %d"
            % (expected,
               FORMAT_NAMES.get(hdr["format"], "?"),
               hdr["width"], hdr["height"], len(sizes), body_len))


# ---------------------------------------------------------------------------
# Decodificadores de bloco (BC1/BC2/BC3) - escritos a mao
# ---------------------------------------------------------------------------
def _rgb565_to_888(v):
    r = (v >> 11) & 0x1F
    g = (v >> 5) & 0x3F
    b = v & 0x1F
    # replicacao de bits: 5->8 e 6->8
    r = (r << 3) | (r >> 2)
    g = (g << 2) | (g >> 4)
    b = (b << 3) | (b >> 2)
    return r, g, b


def _lerp3(a, b):
    """(2*a + b) / 3 arredondado (paleta BC1 de 4 cores)."""
    return ((2 * a + b) // 3,)


def _bc1_color_table(c0, c1):
    """Tabela de 4 cores de um bloco BC1.

    Retorna (tabela, punch_through): quando c0 <= c1 o bloco esta no modo de
    3 cores: a cor 3 fica transparente (preta) - alpha 0.
    """
    r0, g0, b0 = _rgb565_to_888(c0)
    r1, g1, b1 = _rgb565_to_888(c1)
    t0 = (r0, g0, b0)
    t1 = (r1, g1, b1)
    if c0 > c1:
        t2 = ((2 * r0 + r1) // 3, (2 * g0 + g1) // 3, (2 * b0 + b1) // 3)
        t3 = ((r0 + 2 * r1) // 3, (g0 + 2 * g1) // 3, (b0 + 2 * b1) // 3)
        return [t0, t1, t2, t3], False
    t2 = ((r0 + r1) // 2, (g0 + g1) // 2, (b0 + b1) // 2)
    t3 = (0, 0, 0)
    return [t0, t1, t2, t3], True


def _alpha_table_bc3(a0, a1):
    """Tabela de 8 alfas do bloco BC3 (DXT5)."""
    t = [0] * 8
    t[0] = a0
    t[1] = a1
    if a0 > a1:
        for i in range(6):                      # 8 valores interpolados
            t[2 + i] = ((6 - i) * a0 + (i + 1) * a1) // 7
    else:                                       # 6 valores + 0 + 255
        for i in range(4):
            t[2 + i] = ((4 - i) * a0 + (i + 1) * a1) // 5
        t[6] = 0
        t[7] = 255
    return t


def _decode_color_block(block, out, ox, oy, width, height, force_four_color):
    """Escreve as 16 cores RGB do bloco de cor (bytes 8..15 em DXT3/5,
    bytes 0..7 em DXT1)."""
    c0, c1 = struct.unpack_from("<HH", block, 0)
    idx = struct.unpack_from("<I", block, 4)[0]
    if force_four_color:
        # BC2/BC3 sempre usam o modo de 4 cores (sem punch-through).
        # Garantimos c0 > c1 para _bc1_color_table nao entrar no modo de 3
        # cores (que usaria o alpha 0 no indice 3). Quando c0 == c1 a paleta
        # fica toda igual, entao qualquer indice produz a mesma cor.
        if c0 < c1:
            c0, c1 = c1, c0
        table, _ = _bc1_color_table(c0, c1)
        punch = False
    else:
        table, punch = _bc1_color_table(c0, c1)
    for py in range(4):
        y = oy + py
        if y >= height:
            break
        for px in range(4):
            x = ox + px
            if x >= width:
                continue
            i = (idx >> (2 * (py * 4 + px))) & 0x03
            r, g, b = table[i]
            a = 0 if (punch and i == 3) else 255
            o = (y * width + x) * 4
            out[o] = r
            out[o + 1] = g
            out[o + 2] = b
            out[o + 3] = a


def _decode_dxt1_level(data, off, width, height, out):
    p = off
    for by in range((height + 3) // 4):
        for bx in range((width + 3) // 4):
            _decode_color_block(data[p:p + 8], out, bx * 4, by * 4,
                                width, height, False)
            p += 8
    return out


def _decode_dxt3_level(data, off, width, height, out):
    p = off
    for by in range((height + 3) // 4):
        for bx in range((width + 3) // 4):
            blk = data[p:p + 16]
            p += 16
            # alpha: 16 nibbles de 4 bits, pixel i = bits (4*i)..(4*i+3)
            abits = int.from_bytes(blk[0:8], "little")
            _decode_color_block(blk[8:16], out, bx * 4, by * 4,
                                width, height, True)
            for py in range(4):
                y = by * 4 + py
                if y >= height:
                    break
                for px in range(4):
                    x = bx * 4 + px
                    if x >= width:
                        continue
                    i = py * 4 + px
                    a = ((abits >> (4 * i)) & 0x0F) * 17   # 0..15 -> 0..255
                    o = (y * width + x) * 4
                    out[o + 3] = a
    return out


def _decode_dxt5_level(data, off, width, height, out):
    p = off
    for by in range((height + 3) // 4):
        for bx in range((width + 3) // 4):
            blk = data[p:p + 16]
            p += 16
            table = _alpha_table_bc3(blk[0], blk[1])
            abits = int.from_bytes(blk[2:8], "little")
            _decode_color_block(blk[8:16], out, bx * 4, by * 4,
                                width, height, True)
            for py in range(4):
                y = by * 4 + py
                if y >= height:
                    break
                for px in range(4):
                    x = bx * 4 + px
                    if x >= width:
                        continue
                    i = py * 4 + px
                    a = table[(abits >> (3 * i)) & 0x07]
                    o = (y * width + x) * 4
                    out[o + 3] = a
    return out


# ---------------------------------------------------------------------------
# Decodificadores de formatos crus
# ---------------------------------------------------------------------------
def _decode_bgra8888(src, out):
    # bytes B,G,R,A -> RGBA
    for i in range(0, len(src), 4):
        out[i] = src[i + 2]
        out[i + 1] = src[i + 1]
        out[i + 2] = src[i]
        out[i + 3] = src[i + 3]
    return out


def _decode_bgr888(src, out):
    # bytes B,G,R -> RGBA com alfa 255
    o = 0
    for i in range(0, len(src), 3):
        out[o] = src[i + 2]
        out[o + 1] = src[i + 1]
        out[o + 2] = src[i]
        out[o + 3] = 255
        o += 4
    return out


def _decode_argb4444(src, out):
    # u16 LE: A=bits15-12, R=11-8, G=7-4, B=3-0  (validado por
    # hitech_arrow_down.iwi / gradient.iwi: so esta ordem produz a forma
    # correta do alpha)
    for i in range(0, len(src), 2):
        v = src[i] | (src[i + 1] << 8)
        a = ((v >> 12) & 0x0F) * 17
        r = ((v >> 8) & 0x0F) * 17
        g = ((v >> 4) & 0x0F) * 17
        b = (v & 0x0F) * 17
        o = (i // 2) * 4
        out[o] = r
        out[o + 1] = g
        out[o + 2] = b
        out[o + 3] = a
    return out


def _decode_a8(src):
    """Retorna (pixels, canais): mascara de alfa pura -> grayscale."""
    return bytearray(src), 1


# ---------------------------------------------------------------------------
# Decodificacao de um nivel -> RGBA
# ---------------------------------------------------------------------------
def decode_level(hdr, data, off, mip=0):
    """Decodifica um nivel para RGBA (canais=4) ou cinza (canais=1, fmt 4).

    off = offset do nivel 0; para niveis inferiores use level_data_offset().
    """
    w, h = hdr["width"], hdr["height"]
    for _ in range(mip):
        w = max(1, w // 2)
        h = max(1, h // 2)
    fmt = hdr["format"]

    if fmt == FMT_DXT1:
        out = bytearray(w * h * 4)
        _decode_dxt1_level(data, off, w, h, out)
        return out, 4
    if fmt == FMT_DXT3:
        out = bytearray(w * h * 4)
        _decode_dxt3_level(data, off, w, h, out)
        return out, 4
    if fmt == FMT_DXT5:
        out = bytearray(w * h * 4)
        _decode_dxt5_level(data, off, w, h, out)
        return out, 4
    if fmt == FMT_ARGB4444:
        sizes = level_sizes(hdr["width"], hdr["height"], 2)
        if hdr["nomips"]:
            sizes = sizes[:1]
        start = sum(sizes[:mip])
        src = data[start:start + w * h * 2]
        out = bytearray(w * h * 4)
        _decode_argb4444(src, out)
        return out, 4
    if fmt in (FMT_BGRA8888, FMT_BGR888, FMT_A8):
        bpp = BYTES_PER_PIXEL[fmt]
        sizes = level_sizes(hdr["width"], hdr["height"], bpp)
        if hdr["nomips"]:
            sizes = sizes[:1]
        start = sum(sizes[:mip])
        src = data[start:start + w * h * bpp]
        if fmt == FMT_BGRA8888:
            out = bytearray(w * h * 4)
            _decode_bgra8888(src, out)
            return out, 4
        if fmt == FMT_BGR888:
            out = bytearray(w * h * 4)
            _decode_bgr888(src, out)
            return out, 4
        return _decode_a8(src)
    raise UnsupportedFormat(
        "formato %d (%s) nao suportado" % (fmt,
                                           FORMAT_NAMES.get(fmt, "?")))


def decode_mip(hdr, data, mip=0):
    """Decodifica um nivel escolhido. data = corpo completo (pos-header).

    Retorna (pixels, canais, w, h).
    """
    stride = _stride(hdr)
    sizes = level_sizes(hdr["width"], hdr["height"], stride)
    if hdr["nomips"]:
        sizes = sizes[:1]
    if mip < 0 or mip >= len(sizes):
        raise IwiError("nivel de mipmap %d inexistente (o arquivo tem %d: "
                       "0..%d)" % (mip, len(sizes), len(sizes) - 1))
    # mips gravados do menor para o maior: nivel 0 (topo) e o ULTIMO bloco.
    # Em arquivos sem mipmaps o unico nivel ocupa o corpo inteiro (offset 0).
    if hdr["nomips"]:
        off = 0
    else:
        off = len(data) - sum(sizes[:mip + 1])
    pix, channels = decode_level(hdr, data, off, mip)
    w, h = hdr["width"], hdr["height"]
    for _ in range(mip):
        w = max(1, w // 2)
        h = max(1, h // 2)
    if len(pix) != w * h * channels:
        raise IwiError("pixels decodificados (%d) != %dx%d x %d"
                       % (len(pix), w, h, channels))
    return pix, channels, w, h


# ---------------------------------------------------------------------------
# Escritor PNG (zlib + struct; filtro 0 por linha; CRC32 via zlib.crc32)
# ---------------------------------------------------------------------------
def _png_chunk(tag, payload):
    return (struct.pack(">I", len(payload)) + tag + payload +
            struct.pack(">I", zlib.crc32(tag + payload) & 0xFFFFFFFF))


def write_png(path, width, height, pixels, channels):
    """Grava PNG 8 bits.  channels=4 (RGBA, color type 6) ou 1 (grayscale 0)."""
    if channels == 4:
        color_type = 6
    elif channels == 1:
        color_type = 0
    else:
        raise IwiError("canais invalidos para PNG: %d" % channels)
    stride = width * channels
    if len(pixels) != stride * height:
        raise IwiError("tamanho de pixels incoerente com %dx%d" % (width, height))

    ihdr = struct.pack(">IIBBBBB", width, height, 8, color_type, 0, 0, 0)

    raw = bytearray()
    for y in range(height):
        raw.append(0)                       # filtro 0 (None)
        raw += pixels[y * stride:(y + 1) * stride]
    idat = zlib.compress(bytes(raw), 9)

    png = (b"\x89PNG\r\n\x1a\n" +
           _png_chunk(b"IHDR", ihdr) +
           _png_chunk(b"IDAT", idat) +
           _png_chunk(b"IEND", b""))
    with open(path, "wb") as f:
        f.write(png)
    return len(png)


# ---------------------------------------------------------------------------
# Info / CLI
# ---------------------------------------------------------------------------
def describe(hdr, total_size):
    body_len = total_size - HEADER_SIZE
    fmt = hdr["format"]
    name = FORMAT_NAMES.get(fmt, "id invalido")
    supported = fmt in SUPPORTED_FORMATS
    lines = []
    lines.append("arquivo .......... %s" % total_size)
    lines.append("magic ............ 'IWi' + versao %d" % hdr["version"])
    lines.append("formato (0x04) ... %d = %s  [%s]"
                 % (fmt, name, "suportado" if supported else "NAO suportado"))
    lines.append("flags (0x05) ..... 0x%02x  (%s)" % (hdr["flags"],
                                                      FLAG_MEANING_HINT))
    lines.append("  sem mipmaps .... %s" % ("SIM" if hdr["nomips"] else "NAO"))
    lines.append("  cubemap ........ %s" % ("SIM (6 faces)" if hdr["cubemap"]
                                            else "NAO"))
    lines.append("dimensoes ........ %dx%d (profundidade %d)"
                 % (hdr["width"], hdr["height"], hdr["depth"]))
    lines.append("header ........... %d bytes | dados = %d bytes"
                 % (HEADER_SIZE, body_len))
    if supported and not hdr["cubemap"]:
        try:
            stride = _stride(hdr)
        except UnsupportedFormat:
            stride = None
        if stride is not None:
            sizes = level_sizes(hdr["width"], hdr["height"], stride)
            if hdr["nomips"]:
                sizes = sizes[:1]
            if body_len == sum(sizes):
                top_off = top_level_offset(hdr, body_len)
                lines.append("mipmaps ......... %d nivel(is); nivel 0 (topo) "
                             "no offset %d do arquivo"
                             % (len(sizes), HEADER_SIZE + top_off))
                lines.append("ordem dos mips .. menor -> maior (topo e o "
                             "ULTIMO bloco do arquivo)")
            else:
                lines.append("mipmaps ......... AVISO: corpo (%d) != esperado "
                             "(%d)" % (body_len, sum(sizes)))
    lines.append("tabela u32@0x0c .. %s" % (hdr["lumps"],))
    lines.append("                   = bytes a ler sem os 0/1/2/3 maiores mips")
    return "\n".join(lines)


def load(path):
    try:
        with open(path, "rb") as f:
            return f.read()
    except IOError as exc:
        raise IwiError("nao foi possivel ler %s: %s" % (path, exc))


def build_parser():
    class Parser(argparse.ArgumentParser):
        def error(self, message):
            self.print_usage(sys.stderr)
            sys.stderr.write("erro: %s\n" % message)
            sys.exit(1)

    p = Parser(
        description="Converte texturas .IWI (CoD: World at War) para PNG.",
        epilog="codigos de saido: 0=ok, 1=erro, 2=formato nao suportado")
    p.add_argument("arquivo", help="arquivo .iwi")
    p.add_argument("-o", "--output", help="PNG de saida")
    p.add_argument("--list", action="store_true", dest="list_only",
                   help="apenas imprime os campos do header")
    p.add_argument("--mip", type=int, default=0, metavar="N",
                   help="nivel de mipmap a decodificar (0 = topo/padrao)")
    return p


def main(argv):
    args = build_parser().parse_args(argv)

    if not args.list_only and not args.output:
        sys.stderr.write("erro: informe -o saida.png (ou use --list)\n")
        return 1

    try:
        data = load(args.arquivo)
        hdr = parse_header(data)
    except UnsupportedFormat as exc:
        sys.stderr.write("formato nao suportado: %s\n" % exc)
        return 2
    except IwiError as exc:
        sys.stderr.write("erro: %s\n" % exc)
        return 1

    body = data[HEADER_SIZE:]

    if args.list_only:
        print("arquivo .......... %s" % args.arquivo)
        print(describe(hdr, len(data)))
        # qualquer operacao de decodificacao com formato ruim sai com 2
        try:
            _stride(hdr)
            if hdr["cubemap"]:
                raise UnsupportedFormat(
                    "cubemap (flags 0x%02x): 6 faces, ordem nao verificada"
                    % hdr["flags"])
            if hdr["format"] not in SUPPORTED_FORMATS:
                raise UnsupportedFormat(
                    "formato %d (%s): payload nao identificado"
                    % (hdr["format"],
                       FORMAT_NAMES.get(hdr["format"], "?")))
        except UnsupportedFormat as exc:
            sys.stderr.write("formato nao suportado: %s\n" % exc)
            return 2
        return 0

    # conversao
    try:
        validate_layout(hdr, len(body))
        pixels, channels, w, h = decode_mip(hdr, body, args.mip)
        write_png(args.output, w, h, pixels, channels)
    except UnsupportedFormat as exc:
        sys.stderr.write("formato nao suportado: %s\n" % exc)
        return 2
    except IwiError as exc:
        sys.stderr.write("erro: %s\n" % exc)
        return 1

    print("%s -> %s (%dx%d, %s, mip %d)"
          % (args.arquivo, args.output, w, h,
             FORMAT_NAMES.get(hdr["format"], "?"), args.mip))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
