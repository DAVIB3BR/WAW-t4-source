#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""gen_tables.py -- gera src/zonetables.inc a partir das tabelas validadas
em fftool.py (RE fastfile T4, 129/129 EXACT).

Uso:  python3 re/gen_tables.py > src/zonetables.inc

A saida alimenta o walker C (src/zonewalk.c): structs/offsets, regras ANN,
conjunto RUNTIME, reorders e mapa tipo-de-asset -> struct.
"""
import sys, os, dis, collections

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import fftool

T, ANN, RUNTIME = fftool.T, fftool.ANN, fftool.RUNTIME
ORDER, ORDER_V2 = fftool.ORDER, getattr(fftool, 'ORDER_V2', {})
TYPE_STRUCT = {
    1: 'PhysPreset', 2: 'PhysConstraints', 3: 'DestructibleDef', 4: 'XAnimParts',
    5: 'XModel', 6: 'Material', 7: 'MaterialTechniqueSet', 8: 'GfxImage',
    9: 'snd_alias_list_t', 10: 'LoadedSound', 11: 'clipMap_t', 12: 'clipMap_t',
    13: 'ComWorld', 14: 'GameWorldSp', 15: 'GameWorldMp', 16: 'MapEnts',
    17: 'GfxWorld', 18: 'GfxLightDef', 19: 'UiMap', 20: 'Font_s', 21: 'MenuList',
    22: 'menuDef_t', 23: 'LocalizeEntry', 24: 'WeaponDef', 25: 'SndDriverGlobals',
    26: 'FxEffectDef', 27: 'FxImpactTable', 32: 'RawFile', 33: 'StringTable',
    34: 'PackIndex',
}

# ---------------------------------------------------------------- callables
# Lambdas em ANN (choose/dynidx/dynframes/uarr) sao inspecionadas por
# SONDAGEM: cada lambda e chamado com valores sinteticos ate revelar o
# campo/limiar que ela testa.  Assim a geracao fica livre de mapeamento
# manual e 1:1 com a fonte do fftool.py.
PROBE = [-8, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16,
         17, 18, 20, 21, 31, 32, 63, 64, 127, 128, 254, 255, 256, 257, 1000]


def s_escape(s):
    return s.replace('\\', '\\\\').replace('"', '\\"')


def _uses_s32(L):
    """A anotacao py compensa a leitura unsigned do walker com _s32?
    (so para lambdas de choose -- fonte inspecionavel em fftool.py)."""
    try:
        import inspect
        return '_s32' in inspect.getsource(L)
    except Exception:
        return False


def probe_callable(L, owner):
    """Sonda `L(vals)` contra os campos de `owner`.

    Retorna dict:
      bool-op:  {'style':'bool', 'field':f, 'op':op, 'val':v}
      keys:     {'style':'keys', 'field':f, 'map':{x:resultado}}
      const:    {'style':'const', 'value':resultado}
      fail:     {'style':'fail'}
    """
    fields = [fl['name'] for fl in (T.get(owner) or {}).get('fields', [])]
    try:
        consts = [c for c in L.__code__.co_consts if isinstance(c, str)]
    except Exception:
        consts = []
    seen_fields = set(fields)
    for c in consts:
        if c not in seen_fields:
            fields.append(c)
            seen_fields.add(c)
    base = None
    try:
        base = L({})
    except Exception:
        pass
    best = None
    for f in fields:
        seen = {}
        for x in PROBE:
            try:
                r = L({f: x})
            except Exception:
                r = '?ERR?'
            seen[x] = r
        vals = set(seen.values())
        if len(vals) <= 1:
            continue
        # varia com este campo
        best = (f, seen)
        break
    if best is None:
        if base == '?ERR?':
            return {'style': 'fail'}
        return {'style': 'const', 'value': base}
    f, seen = best
    rs = set(seen.values())
    pts = sorted(seen)
    if all(isinstance(v, bool) for v in rs):
        # limiar de corte: x<b -> True/False e x>=b -> False/True
        for b in pts:
            lo = [x for x in pts if x < b]
            hi = [x for x in pts if x >= b]
            if lo and all(seen[x] for x in lo) and not any(seen[x] for x in hi):
                return {'style': 'bool', 'field': f, 'op': 'LT', 'val': b}
            if all(not seen[x] for x in lo) and lo and \
                    all(seen[x] for x in hi):
                if b == 1:
                    return {'style': 'bool', 'field': f, 'op': 'GT', 'val': 0}
                return {'style': 'bool', 'field': f, 'op': 'GE', 'val': b}
        ts = sorted(x for x in pts if seen[x])
        if not ts:
            return {'style': 'fail'}
        rest = [x for x in pts if not seen[x]]
        if rest == [0]:
            return {'style': 'bool', 'field': f, 'op': 'GT', 'val': 0}
        return {'style': 'keys', 'field': f,
                'map': dict((x, True) for x in ts)}
    # resultados nao-booleanos: se for string/None -> seletor de membro
    if all(v is None or isinstance(v, str) for v in rs):
        mp = {}
        for x in seen:
            if seen[x] is not None:
                mp[x] = seen[x]
        return {'style': 'keys', 'field': f, 'map': mp}
    # identidade: o seletor devolve o proprio campo (chaves = valores)
    if all(isinstance(v, int) and not isinstance(v, bool) for v in rs) and \
            all(seen[x] == x for x in pts):
        return {'style': 'ident', 'field': f}
    return {'style': 'fail'}


class Out(object):
    def __init__(self):
        self.lines = []
        self.n = 0

    def emit_spec(self, spec, ctx):
        """spec -> nome C de uma T4ZSpec estatica; imprime na saida."""
        i = self.n
        self.n += 1
        fields = self.spec_fields(spec, ctx, i)
        self.lines.append('static const T4ZSpec sp%d = { %s };' % (i, fields))
        return '&sp%d' % i

    def spec_fields(self, spec, ctx, myid):
        def E(e):
            if e is None:
                return '0'
            if isinstance(e, int):
                e = str(e)
            return '"%s"' % s_escape(e)

        def S(s):
            return self.emit_spec(s, ctx) if s is not None else '0'

        def NE(n):
            if isinstance(n, int):
                return '.a=%d' % n
            return '.e2="%s"' % s_escape(n)

        if spec == 'str':
            return 'K_STR'
        if isinstance(spec, tuple):
            k = spec[0]
            if k == 'skip':
                return 'K_SKIP'
            if k == 'bytes':
                return 'K_BYTES, .e=%s' % E(spec[1])
            if k == 'u16':
                return 'K_U16, .e=%s' % E(spec[1])
            if k == 'struct':
                if isinstance(spec[1], str):
                    return 'K_STRUCT, .e=%s, %s' % (E(spec[1]), NE(spec[2] or 1))
                # array de slots de union (spec[1] = spec do membro ativo)
                return 'K_STRUCTU, .sub=%s, %s' % (S(spec[1]), NE(spec[2]))
            if k == 'struct_extra':
                return 'K_STRUCTEX, .e=%s, %s, .e2=%s' % (
                    E(spec[1]), NE(spec[2] or 1), E(spec[3]))
            if k == 'earr':
                return 'K_EARR, .e=%s, %s' % (E(spec[1]), NE(spec[2]))
            if k == 'earrptr':
                if spec[1] == 'str':
                    return 'K_EARRP, .e=%s, .e2=%s' % (E('str'), E(spec[2]))
                return 'K_EARRP, .e=%s, .e2=%s' % (E(spec[1]), E(spec[2]))
            if k == 'ptrarr':
                if isinstance(spec[1], str):
                    return 'K_PTRARR, .e=%s, .e2=%s' % (E(spec[1]), E(spec[2]))
                return 'K_PTRARR, .sub=%s, .e2=%s' % (S(spec[1]), E(spec[2]))
            if k == 'unionmember':
                return 'K_UM, .name=%s, .sub=%s' % (E(spec[1]), S(spec[2]))
            if k == 'pcount':
                return 'K_PCOUNT, .e=%s, .a=%d' % (E(spec[1]), spec[2])
            if k == 'pstructarr':
                return 'K_PSTARR, .e=%s, .e2=%s' % (E(spec[1]), E(spec[2]))
            if k == 'inline_struct':
                merge = 1 if len(spec) > 2 and spec[2] else 0
                return 'K_INLINE, .e=%s, .a=%d' % (E(spec[1]), merge)
            if k == 'uarr':
                mem, sub = spec[1], spec[2]
                if callable(mem):
                    pi = probe_callable(mem, ctx[0])
                    mem = pi['value'] if pi['style'] == 'const' else None
                if callable(sub):
                    pi = probe_callable(sub, ctx[0])
                    sub = pi['value'] if pi['style'] == 'const' else None
                if mem is None or sub is None:
                    return 'K_BAD, .name="uarr %s.%s com seletor variavel"' % ctx[:2]
                return 'K_UARR, .name=%s, .sub=%s, %s' % (E(mem), S(sub), NE(spec[3]))
            if k == 'parr2':
                n = spec[2] if len(spec) > 2 else 2
                return 'K_PARR2, .e=%s, .a=%d' % (E(spec[1]), n)
            if k == 'choose':
                pi = probe_callable(spec[1], ctx[0])
                if pi['style'] == 'bool':
                    tspec = spec[2].get(True, spec[2].get(1))
                    opmap = {'LT': 'C_LT', 'GT': 'C_GT', 'GE': 'C_GE'}
                    # lambda com _s32 (bit31 lido como signed antes de
                    # comparar) vira op *_S: o walker C le o campo unsigned
                    # (igual ao py) e refaz o cast na comparacao.
                    if _uses_s32(spec[1]):
                        opmap = dict((x, y + '_S') for x, y in opmap.items())
                    return 'K_CHOOSE, .e="%s", .a=%s, .b=%d, .sub=%s, .sub2=%s' % (
                        s_escape(pi['field']), opmap[pi['op']], pi['val'],
                        S(tspec), S(spec[3]))
                if pi['style'] == 'ident':
                    ents = []
                    for x in sorted(k for k in spec[2].keys()
                                    if isinstance(k, int)):
                        ents.append('KENT(%d, %s)' % (x, S(spec[2][x])))
                    if not ents:
                        return 'K_SKIP'
                    return ('K_CHOISK, .e="%s", .a=%d, '
                            '.ents=(const T4ZChooseEnt[]){%s}, .sub=%s' % (
                                s_escape(pi['field']), len(ents),
                                ', '.join(ents), S(spec[3])))
                if pi['style'] == 'keys':
                    ents = []
                    for x in sorted(pi['map'].keys()):
                        t = spec[2].get(pi['map'][x], spec[3])
                        if t is None:
                            continue
                        ents.append('KENT(%d, %s)' % (x, S(t)))
                    if not ents:
                        return 'K_SKIP'
                    return ('K_CHOISK, .e="%s", .a=%d, '
                            '.ents=(const T4ZChooseEnt[]){%s}, .sub=%s' % (
                                s_escape(pi['field']), len(ents),
                                ', '.join(ents), S(spec[3])))
                return 'K_BAD, .name="choose %s.%s nao mapeado"' % ctx[:2]
            if k == 'dynidx':
                pi = probe_callable(spec[1], ctx[0])
                if pi['style'] != 'bool':
                    return 'K_BAD, .name="dynidx %s.%s nao mapeado"' % ctx[:2]
                opmap = {'LT': 'C_LT', 'GT': 'C_GT', 'GE': 'C_GE'}
                return 'K_DYNIDX, .e="%s", .a=%s, .b=%d, .e2=%s' % (
                    s_escape(pi['field']), opmap[pi['op']], pi['val'],
                    E(spec[2]))
            if k == 'dynframes':
                pi = probe_callable(spec[1], ctx[0])
                if pi['style'] != 'bool':
                    return 'K_BAD, .name="dynframes %s.%s nao mapeado"' % ctx[:2]
                opmap = {'LT': 'C_LT', 'GT': 'C_GT', 'GE': 'C_GE'}
                return 'K_DYNFR, .e="%s", .a=%s, .b=%d, .e2=%s' % (
                    s_escape(pi['field']), opmap[pi['op']], pi['val'],
                    E(spec[2]))
            if k == 'inline_at':
                return 'K_INLAT, .a=%d, .e=%s' % (spec[1], E(spec[2]))
            return 'K_BAD, .name="kind %s nao mapeado"' % k
        return 'K_BAD, .name="spec vazia"'


def main():
    o = Out()
    print('/* gerado por re/gen_tables.py -- NAO EDITAR.')
    print(' * fonte: tabelas validadas do re/fftool.py (fastfile T4, 129/129 EXACT).')
    print(' */')

    # ---- specs ANN -------------------------------------------------------
    keys = sorted(ANN.keys())
    spec_names = {}
    print()
    print('/* ---- specs ANN ---- */')
    for (s, f) in keys:
        spec = ANN[(s, f)]
        name = o.emit_spec(spec, (s, f))
        spec_names[(s, f)] = name
    for line in o.lines:
        print(line)
    print()
    print('static const T4ZAnnEnt T4Z_ANN[] = {')
    for (s, f) in keys:
        print('  { "%s", "%s", %s },' % (s_escape(s), s_escape(f),
                                        spec_names[(s, f)]))
    print('};')
    print('#define T4Z_ANN_N (sizeof(T4Z_ANN)/sizeof(T4Z_ANN[0]))')

    # ---- structs ---------------------------------------------------------
    print()
    print('/* ---- structs (blobs MSVC 32 bits) ---- */')
    snames = sorted(T.keys())
    for sn in snames:
        st = T[sn]
        print('static const T4ZField f_%d[] = {' % snames.index(sn))
        for fld in st['fields']:
            cv = -1 if fld.get('countval') is None else fld['countval']
            ty = fld.get('type') or ''
            print('  { "%s", %d, %d, %d, %d, "%s" },' % (
                s_escape(fld['name']), fld.get('off') or 0, fld.get('size') or 0,
                1 if fld.get('ptr') else 0, cv, s_escape(ty)))
        if not st['fields']:
            print('  { 0, 0, 0, 0, -1, 0 },')
        print('};')
    print('static const T4ZStruct T4Z_STRUCTS[] = {')
    for idx, sn in enumerate(snames):
        st = T[sn]
        union = 1 if sn.startswith('union:') else 0
        print('  { "%s", %d, %d, f_%d, %d },' % (
            s_escape(sn), st['size'], len(st['fields']), idx, union))
    print('};')
    print('#define T4Z_STRUCT_N (sizeof(T4Z_STRUCTS)/sizeof(T4Z_STRUCTS[0]))')

    # ---- runtime / orders / type map -------------------------------------
    print()
    print('/* ---- pointees RUNTIME (sem bytes no stream) ---- */')
    print('static const T4ZPairEnt T4Z_RUNTIME[] = {')
    for (s, f) in sorted(RUNTIME):
        print('  { "%s", "%s" },' % (s_escape(s), s_escape(f)))
    print('};')
    print('#define T4Z_RUNTIME_N (sizeof(T4Z_RUNTIME)/sizeof(T4Z_RUNTIME[0]))')

    print()
    print('/* ---- reorders de pontoe (OAT SequenceReorder) ---- */')
    print('static const T4ZOrderEnt T4Z_REORD[] = {')
    for (s, _w) in sorted(ORDER_V2.keys()):
        marker, names = ORDER_V2[(s, 'reorder')]
        anchor = 1 if marker == '...' else 0
        print('  { "%s", %d, %d, (const char *const[]){ %s } },' % (
            s_escape(s), anchor, len(names),
            ', '.join('"%s"' % s_escape(n) for n in names)))
    print('};')
    print('#define T4Z_REORD_N (sizeof(T4Z_REORD)/sizeof(T4Z_REORD[0]))')

    print()
    print('static const T4ZOrderEnt T4Z_ORDER[] = {')
    for (s, _w) in sorted(ORDER.keys()):
        names = ORDER[(s, '_order')]
        print('  { "%s", 0, %d, (const char *const[]){ %s } },' % (
            s_escape(s), len(names),
            ', '.join('"%s"' % s_escape(n) for n in names)))
    print('};')
    print('#define T4Z_ORDER_N (sizeof(T4Z_ORDER)/sizeof(T4Z_ORDER[0]))')

    print()
    print('/* ---- tipo de asset -> struct ---- */')
    print('static const T4ZPairEnt T4Z_TYPESTRUCT[] = {')
    for t in sorted(TYPE_STRUCT):
        print('  { "%d", "%s" },' % (t, s_escape(TYPE_STRUCT[t])))
    print('};')
    print('#define T4Z_TYPESTRUCT_N (sizeof(T4Z_TYPESTRUCT)/sizeof(T4Z_TYPESTRUCT[0]))')


if __name__ == '__main__':
    main()
