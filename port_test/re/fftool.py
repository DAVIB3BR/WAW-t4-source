#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""fftool.py -- Call of Duty: World at War (T4/Treyarch) fastfile tool.

Parses the "IWffu100" container format (version 387) used by .ff fastfiles
and walks the asset index/inline data stream, based on the structure and
load-rule definitions of OpenAssetTools (github.com/Laupetin/OpenAssetTools).

Subcommands:
  info    <file.ff>            header fields, asset list with names
  extract <file.ff> [-o DIR]   extract rawfiles (default: .gsc/.cfg/.txt)

Requires Python 3.6+ (standard library only).
"""
from __future__ import print_function
import argparse
import base64
import binascii
import json
import os
import struct
import sys
import zlib

_STRUCTS_JSON = zlib.decompress(base64.b64decode('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')).decode('utf-8')

"""T4 fastfile content annotations.

Derived from the game's loader behaviour as documented by OpenAssetTools'
T4 ZoneCode command files (src/ZoneCode/Game/T4/XAssets/*.txt) and the
T4 struct definitions (src/Common/Game/T4/T4_Assets.h).
"""
import json

T = json.loads(_STRUCTS_JSON)
FOLLOWING = 0xFFFFFFFF

# GfxImageLoadDef is serialized as 16 bytes (no tail padding) plus
# data[resourceSize]; structs.json size (20) includes 4 tail pad bytes.
T['GfxImageLoadDef']['size'] = 16
for _f in T['GfxImageLoadDef']['fields']:
    if _f['name'] == 'data':
        _f['size'] = 0

def sdef(name):
    t = T.get(name) or T.get('union:' + name)
    if t is None:
        raise KeyError(name)
    return t

def is_union(name):
    return ('union:' + name) in T

# --- spec constructors -----------------------------------------------------
STR = 'str'
SKIP = ('skip',)

def BYTES(n):
    return ('bytes', n)

def U16(n):
    return ('u16', n)

def ST(S, n=1):
    return ('struct', S, n)


def STX(S, n, extra):
    """struct array followed by `extra` literal bytes"""
    return ('struct_extra', S, n, extra)

def PARR(elem, n):
    return ('ptrarr', elem, n)

def EARR(S, n):
    return ('earr', S, n)

def EARRP(elem, n):
    return ('earrptr', elem, n)

def UM(name, spec):
    """union stored by value: use member `name`"""
    return ('unionmember', name, spec)


def PCOUNT(expr, elemsize):
    """pointer array: each following element's pointee is a run of
    elemsize-sized units; expr (may use `i`) gives the unit count."""
    return ('pcount', expr, elemsize)

def PSTRUCTARR(elem, expr):
    """embedded pointer array: each following element's pointee is a run of
    `elem` structs; expr (may use `i`) gives the element count."""
    return ('pstructarr', elem, expr)

def PARR2(elem, n=2):
    """union member that is an embedded array of n pointers, each pointee a
    single `elem` struct (e.g. pathnode_tree_info_t::child[2])."""
    return ('parr2', elem, n)

def CHOOSE(sel, mapping, default=None):
    return ('choose', sel, mapping, default)

def INLINE(S, merge=False):
    """process embedded struct S's pointees at the union/field position"""
    if merge:
        return ('inline_struct', S, True)
    return ('inline_struct', S)


def DYNIDX(sel, n):
    """inline variable-length u8/u16 index array (replaces struct tail)"""
    return ('dynidx', sel, n)


def DYNFRAMES(sel, n):
    """union pointer selecting a u8 or u16 vector of n elements"""
    return ('dynframes', sel, n)

# --- pointee order overrides ----------------------------------------------
ORDER = {
    ('XAnimParts', '_order'): [
        'name', 'names', 'notify', 'deltaPart', 'dataByte', 'dataShort', 'dataInt',
        'randomDataShort', 'randomDataByte', 'randomDataInt', 'indices'],
    ('GfxImage', '_order'): ['name', 'texture'],
    ('XAnimPartTransFrames', '_order'): ['indices', 'frames'],
    ('XAnimDeltaPartQuatDataFrames', '_order'): ['indices', 'frames'],
    ('MaterialTechnique', '_order'): ['passArray', 'name'],
    ('XSurface', '_order'): [
        'zoneHandle', 'vertInfo', 'verts0', 'vertList', 'triIndices'],
    ('XSurfaceVertexInfo', '_order'): ['vertsBlend'],
}

ANN = {}

# struct-level pointee order rewrites following OAT SequenceReorder:
# (first_listed_member, [listed members]) -- with the "..." form the listed
# members are loaded LAST, in listed order.
ORDER_V2 = {}

def reorder(s, first, names):
    ORDER_V2[(s, 'reorder')] = (first, list(names))


def reorder_end(s, anchor, names):
    """SequenceReorder with leading "...": `anchor` stays at its declaration
    position and the other listed members load right after it."""
    ORDER_V2[(s, 'reorder')] = ('...', [anchor] + list(names))


def ann(s, f, spec):
    ANN[(s, f)] = spec


# --- XFile runtime-block members -------------------------------------------
# ZoneCode `set block <member> XFILE_BLOCK_RUNTIME;` marks the pointee as
# belonging to an XFile "runtime" block (RUNTIME / LARGE_RUNTIME /
# PHYSICAL_RUNTIME).  OpenAssetTools' writer emits NO bytes for those
# (ZoneOutputStream::WriteDataInBlock) and the loader zeroes them without
# reading the stream (ZoneInputStream::LoadDataFromBlock), so the pointee
# of such a member consumes 0 stream bytes even when the stored pointer
# is 0xFFFFFFFF (FOLLOWING).  Only these T4 members are marked runtime.
RUNTIME = set([
    ('GfxWorld', 'reflectionProbeTextures'),
    ('GfxWorld', 'lightmapPrimaryTextures'),
    ('GfxWorld', 'lightmapSecondaryTextures'),
    ('GfxWorld', 'cellCasterBits'),
    ('GfxWorld', 'sceneDynModel'),
    ('GfxWorld', 'sceneDynBrush'),
    ('GfxWorld', 'primaryLightEntityShadowVis'),
    ('GfxWorld', 'primaryLightDynEntShadowVis'),
    ('GfxWorld', 'nonSunPrimaryLightForModelDynEnt'),
    ('GfxWorldDpvsPlanes', 'sceneEntCellBits'),
    ('GfxWorldDpvsStatic', 'smodelVisData'),
    ('GfxWorldDpvsStatic', 'surfaceVisData'),
    ('GfxWorldDpvsStatic', 'lodData'),
    ('GfxWorldDpvsStatic', 'surfaceMaterials'),
    ('GfxWorldDpvsStatic', 'surfaceCastsSunShadow'),
    ('GfxWorldDpvsDynamic', 'dynEntCellBits'),
    ('GfxWorldDpvsDynamic', 'dynEntVisData'),
    ('PathData', 'basenodes'),
    ('clipMap_t', 'dynEntPoseList'),
    ('clipMap_t', 'dynEntClientList'),
    ('clipMap_t', 'dynEntServerList'),
    ('clipMap_t', 'dynEntCollList'),
])


def _fill():
    # ---------------- rawfile
    ann('RawFile', 'name', STR)
    ann('RawFile', 'buffer', BYTES('len + 1'))
    # ---------------- localize
    ann('LocalizeEntry', 'value', STR)
    ann('LocalizeEntry', 'name', STR)
    # ---------------- stringtable
    ann('StringTable', 'name', STR)
    ann('StringTable', 'values', PARR(STR, 'columnCount * rowCount'))
    # ---------------- menulist / menu / items
    ann('MenuList', 'name', STR)
    ann('MenuList', 'menus', PARR('menuDef_t', 'menuCount'))
    ann('menuDef_t', 'font', STR)
    ann('menuDef_t', 'onOpen', STR)
    ann('menuDef_t', 'onFocus', STR)
    ann('menuDef_t', 'onClose', STR)
    ann('menuDef_t', 'onESC', STR)
    ann('menuDef_t', 'onKey', ST('ItemKeyHandler'))
    ann('menuDef_t', 'allowedBinding', STR)
    ann('menuDef_t', 'soundName', STR)
    ann('menuDef_t', 'items', PARR('itemDef_s', 'itemCount'))
    ann('windowDef_t', 'name', STR)
    ann('windowDef_t', 'group', STR)
    ann('ItemKeyHandler', 'action', STR)
    ann('ItemKeyHandler', 'next', ST('ItemKeyHandler'))
    ann('statement_s', 'entries', PARR('expressionEntry', 'numEntries'))
    # expressionEntry.data: union -> Operand when type == EET_OPERAND (0)
    ann('expressionEntry', 'data', CHOOSE(
        lambda v: v.get('type') == 1,          # EET_OPERAND
        {True: INLINE('Operand')}, SKIP))
    ann('Operand', 'internals', CHOOSE(
        lambda v: v.get('dataType') == 2,      # VAL_STRING
        {True: UM('stringVal', STR)}, SKIP))
    ann('itemDef_s', 'text', STR)
    ann('itemDef_s', 'parent', SKIP)        # set condition parent never
    ann('itemDef_s', 'mouseEnterText', STR)
    ann('itemDef_s', 'mouseExitText', STR)
    ann('itemDef_s', 'mouseEnter', STR)
    ann('itemDef_s', 'mouseExit', STR)
    ann('itemDef_s', 'action', STR)
    ann('itemDef_s', 'onAccept', STR)
    ann('itemDef_s', 'onFocus', STR)
    ann('itemDef_s', 'leaveFocus', STR)
    ann('itemDef_s', 'dvar', STR)
    ann('itemDef_s', 'dvarTest', STR)
    ann('itemDef_s', 'onListboxSelectionChange', STR)
    ann('itemDef_s', 'onKey', ST('ItemKeyHandler'))
    ann('itemDef_s', 'enableDvar', STR)
    # itemDefData_t union, active member selected by itemDef_s::type
    # (OAT menuDef_t.txt): editField for TEXT/EDITFIELD/NUMERICFIELD/SLIDER/
    # YESNO/BIND/VALIDFILEFIELD/DECIMALFIELD/UPREDITFIELD, listBox for
    # LISTBOX, multi for MULTI, enumDvarName for DVARENUM, data never.
    _edit = UM('editField', ST('editFieldDef_s'))
    ann('itemDef_s', 'typeData', CHOOSE(
        lambda v: v.get('type'),
        {0: _edit, 4: _edit, 9: _edit, 10: _edit, 11: _edit,
         14: _edit, 16: _edit, 17: _edit, 18: _edit,
         6: UM('listBox', ST('listBoxDef_s')),
         12: UM('multi', ST('multiDef_s')),
         13: UM('enumDvarName', STR)}, SKIP))
    ann('listBoxDef_s', 'onDoubleClick', STR)
    ann('multiDef_s', 'dvarList', EARRP(STR, 32))
    ann('multiDef_s', 'dvarStr', EARRP(STR, 32))
    # ---------------- font
    ann('Font_s', 'fontName', STR)
    ann('Font_s', 'glyphs', ST('Glyph', 'glyphCount'))
    # ---------------- packindex
    ann('PackIndex', 'name', STR)
    ann('PackIndex', 'entries', ST('PackIndexEntry', 'header::count'))
    # ---------------- misc singletons
    ann('SndDriverGlobals', 'name', STR)
    ann('GfxLightDef', 'name', STR)
    ann('MapEnts', 'name', STR)
    ann('MapEnts', 'entityString', BYTES('numEntityChars'))
    # ---------------- physics
    ann('PhysPreset', 'name', STR)
    ann('PhysPreset', 'sndAliasPrefix', STR)
    ann('PhysConstraints', 'name', STR)
    ann('PhysConstraints', 'data', EARR('PhysConstraint', 16))
    ann('PhysConstraint', 'target_bone1', STR)
    ann('PhysConstraint', 'target_bone2', STR)
    # ---------------- destructibles
    ann('DestructibleDef', 'name', STR)
    ann('DestructibleDef', 'pieces', ST('DestructiblePiece', 'numPieces'))
    ann('DestructiblePiece', 'attachBone', STR)
    ann('DestructiblePiece', 'dmodels', EARR('DestructibleModel', 5))
    ann('DestructiblePiece', 'damageSound', UM('name', ST('snd_alias_list_name')))
    ann('DestructiblePiece', 'burnSound', UM('name', ST('snd_alias_list_name')))
    ann('DestructibleModel', 'breakNotify', STR)
    ann('DestructibleModel', 'loopSound', STR)
    ann('DestructibleModel', 'breakSound', UM('name', ST('snd_alias_list_name')))
    ann('DestructibleModel', 'spawnModel', EARRP('XModel', 3))
    ann('snd_alias_list_name', 'soundName', STR)
    # ---------------- xanim
    ann('XAnimParts', 'name', STR)
    ann('XAnimParts', 'names', U16('boneCount[9]'))
    ann('XAnimParts', 'dataByte', BYTES('dataByteCount'))
    ann('XAnimParts', 'dataShort', BYTES('2 * dataShortCount'))
    ann('XAnimParts', 'dataInt', BYTES('4 * dataIntCount'))
    ann('XAnimParts', 'randomDataShort', BYTES('2 * randomDataShortCount'))
    ann('XAnimParts', 'randomDataByte', BYTES('randomDataByteCount'))
    ann('XAnimParts', 'randomDataInt', BYTES('4 * randomDataIntCount'))
    ann('XAnimParts', 'indices', CHOOSE(
        lambda v: v.get('numframes', 0) < 256,
        {True: BYTES('indexCount'), False: BYTES('2 * indexCount')}, BYTES(0)))
    ann('XAnimParts', 'notify', ST('XAnimNotifyInfo', 'notifyCount'))
    ann('XAnimParts', 'deltaPart', ST('XAnimDeltaPart', 1))
    ann('XAnimDeltaPart', 'trans', ST('XAnimPartTrans', 1))
    ann('XAnimDeltaPart', 'quat', ST('XAnimDeltaPartQuat', 1))
    # XAnimPartTrans.u / XAnimDeltaPartQuat.u: union by value with either a
    # "frames" sub-struct (size > 0) or a single inline frame (size == 0)
    ann('XAnimPartTrans', 'u', CHOOSE(
        lambda v: v.get('size', 0) > 0,
        {True: INLINE('XAnimPartTransFrames', True)},
        ('inline_at', 4, 12)))
    ann('XAnimPartTransFrames', 'indices', DYNIDX(
        lambda v: v.get('numframes', 0) < 256, 'size + 1'))
    ann('XAnimPartTransFrames', 'frames', DYNFRAMES(
        lambda v: bool(v.get('smallTrans')), '3 * (size + 1)'))
    ann('XAnimDeltaPartQuat', 'u', CHOOSE(
        lambda v: v.get('size', 0) > 0,
        {True: INLINE('XAnimDeltaPartQuatDataFrames', True)},
        ('inline_at', 4, 4)))
    ann('XAnimDeltaPartQuatDataFrames', 'indices', DYNIDX(
        lambda v: v.get('numframes', 0) < 256, 'size + 1'))
    ann('XAnimDeltaPartQuatDataFrames', 'frames', ST('XQuat2', 'size + 1'))
    # ---------------- xmodel
    ann('XModel', 'name', STR)
    ann('XModel', 'boneNames', U16('numBones'))
    ann('XModel', 'parentList', BYTES('numBones - numRootBones'))
    ann('XModel', 'quats', ST('XModelQuat', 'numBones - numRootBones'))
    # OAT: set count trans (numBones - numRootBones) * 4; element = float
    ann('XModel', 'trans', BYTES('16 * (numBones - numRootBones)'))
    ann('XModel', 'partClassification', BYTES('numBones'))
    ann('XModel', 'baseMat', ST('DObjAnimMat', 'numBones'))
    ann('XModel', 'surfs', ST('XSurface', 'numsurfs'))
    ann('XModel', 'materialHandles', PARR('Material', 'numsurfs'))
    ann('XModel', 'collSurfs', ST('XModelCollSurf_s', 'numCollSurfs'))
    ann('XModel', 'boneInfo', ST('XBoneInfo', 'numBones'))
    ann('XModel', 'physGeoms', ST('PhysGeomList', 1))
    ann('XModel', 'collmap', ST('PhysGeomList', 1))
    ann('PhysGeomList', 'geoms', ST('PhysGeomInfo', 'count'))
    ann('PhysGeomInfo', 'brush', ST('BrushWrapper', 1))
    ann('BrushWrapper', 'sides', ST('cbrushside_t', 'numsides'))
    ann('BrushWrapper', 'baseAdjacentSide', BYTES('totalEdgeCount'))
    ann('BrushWrapper', 'verts', BYTES('12 * numverts'))
    ann('BrushWrapper', 'planes', ST('cplane_s', 'numsides'))
    ann('cbrushside_t', 'plane', ST('cplane_s', 1))
    ann('XSurface', 'triIndices', BYTES('6 * triCount'))
    # OAT: set condition vb0 never; set condition indexBuffer never;
    ann('XSurface', 'vb0', SKIP)
    ann('XSurface', 'indexBuffer', SKIP)
    ann('XSurface', 'verts0', ST('GfxPackedVertex', 'vertCount'))
    ann('XSurface', 'vertList', ST('XRigidVertList', 'vertListCount'))
    ann('XSurfaceVertexInfo', 'vertsBlend', BYTES(
        '2 * (vertCount[0] + 3 * vertCount[1] + 5 * vertCount[2] + 7 * vertCount[3])'))
    ann('XRigidVertList', 'collisionTree', ST('XSurfaceCollisionTree', 1))
    ann('XSurfaceCollisionTree', 'nodes', ST('XSurfaceCollisionNode', 'nodeCount'))
    ann('XSurfaceCollisionTree', 'leafs', ST('XSurfaceCollisionLeaf', 'leafCount'))
    ann('XModelCollSurf_s', 'collTris', ST('XModelCollTri_s', 'numCollTris'))
    # ---------------- material
    ann('MaterialInfo', 'name', STR)
    ann('Material', 'textureTable', ST('MaterialTextureDef', 'textureCount'))
    ann('Material', 'constantTable', ST('MaterialConstantDef', 'constantCount'))
    ann('Material', 'stateBitsTable', ST('GfxStateBits', 'stateBitsCount'))
    ann('MaterialTextureDef', 'u', CHOOSE(
        lambda v: v.get('semantic') == 20,
        {True: UM('water', ST('water_t', 1))}, UM('image', ST('GfxImage', 1))))
    ann('water_t', 'H0', BYTES('8 * M * N'))
    ann('water_t', 'wTerm', BYTES('4 * M * N'))
    ann('water_t', 'image', ST('GfxImage', 1))
    # ---------------- techset
    ann('MaterialTechniqueSet', 'name', STR)
    ann('MaterialTechniqueSet', 'techniques', EARRP('MaterialTechnique', 59))
    ann('MaterialTechnique', 'passArray', EARR('MaterialPass', 'passCount'))
    ann('MaterialTechnique', 'name', STR)
    ann('MaterialPass', 'vertexShader', ST('MaterialVertexShader', 1))
    ann('MaterialPass', 'pixelShader', ST('MaterialPixelShader', 1))
    ann('MaterialPass', 'args', ST('MaterialShaderArgument',
                                   'perPrimArgCount + perObjArgCount + stableArgCount'))
    # u::literalConst is a pointer (const float (*)[4]) active when
    # type == MTL_ARG_LITERAL_VERTEX_CONST (1) or MTL_ARG_LITERAL_PIXEL_CONST (7);
    # all other types are by-value members that never follow inline.
    ann('MaterialShaderArgument', 'u', CHOOSE(
        lambda v: v.get('type') in (1, 7),
        {True: UM('literalConst', BYTES(16))}, SKIP))
    ann('MaterialVertexShader', 'name', STR)
    ann('MaterialPixelShader', 'name', STR)
    ann('MaterialVertexShaderProgram', 'vs', SKIP)
    ann('MaterialPixelShaderProgram', 'ps', SKIP)
    ann('MaterialVertexStreamRouting', 'decl', SKIP)
    ann('GfxVertexShaderLoadDef', 'program', BYTES('4 * programSize'))
    ann('GfxPixelShaderLoadDef', 'program', BYTES('4 * programSize'))
    # ---------------- image
    ann('GfxImage', 'name', STR)
    ann('GfxImage', 'texture', UM('loadDef', ST('GfxImageLoadDef', 1)))
    ann('GfxImageLoadDef', 'data', BYTES('resourceSize'))
    # ---------------- sound
    ann('snd_alias_list_t', 'aliasName', STR)
    ann('snd_alias_list_t', 'head', ST('snd_alias_t', 'count'))
    ann('snd_alias_t', 'aliasName', STR)
    ann('snd_alias_t', 'subtitle', STR)
    ann('snd_alias_t', 'secondaryAliasName', STR)
    ann('snd_alias_t', 'chainAliasName', STR)
    ann('snd_alias_t', 'soundFile', ST('SoundFile', 1))
    ann('SoundFile', 'u', CHOOSE(
        lambda v: v.get('type') == 1,
        {True: UM('loadSnd', ST('LoadedSound', 1))},
        UM('streamSnd', ST('StreamedSound', 1))))
    ann('LoadedSound', 'name', STR)
    ann('LoadedSound', 'sound', UM('data', BYTES('data_size')))
    # snd_asset is a plain struct embedded by value; its `data` field is a
    # raw byte buffer of data_size bytes (NOT a NUL-terminated string).
    ann('snd_asset', 'data', BYTES('data_size'))
    ann('StreamedSound', 'primeSnd', ST('PrimedSound', 1))
    ann('StreamFileName', 'dir', STR)
    ann('StreamFileName', 'name', STR)
    ann('PrimedSound', 'name', STR)
    ann('PrimedSound', 'buffer', BYTES('size'))
    # ---------------- fx
    ann('FxEffectDef', 'name', STR)
    ann('FxEffectDef', 'elemDefs', ST(
        'FxElemDef',
        'elemDefCountEmission + elemDefCountLooping + elemDefCountOneShot'))
    ann('FxElemDef', 'velSamples', ST('FxElemVelStateSample', 'velIntervalCount + 1'))
    ann('FxElemDef', 'visSamples', ST('FxElemVisStateSample',
                                      'visStateIntervalCount + 1'))
    ann('FxElemDef', 'trailDef', ST('FxTrailDef', 1))
    ann('FxElemDef', 'effectOnImpact', UM('name', STR))
    ann('FxElemDef', 'effectOnDeath', UM('name', STR))
    ann('FxElemDef', 'effectEmitted', UM('name', STR))
    # visuals union: markArray (decal), array (visualCount > 1), instance
    def _fx_member(v):
        return {0: 'material', 1: 'material', 2: 'material', 3: 'material',
                4: 'material', 5: 'material', 6: 'model', 7: None, 8: None,
                9: 'soundName', 11: 'effectDef'}.get(v.get('elemType'))
    fx_sub = {'material': ST('Material', 1), 'model': ST('XModel', 1),
              'soundName': STR, 'effectDef': UM('name', STR), None: SKIP}
    ann('FxElemDef', 'visuals', CHOOSE(
        lambda v: v.get('elemType') == 10,
        {True: UM('markArray', ST('FxElemMarkVisuals', 'visualCount'))},
        CHOOSE(lambda v: (v.get('visualCount') or 0) > 1,
               {True: UM('array', ('uarr', _fx_member,
                                   lambda v: fx_sub[_fx_member(v)],
                                   'visualCount'))},
               UM('instance', CHOOSE(_fx_member, fx_sub, SKIP)))))
    # FxElemVisuals member by elemType
    ann('FxElemVisuals', 'model', ST('XModel', 1))
    ann('FxElemVisuals', 'effectDef', UM('name', STR))
    ann('FxElemVisuals', 'soundName', STR)
    ann('FxElemVisuals', 'material', ST('Material', 1))
    ann('FxElemVisuals', 'anonymous', SKIP)
    ann('FxTrailDef', 'verts', ST('FxTrailVertex', 'vertCount'))
    ann('FxTrailDef', 'inds', BYTES('2 * indCount'))
    ann('FxImpactTable', 'name', STR)
    # FxImpactTable::table is a POINTER to the 16-entry array (not embedded)
    ann('FxImpactTable', 'table', ST('FxImpactEntry', 16))
    ann('FxImpactEntry', 'nonflesh', EARRP('FxEffectDef', 31))
    ann('FxImpactEntry', 'flesh', EARRP('FxEffectDef', 4))
    # ---------------- weapon
    ann('WeaponDef', 'szInternalName', STR)
    ann('WeaponDef', 'szDisplayName', STR)
    ann('WeaponDef', 'szOverlayName', STR)
    ann('WeaponDef', 'szModeName', STR)
    ann('WeaponDef', 'standMountedWeapdef', STR)
    ann('WeaponDef', 'crouchMountedWeapdef', STR)
    ann('WeaponDef', 'proneMountedWeapdef', STR)
    ann('WeaponDef', 'bounceSound', ('uarr', 'name', ST('snd_alias_list_name'), 31))
    ann('WeaponDef', 'szAmmoName', STR)
    ann('WeaponDef', 'szClipName', STR)
    ann('WeaponDef', 'szSharedAmmoCapName', STR)
    ann('WeaponDef', 'szAltWeaponName', STR)
    ann('WeaponDef', 'szUseHintString', STR)
    ann('WeaponDef', 'dropHintString', STR)
    ann('WeaponDef', 'szScript', STR)
    ann('WeaponDef', 'fireRumble', STR)
    ann('WeaponDef', 'meleeImpactRumble', STR)
    ann('WeaponDef', 'flameTableFirstPerson', STR)
    ann('WeaponDef', 'flameTableThirdPerson', STR)
    ann('WeaponDef', 'flameTableFirstPersonPtr', ST('FlameTable', 1))
    ann('WeaponDef', 'flameTableThirdPersonPtr', ST('FlameTable', 1))
    ann('FlameTable', 'name', STR)
    # SndAliasCustom: the `sound` member is never serialized (OAT:
    # "set condition sound never"), so the `name` (snd_alias_list_name*)
    # pointee always follows inline when the slot is a following pointer
    for _sn in ('flameOffLoopSound', 'flameIgniteSound', 'flameOnLoopSound',
                'flameCooldownSound'):
        ann('FlameTable', _sn, UM('name', ST('snd_alias_list_name')))
    ann('WeaponDef', 'aiVsAiAccuracyGraphName', STR)
    ann('WeaponDef', 'aiVsPlayerAccuracyGraphName', STR)
    ann('WeaponDef', 'aiVsAiAccuracyGraphKnots',
        BYTES('8 * aiVsAiAccuracyGraphKnotCount'))
    ann('WeaponDef', 'aiVsPlayerAccuracyGraphKnots',
        BYTES('8 * aiVsPlayerAccuracyGraphKnotCount'))
    ann('WeaponDef', 'originalAiVsAiAccuracyGraphKnots',
        BYTES('8 * originalAiVsAiAccuracyGraphKnotCount'))
    ann('WeaponDef', 'originalAiVsPlayerAccuracyGraphKnots',
        BYTES('8 * originalAiVsPlayerAccuracyGraphKnotCount'))
    # every SndAliasCustom field in WeaponDef uses the `name` union member
    import json as _json
    _wd = _json.loads(_STRUCTS_JSON)['WeaponDef']['fields']
    for _f in _wd:
        if _f['type'] == 'SndAliasCustom':
            ann('WeaponDef', _f['name'], UM('name', ST('snd_alias_list_name')))
    # WeaponDef pointee order: declaration order except the accuracy graph
    # block, which the game reads in a different order
    _graph = ['aiVsAiAccuracyGraphName', 'aiVsAiAccuracyGraphKnots',
              'originalAiVsAiAccuracyGraphKnots', 'aiVsPlayerAccuracyGraphName',
              'aiVsPlayerAccuracyGraphKnots',
              'originalAiVsPlayerAccuracyGraphKnots']
    _order = []
    _done_graph = False
    for _f in _wd:
        if _f['name'] in _graph:
            if not _done_graph:
                _order.extend(_graph)
                _done_graph = True
            continue
        _order.append(_f['name'])
    ORDER[('WeaponDef', '_order')] = _order


# ---------------- comworld (pointer arrays whose count is a sibling field)
ann('ComWorld', 'primaryLights', ST('ComPrimaryLight', 'primaryLightCount'))
ann('ComWorld', 'waterCells', ST('ComWaterCell', 'numWaterCells'))
ann('ComWorld', 'burnableCells', ST('ComBurnableCell', 'numBurnableCells'))
# OAT ZoneCode: "set count data 32" -> 32 one-byte samples per cell
ann('ComBurnableCell', 'data', ST('ComBurnableSample', 32))

# ---------------- clipmap / gameworld: unions whose active member is chosen
# by a sibling count field (OAT: "set condition data::leaf leafBrushCount > 0")
ann('cLeafBrushNode_s', 'data', CHOOSE(
    lambda v: (v.get('leafBrushCount') or 0) > 0,
    {True: INLINE('cLeafBrushNodeLeaf_t')},
    INLINE('cLeafBrushNodeChildren_t')))
ann('cLeafBrushNodeLeaf_t', 'brushes', U16('leafBrushCount'))

# pathnode_tree_t (GameWorldSp): "set condition u::child axis >= 0"
def _s32(v):
    """interpret an unsigned 4-byte read as a signed 32-bit value"""
    if v is None:
        return None
    return v - (1 << 32) if v & 0x80000000 else v

# pathnode_tree_t (GameWorldSp): "set condition u::child axis >= 0" --
# axis is stored as a signed int but the walker reads it unsigned
ann('pathnode_tree_t', 'u', CHOOSE(
    lambda v: (_s32(v.get('axis')) if v.get('axis') is not None else -1) >= 0,
    {True: UM('child', PARR2('pathnode_tree_t', 2))},
    INLINE('pathnode_tree_nodes_t')))
ann('pathnode_tree_nodes_t', 'nodes', U16('nodeCount'))

# pathnode_t (GameWorldSp.txt): "set condition pathnode_t::transient never"
# -> the embedded transient block's three pointer members never have a
# pointee stored; "set condition pathnode_dynamic_t::pOwner never".
# pathnode_constant_t: "set count Links totalLinkCount".
ann('pathnode_constant_t', 'Links', ST('pathlink_s', 'totalLinkCount'))
ann('pathnode_dynamic_t', 'pOwner', SKIP)
for _n in ('pNextOpen', 'pPrevOpen', 'pParent'):
    ann('pathnode_transient_t', _n, SKIP)

# ---------------- clipmap (clipMap_t.txt): every count comes from an
# explicit `set count`; vec3_t is a union and cbrushedge_t/LeafBrush are
# typedefs of char/unsigned short, so those pointees are raw byte runs.
ann('clipMap_t', 'name', STR)
ann('clipMap_t', 'planes', ST('cplane_s', 'planeCount'))
ann('clipMap_t', 'staticModelList', ST('cStaticModel_s', 'numStaticModels'))
ann('clipMap_t', 'materials', ST('dmaterial_t', 'numMaterials'))
ann('clipMap_t', 'brushsides', ST('cbrushside_t', 'numBrushSides'))
ann('clipMap_t', 'brushEdges', BYTES('numBrushEdges'))
ann('clipMap_t', 'nodes', ST('cNode_t', 'numNodes'))
ann('clipMap_t', 'leafs', ST('cLeaf_t', 'numLeafs'))
ann('clipMap_t', 'leafbrushNodes', ST('cLeafBrushNode_s', 'leafbrushNodesCount'))
ann('clipMap_t', 'leafbrushes', U16('numLeafBrushes'))
ann('clipMap_t', 'leafsurfaces', BYTES('4 * numLeafSurfaces'))
ann('clipMap_t', 'verts', BYTES('12 * vertCount'))
ann('clipMap_t', 'brushVerts', BYTES('12 * numBrushVerts'))
ann('clipMap_t', 'uinds', U16('nuinds'))
ann('clipMap_t', 'triIndices', U16('3 * triCount'))
ann('clipMap_t', 'triEdgeIsWalkable', BYTES('((3 * triCount + 31) // 32) * 4'))
ann('clipMap_t', 'borders', ST('CollisionBorder', 'borderCount'))
ann('clipMap_t', 'partitions', ST('CollisionPartition', 'partitionCount'))
ann('clipMap_t', 'aabbTrees', ST('CollisionAabbTree', 'aabbTreeCount'))
ann('clipMap_t', 'cmodels', ST('cmodel_t', 'numSubModels'))
ann('clipMap_t', 'brushes', ST('cbrush_t', 'numBrushes'))
ann('clipMap_t', 'visibility', BYTES('numClusters * clusterBytes'))
ann('clipMap_t', 'mapEnts', ST('MapEnts', 1))           # set reusable mapEnts
ann('clipMap_t', 'box_brush', ST('cbrush_t', 1))        # set reusable box_brush
ann('clipMap_t', 'dynEntDefList',
    PSTRUCTARR('DynEntityDef', 'dynEntCount[i]'))
ann('clipMap_t', 'constraints', ST('PhysConstraint', 'num_constraints'))
# cbrush_t: OAT marks sides/baseAdjacentSide/verts `set reusable` only, so
# each is a SINGLE pointer (one element) -- the real bulk data lives in the
# clipMap_t arrays above and shows up here as aliases.
ann('cbrush_t', 'sides', ST('cbrushside_t', 1))
ann('cbrush_t', 'baseAdjacentSide', BYTES(1))
ann('cbrush_t', 'verts', BYTES(12))
ann('CollisionPartition', 'borders', ST('CollisionBorder', 1))
ann('XModelPieces', 'name', STR)
ann('XModelPieces', 'pieces', ST('XModelPiece', 'numpieces'))
# MaterialTechniqueSet.txt: "set condition remappedTechniqueSet never"
ann('MaterialTechniqueSet', 'remappedTechniqueSet', SKIP)

# PathData (GameWorldSp.txt): "set count nodes nodeCount",
# "set count chainNodeForNode nodeCount",
# "set count nodeForChainNode nodeCount", "set count pathVis visBytes",
# "set count nodeTree nodeTreeCount" (basenodes is runtime -> RUNTIME set)
ann('PathData', 'nodes', ST('pathnode_t', 'nodeCount'))
ann('PathData', 'chainNodeForNode', U16('nodeCount'))
ann('PathData', 'nodeForChainNode', U16('nodeCount'))
ann('PathData', 'pathVis', BYTES('visBytes'))
ann('PathData', 'nodeTree', ST('pathnode_tree_t', 'nodeTreeCount'))

# ---------------- gfxworld (map assets)
# OAT ZoneCode: src/ZoneCode/Game/T4/XAssets/GfxWorld.txt.
# NOTE: members marked "set block <member> XFILE_BLOCK_RUNTIME" (see the
# RUNTIME set above) consume NO stream bytes -- their pointees live in an
# XFile runtime block that the writer never emits.
ann('GfxWorld', 'name', STR)
ann('GfxWorld', 'baseName', STR)
ann('GfxWorld', 'indices', U16('indexCount'))
ann('GfxWorld', 'skyStartSurfs', BYTES('4 * skySurfCount'))
ann('GfxWorld', 'skyImage', ST('GfxImage', 1))
ann('GfxWorld', 'skyBoxModel', STR)
ann('GfxWorldVertexData', 'vertices', ST('GfxWorldVertex', 'GfxWorld::vertexCount'))
ann('GfxWorldVertexData', 'worldVb', SKIP)             # set condition worldVb never
ann('GfxWorldVertexLayerData', 'data', BYTES('GfxWorld::vertexLayerDataSize'))
ann('GfxWorldVertexLayerData', 'layerVb', SKIP)        # set condition layerVb never
ann('GfxWorld', 'sunLight', ST('GfxLight', 1))         # set reusable sunLight
ann('GfxWorld', 'reflectionProbes', ST('GfxReflectionProbe', 'reflectionProbeCount'))
ann('GfxWorld', 'reflectionProbeTextures',
    ST(UM('loadDef', ST('GfxImageLoadDef', 1)), 'reflectionProbeCount'))
ann('GfxWorld', 'coronas', ST('GfxLightCorona', 'coronaCount'))
ann('GfxWorldDpvsPlanes', 'planes', ST('cplane_s', 'GfxWorld::planeCount'))
ann('GfxWorldDpvsPlanes', 'nodes', U16('GfxWorld::nodeCount'))
ann('GfxWorldDpvsPlanes', 'sceneEntCellBits', BYTES('4 * cellCount * 0x100'))
ann('GfxWorld', 'cells', ST('GfxCell', 'dpvsPlanes::cellCount'))
ann('GfxCell', 'aabbTree', ST('GfxAabbTree', 'aabbTreeCount'))
ann('GfxAabbTree', 'smodelIndexes', U16('smodelIndexCount'))
ann('GfxCell', 'portals', ST('GfxPortal', 'portalCount'))
ann('GfxCell', 'cullGroups', BYTES('4 * cullGroupCount'))
ann('GfxCell', 'reflectionProbes', BYTES('reflectionProbeCount'))
ann('GfxPortal', 'vertices', BYTES('12 * vertexCount'))
ann('GfxPortalWritable', 'hullPoints', SKIP)           # set condition writable never
ann('GfxPortalWritable', 'queuedParent', SKIP)
ann('GfxWorld', 'lightmaps', ST('GfxLightmapArray', 'lightmapCount'))
ann('GfxWorld', 'lightmapPrimaryTextures',
    ST(UM('loadDef', ST('GfxImageLoadDef', 1)), 'lightmapCount'))
ann('GfxWorld', 'lightmapSecondaryTextures',
    ST(UM('loadDef', ST('GfxImageLoadDef', 1)), 'lightmapCount'))
ann('GfxLightGrid', 'rowDataStart', U16('maxs[rowAxis] - mins[rowAxis] + 1'))
ann('GfxLightGrid', 'rawRowData', BYTES('rawRowDataSize'))
ann('GfxLightGrid', 'entries', ST('GfxLightGridEntry', 'entryCount'))
ann('GfxLightGrid', 'colors', ST('GfxLightGridColors', 'colorCount'))
ann('GfxWorld', 'models', ST('GfxBrushModel', 'modelCount'))
ann('GfxWorld', 'materialMemory', ST('MaterialMemory', 'materialMemoryCount'))
ann('GfxWorld', 'outdoorImage', ST('GfxImage', 1))
ann('GfxWorld', 'cellCasterBits',
    BYTES('4 * dpvsPlanes::cellCount * ((dpvsPlanes::cellCount + 31) // 32)'))
ann('GfxWorld', 'sceneDynModel',
    ST('GfxSceneDynModel', 'dpvsDyn::dynEntClientCount[0]'))
ann('GfxWorld', 'sceneDynBrush',
    ST('GfxSceneDynBrush', 'dpvsDyn::dynEntClientCount[1]'))
ann('GfxWorld', 'primaryLightEntityShadowVis',
    BYTES('4 * (primaryLightCount - sunPrimaryLightIndex - 1) * 0x2000'))
ann('GfxWorld', 'primaryLightDynEntShadowVis',
    PCOUNT('4 * dpvsDyn::dynEntClientCount[i] * '
           '(primaryLightCount - sunPrimaryLightIndex - 1)', 1))
ann('GfxWorld', 'nonSunPrimaryLightForModelDynEnt',
    BYTES('dpvsDyn::dynEntClientCount[0]'))
ann('GfxWorld', 'shadowGeom', ST('GfxShadowGeometry', 'primaryLightCount'))
ann('GfxShadowGeometry', 'sortedSurfIndex', U16('surfaceCount'))
ann('GfxShadowGeometry', 'smodelIndex', U16('smodelCount'))
ann('GfxWorld', 'lightRegion', ST('GfxLightRegion', 'primaryLightCount'))
ann('GfxLightRegion', 'hulls', ST('GfxLightRegionHull', 'hullCount'))
ann('GfxLightRegionHull', 'axis', ST('GfxLightRegionAxis', 'axisCount'))
ann('GfxWorldDpvsStatic', 'smodelVisData', PCOUNT('smodelVisDataCount', 1))
ann('GfxWorldDpvsStatic', 'surfaceVisData', PCOUNT('surfaceVisDataCount', 1))
ann('GfxWorldDpvsStatic', 'lodData', BYTES('(2 * smodelVisDataCount) * 16'))
ann('GfxWorldDpvsStatic', 'sortedSurfIndex', U16('staticSurfaceCount'))
ann('GfxWorldDpvsStatic', 'smodelInsts', ST('GfxStaticModelInst', 'smodelCount'))
ann('GfxWorldDpvsStatic', 'surfaces', ST('GfxSurface', 'GfxWorld::surfaceCount'))
ann('GfxWorldDpvsStatic', 'cullGroups', ST('GfxCullGroup', 'GfxWorld::cullGroupCount'))
ann('GfxWorldDpvsStatic', 'smodelDrawInsts', ST('GfxStaticModelDrawInst', 'smodelCount'))
ann('GfxWorldDpvsStatic', 'surfaceMaterials', BYTES('8 * staticSurfaceCount'))
ann('GfxWorldDpvsStatic', 'surfaceCastsSunShadow', BYTES('16 * surfaceVisDataCount'))
ann('GfxWorldDpvsDynamic', 'dynEntCellBits',
    PCOUNT('4 * dynEntClientWordCount[i] * dpvsPlanes::cellCount', 1))
ann('GfxWorldDpvsDynamic', 'dynEntVisData',
    PCOUNT('32 * dynEntClientWordCount[i % 2]', 1))
ann('GfxWorld', 'worldLodChains', ST('GfxWorldLodChain', 'worldLodChainCount'))
ann('GfxWorld', 'worldLodInfos', ST('GfxWorldLodInfo', 'worldLodInfoCount'))
ann('GfxWorld', 'worldLodSurfaces', BYTES('4 * worldLodSurfaceCount'))
ann('GfxWorld', 'waterBuffers', EARR('GfxWaterBuffer', 2))
ann('GfxWaterBuffer', 'buffer', BYTES('16 * (bufferSize // 16)'))

# SequenceReorder rules with leading "..." (OAT ZoneCode): the first listed
# member is the anchor (stays at its declaration position), the others load
# right after it in listed order.
reorder_end('GfxWorld', 'materialMemory', ['vd', 'vld'])
reorder_end('clipMap_t', 'leafs', ['leafbrushes', 'leafbrushNodes',
                                   'leafsurfaces'])
reorder_end('WeaponDef', 'aiVsAiAccuracyGraphName',
            ['aiVsAiAccuracyGraphKnots',
             'originalAiVsAiAccuracyGraphKnots',
             'aiVsPlayerAccuracyGraphName',
             'aiVsPlayerAccuracyGraphKnots',
             'originalAiVsPlayerAccuracyGraphKnots'])

_fill()


import json, re, struct
ANNMOD = sys.modules[__name__]

FOLLOWING = 0xFFFFFFFF
INSERT = 0xFFFFFFFE

def is_following(v):
    '''FOLLOWING (0xFFFFFFFF) and INSERT (0xFFFFFFFE) both store the pointee
    inline in the stream; every other pointer value (including block-7
    addresses such as 0xffff1f00) is an alias and consumes nothing.'''
    return v == FOLLOWING or v == INSERT


_HP = {}
_HP_BUSY = set()

def has_pointers(sname):
    """True if the (struct/union) type stores an inline pointee anywhere."""
    if sname in _HP:
        return _HP[sname]
    if sname in _HP_BUSY:      # recursive type: assume no (checked above)
        return False
    try:
        s = ANNMOD.sdef(sname)
    except KeyError:
        return False
    _HP_BUSY.add(sname)
    try:
        r = False
        for f in s['fields']:
            if f.get('bits') is not None:
                continue
            if f.get('ptr'):
                r = True
                break
            t = f.get('type')
            if t and (is_union(t) or t in T) and has_pointers(t):
                r = True
                break
    finally:
        _HP_BUSY.discard(sname)
    _HP[sname] = r
    return r

class WalkError(Exception):
    pass


# OAT count expressions may be qualified: "GfxWorld::vertexCount" (an
# ancestor struct's field) or "dpvsPlanes::cellCount" (a nested embedded
# struct's field).  Rewrite them into identifiers resolved at eval time.
_QUAL = re.compile(r'([A-Za-z_][A-Za-z0-9_]*)::([A-Za-z_][A-Za-z0-9_]*)'
                   r'((?:\[[^\]]*\])*)')


class _ValView(dict):
    """dict that resolves rewritten qualified names via the walker's
    ancestor-struct stack and nested-struct registry; unknown plain names
    fall back to scanning ancestor structs (embedded sub-struct fields are
    not merged into the value overlay)."""
    def __init__(self, walker, vals):
        dict.__init__(self, vals)
        self._w = walker

    def __missing__(self, key):
        v = None
        if key.startswith('_q_'):
            v = self._w._resolve_qual(key)
        if v is None:
            v = self._w._subfield(key)
        if v is None:
            raise KeyError(key)
        return v


class Walker(object):
    def __init__(self, data, pos):
        self.d = data
        self.pos = pos
        self.names = []
        self._ctx = []
        self._vstack = []       # [(sname, vals)] of the struct chain
        self._subvals = {}      # field name -> vals of embedded sub-structs

    def u(self, off, size):
        if size == 1:
            return self.d[off]
        if size == 2:
            return struct.unpack_from('<H', self.d, off)[0]
        return struct.unpack_from('<I', self.d, off)[0]

    def cstring(self, pos):
        j = self.d.find(b'\x00', pos)
        if j < 0:
            raise WalkError('unterminated string at %d' % pos)
        return self.d[pos:j].decode('latin1'), j + 1

    # ------------------------------------------------------------------
    def _mvals(self, vals):
        """Current struct values overlaid on enclosing struct values
        (inner wins), so cross-struct conditions (e.g. XAnimParts::numframes
        used inside XAnimPartTrans) resolve correctly."""
        m = {}
        for v in self._ctx:
            m.update(v)
        if vals is not None:
            m.update(vals)
        return m

    def _proc(self, sname, base, vals, path):
        self._ctx.append(vals)
        try:
            self._process(sname, base, vals, path)
        finally:
            self._ctx.pop()

    def load_alloc(self, sname, path=''):
        s = sdef(sname)
        if path == '':
            self._vstack = []
            self._subvals = {}
        base = self.pos
        self.pos = base + s['size']
        vals = self._vals(sname, base)
        self._proc(sname, base, vals, path)
        return vals

    def _inline(self, sname, base, pvals, path, merge=False):
        sub = self._vals(sname, base)
        if merge:
            for k, vv in pvals.items():
                if k not in sub:
                    sub[k] = vv
        self._proc(sname, base, sub, path)

    def _vals(self, sname, base):
        s = sdef(sname)
        vals = {}
        for f in s['fields']:
            if f['bits'] is not None:
                continue
            o = base + (f['off'] or 0)
            if f['ptr'] and f['countval'] in (None, 1):
                vals[f['name']] = self.u(o, 4)
            elif f['ptr']:
                n = f['countval']
                vals[f['name']] = list(struct.unpack_from('<%dI' % n, self.d, o))
            elif (f['countval'] not in (None, 1) and f['bits'] is None
                  and f['countval'] and f['size'] % f['countval'] == 0
                  and f['size'] // f['countval'] in (1, 2, 4)
                  and f['type'] != 'float'):
                es = f['size'] // f['countval']
                fmt = {1: 'B', 2: 'H', 4: 'I'}[es]
                vals[f['name']] = list(struct.unpack_from(
                    '<%d%s' % (f['countval'], fmt), self.d, o))
            elif f['size'] in (1, 2, 4) and f['type'] != 'float':
                vals[f['name']] = self.u(o, f['size'])
        return vals

    def _process(self, sname, base, vals, path):
        s = sdef(sname)
        fields = s['fields']
        seq = self._seq(sname, fields, path)
        key = path.rstrip(':').rsplit('::', 1)[-1]
        if key:
            self._subvals[key] = vals
        self._vstack.append((sname, vals, base))
        try:
            for item in seq:
                if isinstance(item, tuple) and item[0] == 'EMBED':
                    _, pf, cf = item
                    self._proc(pf['type'], base + pf['off'],
                               self._vals(pf['type'], base + pf['off']),
                               path + pf['name'] + '::')
                    continue
                f = item
                if f['bits'] is not None:
                    continue
                self._field(sname, f, base, vals, path)
        finally:
            self._vstack.pop()

    def _seq(self, sname, fields, path):
        order = ANNMOD.ORDER.get((sname, '_order'))
        reord = getattr(ANNMOD, 'ORDER_V2', {}).get((sname, 'reorder'))
        if reord:
            marker, names = reord
            nset = set(names)
            byname = dict((f['name'], f) for f in fields)
            if marker == '...':
                # OAT SequenceReorder with "...": the FIRST listed member is
                # the anchor and stays at its declaration position; the other
                # listed members are pulled out and appended right after it.
                anchor = names[0]
                rest = names[1:]
                seq = []
                for f in fields:
                    if f['name'] in nset and f['name'] != anchor:
                        continue
                    seq.append(f)
                    if f['name'] == anchor:
                        break
                for n in rest:
                    seq.append(byname[n])
                seen = False
                for f in fields:
                    if f['name'] == anchor:
                        seen = True
                        continue
                    if not seen or f['name'] in nset:
                        continue
                    seq.append(f)
            else:
                # no "...": the listed members move to the FRONT in listed
                # order, everything else keeps declaration order
                seq = [byname[n] for n in names]
                seq += [f for f in fields if f['name'] not in nset]
        elif order:
            byname = dict((f['name'], f) for f in fields)
            seq = []
            for n in order:
                if n in byname:
                    seq.append(byname[n])
                elif '::' in n:
                    parent, child = n.split('::', 1)
                    pf = byname[parent]
                    for cf in sdef(pf['type'])['fields']:
                        if cf['name'] == child:
                            seq.append(('EMBED', pf, cf))
        else:
            seq = list(fields)
        return seq

    def _field(self, sname, f, base, vals, path):
        spec = ANNMOD.ANN.get((sname, f['name']))
        vals = self._mvals(vals)
        # resolve conditional specs for value fields up front, so that
        # inline_at/dynidx/unionmember resolutions all see the final spec
        if not f['ptr'] and spec is not None and spec[0] == 'choose':
            spec = spec[2].get(spec[1](vals), spec[3])
        if not f['ptr']:
            # inline variable-length array replacing the tail of the struct
            if spec is not None and spec[0] == 'bytes':
                # variable-length literal data replacing the struct tail
                self.pos += self.ecount(spec[1], vals)
                return
            if spec is not None and spec[0] == 'dynidx':
                sel = spec[1](vals)
                n = self.ecount(spec[2], vals)
                self.pos = base + f['off'] + n * (1 if sel else 2)
                return
            if spec is not None and spec[0] == 'inline_at':
                self.pos = base + spec[1] + self.ecount(spec[2], vals)
                return
            # union stored by value (contains pointers)
            if is_union(f['type']):
                if spec is None:
                    # only data unions (no inline pointee anywhere) may be
                    # ignored; anything else must be annotated explicitly
                    if has_pointers(f['type']):
                        raise WalkError('missing annotation for union %s.%s (%s)'
                                        % (sname, f['name'], f['type']))
                    return
                if spec[0] == 'choose':
                    spec = spec[2].get(spec[1](vals), spec[3])
                    if spec is None:
                        return
                if spec[0] == 'inline_struct':
                    self._inline(spec[1], base + f['off'], vals,
                                 path + f['name'] + '::', len(spec) > 2 and spec[2])
                    return
                if spec[0] == 'dynframes':
                    uv = self.u(base + f['off'], 4)
                    if is_following(uv):
                        sel = spec[1](vals)
                        n = self.ecount(spec[2], vals)
                        self.pos += n * (1 if sel else 2)
                    return
                uv = self.u(base + f['off'], 4)
                if spec[0] != 'unionmember':
                    # union whose active member is described directly
                    if is_following(uv):
                        self._pointee(f, vals, spec, path + f['name'],
                                      base + f['off'], uv)
                    return
                member = spec[1](vals) if callable(spec[1]) else spec[1]
                if member is None:
                    return
                uv = self.u(base + f['off'], 4)
                members = dict((m['name'], m) for m in
                               sdef(f['type'])['fields'])
                mf = members[member]
                if mf['ptr'] and not is_following(uv):
                    return
                # by-value union members are always present inline (their
                # bytes are part of the parent struct), so only pointer
                # members are gated on the "following" encoding.  But when
                # the by-value member is itself a union of pointers, the
                # slot holds the active inner pointer: an external/NULL
                # value means no pointee is stored here.
                if not mf['ptr'] and is_union(mf['type']) and not is_following(uv):
                    return
                # ptr members: the pointee is stored at the allocation cursor.
                # by-value members that are unions: the slot holds the active
                # inner pointer, so the spec describes its pointee (cursor too).
                # by-value struct members: the member's own bytes are part of
                # the enclosing struct, so they must not be skipped again.
                self._pointee(mf, vals, spec[2],
                              path + f['name'] + '.' + member,
                              base + f['off'], uv,
                              inline_data=(mf['ptr'] or is_union(mf['type'])))
                return
            if spec is not None and spec[0] == 'earr':
                n = self.ecount(spec[2], vals)
                esz = sdef(spec[1])['size']
                # the array lives inline in the struct: the stream length of
                # the struct differs from the nominal (count=1) size by
                # (n - countval) elements.  adjust the struct-end position
                # before consuming pointees.
                self.pos += n * esz - (f['size'] or 0)
                for i in range(n):
                    sub_base = base + f['off'] + esz * i
                    self._proc(spec[1], sub_base,
                               self._vals(spec[1], sub_base),
                               path + '%s[%d]' % (f['name'], i))
                return
            if spec is not None and spec[0] == 'earrptr':
                arr = self.u(base + f['off'], 4)
                # embedded array of pointers handled via vals list; nothing here
                return
            if spec is not None and spec[0] == 'inline_struct':
                sub_base = base + f['off']
                self._inline(spec[1], sub_base, vals,
                             path + f['name'] + '::',
                             len(spec) > 2 and spec[2])
                return
            # embedded struct by value -> recurse into its fields
            if f['type'] in T and not is_union(f['type']) and f['countval'] in (None, 1):
                sub_base = base + f['off']
                self._proc(f['type'], sub_base,
                           self._vals(f['type'], sub_base),
                           path + f['name'] + '::')
            return
        # ZoneCode `set block <member> XFILE_BLOCK_RUNTIME`: pointee lives in
        # an XFile runtime block -- never written to (and never read from)
        # the stream, so it consumes no bytes regardless of the stored
        # pointer value.
        if (sname, f['name']) in getattr(ANNMOD, 'RUNTIME', ()):
            return
        v = vals.get(f['name'])
        if v is None:
            return
        if isinstance(v, list):
            if spec is not None and spec[0] == 'earrptr':
                sub = spec[1]
                for i, e in enumerate(v):
                    if is_following(e):
                        if sub == 'str':
                            self.read_string(path + '%s[%d]' % (f['name'], i))
                        else:
                            self.load_alloc(sub, path + '%s[%d]' % (f['name'], i))
                return
            if spec is not None and spec[0] == 'pstructarr':
                # embedded array of pointers where each pointee is a run of
                # `spec[1]` structs, count given per element by expr (uses `i`)
                sub = spec[1]
                esz = sdef(sub)['size']
                for i, e in enumerate(v):
                    if not is_following(e):
                        continue
                    ev = dict(vals)
                    ev['i'] = i
                    n = self.ecount(spec[2], ev)
                    start = self.pos
                    self.pos = start + esz * n
                    for j in range(n):
                        sb = start + esz * j
                        self._proc(sub, sb, self._vals(sub, sb),
                                   path + '%s[%d][%d]' % (f['name'], i, j))
                return
            if spec is not None and spec[0] == 'pcount':
                # pointer array where each element's pointee is a run of
                # `elemsize`-sized units, count given per element by expr
                # (the variable `i` is the element index)
                for i, e in enumerate(v):
                    if is_following(e):
                        ev = dict(vals)
                        ev['i'] = i
                        self.pos += spec[2] * self.ecount(spec[1], ev)
                return
            for i, e in enumerate(v):
                if is_following(e):
                    self._pointee(f, vals, spec, path + '%s[%d]' % (f['name'], i),
                                  base + f['off'] + 4 * i)
            return
        if is_following(v):
            self._pointee(f, vals, spec, path + f['name'], base + f['off'])

    def _pointee(self, f, vals, spec, path, poff=None, uv=None,
                 inline_data=True):
        ty = f['type'].rstrip('*') if f.get('type') else None
        if spec == 'str':
            self.read_string(path)
            return
        if spec is None:
            if ty == 'char':
                self.read_string(path)
            elif f.get('type') is not None and f['type'].endswith('**'):
                raise WalkError('missing annotation for %s (%s)' % (path, f['type']))
            elif ty in T and not is_union(ty):
                self.load_alloc(ty, path + ty + '::')
            else:
                raise WalkError('missing annotation for %s (%s)' % (path, f['type']))
            return
        kind = spec[0]
        if kind == 'str':
            self.read_string(path)
        elif kind == 'bytes':
            self.pos += self.ecount(spec[1], vals)
        elif kind == 'u16':
            self.pos += 2 * self.ecount(spec[1], vals)
        elif kind == 'struct':
            n = self.ecount(spec[2], vals) if spec[2] is not None else 1
            if isinstance(spec[1], str):
                # inline struct array: the elements are contiguous in the
                # stream and the pointees of ALL elements follow the array
                # (unless the element bytes are already accounted for by an
                # enclosing struct, i.e. a by-value struct member: then the
                # elements live at their own offset inside the parent struct)
                esz = sdef(spec[1])['size']
                if inline_data:
                    start = self.pos
                    self.pos = start + esz * n
                else:
                    start = poff if poff is not None else self.pos
                for i in range(n):
                    sub = start + esz * i
                    self._proc(spec[1], sub, self._vals(spec[1], sub),
                               path + '%s[%d]' % (spec[1], i))
            else:
                # array of union slots: n * 4-byte unions are contiguous in
                # the stream, each holding the active member pointer; the
                # pointees of the following elements follow the slot array.
                start = self.pos if inline_data else (
                    poff if poff is not None else self.pos)
                self.pos = start + 4 * n
                for i in range(n):
                    uvv = self.u(start + 4 * i, 4)
                    if not is_following(uvv):
                        continue
                    if spec[1][0] == 'unionmember':
                        mf = {'name': spec[1][1], 'type': None, 'ptr': True}
                        sub = spec[1][2]
                    else:
                        mf = {'name': 'x', 'type': None, 'ptr': True}
                        sub = spec[1]
                    self._pointee(mf, vals, sub,
                                  path + '%s[%d]' % (f['name'], i),
                                  start + 4 * i, uvv)
        elif kind == 'struct_extra':
            n = self.ecount(spec[2], vals) if spec[2] is not None else 1
            for i in range(n):
                self.load_alloc(spec[1], path + '%s[%d]' % (spec[1], i))
            self.pos += self.ecount(spec[3], vals)
        elif kind == 'earr':
            # embedded-array spec seen on a pointer field: the pointee is the
            # whole array, so it lays out exactly like ('struct', elem, n)
            self._pointee(f, vals, ('struct', spec[1], spec[2]), path,
                          poff, uv, inline_data)
        elif kind == 'ptrarr':
            cnt = self.ecount(spec[2], vals)
            ptrs = list(struct.unpack_from('<%dI' % cnt, self.d, self.pos))
            self.pos += 4 * cnt
            sub = spec[1]
            for i, p in enumerate(ptrs):
                if is_following(p):
                    if sub == 'str':
                        self.read_string(path + '[%d]' % i)
                    elif isinstance(sub, tuple):
                        self._pointee(f, vals, sub, path + '[%d]' % i)
                    else:
                        self.load_alloc(sub, path + '%s[%d]' % (sub, i))
        elif kind == 'uarr':
            # OAT LoadPtrArray: ALL n pointers are read first, then the
            # pointees of the following elements are loaded in element order.
            n = self.ecount(spec[3], vals)
            member = spec[1](vals) if callable(spec[1]) else spec[1]
            sub = spec[2](vals) if callable(spec[2]) else spec[2]
            pbase = self.pos
            ptrs = list(struct.unpack_from('<%dI' % n, self.d, self.pos))
            self.pos += 4 * n
            for i, uv in enumerate(ptrs):
                if is_following(uv) and member is not None:
                    mf = {'name': member, 'type': None, 'ptr': True}
                    self._pointee(mf, vals, sub,
                                  path + '%s[%d].%s' % (f['name'], i, member),
                                  pbase + 4 * i, uv)
        elif kind == 'unionmember':
            if uv is None:
                uv = vals.get(f['name'])
            if not isinstance(uv, int) and poff is not None:
                uv = self.u(poff, 4)
            if isinstance(uv, int) and is_following(uv):
                member = spec[1](vals) if callable(spec[1]) else spec[1]
                if member is None:
                    return
                # the named member may live in a union nested one level
                # deeper than f's own type (e.g. FxElemVisuals::effectDef is
                # an FxEffectDefRef union whose active member is `name`);
                # descend until the member is found.
                fcur = f
                pcur = path
                while True:
                    members = dict((m['name'], m) for m in
                                   sdef(fcur['type'])['fields'])
                    if member in members:
                        mf = members[member]
                        break
                    nested = None
                    for m in members.values():
                        mt = m.get('type')
                        if mt and is_union(mt) and member in dict(
                                (x['name'], x) for x in sdef(mt)['fields']):
                            nested = m
                            break
                    if nested is None:
                        raise WalkError('union member %r not in %s' %
                                        (member, fcur['type']))
                    fcur = nested
                    pcur = pcur + '.' + fcur['name']
                self._pointee(mf, vals, spec[2],
                              pcur + '.' + member, poff)
        elif kind == 'choose':
            key = spec[1](vals)
            chosen = spec[2].get(key, spec[3])
            if chosen is not None:
                self._pointee(f, vals, chosen, path, poff, uv,
                              inline_data=inline_data)
        elif kind == 'parr2':
            # union member holding an embedded array of N pointers (e.g.
            # pathnode_tree_info_t::child[2]); the pointees are consumed at
            # the allocation cursor in element order
            n = spec[2] if len(spec) > 2 else 2
            pbase = poff if poff is not None else self.pos
            ptrs = struct.unpack_from('<%dI' % n, self.d, pbase)
            for i, p in enumerate(ptrs):
                if is_following(p):
                    self.load_alloc(spec[1], path + '[%d]' % i)
        elif kind == 'inline_struct':
            self._proc(spec[1], 0, {}, path + spec[1] + '::')
        elif kind == 'skip':
            pass
        else:
            raise WalkError('bad spec %r for %s' % (spec, path))

    def ecount(self, expr, vals):
        if isinstance(expr, int):
            return expr
        src = _QUAL.sub(lambda m: '_q_%s_%s%s' % (m.group(1), m.group(2),
                                                  m.group(3)), expr)
        try:
            return int(eval(src, {"__builtins__": {}},
                            _ValView(self, dict(vals))))
        except Exception as e:
            raise WalkError('count %r failed: %s (vals %s)' % (expr, e, vals))

    def _resolve_qual(self, key):
        """resolve a rewritten qualified name '_q_<owner>_<field>'"""
        if not key.startswith('_q_'):
            return None
        body = key[3:]
        cands = [sname for sname, _, _ in reversed(self._vstack)]
        cands += [k for k in self._subvals if k not in cands]
        for x in cands:
            pre = x + '_'
            if not body.startswith(pre):
                continue
            y = body[len(pre):]
            for sname, v, _b in reversed(self._vstack):
                if sname == x and y in v:
                    return v[y]
            if x in self._subvals and y in self._subvals[x]:
                return self._subvals[x][y]
        # owner may be an embedded sub-struct that has not been walked yet:
        # resolve the field straight from the blob.  The owner name is a
        # field name and may contain underscores, so try every split.
        parts = body.split('_')
        for i in range(1, len(parts)):
            x = '_'.join(parts[:i])
            y = '_'.join(parts[i:])
            for sname, _v, base in reversed(self._vstack):
                for f in sdef(sname)['fields']:
                    if (f['name'] == x and f['bits'] is None and not f['ptr']
                            and f['type'] in T and not is_union(f['type'])):
                        sub = self._vals(f['type'], base + (f['off'] or 0))
                        if y in sub:
                            return sub[y]
        return None

    def _subfield(self, key):
        """find a field named `key` in any struct of the current chain and
        read its value straight from the blob (handles forward references
        to embedded sub-structs that are walked later)."""
        for sname, vals, base in reversed(self._vstack):
            if key in vals:
                continue
            for f in sdef(sname)['fields']:
                if f['name'] != key or f['bits'] is not None:
                    continue
                o = base + (f['off'] or 0)
                if f['ptr'] and f['countval'] in (None, 1):
                    return self.u(o, 4)
                if f['ptr']:
                    return list(struct.unpack_from(
                        '<%dI' % f['countval'], self.d, o))
                if (f['countval'] not in (None, 1) and f['countval']
                        and f['size'] % f['countval'] == 0
                        and f['size'] // f['countval'] in (1, 2, 4)
                        and f['type'] != 'float'):
                    es = f['size'] // f['countval']
                    fmt = {1: 'B', 2: 'H', 4: 'I'}[es]
                    return list(struct.unpack_from(
                        '<%d%s' % (f['countval'], fmt), self.d, o))
                if f['size'] in (1, 2, 4) and f['type'] != 'float':
                    return self.u(o, f['size'])
        return None

    def read_string(self, path):
        s, p = self.cstring(self.pos)
        self.pos = p
        self.names.append((path, s))
        return s


import binascii, struct, zlib

XBLOCKS = ['TEMP', 'RUNTIME', 'LARGE_RUNTIME', 'PHYSICAL_RUNTIME',
           'VIRTUAL', 'LARGE', 'PHYSICAL']
ASSET_TYPES = [
    'xmodelpieces', 'physpreset', 'physconstraints', 'destructibledef',
    'xanim', 'xmodel', 'material', 'techset', 'image', 'sound',
    'loadedsound', 'clipmap', 'clipmap_pvs', 'comworld', 'gameworld_sp',
    'gameworld_mp', 'mapents', 'gfxworld', 'lightdef', 'uimap', 'font',
    'menulist', 'menu', 'localize', 'weapon', 'snddriverglobals', 'fx',
    'impactfx', 'aitype', 'mptype', 'character', 'xmodelalias', 'rawfile',
    'stringtable', 'packindex',
]

def decompress(path):
    raw = open(path, 'rb').read()
    if raw[:8] != b'IWffu100':
        raise ValueError('bad magic %r' % raw[:8])
    version = struct.unpack_from('<I', raw, 8)[0]
    data = zlib.decompress(raw[12:])
    return version, data, raw

def parse(path):
    version, d, raw = decompress(path)
    size, extsize = struct.unpack_from('<2I', d, 0)
    blocks = struct.unpack_from('<7I', d, 8)
    pos = 36
    strCount, strPtr, assetCount, assetPtr = struct.unpack_from('<2I2I', d, pos)
    pos += 16
    strings = []
    str_base = pos
    if strCount:
        idx = list(struct.unpack_from('<%dI' % strCount, d, pos))
        pos += strCount * 4
        for e in idx:
            if e == 0:
                strings.append(None)
            elif e == 0xFFFFFFFF:
                j = d.find(b'\x00', pos)
                strings.append(d[pos:j].decode('latin1'))
                pos = j + 1
            else:
                raise ValueError('script string ptr %08x' % e)
    assets = []
    if assetCount:
        arr = struct.unpack_from('<%dI' % (assetCount * 2), d, pos)
        pos += assetCount * 8
        for i in range(assetCount):
            assets.append((arr[2 * i], arr[2 * i + 1]))
    return dict(path=path, version=version, rawsize=len(raw), dsize=len(d),
                size=size, extsize=extsize, blocks=blocks, strings=strings,
                assets=assets, content_pos=pos, data=d)


# ----------------------------------------------------------------------
# asset type tables (T4)

# XBLOCKs in stream order, from T4_Assets.h XFileBlock enum +
# ZoneLoaderFactoryT4::SetupBlock:
# TEMP, RUNTIME, LARGE_RUNTIME, PHYSICAL_RUNTIME, VIRTUAL, LARGE, PHYSICAL
XBLOCKS = ['TEMP', 'RUNTIME', 'LARGE_RUNTIME', 'PHYSICAL_RUNTIME',
           'VIRTUAL', 'LARGE', 'PHYSICAL']

ASSET_TYPES = [
    'xmodelpieces', 'physpreset', 'physconstraints', 'destructibledef',
    'xanim', 'xmodel', 'material', 'techset', 'image', 'sound',
    'loadedsound', 'clipmap', 'clipmap_pvs', 'comworld', 'gameworld_sp',
    'gameworld_mp', 'mapents', 'gfxworld', 'lightdef', 'uimap', 'font',
    'menulist', 'menu', 'localize', 'weapon', 'snddriverglobals', 'fx',
    'impactfx', 'aitype', 'mptype', 'character', 'xmodelalias', 'rawfile',
    'stringtable', 'packindex',
]

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

NAME_PATH = {
    'LocalizeEntry': 'name',
    'Material': 'info::name',
    'MaterialTechnique': 'name',
    'snd_alias_list_t': 'aliasName',
    'snd_alias_list_name': 'soundName',
    'menuDef_t': 'window::name',
    'Font_s': 'fontName',
    'WeaponDef': 'szInternalName',
    'GfxImage': 'name',
}


def type_name(t):
    if 0 <= t < len(ASSET_TYPES):
        return ASSET_TYPES[t]
    return 'type%d' % t


class ExtractWalker(Walker):
    """Walker that additionally records `bytes` consumptions (rawfile data)."""

    def __init__(self, data, pos):
        Walker.__init__(self, data, pos)
        self.byteslog = []

    def _pointee(self, f, vals, spec, path, poff=None, uv=None,
                 inline_data=True):
        if isinstance(spec, tuple) and spec and spec[0] == 'bytes':
            start = self.pos
            n = self.ecount(spec[1], vals)
            self.pos += n
            self.byteslog.append((path, start, n))
            return
        return Walker._pointee(self, f, vals, spec, path, poff, uv,
                               inline_data)


def walk_assets(z):
    """Walk every asset; yield (index, type, struct, start, end, names, err)."""
    d = z['data']
    w = Walker(d, z['content_pos'])
    for i, (t, _off) in enumerate(z['assets']):
        sname = TYPE_STRUCT.get(t)
        start = w.pos
        w.names = []
        err = None
        if sname is None or sname not in T:
            err = 'no struct model for asset type %s' % type_name(t)
            yield i, t, sname, start, start, [], err
            return
        try:
            w.load_alloc(sname, '')
        except Exception as e:
            err = '%s: %s' % (type(e).__name__, e)
            yield i, t, sname, start, w.pos, list(w.names), err
            return
        yield i, t, sname, start, w.pos, list(w.names), None


def pick_name(sname, names):
    want = NAME_PATH.get(sname)
    if want is not None:
        for path, s in names:
            if path == want or path.endswith(want):
                return s
    return names[0][1] if names else None


def cmd_info(args):
    z = parse(args.file)
    print('file:            %s' % args.file)
    print('magic:           IWffu100')
    print('version:         %d' % z['version'])
    print('compressed:      %d bytes' % z['rawsize'])
    print('decompressed:    %d bytes' % z['dsize'])
    print('declared size:   %d' % z['size'])
    print('external size:   %d' % z['extsize'])
    print('memory blocks:   %s' % ', '.join(
        '%s=%d' % (XBLOCKS[i], z['blocks'][i]) for i in range(7)))
    print('script strings:  %d' % len(z['strings']))
    print('asset count:     %d' % len(z['assets']))
    print('content start:   %d' % z['content_pos'])
    print()
    bytype = {}
    for i, (t, _off) in enumerate(z['assets']):
        bytype.setdefault(t, []).append(i)
    print('assets by type:')
    for t in sorted(bytype):
        print('  %-18s %d' % (type_name(t), len(bytype[t])))
    print()
    print('%-5s %-18s %-44s %s' % ('idx', 'type', 'name', 'range'))
    err = None
    for i, t, sname, start, end, names, e in walk_assets(z):
        nm = pick_name(sname, names) if names else None
        print('%-5d %-18s %-44s [%d..%d)' % (
            i, type_name(t), (nm or '')[:44], start, end))
        if e:
            err = 'stopped at asset %d: %s' % (i, e)
            break
    if err:
        print('NOTE: %s' % err)
    return 0


def cmd_extract(args):
    z = parse(args.file)
    outdir = args.output or ('extracted_' + os.path.splitext(
        os.path.basename(args.file))[0])
    exts = None if args.all else tuple(e.lower() for e in args.ext)
    w = ExtractWalker(z['data'], z['content_pos'])
    made = 0
    skipped = 0
    err = None
    for i, (t, _off) in enumerate(z['assets']):
        sname = TYPE_STRUCT.get(t)
        start = w.pos
        w.names = []
        w.byteslog = []
        if sname is None or sname not in T:
            err = 'no struct model for asset type %s' % type_name(t)
            break
        try:
            w.load_alloc(sname, '')
        except Exception as e:
            err = 'stopped at asset %d (%s): %s: %s' % (
                i, type_name(t), type(e).__name__, e)
            break
        if t != 32 or not w.names:
            continue
        name = w.names[0][1]
        buf = None
        for path, pos, n in w.byteslog:
            if path == 'buffer':
                buf = (pos, n)
        if buf is None:
            continue
        if exts is not None and                 os.path.splitext(name)[1].lower() not in exts:
            skipped += 1
            continue
        data = z['data'][buf[0]:buf[0] + max(0, buf[1] - 1)]
        dest = os.path.join(outdir, name.replace(chr(92), '/'))
        destdir = os.path.dirname(dest)
        if destdir and not os.path.isdir(destdir):
            os.makedirs(destdir)
        with open(dest, 'wb') as fh:
            fh.write(data)
        made += 1
        print('extracted %s (%d bytes)' % (name, len(data)))
    print('extracted %d file(s), skipped %d by extension filter' % (
        made, skipped))
    if err:
        print('NOTE: %s' % err)
        return 1
    return 0


def main(argv=None):
    ap = argparse.ArgumentParser(prog='fftool',
                                 description='T4 (WaW) fastfile tool')
    sub = ap.add_subparsers(dest='cmd')
    sub.required = True
    api = sub.add_parser('info', help='show header and asset list')
    api.add_argument('file')
    api.set_defaults(func=cmd_info)
    ape = sub.add_parser('extract', help='extract rawfiles')
    ape.add_argument('file')
    ape.add_argument('-o', '--output', default=None,
                     help='output directory')
    ape.add_argument('--ext', nargs='+', default=['.gsc', '.cfg', '.txt'],
                     help='file extensions to extract')
    ape.add_argument('--all', action='store_true',
                     help='extract every rawfile, ignore --ext')
    ape.set_defaults(func=cmd_extract)
    args = ap.parse_args(argv)
    return args.func(args)


if __name__ == '__main__':
    sys.exit(main())
