/* gerado por re/gen_tables.py -- NAO EDITAR.
 * fonte: tabelas validadas do re/fftool.py (fastfile T4, 129/129 EXACT).
 */

/* ---- specs ANN ---- */
static const T4ZSpec sp0 = { K_BYTES, .e="totalEdgeCount" };
static const T4ZSpec sp1 = { K_STRUCT, .e="cplane_s", .e2="numsides" };
static const T4ZSpec sp2 = { K_STRUCT, .e="cbrushside_t", .e2="numsides" };
static const T4ZSpec sp3 = { K_BYTES, .e="12 * numverts" };
static const T4ZSpec sp4 = { K_STRUCT, .e="CollisionBorder", .a=1 };
static const T4ZSpec sp5 = { K_STRUCT, .e="ComBurnableSample", .a=32 };
static const T4ZSpec sp6 = { K_STRUCT, .e="ComBurnableCell", .e2="numBurnableCells" };
static const T4ZSpec sp7 = { K_STRUCT, .e="ComPrimaryLight", .e2="primaryLightCount" };
static const T4ZSpec sp8 = { K_STRUCT, .e="ComWaterCell", .e2="numWaterCells" };
static const T4ZSpec sp9 = { K_STR };
static const T4ZSpec sp10 = { K_STRUCT, .e="DestructiblePiece", .e2="numPieces" };
static const T4ZSpec sp11 = { K_STR };
static const T4ZSpec sp13 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp12 = { K_UM, .name="name", .sub=&sp13 };
static const T4ZSpec sp14 = { K_STR };
static const T4ZSpec sp15 = { K_EARRP, .e="XModel", .e2="3" };
static const T4ZSpec sp16 = { K_STR };
static const T4ZSpec sp18 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp17 = { K_UM, .name="name", .sub=&sp18 };
static const T4ZSpec sp20 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp19 = { K_UM, .name="name", .sub=&sp20 };
static const T4ZSpec sp21 = { K_EARR, .e="DestructibleModel", .a=5 };
static const T4ZSpec sp23 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp22 = { K_UM, .name="name", .sub=&sp23 };
static const T4ZSpec sp25 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp24 = { K_UM, .name="name", .sub=&sp25 };
static const T4ZSpec sp27 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp26 = { K_UM, .name="name", .sub=&sp27 };
static const T4ZSpec sp29 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp28 = { K_UM, .name="name", .sub=&sp29 };
static const T4ZSpec sp30 = { K_STR };
static const T4ZSpec sp31 = { K_STR };
static const T4ZSpec sp32 = { K_STRUCT, .e="Glyph", .e2="glyphCount" };
static const T4ZSpec sp33 = { K_STRUCT, .e="FxElemDef", .e2="elemDefCountEmission + elemDefCountLooping + elemDefCountOneShot" };
static const T4ZSpec sp34 = { K_STR };
static const T4ZSpec sp36 = { K_STR };
static const T4ZSpec sp35 = { K_UM, .name="name", .sub=&sp36 };
static const T4ZSpec sp38 = { K_STR };
static const T4ZSpec sp37 = { K_UM, .name="name", .sub=&sp38 };
static const T4ZSpec sp40 = { K_STR };
static const T4ZSpec sp39 = { K_UM, .name="name", .sub=&sp40 };
static const T4ZSpec sp41 = { K_STRUCT, .e="FxTrailDef", .a=1 };
static const T4ZSpec sp42 = { K_STRUCT, .e="FxElemVelStateSample", .e2="velIntervalCount + 1" };
static const T4ZSpec sp43 = { K_STRUCT, .e="FxElemVisStateSample", .e2="visStateIntervalCount + 1" };
static const T4ZSpec sp46 = { K_STRUCT, .e="FxElemMarkVisuals", .e2="visualCount" };
static const T4ZSpec sp45 = { K_UM, .name="markArray", .sub=&sp46 };
static const T4ZSpec sp49 = { K_BAD, .name="uarr FxElemDef.visuals com seletor variavel" };
static const T4ZSpec sp48 = { K_UM, .name="array", .sub=&sp49 };
static const T4ZSpec sp52 = { K_STRUCT, .e="Material", .a=1 };
static const T4ZSpec sp53 = { K_STRUCT, .e="Material", .a=1 };
static const T4ZSpec sp54 = { K_STRUCT, .e="Material", .a=1 };
static const T4ZSpec sp55 = { K_STRUCT, .e="Material", .a=1 };
static const T4ZSpec sp56 = { K_STRUCT, .e="Material", .a=1 };
static const T4ZSpec sp57 = { K_STRUCT, .e="Material", .a=1 };
static const T4ZSpec sp58 = { K_STRUCT, .e="XModel", .a=1 };
static const T4ZSpec sp59 = { K_STR };
static const T4ZSpec sp61 = { K_STR };
static const T4ZSpec sp60 = { K_UM, .name="name", .sub=&sp61 };
static const T4ZSpec sp62 = { K_SKIP };
static const T4ZSpec sp51 = { K_CHOISK, .e="elemType", .a=9, .ents=(const T4ZChooseEnt[]){KENT(0, &sp52), KENT(1, &sp53), KENT(2, &sp54), KENT(3, &sp55), KENT(4, &sp56), KENT(5, &sp57), KENT(6, &sp58), KENT(9, &sp59), KENT(11, &sp60)}, .sub=&sp62 };
static const T4ZSpec sp50 = { K_UM, .name="instance", .sub=&sp51 };
static const T4ZSpec sp47 = { K_CHOOSE, .e="visualCount", .a=C_GE, .b=2, .sub=&sp48, .sub2=&sp50 };
static const T4ZSpec sp44 = { K_CHOISK, .e="elemType", .a=1, .ents=(const T4ZChooseEnt[]){KENT(10, &sp45)}, .sub=&sp47 };
static const T4ZSpec sp63 = { K_SKIP };
static const T4ZSpec sp65 = { K_STR };
static const T4ZSpec sp64 = { K_UM, .name="name", .sub=&sp65 };
static const T4ZSpec sp66 = { K_STRUCT, .e="Material", .a=1 };
static const T4ZSpec sp67 = { K_STRUCT, .e="XModel", .a=1 };
static const T4ZSpec sp68 = { K_STR };
static const T4ZSpec sp69 = { K_EARRP, .e="FxEffectDef", .e2="4" };
static const T4ZSpec sp70 = { K_EARRP, .e="FxEffectDef", .e2="31" };
static const T4ZSpec sp71 = { K_STR };
static const T4ZSpec sp72 = { K_STRUCT, .e="FxImpactEntry", .a=16 };
static const T4ZSpec sp73 = { K_BYTES, .e="2 * indCount" };
static const T4ZSpec sp74 = { K_STRUCT, .e="FxTrailVertex", .e2="vertCount" };
static const T4ZSpec sp75 = { K_U16, .e="smodelIndexCount" };
static const T4ZSpec sp76 = { K_STRUCT, .e="GfxAabbTree", .e2="aabbTreeCount" };
static const T4ZSpec sp77 = { K_BYTES, .e="4 * cullGroupCount" };
static const T4ZSpec sp78 = { K_STRUCT, .e="GfxPortal", .e2="portalCount" };
static const T4ZSpec sp79 = { K_BYTES, .e="reflectionProbeCount" };
static const T4ZSpec sp80 = { K_STR };
static const T4ZSpec sp82 = { K_STRUCT, .e="GfxImageLoadDef", .a=1 };
static const T4ZSpec sp81 = { K_UM, .name="loadDef", .sub=&sp82 };
static const T4ZSpec sp83 = { K_BYTES, .e="resourceSize" };
static const T4ZSpec sp84 = { K_STR };
static const T4ZSpec sp85 = { K_STRUCT, .e="GfxLightGridColors", .e2="colorCount" };
static const T4ZSpec sp86 = { K_STRUCT, .e="GfxLightGridEntry", .e2="entryCount" };
static const T4ZSpec sp87 = { K_BYTES, .e="rawRowDataSize" };
static const T4ZSpec sp88 = { K_U16, .e="maxs[rowAxis] - mins[rowAxis] + 1" };
static const T4ZSpec sp89 = { K_STRUCT, .e="GfxLightRegionHull", .e2="hullCount" };
static const T4ZSpec sp90 = { K_STRUCT, .e="GfxLightRegionAxis", .e2="axisCount" };
static const T4ZSpec sp91 = { K_BYTES, .e="4 * programSize" };
static const T4ZSpec sp92 = { K_BYTES, .e="12 * vertexCount" };
static const T4ZSpec sp93 = { K_SKIP };
static const T4ZSpec sp94 = { K_SKIP };
static const T4ZSpec sp95 = { K_U16, .e="smodelCount" };
static const T4ZSpec sp96 = { K_U16, .e="surfaceCount" };
static const T4ZSpec sp97 = { K_BYTES, .e="4 * programSize" };
static const T4ZSpec sp98 = { K_BYTES, .e="16 * (bufferSize // 16)" };
static const T4ZSpec sp99 = { K_STR };
static const T4ZSpec sp100 = { K_BYTES, .e="4 * dpvsPlanes::cellCount * ((dpvsPlanes::cellCount + 31) // 32)" };
static const T4ZSpec sp101 = { K_STRUCT, .e="GfxCell", .e2="dpvsPlanes::cellCount" };
static const T4ZSpec sp102 = { K_STRUCT, .e="GfxLightCorona", .e2="coronaCount" };
static const T4ZSpec sp103 = { K_U16, .e="indexCount" };
static const T4ZSpec sp104 = { K_STRUCT, .e="GfxLightRegion", .e2="primaryLightCount" };
static const T4ZSpec sp107 = { K_STRUCT, .e="GfxImageLoadDef", .a=1 };
static const T4ZSpec sp106 = { K_UM, .name="loadDef", .sub=&sp107 };
static const T4ZSpec sp105 = { K_STRUCTU, .sub=&sp106, .e2="lightmapCount" };
static const T4ZSpec sp110 = { K_STRUCT, .e="GfxImageLoadDef", .a=1 };
static const T4ZSpec sp109 = { K_UM, .name="loadDef", .sub=&sp110 };
static const T4ZSpec sp108 = { K_STRUCTU, .sub=&sp109, .e2="lightmapCount" };
static const T4ZSpec sp111 = { K_STRUCT, .e="GfxLightmapArray", .e2="lightmapCount" };
static const T4ZSpec sp112 = { K_STRUCT, .e="MaterialMemory", .e2="materialMemoryCount" };
static const T4ZSpec sp113 = { K_STRUCT, .e="GfxBrushModel", .e2="modelCount" };
static const T4ZSpec sp114 = { K_STR };
static const T4ZSpec sp115 = { K_BYTES, .e="dpvsDyn::dynEntClientCount[0]" };
static const T4ZSpec sp116 = { K_STRUCT, .e="GfxImage", .a=1 };
static const T4ZSpec sp117 = { K_PCOUNT, .e="4 * dpvsDyn::dynEntClientCount[i] * (primaryLightCount - sunPrimaryLightIndex - 1)", .a=1 };
static const T4ZSpec sp118 = { K_BYTES, .e="4 * (primaryLightCount - sunPrimaryLightIndex - 1) * 0x2000" };
static const T4ZSpec sp121 = { K_STRUCT, .e="GfxImageLoadDef", .a=1 };
static const T4ZSpec sp120 = { K_UM, .name="loadDef", .sub=&sp121 };
static const T4ZSpec sp119 = { K_STRUCTU, .sub=&sp120, .e2="reflectionProbeCount" };
static const T4ZSpec sp122 = { K_STRUCT, .e="GfxReflectionProbe", .e2="reflectionProbeCount" };
static const T4ZSpec sp123 = { K_STRUCT, .e="GfxSceneDynBrush", .e2="dpvsDyn::dynEntClientCount[1]" };
static const T4ZSpec sp124 = { K_STRUCT, .e="GfxSceneDynModel", .e2="dpvsDyn::dynEntClientCount[0]" };
static const T4ZSpec sp125 = { K_STRUCT, .e="GfxShadowGeometry", .e2="primaryLightCount" };
static const T4ZSpec sp126 = { K_STR };
static const T4ZSpec sp127 = { K_STRUCT, .e="GfxImage", .a=1 };
static const T4ZSpec sp128 = { K_BYTES, .e="4 * skySurfCount" };
static const T4ZSpec sp129 = { K_STRUCT, .e="GfxLight", .a=1 };
static const T4ZSpec sp130 = { K_EARR, .e="GfxWaterBuffer", .a=2 };
static const T4ZSpec sp131 = { K_STRUCT, .e="GfxWorldLodChain", .e2="worldLodChainCount" };
static const T4ZSpec sp132 = { K_STRUCT, .e="GfxWorldLodInfo", .e2="worldLodInfoCount" };
static const T4ZSpec sp133 = { K_BYTES, .e="4 * worldLodSurfaceCount" };
static const T4ZSpec sp134 = { K_PCOUNT, .e="4 * dynEntClientWordCount[i] * dpvsPlanes::cellCount", .a=1 };
static const T4ZSpec sp135 = { K_PCOUNT, .e="32 * dynEntClientWordCount[i % 2]", .a=1 };
static const T4ZSpec sp136 = { K_U16, .e="GfxWorld::nodeCount" };
static const T4ZSpec sp137 = { K_STRUCT, .e="cplane_s", .e2="GfxWorld::planeCount" };
static const T4ZSpec sp138 = { K_BYTES, .e="4 * cellCount * 0x100" };
static const T4ZSpec sp139 = { K_STRUCT, .e="GfxCullGroup", .e2="GfxWorld::cullGroupCount" };
static const T4ZSpec sp140 = { K_BYTES, .e="(2 * smodelVisDataCount) * 16" };
static const T4ZSpec sp141 = { K_STRUCT, .e="GfxStaticModelDrawInst", .e2="smodelCount" };
static const T4ZSpec sp142 = { K_STRUCT, .e="GfxStaticModelInst", .e2="smodelCount" };
static const T4ZSpec sp143 = { K_PCOUNT, .e="smodelVisDataCount", .a=1 };
static const T4ZSpec sp144 = { K_U16, .e="staticSurfaceCount" };
static const T4ZSpec sp145 = { K_BYTES, .e="16 * surfaceVisDataCount" };
static const T4ZSpec sp146 = { K_BYTES, .e="8 * staticSurfaceCount" };
static const T4ZSpec sp147 = { K_PCOUNT, .e="surfaceVisDataCount", .a=1 };
static const T4ZSpec sp148 = { K_STRUCT, .e="GfxSurface", .e2="GfxWorld::surfaceCount" };
static const T4ZSpec sp149 = { K_STRUCT, .e="GfxWorldVertex", .e2="GfxWorld::vertexCount" };
static const T4ZSpec sp150 = { K_SKIP };
static const T4ZSpec sp151 = { K_BYTES, .e="GfxWorld::vertexLayerDataSize" };
static const T4ZSpec sp152 = { K_SKIP };
static const T4ZSpec sp153 = { K_STR };
static const T4ZSpec sp154 = { K_STRUCT, .e="ItemKeyHandler", .a=1 };
static const T4ZSpec sp155 = { K_STR };
static const T4ZSpec sp157 = { K_BYTES, .e="data_size" };
static const T4ZSpec sp156 = { K_UM, .name="data", .sub=&sp157 };
static const T4ZSpec sp158 = { K_STR };
static const T4ZSpec sp159 = { K_STR };
static const T4ZSpec sp160 = { K_BYTES, .e="numEntityChars" };
static const T4ZSpec sp161 = { K_STR };
static const T4ZSpec sp162 = { K_STRUCT, .e="MaterialConstantDef", .e2="constantCount" };
static const T4ZSpec sp163 = { K_STRUCT, .e="GfxStateBits", .e2="stateBitsCount" };
static const T4ZSpec sp164 = { K_STRUCT, .e="MaterialTextureDef", .e2="textureCount" };
static const T4ZSpec sp165 = { K_STR };
static const T4ZSpec sp166 = { K_STRUCT, .e="MaterialShaderArgument", .e2="perPrimArgCount + perObjArgCount + stableArgCount" };
static const T4ZSpec sp167 = { K_STRUCT, .e="MaterialPixelShader", .a=1 };
static const T4ZSpec sp168 = { K_STRUCT, .e="MaterialVertexShader", .a=1 };
static const T4ZSpec sp169 = { K_STR };
static const T4ZSpec sp170 = { K_SKIP };
static const T4ZSpec sp173 = { K_BYTES, .e="16" };
static const T4ZSpec sp172 = { K_UM, .name="literalConst", .sub=&sp173 };
static const T4ZSpec sp175 = { K_BYTES, .e="16" };
static const T4ZSpec sp174 = { K_UM, .name="literalConst", .sub=&sp175 };
static const T4ZSpec sp176 = { K_SKIP };
static const T4ZSpec sp171 = { K_CHOISK, .e="type", .a=2, .ents=(const T4ZChooseEnt[]){KENT(1, &sp172), KENT(7, &sp174)}, .sub=&sp176 };
static const T4ZSpec sp177 = { K_STR };
static const T4ZSpec sp178 = { K_EARR, .e="MaterialPass", .e2="passCount" };
static const T4ZSpec sp179 = { K_STR };
static const T4ZSpec sp180 = { K_SKIP };
static const T4ZSpec sp181 = { K_EARRP, .e="MaterialTechnique", .e2="59" };
static const T4ZSpec sp184 = { K_STRUCT, .e="water_t", .a=1 };
static const T4ZSpec sp183 = { K_UM, .name="water", .sub=&sp184 };
static const T4ZSpec sp186 = { K_STRUCT, .e="GfxImage", .a=1 };
static const T4ZSpec sp185 = { K_UM, .name="image", .sub=&sp186 };
static const T4ZSpec sp182 = { K_CHOISK, .e="semantic", .a=1, .ents=(const T4ZChooseEnt[]){KENT(20, &sp183)}, .sub=&sp185 };
static const T4ZSpec sp187 = { K_STR };
static const T4ZSpec sp188 = { K_SKIP };
static const T4ZSpec sp189 = { K_SKIP };
static const T4ZSpec sp190 = { K_PTRARR, .e="menuDef_t", .e2="menuCount" };
static const T4ZSpec sp191 = { K_STR };
static const T4ZSpec sp194 = { K_STR };
static const T4ZSpec sp193 = { K_UM, .name="stringVal", .sub=&sp194 };
static const T4ZSpec sp195 = { K_SKIP };
static const T4ZSpec sp192 = { K_CHOISK, .e="dataType", .a=1, .ents=(const T4ZChooseEnt[]){KENT(2, &sp193)}, .sub=&sp195 };
static const T4ZSpec sp196 = { K_STRUCT, .e="PackIndexEntry", .e2="header::count" };
static const T4ZSpec sp197 = { K_STR };
static const T4ZSpec sp198 = { K_U16, .e="nodeCount" };
static const T4ZSpec sp199 = { K_U16, .e="nodeCount" };
static const T4ZSpec sp200 = { K_STRUCT, .e="pathnode_tree_t", .e2="nodeTreeCount" };
static const T4ZSpec sp201 = { K_STRUCT, .e="pathnode_t", .e2="nodeCount" };
static const T4ZSpec sp202 = { K_BYTES, .e="visBytes" };
static const T4ZSpec sp203 = { K_STR };
static const T4ZSpec sp204 = { K_STR };
static const T4ZSpec sp205 = { K_EARR, .e="PhysConstraint", .a=16 };
static const T4ZSpec sp206 = { K_STR };
static const T4ZSpec sp207 = { K_STRUCT, .e="BrushWrapper", .a=1 };
static const T4ZSpec sp208 = { K_STRUCT, .e="PhysGeomInfo", .e2="count" };
static const T4ZSpec sp209 = { K_STR };
static const T4ZSpec sp210 = { K_STR };
static const T4ZSpec sp211 = { K_BYTES, .e="size" };
static const T4ZSpec sp212 = { K_STR };
static const T4ZSpec sp213 = { K_BYTES, .e="len + 1" };
static const T4ZSpec sp214 = { K_STR };
static const T4ZSpec sp215 = { K_STR };
static const T4ZSpec sp218 = { K_STRUCT, .e="LoadedSound", .a=1 };
static const T4ZSpec sp217 = { K_UM, .name="loadSnd", .sub=&sp218 };
static const T4ZSpec sp220 = { K_STRUCT, .e="StreamedSound", .a=1 };
static const T4ZSpec sp219 = { K_UM, .name="streamSnd", .sub=&sp220 };
static const T4ZSpec sp216 = { K_CHOISK, .e="type", .a=1, .ents=(const T4ZChooseEnt[]){KENT(1, &sp217)}, .sub=&sp219 };
static const T4ZSpec sp221 = { K_STR };
static const T4ZSpec sp222 = { K_STR };
static const T4ZSpec sp223 = { K_STRUCT, .e="PrimedSound", .a=1 };
static const T4ZSpec sp224 = { K_STR };
static const T4ZSpec sp225 = { K_PTRARR, .e="str", .e2="columnCount * rowCount" };
static const T4ZSpec sp226 = { K_BYTES, .e="8 * aiVsAiAccuracyGraphKnotCount" };
static const T4ZSpec sp227 = { K_STR };
static const T4ZSpec sp228 = { K_BYTES, .e="8 * aiVsPlayerAccuracyGraphKnotCount" };
static const T4ZSpec sp229 = { K_STR };
static const T4ZSpec sp231 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp230 = { K_UM, .name="name", .sub=&sp231 };
static const T4ZSpec sp233 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp232 = { K_UM, .name="name", .sub=&sp233 };
static const T4ZSpec sp235 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp234 = { K_UM, .name="name", .sub=&sp235 };
static const T4ZSpec sp237 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp236 = { K_UM, .name="name", .sub=&sp237 };
static const T4ZSpec sp239 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp238 = { K_UARR, .name="name", .sub=&sp239, .a=31 };
static const T4ZSpec sp241 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp240 = { K_UM, .name="name", .sub=&sp241 };
static const T4ZSpec sp243 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp242 = { K_UM, .name="name", .sub=&sp243 };
static const T4ZSpec sp245 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp244 = { K_UM, .name="name", .sub=&sp245 };
static const T4ZSpec sp246 = { K_STR };
static const T4ZSpec sp248 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp247 = { K_UM, .name="name", .sub=&sp248 };
static const T4ZSpec sp250 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp249 = { K_UM, .name="name", .sub=&sp250 };
static const T4ZSpec sp252 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp251 = { K_UM, .name="name", .sub=&sp252 };
static const T4ZSpec sp254 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp253 = { K_UM, .name="name", .sub=&sp254 };
static const T4ZSpec sp255 = { K_STR };
static const T4ZSpec sp257 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp256 = { K_UM, .name="name", .sub=&sp257 };
static const T4ZSpec sp259 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp258 = { K_UM, .name="name", .sub=&sp259 };
static const T4ZSpec sp261 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp260 = { K_UM, .name="name", .sub=&sp261 };
static const T4ZSpec sp263 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp262 = { K_UM, .name="name", .sub=&sp263 };
static const T4ZSpec sp265 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp264 = { K_UM, .name="name", .sub=&sp265 };
static const T4ZSpec sp267 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp266 = { K_UM, .name="name", .sub=&sp267 };
static const T4ZSpec sp269 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp268 = { K_UM, .name="name", .sub=&sp269 };
static const T4ZSpec sp271 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp270 = { K_UM, .name="name", .sub=&sp271 };
static const T4ZSpec sp273 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp272 = { K_UM, .name="name", .sub=&sp273 };
static const T4ZSpec sp275 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp274 = { K_UM, .name="name", .sub=&sp275 };
static const T4ZSpec sp276 = { K_STR };
static const T4ZSpec sp278 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp277 = { K_UM, .name="name", .sub=&sp278 };
static const T4ZSpec sp280 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp279 = { K_UM, .name="name", .sub=&sp280 };
static const T4ZSpec sp282 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp281 = { K_UM, .name="name", .sub=&sp282 };
static const T4ZSpec sp284 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp283 = { K_UM, .name="name", .sub=&sp284 };
static const T4ZSpec sp286 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp285 = { K_UM, .name="name", .sub=&sp286 };
static const T4ZSpec sp288 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp287 = { K_UM, .name="name", .sub=&sp288 };
static const T4ZSpec sp289 = { K_STR };
static const T4ZSpec sp290 = { K_STRUCT, .e="FlameTable", .a=1 };
static const T4ZSpec sp291 = { K_STR };
static const T4ZSpec sp292 = { K_STRUCT, .e="FlameTable", .a=1 };
static const T4ZSpec sp294 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp293 = { K_UM, .name="name", .sub=&sp294 };
static const T4ZSpec sp295 = { K_STR };
static const T4ZSpec sp297 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp296 = { K_UM, .name="name", .sub=&sp297 };
static const T4ZSpec sp299 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp298 = { K_UM, .name="name", .sub=&sp299 };
static const T4ZSpec sp301 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp300 = { K_UM, .name="name", .sub=&sp301 };
static const T4ZSpec sp303 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp302 = { K_UM, .name="name", .sub=&sp303 };
static const T4ZSpec sp305 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp304 = { K_UM, .name="name", .sub=&sp305 };
static const T4ZSpec sp307 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp306 = { K_UM, .name="name", .sub=&sp307 };
static const T4ZSpec sp309 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp308 = { K_UM, .name="name", .sub=&sp309 };
static const T4ZSpec sp311 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp310 = { K_UM, .name="name", .sub=&sp311 };
static const T4ZSpec sp312 = { K_BYTES, .e="8 * originalAiVsAiAccuracyGraphKnotCount" };
static const T4ZSpec sp313 = { K_BYTES, .e="8 * originalAiVsPlayerAccuracyGraphKnotCount" };
static const T4ZSpec sp315 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp314 = { K_UM, .name="name", .sub=&sp315 };
static const T4ZSpec sp317 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp316 = { K_UM, .name="name", .sub=&sp317 };
static const T4ZSpec sp319 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp318 = { K_UM, .name="name", .sub=&sp319 };
static const T4ZSpec sp321 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp320 = { K_UM, .name="name", .sub=&sp321 };
static const T4ZSpec sp323 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp322 = { K_UM, .name="name", .sub=&sp323 };
static const T4ZSpec sp325 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp324 = { K_UM, .name="name", .sub=&sp325 };
static const T4ZSpec sp327 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp326 = { K_UM, .name="name", .sub=&sp327 };
static const T4ZSpec sp329 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp328 = { K_UM, .name="name", .sub=&sp329 };
static const T4ZSpec sp330 = { K_STR };
static const T4ZSpec sp332 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp331 = { K_UM, .name="name", .sub=&sp332 };
static const T4ZSpec sp334 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp333 = { K_UM, .name="name", .sub=&sp334 };
static const T4ZSpec sp336 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp335 = { K_UM, .name="name", .sub=&sp336 };
static const T4ZSpec sp338 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp337 = { K_UM, .name="name", .sub=&sp338 };
static const T4ZSpec sp340 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp339 = { K_UM, .name="name", .sub=&sp340 };
static const T4ZSpec sp342 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp341 = { K_UM, .name="name", .sub=&sp342 };
static const T4ZSpec sp344 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp343 = { K_UM, .name="name", .sub=&sp344 };
static const T4ZSpec sp346 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp345 = { K_UM, .name="name", .sub=&sp346 };
static const T4ZSpec sp348 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp347 = { K_UM, .name="name", .sub=&sp348 };
static const T4ZSpec sp350 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp349 = { K_UM, .name="name", .sub=&sp350 };
static const T4ZSpec sp352 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp351 = { K_UM, .name="name", .sub=&sp352 };
static const T4ZSpec sp354 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp353 = { K_UM, .name="name", .sub=&sp354 };
static const T4ZSpec sp356 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp355 = { K_UM, .name="name", .sub=&sp356 };
static const T4ZSpec sp358 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp357 = { K_UM, .name="name", .sub=&sp358 };
static const T4ZSpec sp360 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp359 = { K_UM, .name="name", .sub=&sp360 };
static const T4ZSpec sp362 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp361 = { K_UM, .name="name", .sub=&sp362 };
static const T4ZSpec sp364 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp363 = { K_UM, .name="name", .sub=&sp364 };
static const T4ZSpec sp366 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp365 = { K_UM, .name="name", .sub=&sp366 };
static const T4ZSpec sp367 = { K_STR };
static const T4ZSpec sp368 = { K_STR };
static const T4ZSpec sp369 = { K_STR };
static const T4ZSpec sp370 = { K_STR };
static const T4ZSpec sp371 = { K_STR };
static const T4ZSpec sp372 = { K_STR };
static const T4ZSpec sp373 = { K_STR };
static const T4ZSpec sp374 = { K_STR };
static const T4ZSpec sp375 = { K_STR };
static const T4ZSpec sp376 = { K_STR };
static const T4ZSpec sp377 = { K_STR };
static const T4ZSpec sp379 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp378 = { K_UM, .name="name", .sub=&sp379 };
static const T4ZSpec sp381 = { K_STRUCT, .e="snd_alias_list_name", .a=1 };
static const T4ZSpec sp380 = { K_UM, .name="name", .sub=&sp381 };
static const T4ZSpec sp382 = { K_STRUCT, .e="XAnimDeltaPartQuat", .a=1 };
static const T4ZSpec sp383 = { K_STRUCT, .e="XAnimPartTrans", .a=1 };
static const T4ZSpec sp385 = { K_INLINE, .e="XAnimDeltaPartQuatDataFrames", .a=1 };
static const T4ZSpec sp386 = { K_INLAT, .a=4, .e="4" };
static const T4ZSpec sp384 = { K_CHOOSE, .e="size", .a=C_GT, .b=0, .sub=&sp385, .sub2=&sp386 };
static const T4ZSpec sp387 = { K_STRUCT, .e="XQuat2", .e2="size + 1" };
static const T4ZSpec sp388 = { K_DYNIDX, .e="numframes", .a=C_LT, .b=256, .e2="size + 1" };
static const T4ZSpec sp390 = { K_INLINE, .e="XAnimPartTransFrames", .a=1 };
static const T4ZSpec sp391 = { K_INLAT, .a=4, .e="12" };
static const T4ZSpec sp389 = { K_CHOOSE, .e="size", .a=C_GT, .b=0, .sub=&sp390, .sub2=&sp391 };
static const T4ZSpec sp392 = { K_DYNFR, .e="smallTrans", .a=C_GT, .b=0, .e2="3 * (size + 1)" };
static const T4ZSpec sp393 = { K_DYNIDX, .e="numframes", .a=C_LT, .b=256, .e2="size + 1" };
static const T4ZSpec sp394 = { K_BYTES, .e="dataByteCount" };
static const T4ZSpec sp395 = { K_BYTES, .e="4 * dataIntCount" };
static const T4ZSpec sp396 = { K_BYTES, .e="2 * dataShortCount" };
static const T4ZSpec sp397 = { K_STRUCT, .e="XAnimDeltaPart", .a=1 };
static const T4ZSpec sp399 = { K_BYTES, .e="indexCount" };
static const T4ZSpec sp400 = { K_BYTES, .e="0" };
static const T4ZSpec sp398 = { K_CHOOSE, .e="numframes", .a=C_LT, .b=256, .sub=&sp399, .sub2=&sp400 };
static const T4ZSpec sp401 = { K_STR };
static const T4ZSpec sp402 = { K_U16, .e="boneCount[9]" };
static const T4ZSpec sp403 = { K_STRUCT, .e="XAnimNotifyInfo", .e2="notifyCount" };
static const T4ZSpec sp404 = { K_BYTES, .e="randomDataByteCount" };
static const T4ZSpec sp405 = { K_BYTES, .e="4 * randomDataIntCount" };
static const T4ZSpec sp406 = { K_BYTES, .e="2 * randomDataShortCount" };
static const T4ZSpec sp407 = { K_STRUCT, .e="DObjAnimMat", .e2="numBones" };
static const T4ZSpec sp408 = { K_STRUCT, .e="XBoneInfo", .e2="numBones" };
static const T4ZSpec sp409 = { K_U16, .e="numBones" };
static const T4ZSpec sp410 = { K_STRUCT, .e="XModelCollSurf_s", .e2="numCollSurfs" };
static const T4ZSpec sp411 = { K_STRUCT, .e="PhysGeomList", .a=1 };
static const T4ZSpec sp412 = { K_PTRARR, .e="Material", .e2="numsurfs" };
static const T4ZSpec sp413 = { K_STR };
static const T4ZSpec sp414 = { K_BYTES, .e="numBones - numRootBones" };
static const T4ZSpec sp415 = { K_BYTES, .e="numBones" };
static const T4ZSpec sp416 = { K_STRUCT, .e="PhysGeomList", .a=1 };
static const T4ZSpec sp417 = { K_STRUCT, .e="XModelQuat", .e2="numBones - numRootBones" };
static const T4ZSpec sp418 = { K_STRUCT, .e="XSurface", .e2="numsurfs" };
static const T4ZSpec sp419 = { K_BYTES, .e="16 * (numBones - numRootBones)" };
static const T4ZSpec sp420 = { K_STRUCT, .e="XModelCollTri_s", .e2="numCollTris" };
static const T4ZSpec sp421 = { K_STR };
static const T4ZSpec sp422 = { K_STRUCT, .e="XModelPiece", .e2="numpieces" };
static const T4ZSpec sp423 = { K_STRUCT, .e="XSurfaceCollisionTree", .a=1 };
static const T4ZSpec sp424 = { K_SKIP };
static const T4ZSpec sp425 = { K_BYTES, .e="6 * triCount" };
static const T4ZSpec sp426 = { K_SKIP };
static const T4ZSpec sp427 = { K_STRUCT, .e="XRigidVertList", .e2="vertListCount" };
static const T4ZSpec sp428 = { K_STRUCT, .e="GfxPackedVertex", .e2="vertCount" };
static const T4ZSpec sp429 = { K_STRUCT, .e="XSurfaceCollisionLeaf", .e2="leafCount" };
static const T4ZSpec sp430 = { K_STRUCT, .e="XSurfaceCollisionNode", .e2="nodeCount" };
static const T4ZSpec sp431 = { K_BYTES, .e="2 * (vertCount[0] + 3 * vertCount[1] + 5 * vertCount[2] + 7 * vertCount[3])" };
static const T4ZSpec sp432 = { K_U16, .e="leafBrushCount" };
static const T4ZSpec sp434 = { K_INLINE, .e="cLeafBrushNodeLeaf_t", .a=0 };
static const T4ZSpec sp435 = { K_INLINE, .e="cLeafBrushNodeChildren_t", .a=0 };
static const T4ZSpec sp433 = { K_CHOOSE, .e="leafBrushCount", .a=C_GT, .b=0, .sub=&sp434, .sub2=&sp435 };
static const T4ZSpec sp436 = { K_BYTES, .e="1" };
static const T4ZSpec sp437 = { K_STRUCT, .e="cbrushside_t", .a=1 };
static const T4ZSpec sp438 = { K_BYTES, .e="12" };
static const T4ZSpec sp439 = { K_STRUCT, .e="cplane_s", .a=1 };
static const T4ZSpec sp440 = { K_STRUCT, .e="CollisionAabbTree", .e2="aabbTreeCount" };
static const T4ZSpec sp441 = { K_STRUCT, .e="CollisionBorder", .e2="borderCount" };
static const T4ZSpec sp442 = { K_STRUCT, .e="cbrush_t", .a=1 };
static const T4ZSpec sp443 = { K_BYTES, .e="numBrushEdges" };
static const T4ZSpec sp444 = { K_BYTES, .e="12 * numBrushVerts" };
static const T4ZSpec sp445 = { K_STRUCT, .e="cbrush_t", .e2="numBrushes" };
static const T4ZSpec sp446 = { K_STRUCT, .e="cbrushside_t", .e2="numBrushSides" };
static const T4ZSpec sp447 = { K_STRUCT, .e="cmodel_t", .e2="numSubModels" };
static const T4ZSpec sp448 = { K_STRUCT, .e="PhysConstraint", .e2="num_constraints" };
static const T4ZSpec sp449 = { K_PSTARR, .e="DynEntityDef", .e2="dynEntCount[i]" };
static const T4ZSpec sp450 = { K_STRUCT, .e="cLeafBrushNode_s", .e2="leafbrushNodesCount" };
static const T4ZSpec sp451 = { K_U16, .e="numLeafBrushes" };
static const T4ZSpec sp452 = { K_STRUCT, .e="cLeaf_t", .e2="numLeafs" };
static const T4ZSpec sp453 = { K_BYTES, .e="4 * numLeafSurfaces" };
static const T4ZSpec sp454 = { K_STRUCT, .e="MapEnts", .a=1 };
static const T4ZSpec sp455 = { K_STRUCT, .e="dmaterial_t", .e2="numMaterials" };
static const T4ZSpec sp456 = { K_STR };
static const T4ZSpec sp457 = { K_STRUCT, .e="cNode_t", .e2="numNodes" };
static const T4ZSpec sp458 = { K_STRUCT, .e="CollisionPartition", .e2="partitionCount" };
static const T4ZSpec sp459 = { K_STRUCT, .e="cplane_s", .e2="planeCount" };
static const T4ZSpec sp460 = { K_STRUCT, .e="cStaticModel_s", .e2="numStaticModels" };
static const T4ZSpec sp461 = { K_BYTES, .e="((3 * triCount + 31) // 32) * 4" };
static const T4ZSpec sp462 = { K_U16, .e="3 * triCount" };
static const T4ZSpec sp463 = { K_U16, .e="nuinds" };
static const T4ZSpec sp464 = { K_BYTES, .e="12 * vertCount" };
static const T4ZSpec sp465 = { K_BYTES, .e="numClusters * clusterBytes" };
static const T4ZSpec sp467 = { K_INLINE, .e="Operand", .a=0 };
static const T4ZSpec sp468 = { K_SKIP };
static const T4ZSpec sp466 = { K_CHOISK, .e="type", .a=1, .ents=(const T4ZChooseEnt[]){KENT(1, &sp467)}, .sub=&sp468 };
static const T4ZSpec sp469 = { K_STR };
static const T4ZSpec sp470 = { K_STR };
static const T4ZSpec sp471 = { K_STR };
static const T4ZSpec sp472 = { K_STR };
static const T4ZSpec sp473 = { K_STR };
static const T4ZSpec sp474 = { K_STR };
static const T4ZSpec sp475 = { K_STR };
static const T4ZSpec sp476 = { K_STR };
static const T4ZSpec sp477 = { K_STR };
static const T4ZSpec sp478 = { K_STR };
static const T4ZSpec sp479 = { K_STR };
static const T4ZSpec sp480 = { K_STRUCT, .e="ItemKeyHandler", .a=1 };
static const T4ZSpec sp481 = { K_STR };
static const T4ZSpec sp482 = { K_SKIP };
static const T4ZSpec sp483 = { K_STR };
static const T4ZSpec sp486 = { K_STRUCT, .e="editFieldDef_s", .a=1 };
static const T4ZSpec sp485 = { K_UM, .name="editField", .sub=&sp486 };
static const T4ZSpec sp488 = { K_STRUCT, .e="editFieldDef_s", .a=1 };
static const T4ZSpec sp487 = { K_UM, .name="editField", .sub=&sp488 };
static const T4ZSpec sp490 = { K_STRUCT, .e="listBoxDef_s", .a=1 };
static const T4ZSpec sp489 = { K_UM, .name="listBox", .sub=&sp490 };
static const T4ZSpec sp492 = { K_STRUCT, .e="editFieldDef_s", .a=1 };
static const T4ZSpec sp491 = { K_UM, .name="editField", .sub=&sp492 };
static const T4ZSpec sp494 = { K_STRUCT, .e="editFieldDef_s", .a=1 };
static const T4ZSpec sp493 = { K_UM, .name="editField", .sub=&sp494 };
static const T4ZSpec sp496 = { K_STRUCT, .e="editFieldDef_s", .a=1 };
static const T4ZSpec sp495 = { K_UM, .name="editField", .sub=&sp496 };
static const T4ZSpec sp498 = { K_STRUCT, .e="multiDef_s", .a=1 };
static const T4ZSpec sp497 = { K_UM, .name="multi", .sub=&sp498 };
static const T4ZSpec sp500 = { K_STR };
static const T4ZSpec sp499 = { K_UM, .name="enumDvarName", .sub=&sp500 };
static const T4ZSpec sp502 = { K_STRUCT, .e="editFieldDef_s", .a=1 };
static const T4ZSpec sp501 = { K_UM, .name="editField", .sub=&sp502 };
static const T4ZSpec sp504 = { K_STRUCT, .e="editFieldDef_s", .a=1 };
static const T4ZSpec sp503 = { K_UM, .name="editField", .sub=&sp504 };
static const T4ZSpec sp506 = { K_STRUCT, .e="editFieldDef_s", .a=1 };
static const T4ZSpec sp505 = { K_UM, .name="editField", .sub=&sp506 };
static const T4ZSpec sp508 = { K_STRUCT, .e="editFieldDef_s", .a=1 };
static const T4ZSpec sp507 = { K_UM, .name="editField", .sub=&sp508 };
static const T4ZSpec sp509 = { K_SKIP };
static const T4ZSpec sp484 = { K_CHOISK, .e="type", .a=12, .ents=(const T4ZChooseEnt[]){KENT(0, &sp485), KENT(4, &sp487), KENT(6, &sp489), KENT(9, &sp491), KENT(10, &sp493), KENT(11, &sp495), KENT(12, &sp497), KENT(13, &sp499), KENT(14, &sp501), KENT(16, &sp503), KENT(17, &sp505), KENT(18, &sp507)}, .sub=&sp509 };
static const T4ZSpec sp510 = { K_STR };
static const T4ZSpec sp511 = { K_STR };
static const T4ZSpec sp512 = { K_STR };
static const T4ZSpec sp513 = { K_PTRARR, .e="itemDef_s", .e2="itemCount" };
static const T4ZSpec sp514 = { K_STR };
static const T4ZSpec sp515 = { K_STR };
static const T4ZSpec sp516 = { K_STR };
static const T4ZSpec sp517 = { K_STRUCT, .e="ItemKeyHandler", .a=1 };
static const T4ZSpec sp518 = { K_STR };
static const T4ZSpec sp519 = { K_STR };
static const T4ZSpec sp520 = { K_EARRP, .e="str", .e2="32" };
static const T4ZSpec sp521 = { K_EARRP, .e="str", .e2="32" };
static const T4ZSpec sp522 = { K_STRUCT, .e="pathlink_s", .e2="totalLinkCount" };
static const T4ZSpec sp523 = { K_SKIP };
static const T4ZSpec sp524 = { K_SKIP };
static const T4ZSpec sp525 = { K_SKIP };
static const T4ZSpec sp526 = { K_SKIP };
static const T4ZSpec sp527 = { K_U16, .e="nodeCount" };
static const T4ZSpec sp530 = { K_PARR2, .e="pathnode_tree_t", .a=2 };
static const T4ZSpec sp529 = { K_UM, .name="child", .sub=&sp530 };
static const T4ZSpec sp531 = { K_INLINE, .e="pathnode_tree_nodes_t", .a=0 };
static const T4ZSpec sp528 = { K_CHOOSE, .e="axis", .a=C_GE_S, .b=0, .sub=&sp529, .sub2=&sp531 };
static const T4ZSpec sp532 = { K_STR };
static const T4ZSpec sp533 = { K_STR };
static const T4ZSpec sp534 = { K_STRUCT, .e="snd_alias_t", .e2="count" };
static const T4ZSpec sp535 = { K_STR };
static const T4ZSpec sp536 = { K_STR };
static const T4ZSpec sp537 = { K_STR };
static const T4ZSpec sp538 = { K_STRUCT, .e="SoundFile", .a=1 };
static const T4ZSpec sp539 = { K_STR };
static const T4ZSpec sp540 = { K_BYTES, .e="data_size" };
static const T4ZSpec sp541 = { K_PTRARR, .e="expressionEntry", .e2="numEntries" };
static const T4ZSpec sp542 = { K_BYTES, .e="8 * M * N" };
static const T4ZSpec sp543 = { K_STRUCT, .e="GfxImage", .a=1 };
static const T4ZSpec sp544 = { K_BYTES, .e="4 * M * N" };
static const T4ZSpec sp545 = { K_STR };
static const T4ZSpec sp546 = { K_STR };

static const T4ZAnnEnt T4Z_ANN[] = {
  { "BrushWrapper", "baseAdjacentSide", &sp0 },
  { "BrushWrapper", "planes", &sp1 },
  { "BrushWrapper", "sides", &sp2 },
  { "BrushWrapper", "verts", &sp3 },
  { "CollisionPartition", "borders", &sp4 },
  { "ComBurnableCell", "data", &sp5 },
  { "ComWorld", "burnableCells", &sp6 },
  { "ComWorld", "primaryLights", &sp7 },
  { "ComWorld", "waterCells", &sp8 },
  { "DestructibleDef", "name", &sp9 },
  { "DestructibleDef", "pieces", &sp10 },
  { "DestructibleModel", "breakNotify", &sp11 },
  { "DestructibleModel", "breakSound", &sp12 },
  { "DestructibleModel", "loopSound", &sp14 },
  { "DestructibleModel", "spawnModel", &sp15 },
  { "DestructiblePiece", "attachBone", &sp16 },
  { "DestructiblePiece", "burnSound", &sp17 },
  { "DestructiblePiece", "damageSound", &sp19 },
  { "DestructiblePiece", "dmodels", &sp21 },
  { "FlameTable", "flameCooldownSound", &sp22 },
  { "FlameTable", "flameIgniteSound", &sp24 },
  { "FlameTable", "flameOffLoopSound", &sp26 },
  { "FlameTable", "flameOnLoopSound", &sp28 },
  { "FlameTable", "name", &sp30 },
  { "Font_s", "fontName", &sp31 },
  { "Font_s", "glyphs", &sp32 },
  { "FxEffectDef", "elemDefs", &sp33 },
  { "FxEffectDef", "name", &sp34 },
  { "FxElemDef", "effectEmitted", &sp35 },
  { "FxElemDef", "effectOnDeath", &sp37 },
  { "FxElemDef", "effectOnImpact", &sp39 },
  { "FxElemDef", "trailDef", &sp41 },
  { "FxElemDef", "velSamples", &sp42 },
  { "FxElemDef", "visSamples", &sp43 },
  { "FxElemDef", "visuals", &sp44 },
  { "FxElemVisuals", "anonymous", &sp63 },
  { "FxElemVisuals", "effectDef", &sp64 },
  { "FxElemVisuals", "material", &sp66 },
  { "FxElemVisuals", "model", &sp67 },
  { "FxElemVisuals", "soundName", &sp68 },
  { "FxImpactEntry", "flesh", &sp69 },
  { "FxImpactEntry", "nonflesh", &sp70 },
  { "FxImpactTable", "name", &sp71 },
  { "FxImpactTable", "table", &sp72 },
  { "FxTrailDef", "inds", &sp73 },
  { "FxTrailDef", "verts", &sp74 },
  { "GfxAabbTree", "smodelIndexes", &sp75 },
  { "GfxCell", "aabbTree", &sp76 },
  { "GfxCell", "cullGroups", &sp77 },
  { "GfxCell", "portals", &sp78 },
  { "GfxCell", "reflectionProbes", &sp79 },
  { "GfxImage", "name", &sp80 },
  { "GfxImage", "texture", &sp81 },
  { "GfxImageLoadDef", "data", &sp83 },
  { "GfxLightDef", "name", &sp84 },
  { "GfxLightGrid", "colors", &sp85 },
  { "GfxLightGrid", "entries", &sp86 },
  { "GfxLightGrid", "rawRowData", &sp87 },
  { "GfxLightGrid", "rowDataStart", &sp88 },
  { "GfxLightRegion", "hulls", &sp89 },
  { "GfxLightRegionHull", "axis", &sp90 },
  { "GfxPixelShaderLoadDef", "program", &sp91 },
  { "GfxPortal", "vertices", &sp92 },
  { "GfxPortalWritable", "hullPoints", &sp93 },
  { "GfxPortalWritable", "queuedParent", &sp94 },
  { "GfxShadowGeometry", "smodelIndex", &sp95 },
  { "GfxShadowGeometry", "sortedSurfIndex", &sp96 },
  { "GfxVertexShaderLoadDef", "program", &sp97 },
  { "GfxWaterBuffer", "buffer", &sp98 },
  { "GfxWorld", "baseName", &sp99 },
  { "GfxWorld", "cellCasterBits", &sp100 },
  { "GfxWorld", "cells", &sp101 },
  { "GfxWorld", "coronas", &sp102 },
  { "GfxWorld", "indices", &sp103 },
  { "GfxWorld", "lightRegion", &sp104 },
  { "GfxWorld", "lightmapPrimaryTextures", &sp105 },
  { "GfxWorld", "lightmapSecondaryTextures", &sp108 },
  { "GfxWorld", "lightmaps", &sp111 },
  { "GfxWorld", "materialMemory", &sp112 },
  { "GfxWorld", "models", &sp113 },
  { "GfxWorld", "name", &sp114 },
  { "GfxWorld", "nonSunPrimaryLightForModelDynEnt", &sp115 },
  { "GfxWorld", "outdoorImage", &sp116 },
  { "GfxWorld", "primaryLightDynEntShadowVis", &sp117 },
  { "GfxWorld", "primaryLightEntityShadowVis", &sp118 },
  { "GfxWorld", "reflectionProbeTextures", &sp119 },
  { "GfxWorld", "reflectionProbes", &sp122 },
  { "GfxWorld", "sceneDynBrush", &sp123 },
  { "GfxWorld", "sceneDynModel", &sp124 },
  { "GfxWorld", "shadowGeom", &sp125 },
  { "GfxWorld", "skyBoxModel", &sp126 },
  { "GfxWorld", "skyImage", &sp127 },
  { "GfxWorld", "skyStartSurfs", &sp128 },
  { "GfxWorld", "sunLight", &sp129 },
  { "GfxWorld", "waterBuffers", &sp130 },
  { "GfxWorld", "worldLodChains", &sp131 },
  { "GfxWorld", "worldLodInfos", &sp132 },
  { "GfxWorld", "worldLodSurfaces", &sp133 },
  { "GfxWorldDpvsDynamic", "dynEntCellBits", &sp134 },
  { "GfxWorldDpvsDynamic", "dynEntVisData", &sp135 },
  { "GfxWorldDpvsPlanes", "nodes", &sp136 },
  { "GfxWorldDpvsPlanes", "planes", &sp137 },
  { "GfxWorldDpvsPlanes", "sceneEntCellBits", &sp138 },
  { "GfxWorldDpvsStatic", "cullGroups", &sp139 },
  { "GfxWorldDpvsStatic", "lodData", &sp140 },
  { "GfxWorldDpvsStatic", "smodelDrawInsts", &sp141 },
  { "GfxWorldDpvsStatic", "smodelInsts", &sp142 },
  { "GfxWorldDpvsStatic", "smodelVisData", &sp143 },
  { "GfxWorldDpvsStatic", "sortedSurfIndex", &sp144 },
  { "GfxWorldDpvsStatic", "surfaceCastsSunShadow", &sp145 },
  { "GfxWorldDpvsStatic", "surfaceMaterials", &sp146 },
  { "GfxWorldDpvsStatic", "surfaceVisData", &sp147 },
  { "GfxWorldDpvsStatic", "surfaces", &sp148 },
  { "GfxWorldVertexData", "vertices", &sp149 },
  { "GfxWorldVertexData", "worldVb", &sp150 },
  { "GfxWorldVertexLayerData", "data", &sp151 },
  { "GfxWorldVertexLayerData", "layerVb", &sp152 },
  { "ItemKeyHandler", "action", &sp153 },
  { "ItemKeyHandler", "next", &sp154 },
  { "LoadedSound", "name", &sp155 },
  { "LoadedSound", "sound", &sp156 },
  { "LocalizeEntry", "name", &sp158 },
  { "LocalizeEntry", "value", &sp159 },
  { "MapEnts", "entityString", &sp160 },
  { "MapEnts", "name", &sp161 },
  { "Material", "constantTable", &sp162 },
  { "Material", "stateBitsTable", &sp163 },
  { "Material", "textureTable", &sp164 },
  { "MaterialInfo", "name", &sp165 },
  { "MaterialPass", "args", &sp166 },
  { "MaterialPass", "pixelShader", &sp167 },
  { "MaterialPass", "vertexShader", &sp168 },
  { "MaterialPixelShader", "name", &sp169 },
  { "MaterialPixelShaderProgram", "ps", &sp170 },
  { "MaterialShaderArgument", "u", &sp171 },
  { "MaterialTechnique", "name", &sp177 },
  { "MaterialTechnique", "passArray", &sp178 },
  { "MaterialTechniqueSet", "name", &sp179 },
  { "MaterialTechniqueSet", "remappedTechniqueSet", &sp180 },
  { "MaterialTechniqueSet", "techniques", &sp181 },
  { "MaterialTextureDef", "u", &sp182 },
  { "MaterialVertexShader", "name", &sp187 },
  { "MaterialVertexShaderProgram", "vs", &sp188 },
  { "MaterialVertexStreamRouting", "decl", &sp189 },
  { "MenuList", "menus", &sp190 },
  { "MenuList", "name", &sp191 },
  { "Operand", "internals", &sp192 },
  { "PackIndex", "entries", &sp196 },
  { "PackIndex", "name", &sp197 },
  { "PathData", "chainNodeForNode", &sp198 },
  { "PathData", "nodeForChainNode", &sp199 },
  { "PathData", "nodeTree", &sp200 },
  { "PathData", "nodes", &sp201 },
  { "PathData", "pathVis", &sp202 },
  { "PhysConstraint", "target_bone1", &sp203 },
  { "PhysConstraint", "target_bone2", &sp204 },
  { "PhysConstraints", "data", &sp205 },
  { "PhysConstraints", "name", &sp206 },
  { "PhysGeomInfo", "brush", &sp207 },
  { "PhysGeomList", "geoms", &sp208 },
  { "PhysPreset", "name", &sp209 },
  { "PhysPreset", "sndAliasPrefix", &sp210 },
  { "PrimedSound", "buffer", &sp211 },
  { "PrimedSound", "name", &sp212 },
  { "RawFile", "buffer", &sp213 },
  { "RawFile", "name", &sp214 },
  { "SndDriverGlobals", "name", &sp215 },
  { "SoundFile", "u", &sp216 },
  { "StreamFileName", "dir", &sp221 },
  { "StreamFileName", "name", &sp222 },
  { "StreamedSound", "primeSnd", &sp223 },
  { "StringTable", "name", &sp224 },
  { "StringTable", "values", &sp225 },
  { "WeaponDef", "aiVsAiAccuracyGraphKnots", &sp226 },
  { "WeaponDef", "aiVsAiAccuracyGraphName", &sp227 },
  { "WeaponDef", "aiVsPlayerAccuracyGraphKnots", &sp228 },
  { "WeaponDef", "aiVsPlayerAccuracyGraphName", &sp229 },
  { "WeaponDef", "altSwitchSound", &sp230 },
  { "WeaponDef", "altSwitchSoundPlayer", &sp232 },
  { "WeaponDef", "ammoPickupSound", &sp234 },
  { "WeaponDef", "ammoPickupSoundPlayer", &sp236 },
  { "WeaponDef", "bounceSound", &sp238 },
  { "WeaponDef", "breakdownSound", &sp240 },
  { "WeaponDef", "breakdownSoundPlayer", &sp242 },
  { "WeaponDef", "crackSound", &sp244 },
  { "WeaponDef", "crouchMountedWeapdef", &sp246 },
  { "WeaponDef", "deploySound", &sp247 },
  { "WeaponDef", "deploySoundPlayer", &sp249 },
  { "WeaponDef", "detonateSound", &sp251 },
  { "WeaponDef", "detonateSoundPlayer", &sp253 },
  { "WeaponDef", "dropHintString", &sp255 },
  { "WeaponDef", "emptyFireSound", &sp256 },
  { "WeaponDef", "emptyFireSoundPlayer", &sp258 },
  { "WeaponDef", "finishBreakdownSound", &sp260 },
  { "WeaponDef", "finishBreakdownSoundPlayer", &sp262 },
  { "WeaponDef", "finishDeploySound", &sp264 },
  { "WeaponDef", "finishDeploySoundPlayer", &sp266 },
  { "WeaponDef", "fireLastSound", &sp268 },
  { "WeaponDef", "fireLastSoundPlayer", &sp270 },
  { "WeaponDef", "fireLoopSound", &sp272 },
  { "WeaponDef", "fireLoopSoundPlayer", &sp274 },
  { "WeaponDef", "fireRumble", &sp276 },
  { "WeaponDef", "fireSound", &sp277 },
  { "WeaponDef", "fireSoundPlayer", &sp279 },
  { "WeaponDef", "fireStopSound", &sp281 },
  { "WeaponDef", "fireStopSoundPlayer", &sp283 },
  { "WeaponDef", "firstRaiseSound", &sp285 },
  { "WeaponDef", "firstRaiseSoundPlayer", &sp287 },
  { "WeaponDef", "flameTableFirstPerson", &sp289 },
  { "WeaponDef", "flameTableFirstPersonPtr", &sp290 },
  { "WeaponDef", "flameTableThirdPerson", &sp291 },
  { "WeaponDef", "flameTableThirdPersonPtr", &sp292 },
  { "WeaponDef", "meleeHitSound", &sp293 },
  { "WeaponDef", "meleeImpactRumble", &sp295 },
  { "WeaponDef", "meleeMissSound", &sp296 },
  { "WeaponDef", "meleeSwipeSound", &sp298 },
  { "WeaponDef", "meleeSwipeSoundPlayer", &sp300 },
  { "WeaponDef", "mortarShellSound", &sp302 },
  { "WeaponDef", "nightVisionRemoveSound", &sp304 },
  { "WeaponDef", "nightVisionRemoveSoundPlayer", &sp306 },
  { "WeaponDef", "nightVisionWearSound", &sp308 },
  { "WeaponDef", "nightVisionWearSoundPlayer", &sp310 },
  { "WeaponDef", "originalAiVsAiAccuracyGraphKnots", &sp312 },
  { "WeaponDef", "originalAiVsPlayerAccuracyGraphKnots", &sp313 },
  { "WeaponDef", "overheatSound", &sp314 },
  { "WeaponDef", "overheatSoundPlayer", &sp316 },
  { "WeaponDef", "pickupSound", &sp318 },
  { "WeaponDef", "pickupSoundPlayer", &sp320 },
  { "WeaponDef", "projDudSound", &sp322 },
  { "WeaponDef", "projExplosionSound", &sp324 },
  { "WeaponDef", "projIgnitionSound", &sp326 },
  { "WeaponDef", "projectileSound", &sp328 },
  { "WeaponDef", "proneMountedWeapdef", &sp330 },
  { "WeaponDef", "pullbackSound", &sp331 },
  { "WeaponDef", "pullbackSoundPlayer", &sp333 },
  { "WeaponDef", "putawaySound", &sp335 },
  { "WeaponDef", "putawaySoundPlayer", &sp337 },
  { "WeaponDef", "raiseSound", &sp339 },
  { "WeaponDef", "raiseSoundPlayer", &sp341 },
  { "WeaponDef", "rechamberSound", &sp343 },
  { "WeaponDef", "rechamberSoundPlayer", &sp345 },
  { "WeaponDef", "reloadEmptySound", &sp347 },
  { "WeaponDef", "reloadEmptySoundPlayer", &sp349 },
  { "WeaponDef", "reloadEndSound", &sp351 },
  { "WeaponDef", "reloadEndSoundPlayer", &sp353 },
  { "WeaponDef", "reloadSound", &sp355 },
  { "WeaponDef", "reloadSoundPlayer", &sp357 },
  { "WeaponDef", "reloadStartSound", &sp359 },
  { "WeaponDef", "reloadStartSoundPlayer", &sp361 },
  { "WeaponDef", "rotateLoopSound", &sp363 },
  { "WeaponDef", "rotateLoopSoundPlayer", &sp365 },
  { "WeaponDef", "standMountedWeapdef", &sp367 },
  { "WeaponDef", "szAltWeaponName", &sp368 },
  { "WeaponDef", "szAmmoName", &sp369 },
  { "WeaponDef", "szClipName", &sp370 },
  { "WeaponDef", "szDisplayName", &sp371 },
  { "WeaponDef", "szInternalName", &sp372 },
  { "WeaponDef", "szModeName", &sp373 },
  { "WeaponDef", "szOverlayName", &sp374 },
  { "WeaponDef", "szScript", &sp375 },
  { "WeaponDef", "szSharedAmmoCapName", &sp376 },
  { "WeaponDef", "szUseHintString", &sp377 },
  { "WeaponDef", "tankShellSound", &sp378 },
  { "WeaponDef", "whizbySound", &sp380 },
  { "XAnimDeltaPart", "quat", &sp382 },
  { "XAnimDeltaPart", "trans", &sp383 },
  { "XAnimDeltaPartQuat", "u", &sp384 },
  { "XAnimDeltaPartQuatDataFrames", "frames", &sp387 },
  { "XAnimDeltaPartQuatDataFrames", "indices", &sp388 },
  { "XAnimPartTrans", "u", &sp389 },
  { "XAnimPartTransFrames", "frames", &sp392 },
  { "XAnimPartTransFrames", "indices", &sp393 },
  { "XAnimParts", "dataByte", &sp394 },
  { "XAnimParts", "dataInt", &sp395 },
  { "XAnimParts", "dataShort", &sp396 },
  { "XAnimParts", "deltaPart", &sp397 },
  { "XAnimParts", "indices", &sp398 },
  { "XAnimParts", "name", &sp401 },
  { "XAnimParts", "names", &sp402 },
  { "XAnimParts", "notify", &sp403 },
  { "XAnimParts", "randomDataByte", &sp404 },
  { "XAnimParts", "randomDataInt", &sp405 },
  { "XAnimParts", "randomDataShort", &sp406 },
  { "XModel", "baseMat", &sp407 },
  { "XModel", "boneInfo", &sp408 },
  { "XModel", "boneNames", &sp409 },
  { "XModel", "collSurfs", &sp410 },
  { "XModel", "collmap", &sp411 },
  { "XModel", "materialHandles", &sp412 },
  { "XModel", "name", &sp413 },
  { "XModel", "parentList", &sp414 },
  { "XModel", "partClassification", &sp415 },
  { "XModel", "physGeoms", &sp416 },
  { "XModel", "quats", &sp417 },
  { "XModel", "surfs", &sp418 },
  { "XModel", "trans", &sp419 },
  { "XModelCollSurf_s", "collTris", &sp420 },
  { "XModelPieces", "name", &sp421 },
  { "XModelPieces", "pieces", &sp422 },
  { "XRigidVertList", "collisionTree", &sp423 },
  { "XSurface", "indexBuffer", &sp424 },
  { "XSurface", "triIndices", &sp425 },
  { "XSurface", "vb0", &sp426 },
  { "XSurface", "vertList", &sp427 },
  { "XSurface", "verts0", &sp428 },
  { "XSurfaceCollisionTree", "leafs", &sp429 },
  { "XSurfaceCollisionTree", "nodes", &sp430 },
  { "XSurfaceVertexInfo", "vertsBlend", &sp431 },
  { "cLeafBrushNodeLeaf_t", "brushes", &sp432 },
  { "cLeafBrushNode_s", "data", &sp433 },
  { "cbrush_t", "baseAdjacentSide", &sp436 },
  { "cbrush_t", "sides", &sp437 },
  { "cbrush_t", "verts", &sp438 },
  { "cbrushside_t", "plane", &sp439 },
  { "clipMap_t", "aabbTrees", &sp440 },
  { "clipMap_t", "borders", &sp441 },
  { "clipMap_t", "box_brush", &sp442 },
  { "clipMap_t", "brushEdges", &sp443 },
  { "clipMap_t", "brushVerts", &sp444 },
  { "clipMap_t", "brushes", &sp445 },
  { "clipMap_t", "brushsides", &sp446 },
  { "clipMap_t", "cmodels", &sp447 },
  { "clipMap_t", "constraints", &sp448 },
  { "clipMap_t", "dynEntDefList", &sp449 },
  { "clipMap_t", "leafbrushNodes", &sp450 },
  { "clipMap_t", "leafbrushes", &sp451 },
  { "clipMap_t", "leafs", &sp452 },
  { "clipMap_t", "leafsurfaces", &sp453 },
  { "clipMap_t", "mapEnts", &sp454 },
  { "clipMap_t", "materials", &sp455 },
  { "clipMap_t", "name", &sp456 },
  { "clipMap_t", "nodes", &sp457 },
  { "clipMap_t", "partitions", &sp458 },
  { "clipMap_t", "planes", &sp459 },
  { "clipMap_t", "staticModelList", &sp460 },
  { "clipMap_t", "triEdgeIsWalkable", &sp461 },
  { "clipMap_t", "triIndices", &sp462 },
  { "clipMap_t", "uinds", &sp463 },
  { "clipMap_t", "verts", &sp464 },
  { "clipMap_t", "visibility", &sp465 },
  { "expressionEntry", "data", &sp466 },
  { "itemDef_s", "action", &sp469 },
  { "itemDef_s", "dvar", &sp470 },
  { "itemDef_s", "dvarTest", &sp471 },
  { "itemDef_s", "enableDvar", &sp472 },
  { "itemDef_s", "leaveFocus", &sp473 },
  { "itemDef_s", "mouseEnter", &sp474 },
  { "itemDef_s", "mouseEnterText", &sp475 },
  { "itemDef_s", "mouseExit", &sp476 },
  { "itemDef_s", "mouseExitText", &sp477 },
  { "itemDef_s", "onAccept", &sp478 },
  { "itemDef_s", "onFocus", &sp479 },
  { "itemDef_s", "onKey", &sp480 },
  { "itemDef_s", "onListboxSelectionChange", &sp481 },
  { "itemDef_s", "parent", &sp482 },
  { "itemDef_s", "text", &sp483 },
  { "itemDef_s", "typeData", &sp484 },
  { "listBoxDef_s", "onDoubleClick", &sp510 },
  { "menuDef_t", "allowedBinding", &sp511 },
  { "menuDef_t", "font", &sp512 },
  { "menuDef_t", "items", &sp513 },
  { "menuDef_t", "onClose", &sp514 },
  { "menuDef_t", "onESC", &sp515 },
  { "menuDef_t", "onFocus", &sp516 },
  { "menuDef_t", "onKey", &sp517 },
  { "menuDef_t", "onOpen", &sp518 },
  { "menuDef_t", "soundName", &sp519 },
  { "multiDef_s", "dvarList", &sp520 },
  { "multiDef_s", "dvarStr", &sp521 },
  { "pathnode_constant_t", "Links", &sp522 },
  { "pathnode_dynamic_t", "pOwner", &sp523 },
  { "pathnode_transient_t", "pNextOpen", &sp524 },
  { "pathnode_transient_t", "pParent", &sp525 },
  { "pathnode_transient_t", "pPrevOpen", &sp526 },
  { "pathnode_tree_nodes_t", "nodes", &sp527 },
  { "pathnode_tree_t", "u", &sp528 },
  { "snd_alias_list_name", "soundName", &sp532 },
  { "snd_alias_list_t", "aliasName", &sp533 },
  { "snd_alias_list_t", "head", &sp534 },
  { "snd_alias_t", "aliasName", &sp535 },
  { "snd_alias_t", "chainAliasName", &sp536 },
  { "snd_alias_t", "secondaryAliasName", &sp537 },
  { "snd_alias_t", "soundFile", &sp538 },
  { "snd_alias_t", "subtitle", &sp539 },
  { "snd_asset", "data", &sp540 },
  { "statement_s", "entries", &sp541 },
  { "water_t", "H0", &sp542 },
  { "water_t", "image", &sp543 },
  { "water_t", "wTerm", &sp544 },
  { "windowDef_t", "group", &sp545 },
  { "windowDef_t", "name", &sp546 },
};
#define T4Z_ANN_N (sizeof(T4Z_ANN)/sizeof(T4Z_ANN[0]))

/* ---- structs (blobs MSVC 32 bits) ---- */
static const T4ZField f_0[] = {
  { "surfId", 0, 2, 0, -1, "uint16_t" },
};
static const T4ZField f_1[] = {
  { "mins", 0, 12, 0, 3, "float" },
  { "contents", 12, 4, 0, -1, "int" },
  { "maxs", 16, 12, 0, 3, "float" },
  { "numsides", 28, 4, 0, -1, "unsigned int" },
  { "sides", 32, 4, 1, -1, "cbrushside_t*" },
  { "axialMaterialNum", 36, 12, 0, 6, "int16_t" },
  { "baseAdjacentSide", 48, 4, 1, -1, "cbrushedge_t*" },
  { "firstAdjacentSideOffsets", 52, 12, 0, 6, "int16_t" },
  { "edgeCount", 64, 6, 0, 6, "char" },
  { "numverts", 72, 4, 0, -1, "unsigned int" },
  { "verts", 76, 4, 1, -1, "vec3_t*" },
  { "totalEdgeCount", 80, 4, 0, -1, "int" },
  { "planes", 84, 4, 1, -1, "cplane_s*" },
};
static const T4ZField f_2[] = {
  { "platform", 0, 8, 0, 2, "int" },
};
static const T4ZField f_3[] = {
  { "origin", 0, 12, 0, 3, "float" },
  { "materialIndex", 12, 2, 0, -1, "uint16_t" },
  { "childCount", 14, 2, 0, -1, "uint16_t" },
  { "halfSize", 16, 12, 0, 3, "float" },
  { "u", 28, 4, 0, -1, "CollisionAabbTreeIndex" },
};
static const T4ZField f_4[] = {
  { "distEq", 0, 12, 0, 3, "float" },
  { "zBase", 12, 4, 0, -1, "float" },
  { "zSlope", 16, 4, 0, -1, "float" },
  { "start", 20, 4, 0, -1, "float" },
  { "length", 24, 4, 0, -1, "float" },
};
static const T4ZField f_5[] = {
  { "triCount", 0, 1, 0, -1, "unsigned char" },
  { "borderCount", 1, 1, 0, -1, "unsigned char" },
  { "firstTri", 4, 4, 0, -1, "int" },
  { "nuinds", 8, 4, 0, -1, "int" },
  { "fuind", 12, 4, 0, -1, "int" },
  { "borders", 16, 4, 1, -1, "CollisionBorder*" },
};
static const T4ZField f_6[] = {
  { "x", 0, 4, 0, -1, "int" },
  { "y", 4, 4, 0, -1, "int" },
  { "data", 8, 4, 1, -1, "ComBurnableSample*" },
};
static const T4ZField f_7[] = {
  { "minx", 0, 4, 0, -1, "int" },
  { "miny", 4, 4, 0, -1, "int" },
  { "maxx", 8, 4, 0, -1, "int" },
  { "maxy", 12, 4, 0, -1, "int" },
};
static const T4ZField f_8[] = {
  { "state", 0, 1, 0, -1, "unsigned char" },
};
static const T4ZField f_9[] = {
  { "type", 0, 1, 0, -1, "unsigned char" },
  { "canUseShadowMap", 1, 1, 0, -1, "unsigned char" },
  { "exponent", 2, 1, 0, -1, "unsigned char" },
  { "priority", 3, 1, 0, -1, "char" },
  { "cullDist", 4, 2, 0, -1, "int16_t" },
  { "color", 8, 12, 0, 3, "float" },
  { "dir", 20, 12, 0, 3, "float" },
  { "origin", 32, 12, 0, 3, "float" },
  { "radius", 44, 4, 0, -1, "float" },
  { "cosHalfFovOuter", 48, 4, 0, -1, "float" },
  { "cosHalfFovInner", 52, 4, 0, -1, "float" },
  { "cosHalfFovExpanded", 56, 4, 0, -1, "float" },
  { "rotationLimit", 60, 4, 0, -1, "float" },
  { "translationLimit", 64, 4, 0, -1, "float" },
  { "defName", 68, 4, 1, -1, "char*" },
};
static const T4ZField f_10[] = {
  { "waterheight", 0, 2, 0, -1, "int16_t" },
  { "flooroffset", 2, 1, 0, -1, "char" },
  { "shoredist", 3, 1, 0, -1, "unsigned char" },
  { "color", 4, 4, 0, 4, "unsigned char" },
};
static const T4ZField f_11[] = {
  { "minx", 0, 4, 0, -1, "int" },
  { "miny", 4, 4, 0, -1, "int" },
  { "maxx", 8, 4, 0, -1, "int" },
  { "maxy", 12, 4, 0, -1, "int" },
};
static const T4ZField f_12[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "isInUse", 4, 4, 0, -1, "int" },
  { "primaryLightCount", 8, 4, 0, -1, "unsigned int" },
  { "primaryLights", 12, 4, 1, -1, "ComPrimaryLight*" },
  { "waterHeader", 16, 16, 0, -1, "ComWaterHeader" },
  { "numWaterCells", 32, 4, 0, -1, "unsigned int" },
  { "waterCells", 36, 4, 1, -1, "ComWaterCell*" },
  { "burnableHeader", 40, 16, 0, -1, "ComBurnableHeader" },
  { "numBurnableCells", 56, 4, 0, -1, "unsigned int" },
  { "burnableCells", 60, 4, 1, -1, "ComBurnableCell*" },
};
static const T4ZField f_13[] = {
  { "quat", 0, 16, 0, -1, "vec4_t" },
  { "trans", 16, 12, 0, -1, "vec3_t" },
  { "transWeight", 28, 4, 0, -1, "float" },
};
static const T4ZField f_14[] = {
  { "axis", 0, 48, 0, 12, "float" },
  { "origin", 48, 16, 0, 4, "float" },
};
static const T4ZField f_15[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "baseModel", 4, 4, 1, -1, "XModel*" },
  { "numPieces", 8, 4, 0, -1, "int" },
  { "pieces", 12, 4, 1, -1, "DestructiblePiece*" },
  { "clientOnly", 16, 4, 0, -1, "int" },
};
static const T4ZField f_16[] = {
  { "model", 0, 4, 1, -1, "XModel*" },
  { "breakHealth", 4, 4, 0, -1, "float" },
  { "flags", 8, 4, 0, -1, "unsigned int" },
  { "breakEffect", 12, 4, 1, -1, "FxEffectDef*" },
  { "breakSound", 16, 4, 0, -1, "SndAliasCustom" },
  { "breakNotify", 20, 4, 1, -1, "char*" },
  { "loopSound", 24, 4, 1, -1, "char*" },
  { "spawnModel", 28, 12, 1, 3, "XModel*" },
};
static const T4ZField f_17[] = {
  { "dmodels", 0, 200, 0, 5, "DestructibleModel" },
  { "attachBone", 200, 4, 1, -1, "char*" },
  { "attachBoneHash", 204, 2, 0, -1, "ScriptString" },
  { "parentPiece", 206, 1, 0, -1, "unsigned char" },
  { "parentDamagePercent", 208, 4, 0, -1, "float" },
  { "bulletDamageScale", 212, 4, 0, -1, "float" },
  { "explosiveDamageScale", 216, 4, 0, -1, "float" },
  { "entityDamageTransfer", 220, 4, 0, -1, "float" },
  { "physConstraints", 224, 4, 1, -1, "PhysConstraints*" },
  { "health", 228, 4, 0, -1, "int" },
  { "damageSound", 232, 4, 0, -1, "SndAliasCustom" },
  { "burnEffect", 236, 4, 1, -1, "FxEffectDef*" },
  { "burnSound", 240, 4, 0, -1, "SndAliasCustom" },
  { "enableLabel", 244, 2, 0, -1, "ScriptString" },
};
static const T4ZField f_18[] = {
  { "coeffs", 0, 16, 0, 4, "float" },
  { "side", 16, 3, 0, 3, "char" },
  { "pad", 19, 1, 0, -1, "char" },
};
static const T4ZField f_19[] = {
  { "physObjId", 0, 4, 0, -1, "int" },
  { "flags", 4, 2, 0, -1, "uint16_t" },
  { "lightingHandle", 6, 2, 0, -1, "uint16_t" },
  { "health", 8, 4, 0, -1, "int" },
  { "burnTime", 12, 2, 0, -1, "uint16_t" },
  { "fadeTime", 14, 2, 0, -1, "uint16_t" },
};
static const T4ZField f_20[] = {
  { "sector", 0, 2, 0, -1, "uint16_t" },
  { "nextEntInSector", 2, 2, 0, -1, "uint16_t" },
  { "linkMins", 4, 12, 0, 3, "float" },
  { "linkMaxs", 16, 12, 0, 3, "float" },
  { "contents", 28, 4, 0, -1, "int" },
};
static const T4ZField f_21[] = {
  { "type", 0, 4, 0, -1, "DynEntityType" },
  { "pose", 4, 28, 0, -1, "GfxPlacement" },
  { "xModel", 32, 4, 1, -1, "XModel*" },
  { "brushModel", 36, 2, 0, -1, "uint16_t" },
  { "physicsBrushModel", 38, 2, 0, -1, "uint16_t" },
  { "destroyFx", 40, 4, 1, -1, "FxEffectDef*" },
  { "destroyPieces", 44, 4, 1, -1, "XModelPieces*" },
  { "physPreset", 48, 4, 1, -1, "PhysPreset*" },
  { "physConstraints", 52, 4, 0, 4, "unsigned char" },
  { "health", 56, 4, 0, -1, "int" },
  { "mass", 60, 12, 0, -1, "PhysMass" },
  { "flags", 72, 4, 0, -1, "int" },
  { "contents", 76, 4, 0, -1, "int" },
  { "targetname", 80, 2, 0, -1, "ScriptString" },
};
static const T4ZField f_22[] = {
  { "pose", 0, 28, 0, -1, "GfxPlacement" },
  { "radius", 28, 4, 0, -1, "float" },
};
static const T4ZField f_23[] = {
  { "flags", 0, 2, 0, -1, "uint16_t" },
  { "health", 4, 4, 0, -1, "int" },
};
static const T4ZField f_24[] = {
  { "flameVar_streamChunkGravityStart", 0, 4, 0, -1, "float" },
  { "flameVar_streamChunkGravityEnd", 4, 4, 0, -1, "float" },
  { "flameVar_streamChunkMaxSize", 8, 4, 0, -1, "float" },
  { "flameVar_streamChunkStartSize", 12, 4, 0, -1, "float" },
  { "flameVar_streamChunkEndSize", 16, 4, 0, -1, "float" },
  { "flameVar_streamChunkStartSizeRand", 20, 4, 0, -1, "float" },
  { "flameVar_streamChunkEndSizeRand", 24, 4, 0, -1, "float" },
  { "flameVar_streamChunkDistScalar", 28, 4, 0, -1, "float" },
  { "flameVar_streamChunkDistSwayScale", 32, 4, 0, -1, "float" },
  { "flameVar_streamChunkDistSwayVelMax", 36, 4, 0, -1, "float" },
  { "flameVar_streamChunkSpeed", 40, 4, 0, -1, "float" },
  { "flameVar_streamChunkDecel", 44, 4, 0, -1, "float" },
  { "flameVar_streamChunkVelocityAddScale", 48, 4, 0, -1, "float" },
  { "flameVar_streamChunkDuration", 52, 4, 0, -1, "float" },
  { "flameVar_streamChunkDurationScaleMaxVel", 56, 4, 0, -1, "float" },
  { "flameVar_streamChunkDurationVelScalar", 60, 4, 0, -1, "float" },
  { "flameVar_streamChunkSizeSpeedScale", 64, 4, 0, -1, "float" },
  { "flameVar_streamChunkSizeAgeScale", 68, 4, 0, -1, "float" },
  { "flameVar_streamChunkSpawnFireIntervalStart", 72, 4, 0, -1, "float" },
  { "flameVar_streamChunkSpawnFireIntervalEnd", 76, 4, 0, -1, "float" },
  { "flameVar_streamChunkSpawnFireMinLifeFrac", 80, 4, 0, -1, "float" },
  { "flameVar_streamChunkSpawnFireMaxLifeFrac", 84, 4, 0, -1, "float" },
  { "flameVar_streamChunkFireMinLifeFrac", 88, 4, 0, -1, "float" },
  { "flameVar_streamChunkFireMinLifeFracStart", 92, 4, 0, -1, "float" },
  { "flameVar_streamChunkFireMinLifeFracEnd", 96, 4, 0, -1, "float" },
  { "flameVar_streamChunkDripsMinLifeFrac", 100, 4, 0, -1, "float" },
  { "flameVar_streamChunkDripsMinLifeFracStart", 104, 4, 0, -1, "float" },
  { "flameVar_streamChunkDripsMinLifeFracEnd", 108, 4, 0, -1, "float" },
  { "flameVar_streamChunkRotationRange", 112, 4, 0, -1, "float" },
  { "flameVar_streamSizeRandSinWave", 116, 4, 0, -1, "float" },
  { "flameVar_streamSizeRandCosWave", 120, 4, 0, -1, "float" },
  { "flameVar_streamDripsChunkInterval", 124, 4, 0, -1, "float" },
  { "flameVar_streamDripsChunkMinFrac", 128, 4, 0, -1, "float" },
  { "flameVar_streamDripsChunkRandFrac", 132, 4, 0, -1, "float" },
  { "flameVar_streamSmokeChunkInterval", 136, 4, 0, -1, "float" },
  { "flameVar_streamSmokeChunkMinFrac", 140, 4, 0, -1, "float" },
  { "flameVar_streamSmokeChunkRandFrac", 144, 4, 0, -1, "float" },
  { "flameVar_streamChunkCullDistSizeFrac", 148, 4, 0, -1, "float" },
  { "flameVar_streamChunkCullMinLife", 152, 4, 0, -1, "float" },
  { "flameVar_streamChunkCullMaxLife", 156, 4, 0, -1, "float" },
  { "flameVar_streamFuelSizeStart", 160, 4, 0, -1, "float" },
  { "flameVar_streamFuelSizeEnd", 164, 4, 0, -1, "float" },
  { "flameVar_streamFuelLength", 168, 4, 0, -1, "float" },
  { "flameVar_streamFuelNumSegments", 172, 4, 0, -1, "float" },
  { "flameVar_streamFuelAnimLoopTime", 176, 4, 0, -1, "float" },
  { "flameVar_streamFlameSizeStart", 180, 4, 0, -1, "float" },
  { "flameVar_streamFlameSizeEnd", 184, 4, 0, -1, "float" },
  { "flameVar_streamFlameLength", 188, 4, 0, -1, "float" },
  { "flameVar_streamFlameNumSegments", 192, 4, 0, -1, "float" },
  { "flameVar_streamFlameAnimLoopTime", 196, 4, 0, -1, "float" },
  { "flameVar_streamPrimaryLightRadius", 200, 4, 0, -1, "float" },
  { "flameVar_streamPrimaryLightRadiusFlutter", 204, 4, 0, -1, "float" },
  { "flameVar_streamPrimaryLightR", 208, 4, 0, -1, "float" },
  { "flameVar_streamPrimaryLightG", 212, 4, 0, -1, "float" },
  { "flameVar_streamPrimaryLightB", 216, 4, 0, -1, "float" },
  { "flameVar_streamPrimaryLightFlutterR", 220, 4, 0, -1, "float" },
  { "flameVar_streamPrimaryLightFlutterG", 224, 4, 0, -1, "float" },
  { "flameVar_streamPrimaryLightFlutterB", 228, 4, 0, -1, "float" },
  { "flameVar_fireLife", 232, 4, 0, -1, "float" },
  { "flameVar_fireLifeRand", 236, 4, 0, -1, "float" },
  { "flameVar_fireSpeedScale", 240, 4, 0, -1, "float" },
  { "flameVar_fireSpeedScaleRand", 244, 4, 0, -1, "float" },
  { "flameVar_fireVelocityAddZ", 248, 4, 0, -1, "float" },
  { "flameVar_fireVelocityAddZRand", 252, 4, 0, -1, "float" },
  { "flameVar_fireVelocityAddSideways", 256, 4, 0, -1, "float" },
  { "flameVar_fireGravity", 260, 4, 0, -1, "float" },
  { "flameVar_fireGravityEnd", 264, 4, 0, -1, "float" },
  { "flameVar_fireMaxRotVel", 268, 4, 0, -1, "float" },
  { "flameVar_fireFriction", 272, 4, 0, -1, "float" },
  { "flameVar_fireEndSizeAdd", 276, 4, 0, -1, "float" },
  { "flameVar_fireStartSizeScale", 280, 4, 0, -1, "float" },
  { "flameVar_fireEndSizeScale", 284, 4, 0, -1, "float" },
  { "flameVar_dripsLife", 288, 4, 0, -1, "float" },
  { "flameVar_dripsLifeRand", 292, 4, 0, -1, "float" },
  { "flameVar_dripsSpeedScale", 296, 4, 0, -1, "float" },
  { "flameVar_dripsSpeedScaleRand", 300, 4, 0, -1, "float" },
  { "flameVar_dripsVelocityAddZ", 304, 4, 0, -1, "float" },
  { "flameVar_dripsVelocityAddZRand", 308, 4, 0, -1, "float" },
  { "flameVar_dripsVelocityAddSideways", 312, 4, 0, -1, "float" },
  { "flameVar_dripsGravity", 316, 4, 0, -1, "float" },
  { "flameVar_dripsGravityEnd", 320, 4, 0, -1, "float" },
  { "flameVar_dripsMaxRotVel", 324, 4, 0, -1, "float" },
  { "flameVar_dripsFriction", 328, 4, 0, -1, "float" },
  { "flameVar_dripsEndSizeAdd", 332, 4, 0, -1, "float" },
  { "flameVar_dripsStartSizeScale", 336, 4, 0, -1, "float" },
  { "flameVar_dripsEndSizeScale", 340, 4, 0, -1, "float" },
  { "flameVar_smokeLife", 344, 4, 0, -1, "float" },
  { "flameVar_smokeLifeRand", 348, 4, 0, -1, "float" },
  { "flameVar_smokeSpeedScale", 352, 4, 0, -1, "float" },
  { "flameVar_smokeVelocityAddZ", 356, 4, 0, -1, "float" },
  { "flameVar_smokeGravity", 360, 4, 0, -1, "float" },
  { "flameVar_smokeGravityEnd", 364, 4, 0, -1, "float" },
  { "flameVar_smokeMaxRotation", 368, 4, 0, -1, "float" },
  { "flameVar_smokeMaxRotVel", 372, 4, 0, -1, "float" },
  { "flameVar_smokeFriction", 376, 4, 0, -1, "float" },
  { "flameVar_smokeEndSizeAdd", 380, 4, 0, -1, "float" },
  { "flameVar_smokeStartSizeAdd", 384, 4, 0, -1, "float" },
  { "flameVar_smokeOriginSizeOfsZScale", 388, 4, 0, -1, "float" },
  { "flameVar_smokeOriginOfsZ", 392, 4, 0, -1, "float" },
  { "flameVar_smokeFadein", 396, 4, 0, -1, "float" },
  { "flameVar_smokeFadeout", 400, 4, 0, -1, "float" },
  { "flameVar_smokeMaxAlpha", 404, 4, 0, -1, "float" },
  { "flameVar_smokeBrightness", 408, 4, 0, -1, "float" },
  { "flameVar_smokeOriginOffset", 412, 4, 0, -1, "float" },
  { "flameVar_collisionSpeedScale", 416, 4, 0, -1, "float" },
  { "flameVar_collisionVolumeScale", 420, 4, 0, -1, "float" },
  { "name", 424, 4, 1, -1, "char*" },
  { "fire", 428, 4, 1, -1, "Material*" },
  { "smoke", 432, 4, 1, -1, "Material*" },
  { "heat", 436, 4, 1, -1, "Material*" },
  { "drips", 440, 4, 1, -1, "Material*" },
  { "streamFuel", 444, 4, 1, -1, "Material*" },
  { "streamFuel2", 448, 4, 1, -1, "Material*" },
  { "streamFlame", 452, 4, 1, -1, "Material*" },
  { "streamFlame2", 456, 4, 1, -1, "Material*" },
  { "flameOffLoopSound", 460, 4, 0, -1, "SndAliasCustom" },
  { "flameIgniteSound", 464, 4, 0, -1, "SndAliasCustom" },
  { "flameOnLoopSound", 468, 4, 0, -1, "SndAliasCustom" },
  { "flameCooldownSound", 472, 4, 0, -1, "SndAliasCustom" },
};
static const T4ZField f_25[] = {
  { "fontName", 0, 4, 1, -1, "char*" },
  { "pixelHeight", 4, 4, 0, -1, "int" },
  { "glyphCount", 8, 4, 0, -1, "int" },
  { "material", 12, 4, 1, -1, "Material*" },
  { "glowMaterial", 16, 4, 1, -1, "Material*" },
  { "glyphs", 20, 4, 1, -1, "Glyph*" },
};
static const T4ZField f_26[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "flags", 4, 4, 0, -1, "int" },
  { "totalSize", 8, 4, 0, -1, "int" },
  { "msecLoopingLife", 12, 4, 0, -1, "int" },
  { "elemDefCountLooping", 16, 4, 0, -1, "int" },
  { "elemDefCountOneShot", 20, 4, 0, -1, "int" },
  { "elemDefCountEmission", 24, 4, 0, -1, "int" },
  { "efPriority", 28, 1, 0, -1, "char" },
  { "elemDefs", 32, 4, 1, -1, "FxElemDef*" },
};
static const T4ZField f_27[] = {
  { "behavior", 0, 1, 0, -1, "char" },
  { "index", 1, 1, 0, -1, "char" },
  { "fps", 2, 1, 0, -1, "char" },
  { "loopCount", 3, 1, 0, -1, "char" },
  { "colIndexBits", 4, 1, 0, -1, "char" },
  { "rowIndexBits", 5, 1, 0, -1, "char" },
  { "entryCount", 6, 2, 0, -1, "int16_t" },
};
static const T4ZField f_28[] = {
  { "flags", 0, 4, 0, -1, "int" },
  { "spawn", 4, 8, 0, -1, "FxSpawnDef" },
  { "spawnRange", 12, 8, 0, -1, "FxFloatRange" },
  { "fadeInRange", 20, 8, 0, -1, "FxFloatRange" },
  { "fadeOutRange", 28, 8, 0, -1, "FxFloatRange" },
  { "spawnFrustumCullRadius", 36, 4, 0, -1, "float" },
  { "spawnDelayMsec", 40, 8, 0, -1, "FxIntRange" },
  { "lifeSpanMsec", 48, 8, 0, -1, "FxIntRange" },
  { "spawnOrigin", 56, 24, 0, 3, "FxFloatRange" },
  { "spawnOffsetRadius", 80, 8, 0, -1, "FxFloatRange" },
  { "spawnOffsetHeight", 88, 8, 0, -1, "FxFloatRange" },
  { "spawnAngles", 96, 24, 0, 3, "FxFloatRange" },
  { "angularVelocity", 120, 24, 0, 3, "FxFloatRange" },
  { "initialRotation", 144, 8, 0, -1, "FxFloatRange" },
  { "gravity", 152, 8, 0, -1, "FxFloatRange" },
  { "reflectionFactor", 160, 8, 0, -1, "FxFloatRange" },
  { "atlas", 168, 8, 0, -1, "FxElemAtlas" },
  { "windInfluence", 176, 4, 0, -1, "float" },
  { "elemType", 180, 1, 0, -1, "char" },
  { "visualCount", 181, 1, 0, -1, "char" },
  { "velIntervalCount", 182, 1, 0, -1, "char" },
  { "visStateIntervalCount", 183, 1, 0, -1, "char" },
  { "velSamples", 184, 4, 1, -1, "FxElemVelStateSample*" },
  { "visSamples", 188, 4, 1, -1, "FxElemVisStateSample*" },
  { "visuals", 192, 4, 0, -1, "FxElemDefVisuals" },
  { "collMins", 196, 12, 0, 3, "float" },
  { "collMaxs", 208, 12, 0, 3, "float" },
  { "effectOnImpact", 220, 4, 0, -1, "FxEffectDefRef" },
  { "effectOnDeath", 224, 4, 0, -1, "FxEffectDefRef" },
  { "effectEmitted", 228, 4, 0, -1, "FxEffectDefRef" },
  { "emitDist", 232, 8, 0, -1, "FxFloatRange" },
  { "emitDistVariance", 240, 8, 0, -1, "FxFloatRange" },
  { "trailDef", 248, 4, 1, -1, "FxTrailDef*" },
  { "sortOrder", 252, 1, 0, -1, "char" },
  { "lightingFrac", 253, 1, 0, -1, "char" },
  { "useItemClip", 254, 1, 0, -1, "char" },
  { "unused", 255, 1, 0, 1, "char" },
};
static const T4ZField f_29[] = {
  { "materials", 0, 8, 1, 2, "Material*" },
};
static const T4ZField f_30[] = {
  { "base", 0, 12, 0, 3, "float" },
  { "amplitude", 12, 12, 0, 3, "float" },
};
static const T4ZField f_31[] = {
  { "velocity", 0, 24, 0, -1, "FxElemVec3Range" },
  { "totalDelta", 24, 24, 0, -1, "FxElemVec3Range" },
};
static const T4ZField f_32[] = {
  { "local", 0, 48, 0, -1, "FxElemVelStateInFrame" },
  { "world", 48, 48, 0, -1, "FxElemVelStateInFrame" },
};
static const T4ZField f_33[] = {
  { "base", 0, 24, 0, -1, "FxElemVisualState" },
  { "amplitude", 24, 24, 0, -1, "FxElemVisualState" },
};
static const T4ZField f_34[] = {
  { "color", 0, 4, 0, 4, "char" },
  { "rotationDelta", 4, 4, 0, -1, "float" },
  { "rotationTotal", 8, 4, 0, -1, "float" },
  { "size", 12, 8, 0, 2, "float" },
  { "scale", 20, 4, 0, -1, "float" },
};
static const T4ZField f_35[] = {
  { "base", 0, 4, 0, -1, "float" },
  { "amplitude", 4, 4, 0, -1, "float" },
};
static const T4ZField f_36[] = {
  { "nonflesh", 0, 124, 1, 31, "FxEffectDef*" },
  { "flesh", 124, 16, 1, 4, "FxEffectDef*" },
};
static const T4ZField f_37[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "table", 4, 4, 1, -1, "FxImpactEntry*" },
};
static const T4ZField f_38[] = {
  { "base", 0, 4, 0, -1, "int" },
  { "amplitude", 4, 4, 0, -1, "int" },
};
static const T4ZField f_39[] = {
  { "intervalMsec", 0, 4, 0, -1, "int" },
  { "count", 4, 4, 0, -1, "int" },
};
static const T4ZField f_40[] = {
  { "count", 0, 8, 0, -1, "FxIntRange" },
};
static const T4ZField f_41[] = {
  { "scrollTimeMsec", 0, 4, 0, -1, "int" },
  { "repeatDist", 4, 4, 0, -1, "int" },
  { "splitDist", 8, 4, 0, -1, "int" },
  { "vertCount", 12, 4, 0, -1, "int" },
  { "verts", 16, 4, 1, -1, "FxTrailVertex*" },
  { "indCount", 20, 4, 0, -1, "int" },
  { "inds", 24, 4, 1, -1, "uint16_t*" },
};
static const T4ZField f_42[] = {
  { "pos", 0, 8, 0, 2, "float" },
  { "normal", 8, 8, 0, 2, "float" },
  { "texCoord", 16, 4, 0, -1, "float" },
};
static const T4ZField f_43[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "path", 4, 40, 0, -1, "PathData" },
};
static const T4ZField f_44[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "path", 4, 40, 0, -1, "PathData" },
};
static const T4ZField f_45[] = {
  { "mins", 0, 12, 0, 3, "float" },
  { "maxs", 12, 12, 0, 3, "float" },
  { "childCount", 24, 2, 0, -1, "uint16_t" },
  { "surfaceCount", 26, 2, 0, -1, "uint16_t" },
  { "startSurfIndex", 28, 2, 0, -1, "uint16_t" },
  { "smodelIndexCount", 30, 2, 0, -1, "uint16_t" },
  { "smodelIndexes", 32, 4, 1, -1, "uint16_t*" },
  { "childrenOffset", 36, 4, 0, -1, "int" },
};
static const T4ZField f_46[] = {
  { "writable", 0, 24, 0, -1, "GfxBrushModelWritable" },
  { "bounds", 24, 24, 0, 6, "float" },
  { "surfaceCount", 48, 4, 0, -1, "unsigned int" },
  { "startSurfIndex", 52, 4, 0, -1, "unsigned int" },
};
static const T4ZField f_47[] = {
  { "mins", 0, 12, 0, 3, "float" },
  { "maxs", 12, 12, 0, 3, "float" },
};
static const T4ZField f_48[] = {
  { "mins", 0, 12, 0, 3, "float" },
  { "maxs", 12, 12, 0, 3, "float" },
  { "aabbTreeCount", 24, 4, 0, -1, "int" },
  { "aabbTree", 28, 4, 1, -1, "GfxAabbTree*" },
  { "portalCount", 32, 4, 0, -1, "int" },
  { "portals", 36, 4, 1, -1, "GfxPortal*" },
  { "cullGroupCount", 40, 4, 0, -1, "int" },
  { "cullGroups", 44, 4, 1, -1, "int*" },
  { "reflectionProbeCount", 48, 1, 0, -1, "char" },
  { "reflectionProbes", 52, 4, 1, -1, "char*" },
};
static const T4ZField f_49[] = {
  { "mins", 0, 12, 0, 3, "float" },
  { "maxs", 12, 12, 0, 3, "float" },
  { "surfaceCount", 24, 4, 0, -1, "int" },
  { "startSurfIndex", 28, 4, 0, -1, "int" },
};
static const T4ZField f_50[] = {
  { "objectId", 0, 0, 0, -1, "uint64_t" },
  { "reflectionProbeIndex", 0, 0, 0, -1, "uint64_t" },
  { "customIndex", 0, 0, 0, -1, "uint64_t" },
  { "materialSortedIndex", 0, 0, 0, -1, "uint64_t" },
  { "prepass", 0, 0, 0, -1, "uint64_t" },
  { "primaryLightIndex", 0, 0, 0, -1, "uint64_t" },
  { "surfType", 0, 0, 0, -1, "uint64_t" },
  { "primarySortKey", 0, 0, 0, -1, "uint64_t" },
  { "unused", 0, 0, 0, -1, "uint64_t" },
};
static const T4ZField f_51[] = {
  { "mapType", 0, 4, 0, -1, "MapType" },
  { "texture", 4, 4, 0, -1, "GfxTexture" },
  { "picmip", 8, 2, 0, -1, "Picmip" },
  { "noPicmip", 10, 1, 0, -1, "bool" },
  { "semantic", 11, 1, 0, -1, "char" },
  { "track", 12, 1, 0, -1, "char" },
  { "cardMemory", 16, 8, 0, -1, "CardMemory" },
  { "width", 24, 2, 0, -1, "uint16_t" },
  { "height", 26, 2, 0, -1, "uint16_t" },
  { "depth", 28, 2, 0, -1, "uint16_t" },
  { "category", 30, 1, 0, -1, "char" },
  { "delayLoadPixels", 31, 1, 0, -1, "bool" },
  { "name", 32, 4, 1, -1, "char*" },
};
static const T4ZField f_52[] = {
  { "levelCount", 0, 1, 0, -1, "char" },
  { "flags", 1, 1, 0, -1, "char" },
  { "dimensions", 2, 6, 0, 3, "uint16_t" },
  { "format", 8, 4, 0, -1, "int" },
  { "resourceSize", 12, 4, 0, -1, "unsigned int" },
  { "data", 16, 0, 0, 1, "char" },
};
static const T4ZField f_53[] = {
  { "type", 0, 1, 0, -1, "unsigned char" },
  { "canUseShadowMap", 1, 1, 0, -1, "unsigned char" },
  { "cullDist", 2, 2, 0, -1, "int16_t" },
  { "color", 4, 12, 0, 3, "float" },
  { "dir", 16, 12, 0, 3, "float" },
  { "origin", 28, 12, 0, 3, "float" },
  { "radius", 40, 4, 0, -1, "float" },
  { "cosHalfFovOuter", 44, 4, 0, -1, "float" },
  { "cosHalfFovInner", 48, 4, 0, -1, "float" },
  { "exponent", 52, 4, 0, -1, "int" },
  { "spotShadowIndex", 56, 4, 0, -1, "unsigned int" },
  { "def", 60, 4, 1, -1, "GfxLightDef*" },
};
static const T4ZField f_54[] = {
  { "origin", 0, 12, 0, 3, "float" },
  { "radius", 12, 4, 0, -1, "float" },
  { "color", 16, 12, 0, 3, "float" },
  { "intensity", 28, 4, 0, -1, "float" },
};
static const T4ZField f_55[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "attenuation", 4, 8, 0, -1, "GfxLightImage" },
  { "lmapLookupStart", 12, 4, 0, -1, "int" },
};
static const T4ZField f_56[] = {
  { "hasLightRegions", 0, 1, 0, -1, "bool" },
  { "sunPrimaryLightIndex", 4, 4, 0, -1, "unsigned int" },
  { "mins", 8, 6, 0, 3, "uint16_t" },
  { "maxs", 14, 6, 0, 3, "uint16_t" },
  { "rowAxis", 20, 4, 0, -1, "unsigned int" },
  { "colAxis", 24, 4, 0, -1, "unsigned int" },
  { "rowDataStart", 28, 4, 1, -1, "uint16_t*" },
  { "rawRowDataSize", 32, 4, 0, -1, "unsigned int" },
  { "rawRowData", 36, 4, 1, -1, "char*" },
  { "entryCount", 40, 4, 0, -1, "unsigned int" },
  { "entries", 44, 4, 1, -1, "GfxLightGridEntry*" },
  { "colorCount", 48, 4, 0, -1, "unsigned int" },
  { "colors", 52, 4, 1, -1, "GfxLightGridColors*" },
};
static const T4ZField f_57[] = {
  { "rgb", 0, 168, 0, 168, "char" },
};
static const T4ZField f_58[] = {
  { "colorsIndex", 0, 2, 0, -1, "uint16_t" },
  { "primaryLightIndex", 2, 1, 0, -1, "char" },
  { "needsTrace", 3, 1, 0, -1, "char" },
};
static const T4ZField f_59[] = {
  { "image", 0, 4, 1, -1, "GfxImage*" },
  { "samplerState", 4, 1, 0, -1, "char" },
};
static const T4ZField f_60[] = {
  { "hullCount", 0, 4, 0, -1, "unsigned int" },
  { "hulls", 4, 4, 1, -1, "GfxLightRegionHull*" },
};
static const T4ZField f_61[] = {
  { "dir", 0, 12, 0, 3, "float" },
  { "midPoint", 12, 4, 0, -1, "float" },
  { "halfSize", 16, 4, 0, -1, "float" },
};
static const T4ZField f_62[] = {
  { "kdopMidPoint", 0, 36, 0, 9, "float" },
  { "kdopHalfSize", 36, 36, 0, 9, "float" },
  { "axisCount", 72, 4, 0, -1, "unsigned int" },
  { "axis", 76, 4, 1, -1, "GfxLightRegionAxis*" },
};
static const T4ZField f_63[] = {
  { "primary", 0, 4, 1, -1, "GfxImage*" },
  { "secondary", 4, 4, 1, -1, "GfxImage*" },
};
static const T4ZField f_64[] = {
  { "origin", 0, 12, 0, 3, "float" },
  { "axis", 12, 36, 0, 9, "float" },
  { "scale", 48, 4, 0, -1, "float" },
};
static const T4ZField f_65[] = {
  { "xyz", 0, 12, 0, -1, "vec3_t" },
  { "binormalSign", 12, 4, 0, -1, "float" },
  { "color", 16, 4, 0, -1, "GfxColor" },
  { "texCoord", 20, 4, 0, -1, "PackedTexCoords" },
  { "normal", 24, 4, 0, -1, "PackedUnitVec" },
  { "tangent", 28, 4, 0, -1, "PackedUnitVec" },
};
static const T4ZField f_66[] = {
  { "program", 0, 4, 1, -1, "unsigned int*" },
  { "programSize", 4, 2, 0, -1, "uint16_t" },
  { "loadForRenderer", 6, 2, 0, -1, "uint16_t" },
};
static const T4ZField f_67[] = {
  { "quat", 0, 16, 0, 4, "float" },
  { "origin", 16, 12, 0, 3, "float" },
};
static const T4ZField f_68[] = {
  { "writable", 0, 12, 0, -1, "GfxPortalWritable" },
  { "plane", 12, 20, 0, -1, "DpvsPlane" },
  { "cell", 32, 4, 1, -1, "GfxCell*" },
  { "vertices", 36, 4, 1, -1, "vec3_t*" },
  { "vertexCount", 40, 1, 0, -1, "char" },
  { "hullAxis", 44, 24, 0, 6, "float" },
};
static const T4ZField f_69[] = {
  { "isQueued", 0, 1, 0, -1, "bool" },
  { "isAncestor", 1, 1, 0, -1, "bool" },
  { "recursionDepth", 2, 1, 0, -1, "char" },
  { "hullPointCount", 3, 1, 0, -1, "char" },
  { "hullPoints", 4, 4, 1, -1, "float*" },
  { "queuedParent", 8, 4, 1, -1, "GfxPortal*" },
};
static const T4ZField f_70[] = {
  { "origin", 0, 12, 0, 3, "float" },
  { "reflectionImage", 12, 4, 1, -1, "GfxImage*" },
};
static const T4ZField f_71[] = {
  { "info", 0, 2, 0, -1, "BModelDrawInfo" },
  { "dynEntId", 2, 2, 0, -1, "uint16_t" },
};
static const T4ZField f_72[] = {
  { "info", 0, 4, 0, -1, "XModelDrawInfo" },
  { "dynEntId", 4, 2, 0, -1, "uint16_t" },
};
static const T4ZField f_73[] = {
  { "surfaceCount", 0, 2, 0, -1, "uint16_t" },
  { "smodelCount", 2, 2, 0, -1, "uint16_t" },
  { "sortedSurfIndex", 4, 4, 1, -1, "uint16_t*" },
  { "smodelIndex", 8, 4, 1, -1, "uint16_t*" },
};
static const T4ZField f_74[] = {
  { "loadBits", 0, 8, 0, -1, "GfxStateBitsLoadBits" },
};
static const T4ZField f_75[] = {
  { "srcBlendRgb", 0, 0, 0, -1, "unsigned int" },
  { "dstBlendRgb", 0, 0, 0, -1, "unsigned int" },
  { "blendOpRgb", 0, 0, 0, -1, "unsigned int" },
  { "alphaTestDisabled", 0, 0, 0, -1, "unsigned int" },
  { "alphaTest", 0, 0, 0, -1, "unsigned int" },
  { "cullFace", 0, 0, 0, -1, "unsigned int" },
  { "srcBlendAlpha", 0, 0, 0, -1, "unsigned int" },
  { "dstBlendAlpha", 0, 0, 0, -1, "unsigned int" },
  { "blendOpAlpha", 0, 0, 0, -1, "unsigned int" },
  { "colorWriteRgb", 0, 0, 0, -1, "unsigned int" },
  { "colorWriteAlpha", 0, 0, 0, -1, "unsigned int" },
  { "unused1", 0, 0, 0, -1, "unsigned int" },
  { "polymodeLine", 0, 0, 0, -1, "unsigned int" },
  { "depthWrite", 0, 0, 0, -1, "unsigned int" },
  { "depthTestDisabled", 0, 0, 0, -1, "unsigned int" },
  { "depthTest", 0, 0, 0, -1, "unsigned int" },
  { "polygonOffset", 0, 0, 0, -1, "unsigned int" },
  { "stencilFrontEnabled", 0, 0, 0, -1, "unsigned int" },
  { "stencilBackEnabled", 0, 0, 0, -1, "unsigned int" },
  { "stencilFrontPass", 0, 0, 0, -1, "unsigned int" },
  { "stencilFrontFail", 0, 0, 0, -1, "unsigned int" },
  { "stencilFrontZFail", 0, 0, 0, -1, "unsigned int" },
  { "stencilFrontFunc", 0, 0, 0, -1, "unsigned int" },
  { "stencilBackPass", 0, 0, 0, -1, "unsigned int" },
  { "stencilBackFail", 0, 0, 0, -1, "unsigned int" },
  { "stencilBackZFail", 0, 0, 0, -1, "unsigned int" },
  { "stencilBackFunc", 0, 0, 0, -1, "unsigned int" },
};
static const T4ZField f_76[] = {
  { "cullDist", 0, 4, 0, -1, "float" },
  { "placement", 4, 52, 0, -1, "GfxPackedPlacement" },
  { "model", 56, 4, 1, -1, "XModel*" },
  { "lodFade", 60, 4, 0, -1, "ModelLodFade" },
  { "cachedLightSettingIndex", 64, 4, 0, -1, "int" },
  { "field_44", 68, 1, 0, -1, "char" },
  { "field_45", 69, 1, 0, -1, "char" },
  { "reflectionProbeIndex", 70, 1, 0, -1, "unsigned char" },
  { "primaryLightIndex", 71, 1, 0, -1, "unsigned char" },
  { "flags", 72, 4, 0, -1, "int" },
  { "smodelCacheIndex", 76, 16, 0, 4, "unsigned int" },
};
static const T4ZField f_77[] = {
  { "mins", 0, 12, 0, 3, "float" },
  { "maxs", 12, 12, 0, 3, "float" },
  { "groundLighting", 24, 4, 0, -1, "GfxColor" },
};
static const T4ZField f_78[] = {
  { "tris", 0, 16, 0, -1, "srfTriangles_t" },
  { "pad", 16, 4, 0, -1, "int" },
  { "material", 20, 4, 1, -1, "Material*" },
  { "lightmapIndex", 24, 1, 0, -1, "char" },
  { "reflectionProbeIndex", 25, 1, 0, -1, "char" },
  { "primaryLightIndex", 26, 1, 0, -1, "char" },
  { "flags", 27, 1, 0, -1, "char" },
  { "bounds", 28, 24, 0, 6, "float" },
};
static const T4ZField f_79[] = {
  { "program", 0, 4, 1, -1, "unsigned int*" },
  { "programSize", 4, 2, 0, -1, "uint16_t" },
  { "loadForRenderer", 6, 2, 0, -1, "uint16_t" },
};
static const T4ZField f_80[] = {
  { "bufferSize", 0, 4, 0, -1, "unsigned int" },
  { "buffer", 4, 4, 1, -1, "vec4_t*" },
};
static const T4ZField f_81[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "baseName", 4, 4, 1, -1, "char*" },
  { "planeCount", 8, 4, 0, -1, "int" },
  { "nodeCount", 12, 4, 0, -1, "int" },
  { "indexCount", 16, 4, 0, -1, "int" },
  { "indices", 20, 4, 1, -1, "uint16_t*" },
  { "surfaceCount", 24, 4, 0, -1, "int" },
  { "streamInfo", 28, 4, 0, -1, "int" },
  { "skySurfCount", 32, 4, 0, -1, "int" },
  { "skyStartSurfs", 36, 4, 1, -1, "int*" },
  { "skyImage", 40, 4, 1, -1, "GfxImage*" },
  { "skySamplerState", 44, 1, 0, -1, "char" },
  { "skyBoxModel", 48, 4, 1, -1, "char*" },
  { "vertexCount", 52, 4, 0, -1, "unsigned int" },
  { "vd", 56, 8, 0, -1, "GfxWorldVertexData" },
  { "vertexLayerDataSize", 64, 4, 0, -1, "unsigned int" },
  { "vld", 68, 8, 0, -1, "GfxWorldVertexLayerData" },
  { "vertexStream2DataSize", 76, 4, 0, -1, "unsigned int" },
  { "sunParse", 80, 136, 0, -1, "SunLightParseParams" },
  { "sunLight", 216, 4, 1, -1, "GfxLight*" },
  { "sunColorFromBsp", 220, 12, 0, 3, "float" },
  { "sunPrimaryLightIndex", 232, 4, 0, -1, "unsigned int" },
  { "primaryLightCount", 236, 4, 0, -1, "unsigned int" },
  { "cullGroupCount", 240, 4, 0, -1, "int" },
  { "reflectionProbeCount", 244, 4, 0, -1, "unsigned int" },
  { "reflectionProbes", 248, 4, 1, -1, "GfxReflectionProbe*" },
  { "reflectionProbeTextures", 252, 4, 1, -1, "GfxTexture*" },
  { "coronaCount", 256, 4, 0, -1, "unsigned int" },
  { "coronas", 260, 4, 1, -1, "GfxLightCorona*" },
  { "dpvsPlanes", 264, 16, 0, -1, "GfxWorldDpvsPlanes" },
  { "cellBitsCount", 280, 4, 0, -1, "int" },
  { "cells", 284, 4, 1, -1, "GfxCell*" },
  { "lightmapCount", 288, 4, 0, -1, "int" },
  { "lightmaps", 292, 4, 1, -1, "GfxLightmapArray*" },
  { "lightGrid", 296, 56, 0, -1, "GfxLightGrid" },
  { "lightmapPrimaryTextures", 352, 4, 1, -1, "GfxTexture*" },
  { "lightmapSecondaryTextures", 356, 4, 1, -1, "GfxTexture*" },
  { "modelCount", 360, 4, 0, -1, "int" },
  { "models", 364, 4, 1, -1, "GfxBrushModel*" },
  { "mins", 368, 12, 0, 3, "float" },
  { "maxs", 380, 12, 0, 3, "float" },
  { "checksum", 392, 4, 0, -1, "unsigned int" },
  { "materialMemoryCount", 396, 4, 0, -1, "int" },
  { "materialMemory", 400, 4, 1, -1, "MaterialMemory*" },
  { "sun", 404, 96, 0, -1, "sunflare_t" },
  { "outdoorLookupMatrix", 500, 64, 0, 16, "float" },
  { "outdoorImage", 564, 4, 1, -1, "GfxImage*" },
  { "cellCasterBits", 568, 4, 1, -1, "unsigned int*" },
  { "sceneDynModel", 572, 4, 1, -1, "GfxSceneDynModel4*" },
  { "sceneDynBrush", 576, 4, 1, -1, "GfxSceneDynBrush*" },
  { "primaryLightEntityShadowVis", 580, 4, 1, -1, "unsigned int*" },
  { "primaryLightDynEntShadowVis", 584, 8, 1, 2, "unsigned int*" },
  { "nonSunPrimaryLightForModelDynEnt", 592, 4, 1, -1, "char*" },
  { "shadowGeom", 596, 4, 1, -1, "GfxShadowGeometry*" },
  { "lightRegion", 600, 4, 1, -1, "GfxLightRegion*" },
  { "dpvs", 604, 100, 0, -1, "GfxWorldDpvsStatic" },
  { "dpvsDyn", 704, 48, 0, -1, "GfxWorldDpvsDynamic" },
  { "worldLodChainCount", 752, 4, 0, -1, "unsigned int" },
  { "worldLodChains", 756, 4, 1, -1, "GfxWorldLodChain*" },
  { "worldLodInfoCount", 760, 4, 0, -1, "unsigned int" },
  { "worldLodInfos", 764, 4, 1, -1, "GfxWorldLodInfo*" },
  { "worldLodSurfaceCount", 768, 4, 0, -1, "unsigned int" },
  { "worldLodSurfaces", 772, 4, 1, -1, "unsigned int*" },
  { "waterDirection", 776, 4, 0, -1, "float" },
  { "waterBuffers", 780, 16, 0, 2, "GfxWaterBuffer" },
  { "waterMaterial", 796, 4, 1, -1, "Material*" },
};
static const T4ZField f_82[] = {
  { "dynEntClientWordCount", 0, 8, 0, 2, "unsigned int" },
  { "dynEntClientCount", 8, 8, 0, 2, "unsigned int" },
  { "dynEntCellBits", 16, 8, 1, 2, "unsigned int*" },
  { "dynEntVisData", 24, 24, 1, 6, "raw_byte16*" },
};
static const T4ZField f_83[] = {
  { "cellCount", 0, 4, 0, -1, "int" },
  { "planes", 4, 4, 1, -1, "cplane_s*" },
  { "nodes", 8, 4, 1, -1, "uint16_t*" },
  { "sceneEntCellBits", 12, 4, 1, -1, "unsigned int*" },
};
static const T4ZField f_84[] = {
  { "smodelCount", 0, 4, 0, -1, "unsigned int" },
  { "staticSurfaceCount", 4, 4, 0, -1, "unsigned int" },
  { "litSurfsBegin", 8, 4, 0, -1, "unsigned int" },
  { "litSurfsEnd", 12, 4, 0, -1, "unsigned int" },
  { "decalSurfsBegin", 16, 4, 0, -1, "unsigned int" },
  { "decalSurfsEnd", 20, 4, 0, -1, "unsigned int" },
  { "emissiveSurfsBegin", 24, 4, 0, -1, "unsigned int" },
  { "emissiveSurfsEnd", 28, 4, 0, -1, "unsigned int" },
  { "smodelVisDataCount", 32, 4, 0, -1, "unsigned int" },
  { "surfaceVisDataCount", 36, 4, 0, -1, "unsigned int" },
  { "smodelVisData", 40, 12, 1, 3, "char*" },
  { "surfaceVisData", 52, 12, 1, 3, "char*" },
  { "lodData", 64, 4, 1, -1, "raw_uint128*" },
  { "sortedSurfIndex", 68, 4, 1, -1, "uint16_t*" },
  { "smodelInsts", 72, 4, 1, -1, "GfxStaticModelInst*" },
  { "surfaces", 76, 4, 1, -1, "GfxSurface*" },
  { "cullGroups", 80, 4, 1, -1, "GfxCullGroup*" },
  { "smodelDrawInsts", 84, 4, 1, -1, "GfxStaticModelDrawInst*" },
  { "surfaceMaterials", 88, 4, 1, -1, "GfxDrawSurf*" },
  { "surfaceCastsSunShadow", 92, 4, 1, -1, "raw_uint128*" },
  { "usageCount", 96, 4, 0, -1, "int" },
};
static const T4ZField f_85[] = {
  { "origin", 0, 12, 0, 3, "float" },
  { "lastDist", 12, 4, 0, -1, "float" },
  { "firstLodInfo", 16, 4, 0, -1, "unsigned int" },
  { "lodInfoCount", 20, 2, 0, -1, "uint16_t" },
};
static const T4ZField f_86[] = {
  { "dist", 0, 4, 0, -1, "float" },
  { "firstSurf", 4, 4, 0, -1, "unsigned int" },
  { "surfCount", 8, 2, 0, -1, "uint16_t" },
};
static const T4ZField f_87[] = {
  { "xyz", 0, 12, 0, 3, "float" },
  { "binormalSign", 12, 4, 0, -1, "float" },
  { "color", 16, 4, 0, -1, "GfxColor" },
  { "texCoord", 20, 8, 0, 2, "float" },
  { "lmapCoord", 28, 8, 0, 2, "float" },
  { "normal", 36, 4, 0, -1, "PackedUnitVec" },
  { "tangent", 40, 4, 0, -1, "PackedUnitVec" },
};
static const T4ZField f_88[] = {
  { "vertices", 0, 4, 1, -1, "GfxWorldVertex*" },
  { "worldVb", 4, 4, 1, -1, "void *" },
};
static const T4ZField f_89[] = {
  { "data", 0, 4, 1, -1, "char*" },
  { "layerVb", 4, 4, 1, -1, "void *" },
};
static const T4ZField f_90[] = {
  { "letter", 0, 2, 0, -1, "uint16_t" },
  { "x0", 2, 1, 0, -1, "char" },
  { "y0", 3, 1, 0, -1, "char" },
  { "dx", 4, 1, 0, -1, "unsigned char" },
  { "pixelWidth", 5, 1, 0, -1, "unsigned char" },
  { "pixelHeight", 6, 1, 0, -1, "unsigned char" },
  { "s0", 8, 4, 0, -1, "float" },
  { "t0", 12, 4, 0, -1, "float" },
  { "s1", 16, 4, 0, -1, "float" },
  { "t1", 20, 4, 0, -1, "float" },
};
static const T4ZField f_91[] = {
  { "key", 0, 4, 0, -1, "int" },
  { "action", 4, 4, 1, -1, "char*" },
  { "next", 8, 4, 1, -1, "ItemKeyHandler*" },
};
static const T4ZField f_92[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "sound", 4, 8, 0, -1, "snd_asset" },
};
static const T4ZField f_93[] = {
  { "value", 0, 4, 1, -1, "char*" },
  { "name", 4, 4, 1, -1, "char*" },
};
static const T4ZField f_94[] = {
  { "speakerCount", 0, 4, 0, -1, "int" },
  { "speakers", 4, 96, 0, 6, "MSSSpeakerLevels" },
};
static const T4ZField f_95[] = {
  { "speaker", 0, 4, 0, -1, "int" },
  { "numLevels", 4, 4, 0, -1, "int" },
  { "levels", 8, 8, 0, 2, "float" },
};
static const T4ZField f_96[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "entityString", 4, 4, 1, -1, "char*" },
  { "numEntityChars", 8, 4, 0, -1, "int" },
};
static const T4ZField f_97[] = {
  { "info", 0, 32, 0, -1, "MaterialInfo" },
  { "stateBitsEntry", 32, 59, 0, 59, "char" },
  { "textureCount", 91, 1, 0, -1, "unsigned char" },
  { "constantCount", 92, 1, 0, -1, "unsigned char" },
  { "stateBitsCount", 93, 1, 0, -1, "unsigned char" },
  { "stateFlags", 94, 1, 0, -1, "unsigned char" },
  { "cameraRegion", 95, 1, 0, -1, "unsigned char" },
  { "techniqueSet", 96, 4, 1, -1, "MaterialTechniqueSet*" },
  { "textureTable", 100, 4, 1, -1, "MaterialTextureDef*" },
  { "constantTable", 104, 4, 1, -1, "MaterialConstantDef*" },
  { "stateBitsTable", 108, 4, 1, -1, "GfxStateBits*" },
};
static const T4ZField f_98[] = {
  { "index", 0, 2, 0, -1, "uint16_t" },
  { "firstRow", 2, 1, 0, -1, "unsigned char" },
  { "rowCount", 3, 1, 0, -1, "unsigned char" },
};
static const T4ZField f_99[] = {
  { "nameHash", 0, 4, 0, -1, "unsigned int" },
  { "name", 4, 12, 0, 12, "char" },
  { "literal", 16, 16, 0, -1, "vec4_t" },
};
static const T4ZField f_100[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "gameFlags", 4, 1, 0, -1, "unsigned char" },
  { "sortKey", 5, 1, 0, -1, "unsigned char" },
  { "textureAtlasRowCount", 6, 1, 0, -1, "unsigned char" },
  { "textureAtlasColumnCount", 7, 1, 0, -1, "unsigned char" },
  { "drawSurf", 8, 8, 0, -1, "GfxDrawSurf" },
  { "surfaceTypeBits", 16, 4, 0, -1, "unsigned int" },
  { "layeredSurfaceTypes", 20, 4, 0, -1, "unsigned int" },
  { "hashIndex", 24, 2, 0, -1, "uint16_t" },
};
static const T4ZField f_101[] = {
  { "material", 0, 4, 1, -1, "Material*" },
  { "memory", 4, 4, 0, -1, "int" },
};
static const T4ZField f_102[] = {
  { "vertexDecl", 0, 4, 1, -1, "MaterialVertexDeclaration*" },
  { "vertexShader", 4, 4, 1, -1, "MaterialVertexShader*" },
  { "pixelShader", 8, 4, 1, -1, "MaterialPixelShader*" },
  { "perPrimArgCount", 12, 1, 0, -1, "unsigned char" },
  { "perObjArgCount", 13, 1, 0, -1, "unsigned char" },
  { "stableArgCount", 14, 1, 0, -1, "unsigned char" },
  { "customSamplerFlags", 15, 1, 0, -1, "unsigned char" },
  { "args", 16, 4, 1, -1, "MaterialShaderArgument*" },
};
static const T4ZField f_103[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "prog", 4, 12, 0, -1, "MaterialPixelShaderProgram" },
};
static const T4ZField f_104[] = {
  { "ps", 0, 4, 1, -1, "void *" },
  { "loadDef", 4, 8, 0, -1, "GfxPixelShaderLoadDef" },
};
static const T4ZField f_105[] = {
  { "type", 0, 2, 0, -1, "uint16_t" },
  { "dest", 2, 2, 0, -1, "uint16_t" },
  { "u", 4, 4, 0, -1, "MaterialArgumentDef" },
};
static const T4ZField f_106[] = {
  { "source", 0, 1, 0, -1, "unsigned char" },
  { "dest", 1, 1, 0, -1, "unsigned char" },
};
static const T4ZField f_107[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "flags", 4, 2, 0, -1, "uint16_t" },
  { "passCount", 6, 2, 0, -1, "uint16_t" },
  { "passArray", 8, 20, 0, 1, "MaterialPass" },
};
static const T4ZField f_108[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "worldVertFormat", 4, 1, 0, -1, "MaterialWorldVertexFormat" },
  { "hasBeenUploaded", 5, 1, 0, -1, "bool" },
  { "unused", 6, 1, 0, 1, "char" },
  { "remappedTechniqueSet", 8, 4, 1, -1, "MaterialTechniqueSet*" },
  { "techniques", 12, 236, 1, 59, "MaterialTechnique*" },
};
static const T4ZField f_109[] = {
  { "nameHash", 0, 4, 0, -1, "unsigned int" },
  { "nameStart", 4, 1, 0, -1, "char" },
  { "nameEnd", 5, 1, 0, -1, "char" },
  { "samplerState", 6, 1, 0, -1, "MaterialTextureDefSamplerState" },
  { "semantic", 7, 1, 0, -1, "unsigned char" },
  { "isMatureContent", 8, 1, 0, -1, "char" },
  { "u", 12, 4, 0, -1, "MaterialTextureDefInfo" },
};
static const T4ZField f_110[] = {
  { "filter", 0, 0, 0, -1, "unsigned char" },
  { "mipMap", 0, 0, 0, -1, "unsigned char" },
  { "clampU", 0, 0, 0, -1, "unsigned char" },
  { "clampV", 0, 0, 0, -1, "unsigned char" },
  { "clampW", 0, 0, 0, -1, "unsigned char" },
};
static const T4ZField f_111[] = {
  { "materialPrefix", 0, 4, 1, -1, "char*" },
  { "techniqueSetPrefix", 4, 4, 1, -1, "char*" },
};
static const T4ZField f_112[] = {
  { "streamCount", 0, 1, 0, -1, "unsigned char" },
  { "hasOptionalSource", 1, 1, 0, -1, "bool" },
  { "isLoaded", 2, 1, 0, -1, "bool" },
  { "routing", 4, 100, 0, -1, "MaterialVertexStreamRouting" },
};
static const T4ZField f_113[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "prog", 4, 12, 0, -1, "MaterialVertexShaderProgram" },
};
static const T4ZField f_114[] = {
  { "vs", 0, 4, 1, -1, "void *" },
  { "loadDef", 4, 8, 0, -1, "GfxVertexShaderLoadDef" },
};
static const T4ZField f_115[] = {
  { "data", 0, 32, 0, 16, "MaterialStreamRouting" },
  { "decl", 32, 68, 1, 17, "void *" },
};
static const T4ZField f_116[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "menuCount", 4, 4, 0, -1, "int" },
  { "menus", 8, 4, 1, -1, "menuDef_t**" },
};
static const T4ZField f_117[] = {
  { "value", 0, 4, 0, -1, "unsigned int" },
};
static const T4ZField f_118[] = {
  { "dataType", 0, 4, 0, -1, "expDataType" },
  { "internals", 4, 4, 0, -1, "operandInternalDataUnion" },
};
static const T4ZField f_119[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "header", 4, 20, 0, -1, "PackIndexHeader" },
  { "entries", 24, 4, 1, -1, "PackIndexEntry*" },
};
static const T4ZField f_120[] = {
  { "hash", 0, 4, 0, -1, "unsigned int" },
  { "offset", 4, 4, 0, -1, "unsigned int" },
  { "size", 8, 4, 0, -1, "unsigned int" },
};
static const T4ZField f_121[] = {
  { "magic", 0, 4, 0, -1, "unsigned int" },
  { "timestamp", 4, 4, 0, -1, "unsigned int" },
  { "count", 8, 4, 0, -1, "unsigned int" },
  { "alignment", 12, 4, 0, -1, "unsigned int" },
  { "data_start", 16, 4, 0, -1, "unsigned int" },
};
static const T4ZField f_122[] = {
  { "nodeCount", 0, 4, 0, -1, "unsigned int" },
  { "nodes", 4, 4, 1, -1, "pathnode_t*" },
  { "basenodes", 8, 4, 1, -1, "pathbasenode_t*" },
  { "chainNodeCount", 12, 4, 0, -1, "unsigned int" },
  { "chainNodeForNode", 16, 4, 1, -1, "uint16_t*" },
  { "nodeForChainNode", 20, 4, 1, -1, "uint16_t*" },
  { "visBytes", 24, 4, 0, -1, "int" },
  { "pathVis", 28, 4, 1, -1, "char*" },
  { "nodeTreeCount", 32, 4, 0, -1, "int" },
  { "nodeTree", 36, 4, 1, -1, "pathnode_tree_t*" },
};
static const T4ZField f_123[] = {
  { "targetname", 0, 2, 0, -1, "ScriptString" },
  { "type", 4, 4, 0, -1, "ConstraintType" },
  { "attach_point_type1", 8, 4, 0, -1, "AttachPointType" },
  { "target_index1", 12, 4, 0, -1, "int" },
  { "target_ent1", 16, 2, 0, -1, "ScriptString" },
  { "target_bone1", 20, 4, 1, -1, "char*" },
  { "attach_point_type2", 24, 4, 0, -1, "AttachPointType" },
  { "target_index2", 28, 4, 0, -1, "int" },
  { "target_ent2", 32, 2, 0, -1, "ScriptString" },
  { "target_bone2", 36, 4, 1, -1, "char*" },
  { "offset", 40, 12, 0, 3, "float" },
  { "pos", 52, 12, 0, 3, "float" },
  { "pos2", 64, 12, 0, 3, "float" },
  { "dir", 76, 12, 0, 3, "float" },
  { "flags", 88, 4, 0, -1, "int" },
  { "timeout", 92, 4, 0, -1, "int" },
  { "min_health", 96, 4, 0, -1, "int" },
  { "max_health", 100, 4, 0, -1, "int" },
  { "distance", 104, 4, 0, -1, "float" },
  { "damp", 108, 4, 0, -1, "float" },
  { "power", 112, 4, 0, -1, "float" },
  { "scale", 116, 12, 0, 3, "float" },
  { "spin_scale", 128, 4, 0, -1, "float" },
  { "minAngle", 132, 4, 0, -1, "float" },
  { "maxAngle", 136, 4, 0, -1, "float" },
  { "material", 140, 4, 1, -1, "Material*" },
  { "constraintHandle", 144, 4, 0, -1, "int" },
  { "rope_index", 148, 4, 0, -1, "int" },
};
static const T4ZField f_124[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "count", 4, 4, 0, -1, "unsigned int" },
  { "data", 8, 2432, 0, 16, "PhysConstraint" },
};
static const T4ZField f_125[] = {
  { "brush", 0, 4, 1, -1, "BrushWrapper*" },
  { "type", 4, 4, 0, -1, "int" },
  { "orientation", 8, 36, 0, 9, "float" },
  { "offset", 44, 12, 0, 3, "float" },
  { "halfLengths", 56, 12, 0, 3, "float" },
};
static const T4ZField f_126[] = {
  { "count", 0, 4, 0, -1, "unsigned int" },
  { "geoms", 4, 4, 1, -1, "PhysGeomInfo*" },
  { "mass", 8, 12, 0, -1, "PhysMass" },
};
static const T4ZField f_127[] = {
  { "centerOfMass", 0, 12, 0, 3, "float" },
};
static const T4ZField f_128[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "type", 4, 4, 0, -1, "int" },
  { "mass", 8, 4, 0, -1, "float" },
  { "bounce", 12, 4, 0, -1, "float" },
  { "friction", 16, 4, 0, -1, "float" },
  { "bulletForceScale", 20, 4, 0, -1, "float" },
  { "explosiveForceScale", 24, 4, 0, -1, "float" },
  { "sndAliasPrefix", 28, 4, 1, -1, "char*" },
  { "piecesSpreadFraction", 32, 4, 0, -1, "float" },
  { "piecesUpwardVelocity", 36, 4, 0, -1, "float" },
  { "canFloat", 40, 4, 0, -1, "int" },
  { "gravityScale", 44, 4, 0, -1, "float" },
};
static const T4ZField f_129[] = {
  { "mass", 0, 4, 0, -1, "float" },
  { "bounce", 4, 4, 0, -1, "float" },
  { "friction", 8, 4, 0, -1, "float" },
  { "isFrictionInfinity", 12, 4, 0, -1, "int" },
  { "bulletForceScale", 16, 4, 0, -1, "float" },
  { "explosiveForceScale", 20, 4, 0, -1, "float" },
  { "sndAliasPrefix", 24, 4, 1, -1, "char*" },
  { "piecesSpreadFraction", 28, 4, 0, -1, "float" },
  { "piecesUpwardVelocity", 32, 4, 0, -1, "float" },
  { "canFloat", 36, 4, 0, -1, "int" },
  { "gravityScale", 40, 4, 0, -1, "float" },
};
static const T4ZField f_130[] = {
  { "platform", 0, 2, 0, 2, "char" },
};
static const T4ZField f_131[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "buffer", 4, 4, 1, -1, "unsigned char*" },
  { "size", 8, 4, 0, -1, "unsigned int" },
};
static const T4ZField f_132[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "len", 4, 4, 0, -1, "int" },
  { "buffer", 8, 4, 1, -1, "char*" },
};
static const T4ZField f_133[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "reverbPatches", 4, 5120, 0, 64, "snd_reverb_patch_t" },
  { "buses", 5124, 4096, 0, 64, "snd_bus_info_t" },
  { "curves", 9220, 3200, 0, 32, "snd_curve_t" },
  { "speakerMaps", 12420, 10240, 0, 32, "snd_speaker_map_set" },
  { "masterEffects", 22660, 896, 0, 16, "snd_master_effect_t" },
};
static const T4ZField f_134[] = {
  { "type", 0, 1, 0, -1, "unsigned char" },
  { "exists", 1, 1, 0, -1, "unsigned char" },
  { "u", 4, 16, 0, -1, "SoundFileRef" },
};
static const T4ZField f_135[] = {
  { "isDefault", 0, 1, 0, -1, "bool" },
  { "name", 4, 4, 1, -1, "char*" },
  { "channelMaps", 8, 400, 0, 4, "MSSChannelMap" },
};
static const T4ZField f_136[] = {
  { "hash", 0, 4, 0, -1, "unsigned int" },
  { "dir", 4, 4, 1, -1, "char*" },
  { "name", 8, 4, 1, -1, "char*" },
};
static const T4ZField f_137[] = {
  { "filename", 0, 12, 0, -1, "StreamFileName" },
  { "primeSnd", 12, 4, 1, -1, "PrimedSound*" },
};
static const T4ZField f_138[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "columnCount", 4, 4, 0, -1, "int" },
  { "rowCount", 8, 4, 0, -1, "int" },
  { "values", 12, 4, 1, -1, "char**" },
};
static const T4ZField f_139[] = {
  { "name", 0, 64, 0, 64, "char" },
  { "ambientScale", 64, 4, 0, -1, "float" },
  { "ambientColor", 68, 12, 0, 3, "float" },
  { "diffuseFraction", 80, 4, 0, -1, "float" },
  { "sunLight", 84, 4, 0, -1, "float" },
  { "sunColor", 88, 12, 0, 3, "float" },
  { "diffuseColor", 100, 12, 0, 3, "float" },
  { "diffuseColorHasBeenSet", 112, 1, 0, -1, "bool" },
  { "angles", 116, 12, 0, 3, "float" },
  { "treeScatterIntensity", 128, 4, 0, -1, "float" },
  { "treeScatterAmount", 132, 4, 0, -1, "float" },
};
static const T4ZField f_140[] = {
  { "floatTime", 0, 4, 0, -1, "float" },
};
static const T4ZField f_141[] = {
  { "szInternalName", 0, 4, 1, -1, "char*" },
  { "szDisplayName", 4, 4, 1, -1, "char*" },
  { "szOverlayName", 8, 4, 1, -1, "char*" },
  { "gunXModel", 12, 64, 1, 16, "XModel*" },
  { "handXModel", 76, 4, 1, -1, "XModel*" },
  { "szXAnims", 80, 140, 1, 35, "char*" },
  { "szModeName", 220, 4, 1, -1, "char*" },
  { "hideTags", 224, 16, 0, 8, "uint16_t" },
  { "notetrackSoundMapKeys", 240, 40, 0, 20, "uint16_t" },
  { "notetrackSoundMapValues", 280, 40, 0, 20, "uint16_t" },
  { "playerAnimType", 320, 4, 0, -1, "int" },
  { "weapType", 324, 4, 0, -1, "weapType_t" },
  { "weapClass", 328, 4, 0, -1, "weapClass_t" },
  { "penetrateType", 332, 4, 0, -1, "PenetrateType" },
  { "impactType", 336, 4, 0, -1, "ImpactType" },
  { "inventoryType", 340, 4, 0, -1, "weapInventoryType_t" },
  { "fireType", 344, 4, 0, -1, "weapFireType_t" },
  { "clipType", 348, 4, 0, -1, "weapClipType_t" },
  { "overheatWeapon", 352, 4, 0, -1, "int" },
  { "overheatRate", 356, 4, 0, -1, "float" },
  { "cooldownRate", 360, 4, 0, -1, "float" },
  { "overheatEndVal", 364, 4, 0, -1, "float" },
  { "coolWhileFiring", 368, 4, 0, -1, "int" },
  { "offhandClass", 372, 4, 0, -1, "OffhandClass" },
  { "stance", 376, 4, 0, -1, "weapStance_t" },
  { "viewFlashEffect", 380, 4, 1, -1, "FxEffectDef*" },
  { "worldFlashEffect", 384, 4, 1, -1, "FxEffectDef*" },
  { "pickupSound", 388, 4, 0, -1, "SndAliasCustom" },
  { "pickupSoundPlayer", 392, 4, 0, -1, "SndAliasCustom" },
  { "ammoPickupSound", 396, 4, 0, -1, "SndAliasCustom" },
  { "ammoPickupSoundPlayer", 400, 4, 0, -1, "SndAliasCustom" },
  { "projectileSound", 404, 4, 0, -1, "SndAliasCustom" },
  { "pullbackSound", 408, 4, 0, -1, "SndAliasCustom" },
  { "pullbackSoundPlayer", 412, 4, 0, -1, "SndAliasCustom" },
  { "fireSound", 416, 4, 0, -1, "SndAliasCustom" },
  { "fireSoundPlayer", 420, 4, 0, -1, "SndAliasCustom" },
  { "fireLoopSound", 424, 4, 0, -1, "SndAliasCustom" },
  { "fireLoopSoundPlayer", 428, 4, 0, -1, "SndAliasCustom" },
  { "fireStopSound", 432, 4, 0, -1, "SndAliasCustom" },
  { "fireStopSoundPlayer", 436, 4, 0, -1, "SndAliasCustom" },
  { "fireLastSound", 440, 4, 0, -1, "SndAliasCustom" },
  { "fireLastSoundPlayer", 444, 4, 0, -1, "SndAliasCustom" },
  { "emptyFireSound", 448, 4, 0, -1, "SndAliasCustom" },
  { "emptyFireSoundPlayer", 452, 4, 0, -1, "SndAliasCustom" },
  { "crackSound", 456, 4, 0, -1, "SndAliasCustom" },
  { "whizbySound", 460, 4, 0, -1, "SndAliasCustom" },
  { "meleeSwipeSound", 464, 4, 0, -1, "SndAliasCustom" },
  { "meleeSwipeSoundPlayer", 468, 4, 0, -1, "SndAliasCustom" },
  { "meleeHitSound", 472, 4, 0, -1, "SndAliasCustom" },
  { "meleeMissSound", 476, 4, 0, -1, "SndAliasCustom" },
  { "rechamberSound", 480, 4, 0, -1, "SndAliasCustom" },
  { "rechamberSoundPlayer", 484, 4, 0, -1, "SndAliasCustom" },
  { "reloadSound", 488, 4, 0, -1, "SndAliasCustom" },
  { "reloadSoundPlayer", 492, 4, 0, -1, "SndAliasCustom" },
  { "reloadEmptySound", 496, 4, 0, -1, "SndAliasCustom" },
  { "reloadEmptySoundPlayer", 500, 4, 0, -1, "SndAliasCustom" },
  { "reloadStartSound", 504, 4, 0, -1, "SndAliasCustom" },
  { "reloadStartSoundPlayer", 508, 4, 0, -1, "SndAliasCustom" },
  { "reloadEndSound", 512, 4, 0, -1, "SndAliasCustom" },
  { "reloadEndSoundPlayer", 516, 4, 0, -1, "SndAliasCustom" },
  { "rotateLoopSound", 520, 4, 0, -1, "SndAliasCustom" },
  { "rotateLoopSoundPlayer", 524, 4, 0, -1, "SndAliasCustom" },
  { "deploySound", 528, 4, 0, -1, "SndAliasCustom" },
  { "deploySoundPlayer", 532, 4, 0, -1, "SndAliasCustom" },
  { "finishDeploySound", 536, 4, 0, -1, "SndAliasCustom" },
  { "finishDeploySoundPlayer", 540, 4, 0, -1, "SndAliasCustom" },
  { "breakdownSound", 544, 4, 0, -1, "SndAliasCustom" },
  { "breakdownSoundPlayer", 548, 4, 0, -1, "SndAliasCustom" },
  { "finishBreakdownSound", 552, 4, 0, -1, "SndAliasCustom" },
  { "finishBreakdownSoundPlayer", 556, 4, 0, -1, "SndAliasCustom" },
  { "detonateSound", 560, 4, 0, -1, "SndAliasCustom" },
  { "detonateSoundPlayer", 564, 4, 0, -1, "SndAliasCustom" },
  { "nightVisionWearSound", 568, 4, 0, -1, "SndAliasCustom" },
  { "nightVisionWearSoundPlayer", 572, 4, 0, -1, "SndAliasCustom" },
  { "nightVisionRemoveSound", 576, 4, 0, -1, "SndAliasCustom" },
  { "nightVisionRemoveSoundPlayer", 580, 4, 0, -1, "SndAliasCustom" },
  { "altSwitchSound", 584, 4, 0, -1, "SndAliasCustom" },
  { "altSwitchSoundPlayer", 588, 4, 0, -1, "SndAliasCustom" },
  { "raiseSound", 592, 4, 0, -1, "SndAliasCustom" },
  { "raiseSoundPlayer", 596, 4, 0, -1, "SndAliasCustom" },
  { "firstRaiseSound", 600, 4, 0, -1, "SndAliasCustom" },
  { "firstRaiseSoundPlayer", 604, 4, 0, -1, "SndAliasCustom" },
  { "putawaySound", 608, 4, 0, -1, "SndAliasCustom" },
  { "putawaySoundPlayer", 612, 4, 0, -1, "SndAliasCustom" },
  { "overheatSound", 616, 4, 0, -1, "SndAliasCustom" },
  { "overheatSoundPlayer", 620, 4, 0, -1, "SndAliasCustom" },
  { "bounceSound", 624, 4, 1, -1, "SndAliasCustom*" },
  { "standMountedWeapdef", 628, 4, 1, -1, "char*" },
  { "crouchMountedWeapdef", 632, 4, 1, -1, "char*" },
  { "proneMountedWeapdef", 636, 4, 1, -1, "char*" },
  { "standMountedIndex", 640, 4, 0, -1, "int" },
  { "crouchMountedIndex", 644, 4, 0, -1, "int" },
  { "proneMountedIndex", 648, 4, 0, -1, "int" },
  { "viewShellEjectEffect", 652, 4, 1, -1, "FxEffectDef*" },
  { "worldShellEjectEffect", 656, 4, 1, -1, "FxEffectDef*" },
  { "viewLastShotEjectEffect", 660, 4, 1, -1, "FxEffectDef*" },
  { "worldLastShotEjectEffect", 664, 4, 1, -1, "FxEffectDef*" },
  { "reticleCenter", 668, 4, 1, -1, "Material*" },
  { "reticleSide", 672, 4, 1, -1, "Material*" },
  { "iReticleCenterSize", 676, 4, 0, -1, "int" },
  { "iReticleSideSize", 680, 4, 0, -1, "int" },
  { "iReticleMinOfs", 684, 4, 0, -1, "int" },
  { "activeReticleType", 688, 4, 0, -1, "activeReticleType_t" },
  { "vStandMove", 692, 12, 0, 3, "float" },
  { "vStandRot", 704, 12, 0, 3, "float" },
  { "vDuckedOfs", 716, 12, 0, 3, "float" },
  { "vDuckedMove", 728, 12, 0, 3, "float" },
  { "duckedSprintOfs", 740, 12, 0, 3, "float" },
  { "duckedSprintRot", 752, 12, 0, 3, "float" },
  { "duckedSprintBob", 764, 8, 0, 2, "float" },
  { "duckedSprintScale", 772, 4, 0, -1, "float" },
  { "sprintOfs", 776, 12, 0, 3, "float" },
  { "sprintRot", 788, 12, 0, 3, "float" },
  { "sprintBob", 800, 8, 0, 2, "float" },
  { "sprintScale", 808, 4, 0, -1, "float" },
  { "vDuckedRot", 812, 12, 0, 3, "float" },
  { "vProneOfs", 824, 12, 0, 3, "float" },
  { "vProneMove", 836, 12, 0, 3, "float" },
  { "vProneRot", 848, 12, 0, 3, "float" },
  { "fPosMoveRate", 860, 4, 0, -1, "float" },
  { "fPosProneMoveRate", 864, 4, 0, -1, "float" },
  { "fStandMoveMinSpeed", 868, 4, 0, -1, "float" },
  { "fDuckedMoveMinSpeed", 872, 4, 0, -1, "float" },
  { "fProneMoveMinSpeed", 876, 4, 0, -1, "float" },
  { "fPosRotRate", 880, 4, 0, -1, "float" },
  { "fPosProneRotRate", 884, 4, 0, -1, "float" },
  { "fStandRotMinSpeed", 888, 4, 0, -1, "float" },
  { "fDuckedRotMinSpeed", 892, 4, 0, -1, "float" },
  { "fProneRotMinSpeed", 896, 4, 0, -1, "float" },
  { "worldModel", 900, 64, 1, 16, "XModel*" },
  { "worldClipModel", 964, 4, 1, -1, "XModel*" },
  { "rocketModel", 968, 4, 1, -1, "XModel*" },
  { "knifeModel", 972, 4, 1, -1, "XModel*" },
  { "worldKnifeModel", 976, 4, 1, -1, "XModel*" },
  { "mountedModel", 980, 4, 1, -1, "XModel*" },
  { "hudIcon", 984, 4, 1, -1, "Material*" },
  { "hudIconRatio", 988, 4, 0, -1, "weaponIconRatioType_t" },
  { "ammoCounterIcon", 992, 4, 1, -1, "Material*" },
  { "ammoCounterIconRatio", 996, 4, 0, -1, "weaponIconRatioType_t" },
  { "ammoCounterClip", 1000, 4, 0, -1, "ammoCounterClipType_t" },
  { "iStartAmmo", 1004, 4, 0, -1, "int" },
  { "szAmmoName", 1008, 4, 1, -1, "char*" },
  { "iAmmoIndex", 1012, 4, 0, -1, "int" },
  { "szClipName", 1016, 4, 1, -1, "char*" },
  { "iClipIndex", 1020, 4, 0, -1, "int" },
  { "iHeatIndex", 1024, 4, 0, -1, "int" },
  { "iMaxAmmo", 1028, 4, 0, -1, "int" },
  { "iClipSize", 1032, 4, 0, -1, "int" },
  { "shotCount", 1036, 4, 0, -1, "int" },
  { "szSharedAmmoCapName", 1040, 4, 1, -1, "char*" },
  { "iSharedAmmoCapIndex", 1044, 4, 0, -1, "int" },
  { "iSharedAmmoCap", 1048, 4, 0, -1, "int" },
  { "unlimitedAmmo", 1052, 4, 0, -1, "int" },
  { "damage", 1056, 4, 0, -1, "int" },
  { "damageDuration", 1060, 4, 0, -1, "int" },
  { "damageInterval", 1064, 4, 0, -1, "int" },
  { "playerDamage", 1068, 4, 0, -1, "int" },
  { "iMeleeDamage", 1072, 4, 0, -1, "int" },
  { "iDamageType", 1076, 4, 0, -1, "int" },
  { "iFireDelay", 1080, 4, 0, -1, "int" },
  { "iMeleeDelay", 1084, 4, 0, -1, "int" },
  { "meleeChargeDelay", 1088, 4, 0, -1, "int" },
  { "iDetonateDelay", 1092, 4, 0, -1, "int" },
  { "iFireTime", 1096, 4, 0, -1, "int" },
  { "iRechamberTime", 1100, 4, 0, -1, "int" },
  { "iRechamberBoltTime", 1104, 4, 0, -1, "int" },
  { "iHoldFireTime", 1108, 4, 0, -1, "int" },
  { "iDetonateTime", 1112, 4, 0, -1, "int" },
  { "iMeleeTime", 1116, 4, 0, -1, "int" },
  { "meleeChargeTime", 1120, 4, 0, -1, "int" },
  { "iReloadTime", 1124, 4, 0, -1, "int" },
  { "reloadShowRocketTime", 1128, 4, 0, -1, "int" },
  { "iReloadEmptyTime", 1132, 4, 0, -1, "int" },
  { "iReloadAddTime", 1136, 4, 0, -1, "int" },
  { "reloadEmptyAddTime", 1140, 4, 0, -1, "int" },
  { "iReloadStartTime", 1144, 4, 0, -1, "int" },
  { "iReloadStartAddTime", 1148, 4, 0, -1, "int" },
  { "iReloadEndTime", 1152, 4, 0, -1, "int" },
  { "iDropTime", 1156, 4, 0, -1, "int" },
  { "iRaiseTime", 1160, 4, 0, -1, "int" },
  { "iAltDropTime", 1164, 4, 0, -1, "int" },
  { "iAltRaiseTime", 1168, 4, 0, -1, "int" },
  { "quickDropTime", 1172, 4, 0, -1, "int" },
  { "quickRaiseTime", 1176, 4, 0, -1, "int" },
  { "iFirstRaiseTime", 1180, 4, 0, -1, "int" },
  { "iEmptyRaiseTime", 1184, 4, 0, -1, "int" },
  { "iEmptyDropTime", 1188, 4, 0, -1, "int" },
  { "sprintInTime", 1192, 4, 0, -1, "int" },
  { "sprintLoopTime", 1196, 4, 0, -1, "int" },
  { "sprintOutTime", 1200, 4, 0, -1, "int" },
  { "deployTime", 1204, 4, 0, -1, "int" },
  { "breakdownTime", 1208, 4, 0, -1, "int" },
  { "nightVisionWearTime", 1212, 4, 0, -1, "int" },
  { "nightVisionWearTimeFadeOutEnd", 1216, 4, 0, -1, "int" },
  { "nightVisionWearTimePowerUp", 1220, 4, 0, -1, "int" },
  { "nightVisionRemoveTime", 1224, 4, 0, -1, "int" },
  { "nightVisionRemoveTimePowerDown", 1228, 4, 0, -1, "int" },
  { "nightVisionRemoveTimeFadeInStart", 1232, 4, 0, -1, "int" },
  { "fuseTime", 1236, 4, 0, -1, "int" },
  { "aiFuseTime", 1240, 4, 0, -1, "int" },
  { "requireLockonToFire", 1244, 4, 0, -1, "int" },
  { "noAdsWhenMagEmpty", 1248, 4, 0, -1, "int" },
  { "avoidDropCleanup", 1252, 4, 0, -1, "int" },
  { "autoAimRange", 1256, 4, 0, -1, "float" },
  { "aimAssistRange", 1260, 4, 0, -1, "float" },
  { "aimAssistRangeAds", 1264, 4, 0, -1, "float" },
  { "mountableWeapon", 1268, 4, 0, -1, "int" },
  { "aimPadding", 1272, 4, 0, -1, "float" },
  { "enemyCrosshairRange", 1276, 4, 0, -1, "float" },
  { "crosshairColorChange", 1280, 4, 0, -1, "int" },
  { "moveSpeedScale", 1284, 4, 0, -1, "float" },
  { "adsMoveSpeedScale", 1288, 4, 0, -1, "float" },
  { "sprintDurationScale", 1292, 4, 0, -1, "float" },
  { "fAdsZoomFov", 1296, 4, 0, -1, "float" },
  { "fAdsZoomInFrac", 1300, 4, 0, -1, "float" },
  { "fAdsZoomOutFrac", 1304, 4, 0, -1, "float" },
  { "overlayMaterial", 1308, 4, 1, -1, "Material*" },
  { "overlayMaterialLowRes", 1312, 4, 1, -1, "Material*" },
  { "overlayReticle", 1316, 4, 0, -1, "weapOverlayReticle_t" },
  { "overlayInterface", 1320, 4, 0, -1, "WeapOverlayInterface_t" },
  { "overlayWidth", 1324, 4, 0, -1, "float" },
  { "overlayHeight", 1328, 4, 0, -1, "float" },
  { "fAdsBobFactor", 1332, 4, 0, -1, "float" },
  { "fAdsViewBobMult", 1336, 4, 0, -1, "float" },
  { "fHipSpreadStandMin", 1340, 4, 0, -1, "float" },
  { "fHipSpreadDuckedMin", 1344, 4, 0, -1, "float" },
  { "fHipSpreadProneMin", 1348, 4, 0, -1, "float" },
  { "hipSpreadStandMax", 1352, 4, 0, -1, "float" },
  { "hipSpreadDuckedMax", 1356, 4, 0, -1, "float" },
  { "hipSpreadProneMax", 1360, 4, 0, -1, "float" },
  { "fHipSpreadDecayRate", 1364, 4, 0, -1, "float" },
  { "fHipSpreadFireAdd", 1368, 4, 0, -1, "float" },
  { "fHipSpreadTurnAdd", 1372, 4, 0, -1, "float" },
  { "fHipSpreadMoveAdd", 1376, 4, 0, -1, "float" },
  { "fHipSpreadDuckedDecay", 1380, 4, 0, -1, "float" },
  { "fHipSpreadProneDecay", 1384, 4, 0, -1, "float" },
  { "fHipReticleSidePos", 1388, 4, 0, -1, "float" },
  { "iAdsTransInTime", 1392, 4, 0, -1, "int" },
  { "iAdsTransOutTime", 1396, 4, 0, -1, "int" },
  { "fAdsIdleAmount", 1400, 4, 0, -1, "float" },
  { "fHipIdleAmount", 1404, 4, 0, -1, "float" },
  { "adsIdleSpeed", 1408, 4, 0, -1, "float" },
  { "hipIdleSpeed", 1412, 4, 0, -1, "float" },
  { "fIdleCrouchFactor", 1416, 4, 0, -1, "float" },
  { "fIdleProneFactor", 1420, 4, 0, -1, "float" },
  { "fGunMaxPitch", 1424, 4, 0, -1, "float" },
  { "fGunMaxYaw", 1428, 4, 0, -1, "float" },
  { "swayMaxAngle", 1432, 4, 0, -1, "float" },
  { "swayLerpSpeed", 1436, 4, 0, -1, "float" },
  { "swayPitchScale", 1440, 4, 0, -1, "float" },
  { "swayYawScale", 1444, 4, 0, -1, "float" },
  { "swayHorizScale", 1448, 4, 0, -1, "float" },
  { "swayVertScale", 1452, 4, 0, -1, "float" },
  { "swayShellShockScale", 1456, 4, 0, -1, "float" },
  { "adsSwayMaxAngle", 1460, 4, 0, -1, "float" },
  { "adsSwayLerpSpeed", 1464, 4, 0, -1, "float" },
  { "adsSwayPitchScale", 1468, 4, 0, -1, "float" },
  { "adsSwayYawScale", 1472, 4, 0, -1, "float" },
  { "adsSwayHorizScale", 1476, 4, 0, -1, "float" },
  { "adsSwayVertScale", 1480, 4, 0, -1, "float" },
  { "bRifleBullet", 1484, 4, 0, -1, "int" },
  { "armorPiercing", 1488, 4, 0, -1, "int" },
  { "bBoltAction", 1492, 4, 0, -1, "int" },
  { "aimDownSight", 1496, 4, 0, -1, "int" },
  { "bRechamberWhileAds", 1500, 4, 0, -1, "int" },
  { "adsViewErrorMin", 1504, 4, 0, -1, "float" },
  { "adsViewErrorMax", 1508, 4, 0, -1, "float" },
  { "bCookOffHold", 1512, 4, 0, -1, "int" },
  { "bClipOnly", 1516, 4, 0, -1, "int" },
  { "canUseInVehicle", 1520, 4, 0, -1, "int" },
  { "noDropsOrRaises", 1524, 4, 0, -1, "int" },
  { "adsFireOnly", 1528, 4, 0, -1, "int" },
  { "cancelAutoHolsterWhenEmpty", 1532, 4, 0, -1, "int" },
  { "suppressAmmoReserveDisplay", 1536, 4, 0, -1, "int" },
  { "enhanced", 1540, 4, 0, -1, "int" },
  { "laserSightDuringNightvision", 1544, 4, 0, -1, "int" },
  { "bayonet", 1548, 4, 0, -1, "int" },
  { "killIcon", 1552, 4, 1, -1, "Material*" },
  { "killIconRatio", 1556, 4, 0, -1, "weaponIconRatioType_t" },
  { "flipKillIcon", 1560, 4, 0, -1, "int" },
  { "dpadIcon", 1564, 4, 1, -1, "Material*" },
  { "dpadIconRatio", 1568, 4, 0, -1, "weaponIconRatioType_t" },
  { "bNoPartialReload", 1572, 4, 0, -1, "int" },
  { "bSegmentedReload", 1576, 4, 0, -1, "int" },
  { "noADSAutoReload", 1580, 4, 0, -1, "int" },
  { "iReloadAmmoAdd", 1584, 4, 0, -1, "int" },
  { "iReloadStartAdd", 1588, 4, 0, -1, "int" },
  { "szAltWeaponName", 1592, 4, 1, -1, "char*" },
  { "altWeaponIndex", 1596, 4, 0, -1, "unsigned int" },
  { "iDropAmmoMin", 1600, 4, 0, -1, "int" },
  { "iDropAmmoMax", 1604, 4, 0, -1, "int" },
  { "blocksProne", 1608, 4, 0, -1, "int" },
  { "silenced", 1612, 4, 0, -1, "int" },
  { "iExplosionRadius", 1616, 4, 0, -1, "int" },
  { "iExplosionRadiusMin", 1620, 4, 0, -1, "int" },
  { "iExplosionInnerDamage", 1624, 4, 0, -1, "int" },
  { "iExplosionOuterDamage", 1628, 4, 0, -1, "int" },
  { "damageConeAngle", 1632, 4, 0, -1, "float" },
  { "iProjectileSpeed", 1636, 4, 0, -1, "int" },
  { "iProjectileSpeedUp", 1640, 4, 0, -1, "int" },
  { "iProjectileSpeedForward", 1644, 4, 0, -1, "int" },
  { "iProjectileActivateDist", 1648, 4, 0, -1, "int" },
  { "projLifetime", 1652, 4, 0, -1, "float" },
  { "timeToAccelerate", 1656, 4, 0, -1, "float" },
  { "projectileCurvature", 1660, 4, 0, -1, "float" },
  { "projectileModel", 1664, 4, 1, -1, "XModel*" },
  { "projExplosion", 1668, 4, 0, -1, "weapProjExposion_t" },
  { "projExplosionEffect", 1672, 4, 1, -1, "FxEffectDef*" },
  { "projExplosionEffectForceNormalUp", 1676, 4, 0, -1, "int" },
  { "projDudEffect", 1680, 4, 1, -1, "FxEffectDef*" },
  { "projExplosionSound", 1684, 4, 0, -1, "SndAliasCustom" },
  { "projDudSound", 1688, 4, 0, -1, "SndAliasCustom" },
  { "mortarShellSound", 1692, 4, 0, -1, "SndAliasCustom" },
  { "tankShellSound", 1696, 4, 0, -1, "SndAliasCustom" },
  { "bProjImpactExplode", 1700, 4, 0, -1, "int" },
  { "stickiness", 1704, 4, 0, -1, "WeapStickinessType" },
  { "hasDetonator", 1708, 4, 0, -1, "int" },
  { "timedDetonation", 1712, 4, 0, -1, "int" },
  { "rotate", 1716, 4, 0, -1, "int" },
  { "holdButtonToThrow", 1720, 4, 0, -1, "int" },
  { "freezeMovementWhenFiring", 1724, 4, 0, -1, "int" },
  { "lowAmmoWarningThreshold", 1728, 4, 0, -1, "float" },
  { "parallelBounce", 1732, 124, 0, 31, "float" },
  { "perpendicularBounce", 1856, 124, 0, 31, "float" },
  { "projTrailEffect", 1980, 4, 1, -1, "FxEffectDef*" },
  { "vProjectileColor", 1984, 12, 0, 3, "float" },
  { "guidedMissileType", 1996, 4, 0, -1, "guidedMissileType_t" },
  { "maxSteeringAccel", 2000, 4, 0, -1, "float" },
  { "projIgnitionDelay", 2004, 4, 0, -1, "int" },
  { "projIgnitionEffect", 2008, 4, 1, -1, "FxEffectDef*" },
  { "projIgnitionSound", 2012, 4, 0, -1, "SndAliasCustom" },
  { "fAdsAimPitch", 2016, 4, 0, -1, "float" },
  { "fAdsCrosshairInFrac", 2020, 4, 0, -1, "float" },
  { "fAdsCrosshairOutFrac", 2024, 4, 0, -1, "float" },
  { "adsGunKickReducedKickBullets", 2028, 4, 0, -1, "int" },
  { "adsGunKickReducedKickPercent", 2032, 4, 0, -1, "float" },
  { "fAdsGunKickPitchMin", 2036, 4, 0, -1, "float" },
  { "fAdsGunKickPitchMax", 2040, 4, 0, -1, "float" },
  { "fAdsGunKickYawMin", 2044, 4, 0, -1, "float" },
  { "fAdsGunKickYawMax", 2048, 4, 0, -1, "float" },
  { "fAdsGunKickAccel", 2052, 4, 0, -1, "float" },
  { "fAdsGunKickSpeedMax", 2056, 4, 0, -1, "float" },
  { "fAdsGunKickSpeedDecay", 2060, 4, 0, -1, "float" },
  { "fAdsGunKickStaticDecay", 2064, 4, 0, -1, "float" },
  { "fAdsViewKickPitchMin", 2068, 4, 0, -1, "float" },
  { "fAdsViewKickPitchMax", 2072, 4, 0, -1, "float" },
  { "fAdsViewKickYawMin", 2076, 4, 0, -1, "float" },
  { "fAdsViewKickYawMax", 2080, 4, 0, -1, "float" },
  { "fAdsViewKickCenterSpeed", 2084, 4, 0, -1, "float" },
  { "fAdsViewScatterMin", 2088, 4, 0, -1, "float" },
  { "fAdsViewScatterMax", 2092, 4, 0, -1, "float" },
  { "fAdsSpread", 2096, 4, 0, -1, "float" },
  { "hipGunKickReducedKickBullets", 2100, 4, 0, -1, "int" },
  { "hipGunKickReducedKickPercent", 2104, 4, 0, -1, "float" },
  { "fHipGunKickPitchMin", 2108, 4, 0, -1, "float" },
  { "fHipGunKickPitchMax", 2112, 4, 0, -1, "float" },
  { "fHipGunKickYawMin", 2116, 4, 0, -1, "float" },
  { "fHipGunKickYawMax", 2120, 4, 0, -1, "float" },
  { "fHipGunKickAccel", 2124, 4, 0, -1, "float" },
  { "fHipGunKickSpeedMax", 2128, 4, 0, -1, "float" },
  { "fHipGunKickSpeedDecay", 2132, 4, 0, -1, "float" },
  { "fHipGunKickStaticDecay", 2136, 4, 0, -1, "float" },
  { "fHipViewKickPitchMin", 2140, 4, 0, -1, "float" },
  { "fHipViewKickPitchMax", 2144, 4, 0, -1, "float" },
  { "fHipViewKickYawMin", 2148, 4, 0, -1, "float" },
  { "fHipViewKickYawMax", 2152, 4, 0, -1, "float" },
  { "fHipViewKickCenterSpeed", 2156, 4, 0, -1, "float" },
  { "fHipViewScatterMin", 2160, 4, 0, -1, "float" },
  { "fHipViewScatterMax", 2164, 4, 0, -1, "float" },
  { "fightDist", 2168, 4, 0, -1, "float" },
  { "maxDist", 2172, 4, 0, -1, "float" },
  { "aiVsAiAccuracyGraphName", 2176, 4, 1, -1, "char*" },
  { "aiVsPlayerAccuracyGraphName", 2180, 4, 1, -1, "char*" },
  { "aiVsAiAccuracyGraphKnots", 2184, 4, 1, -1, "vec2_t*" },
  { "aiVsPlayerAccuracyGraphKnots", 2188, 4, 1, -1, "vec2_t*" },
  { "originalAiVsAiAccuracyGraphKnots", 2192, 4, 1, -1, "vec2_t*" },
  { "originalAiVsPlayerAccuracyGraphKnots", 2196, 4, 1, -1, "vec2_t*" },
  { "aiVsAiAccuracyGraphKnotCount", 2200, 4, 0, -1, "int" },
  { "aiVsPlayerAccuracyGraphKnotCount", 2204, 4, 0, -1, "int" },
  { "originalAiVsAiAccuracyGraphKnotCount", 2208, 4, 0, -1, "int" },
  { "originalAiVsPlayerAccuracyGraphKnotCount", 2212, 4, 0, -1, "int" },
  { "iPositionReloadTransTime", 2216, 4, 0, -1, "int" },
  { "leftArc", 2220, 4, 0, -1, "float" },
  { "rightArc", 2224, 4, 0, -1, "float" },
  { "topArc", 2228, 4, 0, -1, "float" },
  { "bottomArc", 2232, 4, 0, -1, "float" },
  { "accuracy", 2236, 4, 0, -1, "float" },
  { "aiSpread", 2240, 4, 0, -1, "float" },
  { "playerSpread", 2244, 4, 0, -1, "float" },
  { "minTurnSpeed", 2248, 8, 0, 2, "float" },
  { "maxTurnSpeed", 2256, 8, 0, 2, "float" },
  { "pitchConvergenceTime", 2264, 4, 0, -1, "float" },
  { "yawConvergenceTime", 2268, 4, 0, -1, "float" },
  { "suppressTime", 2272, 4, 0, -1, "float" },
  { "maxRange", 2276, 4, 0, -1, "float" },
  { "fAnimHorRotateInc", 2280, 4, 0, -1, "float" },
  { "fPlayerPositionDist", 2284, 4, 0, -1, "float" },
  { "szUseHintString", 2288, 4, 1, -1, "char*" },
  { "dropHintString", 2292, 4, 1, -1, "char*" },
  { "iUseHintStringIndex", 2296, 4, 0, -1, "int" },
  { "dropHintStringIndex", 2300, 4, 0, -1, "int" },
  { "horizViewJitter", 2304, 4, 0, -1, "float" },
  { "vertViewJitter", 2308, 4, 0, -1, "float" },
  { "szScript", 2312, 4, 1, -1, "char*" },
  { "fOOPosAnimLength", 2316, 8, 0, 2, "float" },
  { "minDamage", 2324, 4, 0, -1, "int" },
  { "minPlayerDamage", 2328, 4, 0, -1, "int" },
  { "fMaxDamageRange", 2332, 4, 0, -1, "float" },
  { "fMinDamageRange", 2336, 4, 0, -1, "float" },
  { "destabilizationRateTime", 2340, 4, 0, -1, "float" },
  { "destabilizationCurvatureMax", 2344, 4, 0, -1, "float" },
  { "destabilizeDistance", 2348, 4, 0, -1, "int" },
  { "locationDamageMultipliers", 2352, 76, 0, 19, "float" },
  { "fireRumble", 2428, 4, 1, -1, "char*" },
  { "meleeImpactRumble", 2432, 4, 1, -1, "char*" },
  { "adsDofStart", 2436, 4, 0, -1, "float" },
  { "adsDofEnd", 2440, 4, 0, -1, "float" },
  { "hipDofStart", 2444, 4, 0, -1, "float" },
  { "hipDofEnd", 2448, 4, 0, -1, "float" },
  { "flameTableFirstPerson", 2452, 4, 1, -1, "char*" },
  { "flameTableThirdPerson", 2456, 4, 1, -1, "char*" },
  { "flameTableFirstPersonPtr", 2460, 4, 1, -1, "FlameTable*" },
  { "flameTableThirdPersonPtr", 2464, 4, 1, -1, "FlameTable*" },
  { "tagFx_preparationEffect", 2468, 4, 1, -1, "FxEffectDef*" },
  { "tagFlash_preparationEffect", 2472, 4, 1, -1, "FxEffectDef*" },
};
static const T4ZField f_142[] = {
  { "trans", 0, 4, 1, -1, "XAnimPartTrans*" },
  { "quat", 4, 4, 1, -1, "XAnimDeltaPartQuat*" },
};
static const T4ZField f_143[] = {
  { "size", 0, 2, 0, -1, "uint16_t" },
  { "u", 4, 8, 0, -1, "XAnimDeltaPartQuatData" },
};
static const T4ZField f_144[] = {
  { "frames", 0, 4, 1, -1, "XQuat2*" },
  { "indices", 4, 2, 0, -1, "XAnimDynamicIndicesQuat" },
};
static const T4ZField f_145[] = {
  { "name", 0, 2, 0, -1, "ScriptString" },
  { "time", 4, 4, 0, -1, "float" },
};
static const T4ZField f_146[] = {
  { "size", 0, 2, 0, -1, "uint16_t" },
  { "smallTrans", 2, 1, 0, -1, "unsigned char" },
  { "u", 4, 32, 0, -1, "XAnimPartTransData" },
};
static const T4ZField f_147[] = {
  { "mins", 0, 12, 0, -1, "vec3_t" },
  { "size", 12, 12, 0, -1, "vec3_t" },
  { "frames", 24, 4, 0, -1, "XAnimDynamicFrames" },
  { "indices", 28, 2, 0, -1, "XAnimDynamicIndicesTrans" },
};
static const T4ZField f_148[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "dataByteCount", 4, 2, 0, -1, "uint16_t" },
  { "dataShortCount", 6, 2, 0, -1, "uint16_t" },
  { "dataIntCount", 8, 2, 0, -1, "uint16_t" },
  { "randomDataByteCount", 10, 2, 0, -1, "uint16_t" },
  { "randomDataIntCount", 12, 2, 0, -1, "uint16_t" },
  { "numframes", 14, 2, 0, -1, "uint16_t" },
  { "bLoop", 16, 1, 0, -1, "bool" },
  { "bDelta", 17, 1, 0, -1, "bool" },
  { "boneCount", 18, 10, 0, 10, "unsigned char" },
  { "notifyCount", 28, 1, 0, -1, "unsigned char" },
  { "assetType", 29, 1, 0, -1, "unsigned char" },
  { "isDefault", 30, 1, 0, -1, "bool" },
  { "randomDataShortCount", 32, 4, 0, -1, "unsigned int" },
  { "indexCount", 36, 4, 0, -1, "unsigned int" },
  { "framerate", 40, 4, 0, -1, "float" },
  { "frequency", 44, 4, 0, -1, "float" },
  { "names", 48, 4, 1, -1, "ScriptString*" },
  { "dataByte", 52, 4, 1, -1, "unsigned char*" },
  { "dataShort", 56, 4, 1, -1, "int16_t*" },
  { "dataInt", 60, 4, 1, -1, "int*" },
  { "randomDataShort", 64, 4, 1, -1, "int16_t*" },
  { "randomDataByte", 68, 4, 1, -1, "unsigned char*" },
  { "randomDataInt", 72, 4, 1, -1, "int*" },
  { "indices", 76, 4, 0, -1, "XAnimIndices" },
  { "notify", 80, 4, 1, -1, "XAnimNotifyInfo*" },
  { "deltaPart", 84, 4, 1, -1, "XAnimDeltaPart*" },
};
static const T4ZField f_149[] = {
  { "bounds", 0, 24, 0, 2, "vec3_t" },
  { "offset", 24, 12, 0, -1, "vec3_t" },
  { "radiusSquared", 36, 4, 0, -1, "float" },
};
static const T4ZField f_150[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "numBones", 4, 1, 0, -1, "unsigned char" },
  { "numRootBones", 5, 1, 0, -1, "unsigned char" },
  { "numsurfs", 6, 1, 0, -1, "unsigned char" },
  { "lodRampType", 7, 1, 0, -1, "XModelLodRampType" },
  { "boneNames", 8, 4, 1, -1, "ScriptString*" },
  { "parentList", 12, 4, 1, -1, "unsigned char*" },
  { "quats", 16, 4, 1, -1, "XModelQuat*" },
  { "trans", 20, 4, 1, -1, "float*" },
  { "partClassification", 24, 4, 1, -1, "unsigned char*" },
  { "baseMat", 28, 4, 1, -1, "DObjAnimMat*" },
  { "surfs", 32, 4, 1, -1, "XSurface*" },
  { "materialHandles", 36, 4, 1, -1, "Material**" },
  { "lodInfo", 40, 112, 0, 4, "XModelLodInfo" },
  { "collSurfs", 152, 4, 1, -1, "XModelCollSurf_s*" },
  { "numCollSurfs", 156, 4, 0, -1, "int" },
  { "contents", 160, 4, 0, -1, "int" },
  { "boneInfo", 164, 4, 1, -1, "XBoneInfo*" },
  { "radius", 168, 4, 0, -1, "float" },
  { "mins", 172, 12, 0, -1, "vec3_t" },
  { "maxs", 184, 12, 0, -1, "vec3_t" },
  { "numLods", 196, 2, 0, -1, "uint16_t" },
  { "collLod", 198, 2, 0, -1, "int16_t" },
  { "streamInfo", 200, 1, 0, -1, "XModelStreamInfo" },
  { "memUsage", 204, 4, 0, -1, "int" },
  { "flags", 208, 1, 0, -1, "unsigned char" },
  { "bad", 209, 1, 0, -1, "bool" },
  { "physPreset", 212, 4, 1, -1, "PhysPreset*" },
  { "physGeoms", 216, 4, 1, -1, "PhysGeomList*" },
  { "collmap", 220, 4, 1, -1, "PhysGeomList*" },
  { "physConstraints", 224, 4, 1, -1, "PhysConstraints*" },
};
static const T4ZField f_151[] = {
  { "collTris", 0, 4, 1, -1, "XModelCollTri_s*" },
  { "numCollTris", 4, 4, 0, -1, "int" },
  { "mins", 8, 12, 0, 3, "float" },
  { "maxs", 20, 12, 0, 3, "float" },
  { "boneIdx", 32, 4, 0, -1, "int" },
  { "contents", 36, 4, 0, -1, "int" },
  { "surfFlags", 40, 4, 0, -1, "int" },
};
static const T4ZField f_152[] = {
  { "plane", 0, 16, 0, 4, "float" },
  { "svec", 16, 16, 0, 4, "float" },
  { "tvec", 32, 16, 0, 4, "float" },
};
static const T4ZField f_153[] = {
  { "lod", 0, 2, 0, -1, "uint16_t" },
  { "surfId", 2, 2, 0, -1, "uint16_t" },
};
static const T4ZField f_154[] = {
  { "dist", 0, 4, 0, -1, "float" },
  { "numsurfs", 4, 2, 0, -1, "uint16_t" },
  { "surfIndex", 6, 2, 0, -1, "uint16_t" },
  { "partBits", 8, 16, 0, 4, "int" },
  { "lod", 24, 1, 0, -1, "char" },
  { "smcIndexPlusOne", 25, 1, 0, -1, "char" },
  { "smcAllocBits", 26, 1, 0, -1, "char" },
  { "unused", 27, 1, 0, -1, "char" },
};
static const T4ZField f_155[] = {
  { "model", 0, 4, 1, -1, "XModel*" },
  { "offset", 4, 12, 0, 3, "float" },
};
static const T4ZField f_156[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "numpieces", 4, 4, 0, -1, "int" },
  { "pieces", 8, 4, 1, -1, "XModelPiece*" },
};
static const T4ZField f_157[] = {
  { "v", 0, 8, 0, 4, "int16_t" },
};
static const T4ZField f_158[] = {
  { "pad", 0, 1, 0, -1, "char" },
};
static const T4ZField f_159[] = {
  { "value", 0, 8, 0, 4, "int16_t" },
};
static const T4ZField f_160[] = {
  { "value", 0, 4, 0, 2, "int16_t" },
};
static const T4ZField f_161[] = {
  { "boneOffset", 0, 2, 0, -1, "uint16_t" },
  { "vertCount", 2, 2, 0, -1, "uint16_t" },
  { "triOffset", 4, 2, 0, -1, "uint16_t" },
  { "triCount", 6, 2, 0, -1, "uint16_t" },
  { "collisionTree", 8, 4, 1, -1, "XSurfaceCollisionTree*" },
};
static const T4ZField f_162[] = {
  { "tileMode", 0, 1, 0, -1, "char" },
  { "deformed", 1, 1, 0, -1, "bool" },
  { "vertCount", 2, 2, 0, -1, "uint16_t" },
  { "triCount", 4, 2, 0, -1, "uint16_t" },
  { "zoneHandle", 6, 1, 0, -1, "char" },
  { "baseTriIndex", 8, 2, 0, -1, "uint16_t" },
  { "baseVertIndex", 10, 2, 0, -1, "uint16_t" },
  { "triIndices", 12, 4, 1, -1, "XSurfaceTri16*" },
  { "vertInfo", 16, 12, 0, -1, "XSurfaceVertexInfo" },
  { "verts0", 28, 4, 1, -1, "GfxPackedVertex*" },
  { "vb0", 32, 4, 1, -1, "void *" },
  { "vertListCount", 36, 4, 0, -1, "unsigned int" },
  { "vertList", 40, 4, 1, -1, "XRigidVertList*" },
  { "indexBuffer", 44, 4, 1, -1, "void *" },
  { "partBits", 48, 16, 0, 4, "int" },
};
static const T4ZField f_163[] = {
  { "mins", 0, 6, 0, 3, "uint16_t" },
  { "maxs", 6, 6, 0, 3, "uint16_t" },
};
static const T4ZField f_164[] = {
  { "triangleBeginIndex", 0, 2, 0, -1, "uint16_t" },
};
static const T4ZField f_165[] = {
  { "aabb", 0, 12, 0, -1, "XSurfaceCollisionAabb" },
  { "childBeginIndex", 12, 2, 0, -1, "uint16_t" },
  { "childCount", 14, 2, 0, -1, "uint16_t" },
};
static const T4ZField f_166[] = {
  { "trans", 0, 12, 0, -1, "vec3_t" },
  { "scale", 12, 12, 0, -1, "vec3_t" },
  { "nodeCount", 24, 4, 0, -1, "unsigned int" },
  { "nodes", 28, 4, 1, -1, "XSurfaceCollisionNode*" },
  { "leafCount", 32, 4, 0, -1, "unsigned int" },
  { "leafs", 36, 4, 1, -1, "XSurfaceCollisionLeaf*" },
};
static const T4ZField f_167[] = {
  { "i", 0, 6, 0, 3, "uint16_t" },
};
static const T4ZField f_168[] = {
  { "vertCount", 0, 8, 0, 4, "int16_t" },
  { "vertsBlend", 8, 4, 1, -1, "uint16_t*" },
};
static const T4ZField f_169[] = {
  { "dist", 0, 4, 0, -1, "float" },
  { "range", 4, 4, 0, -1, "float" },
  { "childOffset", 8, 4, 0, 2, "uint16_t" },
};
static const T4ZField f_170[] = {
  { "brushes", 0, 4, 1, -1, "uint16_t*" },
};
static const T4ZField f_171[] = {
  { "axis", 0, 1, 0, -1, "char" },
  { "leafBrushCount", 2, 2, 0, -1, "int16_t" },
  { "contents", 4, 4, 0, -1, "int" },
  { "data", 8, 12, 0, -1, "cLeafBrushNodeData_t" },
};
static const T4ZField f_172[] = {
  { "firstCollAabbIndex", 0, 2, 0, -1, "uint16_t" },
  { "collAabbCount", 2, 2, 0, -1, "uint16_t" },
  { "brushContents", 4, 4, 0, -1, "int" },
  { "terrainContents", 8, 4, 0, -1, "int" },
  { "mins", 12, 12, 0, 3, "float" },
  { "maxs", 24, 12, 0, 3, "float" },
  { "leafBrushNode", 36, 4, 0, -1, "int" },
  { "cluster", 40, 2, 0, -1, "int16_t" },
};
static const T4ZField f_173[] = {
  { "plane", 0, 4, 1, -1, "cplane_s*" },
  { "children", 4, 4, 0, 2, "int16_t" },
};
static const T4ZField f_174[] = {
  { "nextModelInWorldSector", 0, 2, 0, -1, "uint16_t" },
};
static const T4ZField f_175[] = {
  { "writable", 0, 2, 0, -1, "cStaticModelWritable" },
  { "xmodel", 4, 4, 1, -1, "XModel*" },
  { "origin", 8, 12, 0, 3, "float" },
  { "invScaledAxis", 20, 36, 0, 9, "float" },
  { "absmin", 56, 12, 0, 3, "float" },
  { "absmax", 68, 12, 0, 3, "float" },
};
static const T4ZField f_176[] = {
  { "mins", 0, 12, 0, 3, "float" },
  { "contents", 12, 4, 0, -1, "int" },
  { "maxs", 16, 12, 0, 3, "float" },
  { "numsides", 28, 4, 0, -1, "unsigned int" },
  { "sides", 32, 4, 1, -1, "cbrushside_t*" },
  { "axialMaterialNum", 36, 12, 0, 6, "int16_t" },
  { "baseAdjacentSide", 48, 4, 1, -1, "cbrushedge_t*" },
  { "firstAdjacentSideOffsets", 52, 12, 0, 6, "int16_t" },
  { "edgeCount", 64, 6, 0, 6, "char" },
  { "numverts", 72, 4, 0, -1, "unsigned int" },
  { "verts", 76, 4, 1, -1, "vec3_t*" },
};
static const T4ZField f_177[] = {
  { "plane", 0, 4, 1, -1, "cplane_s*" },
  { "materialNum", 4, 4, 0, -1, "unsigned int" },
  { "firstAdjacentSideOffset", 8, 2, 0, -1, "int16_t" },
  { "edgeCount", 10, 1, 0, -1, "char" },
};
static const T4ZField f_178[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "isInUse", 4, 4, 0, -1, "int" },
  { "planeCount", 8, 4, 0, -1, "int" },
  { "planes", 12, 4, 1, -1, "cplane_s*" },
  { "numStaticModels", 16, 4, 0, -1, "unsigned int" },
  { "staticModelList", 20, 4, 1, -1, "cStaticModel_s*" },
  { "numMaterials", 24, 4, 0, -1, "unsigned int" },
  { "materials", 28, 4, 1, -1, "dmaterial_t*" },
  { "numBrushSides", 32, 4, 0, -1, "unsigned int" },
  { "brushsides", 36, 4, 1, -1, "cbrushside_t*" },
  { "numBrushEdges", 40, 4, 0, -1, "unsigned int" },
  { "brushEdges", 44, 4, 1, -1, "cbrushedge_t*" },
  { "numNodes", 48, 4, 0, -1, "unsigned int" },
  { "nodes", 52, 4, 1, -1, "cNode_t*" },
  { "numLeafs", 56, 4, 0, -1, "unsigned int" },
  { "leafs", 60, 4, 1, -1, "cLeaf_t*" },
  { "leafbrushNodesCount", 64, 4, 0, -1, "unsigned int" },
  { "leafbrushNodes", 68, 4, 1, -1, "cLeafBrushNode_s*" },
  { "numLeafBrushes", 72, 4, 0, -1, "unsigned int" },
  { "leafbrushes", 76, 4, 1, -1, "LeafBrush*" },
  { "numLeafSurfaces", 80, 4, 0, -1, "unsigned int" },
  { "leafsurfaces", 84, 4, 1, -1, "unsigned int*" },
  { "vertCount", 88, 4, 0, -1, "unsigned int" },
  { "verts", 92, 4, 1, -1, "vec3_t*" },
  { "numBrushVerts", 96, 4, 0, -1, "unsigned int" },
  { "brushVerts", 100, 4, 1, -1, "vec3_t*" },
  { "nuinds", 104, 4, 0, -1, "unsigned int" },
  { "uinds", 108, 4, 1, -1, "uint16_t*" },
  { "triCount", 112, 4, 0, -1, "int" },
  { "triIndices", 116, 4, 1, -1, "uint16_t*" },
  { "triEdgeIsWalkable", 120, 4, 1, -1, "char*" },
  { "borderCount", 124, 4, 0, -1, "int" },
  { "borders", 128, 4, 1, -1, "CollisionBorder*" },
  { "partitionCount", 132, 4, 0, -1, "int" },
  { "partitions", 136, 4, 1, -1, "CollisionPartition*" },
  { "aabbTreeCount", 140, 4, 0, -1, "int" },
  { "aabbTrees", 144, 4, 1, -1, "CollisionAabbTree*" },
  { "numSubModels", 148, 4, 0, -1, "unsigned int" },
  { "cmodels", 152, 4, 1, -1, "cmodel_t*" },
  { "numBrushes", 156, 2, 0, -1, "uint16_t" },
  { "brushes", 160, 4, 1, -1, "cbrush_t*" },
  { "numClusters", 164, 4, 0, -1, "int" },
  { "clusterBytes", 168, 4, 0, -1, "int" },
  { "visibility", 172, 4, 1, -1, "char*" },
  { "vised", 176, 4, 0, -1, "int" },
  { "mapEnts", 180, 4, 1, -1, "MapEnts*" },
  { "box_brush", 184, 4, 1, -1, "cbrush_t*" },
  { "box_model", 188, 72, 0, -1, "cmodel_t" },
  { "originalDynEntCount", 260, 2, 0, -1, "uint16_t" },
  { "dynEntCount", 262, 8, 0, 4, "uint16_t" },
  { "dynEntDefList", 272, 8, 1, 2, "DynEntityDef*" },
  { "dynEntPoseList", 280, 8, 1, 2, "DynEntityPose*" },
  { "dynEntClientList", 288, 8, 1, 2, "DynEntityClient*" },
  { "dynEntServerList", 296, 8, 1, 2, "DynEntityServer*" },
  { "dynEntCollList", 304, 16, 1, 4, "DynEntityColl*" },
  { "num_constraints", 320, 4, 0, -1, "int" },
  { "constraints", 324, 4, 1, -1, "PhysConstraint*" },
  { "checksum", 328, 4, 0, -1, "unsigned int" },
};
static const T4ZField f_179[] = {
  { "mins", 0, 12, 0, 3, "float" },
  { "maxs", 12, 12, 0, 3, "float" },
  { "radius", 24, 4, 0, -1, "float" },
  { "leaf", 28, 44, 0, -1, "cLeaf_t" },
};
static const T4ZField f_180[] = {
  { "pos", 0, 4, 0, -1, "int" },
  { "width", 4, 4, 0, -1, "int" },
  { "maxChars", 8, 4, 0, -1, "int" },
  { "alignment", 12, 4, 0, -1, "int" },
};
static const T4ZField f_181[] = {
  { "real", 0, 4, 0, -1, "float" },
  { "imag", 4, 4, 0, -1, "float" },
};
static const T4ZField f_182[] = {
  { "normal", 0, 12, 0, 3, "float" },
  { "dist", 12, 4, 0, -1, "float" },
  { "type", 16, 1, 0, -1, "char" },
  { "signbits", 17, 1, 0, -1, "char" },
  { "pad", 18, 2, 0, 2, "char" },
};
static const T4ZField f_183[] = {
  { "material", 0, 64, 0, 64, "char" },
  { "surfaceFlags", 64, 4, 0, -1, "int" },
  { "contentFlags", 68, 4, 0, -1, "int" },
};
static const T4ZField f_184[] = {
  { "minVal", 0, 4, 0, -1, "float" },
  { "maxVal", 4, 4, 0, -1, "float" },
  { "defVal", 8, 4, 0, -1, "float" },
  { "range", 12, 4, 0, -1, "float" },
  { "maxChars", 16, 4, 0, -1, "int" },
  { "maxCharsGotoNext", 20, 4, 0, -1, "int" },
  { "maxPaintChars", 24, 4, 0, -1, "int" },
  { "paintOffset", 28, 4, 0, -1, "int" },
};
static const T4ZField f_185[] = {
  { "type", 0, 4, 0, -1, "int" },
  { "data", 4, 8, 0, -1, "entryInternalData" },
};
static const T4ZField f_186[] = {
  { "window", 0, 156, 0, -1, "windowDef_t" },
  { "textRect", 156, 24, 0, 1, "rectDef_s" },
  { "type", 180, 4, 0, -1, "int" },
  { "dataType", 184, 4, 0, -1, "int" },
  { "alignment", 188, 4, 0, -1, "int" },
  { "fontEnum", 192, 4, 0, -1, "int" },
  { "textAlignMode", 196, 4, 0, -1, "int" },
  { "textalignx", 200, 4, 0, -1, "float" },
  { "textaligny", 204, 4, 0, -1, "float" },
  { "textscale", 208, 4, 0, -1, "float" },
  { "textStyle", 212, 4, 0, -1, "int" },
  { "gameMsgWindowIndex", 216, 4, 0, -1, "int" },
  { "gameMsgWindowMode", 220, 4, 0, -1, "int" },
  { "text", 224, 4, 1, -1, "char*" },
  { "itemFlags", 228, 4, 0, -1, "unsigned int" },
  { "parent", 232, 4, 1, -1, "menuDef_t*" },
  { "mouseEnterText", 236, 4, 1, -1, "char*" },
  { "mouseExitText", 240, 4, 1, -1, "char*" },
  { "mouseEnter", 244, 4, 1, -1, "char*" },
  { "mouseExit", 248, 4, 1, -1, "char*" },
  { "action", 252, 4, 1, -1, "char*" },
  { "onAccept", 256, 4, 1, -1, "char*" },
  { "onFocus", 260, 4, 1, -1, "char*" },
  { "leaveFocus", 264, 4, 1, -1, "char*" },
  { "dvar", 268, 4, 1, -1, "char*" },
  { "dvarTest", 272, 4, 1, -1, "char*" },
  { "onListboxSelectionChange", 276, 4, 1, -1, "char*" },
  { "onKey", 280, 4, 1, -1, "ItemKeyHandler*" },
  { "enableDvar", 284, 4, 1, -1, "char*" },
  { "dvarFlags", 288, 4, 0, -1, "int" },
  { "focusSound", 292, 4, 1, -1, "snd_alias_list_t*" },
  { "special", 296, 4, 0, -1, "float" },
  { "cursorPos", 300, 4, 0, 1, "int" },
  { "typeData", 304, 4, 0, -1, "itemDefData_t" },
  { "imageTrack", 308, 4, 0, -1, "int" },
  { "visibleExp", 312, 8, 0, -1, "statement_s" },
  { "textExp", 320, 8, 0, -1, "statement_s" },
  { "materialExp", 328, 8, 0, -1, "statement_s" },
  { "rectXExp", 336, 8, 0, -1, "statement_s" },
  { "rectYExp", 344, 8, 0, -1, "statement_s" },
  { "rectWExp", 352, 8, 0, -1, "statement_s" },
  { "rectHExp", 360, 8, 0, -1, "statement_s" },
  { "forecolorAExp", 368, 8, 0, -1, "statement_s" },
};
static const T4ZField f_187[] = {
  { "mousePos", 0, 4, 0, -1, "int" },
  { "startPos", 4, 4, 0, 1, "int" },
  { "endPos", 8, 4, 0, 1, "int" },
  { "drawPadding", 12, 4, 0, -1, "int" },
  { "elementWidth", 16, 4, 0, -1, "float" },
  { "elementHeight", 20, 4, 0, -1, "float" },
  { "elementStyle", 24, 4, 0, -1, "int" },
  { "numColumns", 28, 4, 0, -1, "int" },
  { "columnInfo", 32, 256, 0, 16, "columnInfo_s" },
  { "onDoubleClick", 288, 4, 1, -1, "char*" },
  { "notselectable", 292, 4, 0, -1, "int" },
  { "noScrollBars", 296, 4, 0, -1, "int" },
  { "usePaging", 300, 4, 0, -1, "int" },
  { "selectBorder", 304, 16, 0, 4, "float" },
  { "disableColor", 320, 16, 0, 4, "float" },
  { "focusColor", 336, 16, 0, 4, "float" },
  { "selectIcon", 352, 4, 1, -1, "Material*" },
  { "backgroundItemListbox", 356, 4, 1, -1, "Material*" },
  { "highlightTexture", 360, 4, 1, -1, "Material*" },
};
static const T4ZField f_188[] = {
  { "window", 0, 156, 0, -1, "windowDef_t" },
  { "font", 156, 4, 1, -1, "char*" },
  { "fullScreen", 160, 4, 0, -1, "int" },
  { "itemCount", 164, 4, 0, -1, "int" },
  { "fontIndex", 168, 4, 0, -1, "int" },
  { "cursorItem", 172, 4, 0, 1, "int" },
  { "fadeCycle", 176, 4, 0, -1, "int" },
  { "fadeClamp", 180, 4, 0, -1, "float" },
  { "fadeAmount", 184, 4, 0, -1, "float" },
  { "fadeInAmount", 188, 4, 0, -1, "float" },
  { "blurRadius", 192, 4, 0, -1, "float" },
  { "onOpen", 196, 4, 1, -1, "char*" },
  { "onFocus", 200, 4, 1, -1, "char*" },
  { "onClose", 204, 4, 1, -1, "char*" },
  { "onESC", 208, 4, 1, -1, "char*" },
  { "onKey", 212, 4, 1, -1, "ItemKeyHandler*" },
  { "visibleExp", 216, 8, 0, -1, "statement_s" },
  { "allowedBinding", 224, 4, 1, -1, "char*" },
  { "soundName", 228, 4, 1, -1, "char*" },
  { "imageTrack", 232, 4, 0, -1, "int" },
  { "focusColor", 236, 16, 0, 4, "float" },
  { "disableColor", 252, 16, 0, 4, "float" },
  { "rectXExp", 268, 8, 0, -1, "statement_s" },
  { "rectYExp", 276, 8, 0, -1, "statement_s" },
  { "items", 284, 4, 1, -1, "itemDef_s**" },
};
static const T4ZField f_189[] = {
  { "dvarList", 0, 128, 1, 32, "char*" },
  { "dvarStr", 128, 128, 1, 32, "char*" },
  { "dvarValue", 256, 128, 0, 32, "float" },
  { "count", 384, 4, 0, -1, "int" },
  { "strDef", 388, 4, 0, -1, "int" },
};
static const T4ZField f_190[] = {
  { "vOrigin", 0, 12, 0, 3, "float" },
  { "type", 12, 4, 0, -1, "unsigned int" },
};
static const T4ZField f_191[] = {
  { "fDist", 0, 4, 0, -1, "float" },
  { "nodeNum", 4, 2, 0, -1, "uint16_t" },
  { "disconnectCount", 6, 1, 0, -1, "char" },
  { "negotiationLink", 7, 1, 0, -1, "char" },
  { "ubBadPlaceCount", 8, 4, 0, 4, "char" },
};
static const T4ZField f_192[] = {
  { "type", 0, 4, 0, -1, "nodeType" },
  { "spawnflags", 4, 2, 0, -1, "uint16_t" },
  { "targetname", 6, 2, 0, -1, "uint16_t" },
  { "script_linkName", 8, 2, 0, -1, "uint16_t" },
  { "script_noteworthy", 10, 2, 0, -1, "uint16_t" },
  { "target", 12, 2, 0, -1, "uint16_t" },
  { "animscript", 14, 2, 0, -1, "uint16_t" },
  { "animscriptfunc", 16, 4, 0, -1, "int" },
  { "vOrigin", 20, 12, 0, 3, "float" },
  { "fAngle", 32, 4, 0, -1, "float" },
  { "forward", 36, 8, 0, 2, "float" },
  { "fRadius", 44, 4, 0, -1, "float" },
  { "minUseDistSq", 48, 4, 0, -1, "float" },
  { "wOverlapNode", 52, 4, 0, 2, "int16_t" },
  { "wChainId", 56, 2, 0, -1, "int16_t" },
  { "wChainDepth", 58, 2, 0, -1, "int16_t" },
  { "wChainParent", 60, 2, 0, -1, "int16_t" },
  { "totalLinkCount", 62, 2, 0, -1, "uint16_t" },
  { "Links", 64, 4, 1, -1, "pathlink_s*" },
};
static const T4ZField f_193[] = {
  { "pOwner", 0, 4, 1, -1, "void*" },
  { "iFreeTime", 4, 4, 0, -1, "int" },
  { "iValidTime", 8, 12, 0, 3, "int" },
  { "inPlayerLOSTime", 20, 4, 0, -1, "int" },
  { "wLinkCount", 24, 2, 0, -1, "int16_t" },
  { "wOverlapCount", 26, 2, 0, -1, "int16_t" },
  { "turretEntNumber", 28, 2, 0, -1, "int16_t" },
  { "userCount", 30, 2, 0, -1, "int16_t" },
};
static const T4ZField f_194[] = {
  { "constant", 0, 68, 0, -1, "pathnode_constant_t" },
  { "dynamic", 68, 32, 0, -1, "pathnode_dynamic_t" },
  { "transient", 100, 28, 0, -1, "pathnode_transient_t" },
};
static const T4ZField f_195[] = {
  { "iSearchFrame", 0, 4, 0, -1, "int" },
  { "pNextOpen", 4, 4, 1, -1, "pathnode_t*" },
  { "pPrevOpen", 8, 4, 1, -1, "pathnode_t*" },
  { "pParent", 12, 4, 1, -1, "pathnode_t*" },
  { "fCost", 16, 4, 0, -1, "float" },
  { "fHeuristic", 20, 4, 0, -1, "float" },
  { "costFactor", 24, 4, 0, -1, "float" },
};
static const T4ZField f_196[] = {
  { "nodeCount", 0, 4, 0, -1, "int" },
  { "nodes", 4, 4, 1, -1, "uint16_t*" },
};
static const T4ZField f_197[] = {
  { "axis", 0, 4, 0, -1, "int" },
  { "dist", 4, 4, 0, -1, "float" },
  { "u", 8, 8, 0, -1, "pathnode_tree_info_t" },
};
static const T4ZField f_198[] = {
  { "x", 0, 4, 0, -1, "float" },
  { "y", 4, 4, 0, -1, "float" },
  { "w", 8, 4, 0, -1, "float" },
  { "h", 12, 4, 0, -1, "float" },
  { "horzAlign", 16, 4, 0, -1, "int" },
  { "vertAlign", 20, 4, 0, -1, "int" },
};
static const T4ZField f_199[] = {
  { "soundName", 0, 4, 1, -1, "char*" },
};
static const T4ZField f_200[] = {
  { "aliasName", 0, 4, 1, -1, "char*" },
  { "head", 4, 4, 1, -1, "snd_alias_t*" },
  { "count", 8, 4, 0, -1, "int" },
};
static const T4ZField f_201[] = {
  { "aliasName", 0, 4, 1, -1, "char*" },
  { "aliasNameHash", 4, 4, 0, -1, "int" },
  { "subtitle", 8, 4, 1, -1, "char*" },
  { "secondaryAliasName", 12, 4, 1, -1, "char*" },
  { "chainAliasName", 16, 4, 1, -1, "char*" },
  { "soundFile", 20, 4, 1, -1, "SoundFile*" },
  { "sequence", 24, 4, 0, -1, "unsigned int" },
  { "volMin", 28, 4, 0, -1, "float" },
  { "volMax", 32, 4, 0, -1, "float" },
  { "pitchMin", 36, 4, 0, -1, "float" },
  { "pitchMax", 40, 4, 0, -1, "float" },
  { "distMin", 44, 4, 0, -1, "float" },
  { "distMax", 48, 4, 0, -1, "float" },
  { "distReverbMax", 52, 4, 0, -1, "float" },
  { "slavePercentage", 56, 4, 0, -1, "float" },
  { "probability", 60, 4, 0, -1, "float" },
  { "lfePercentage", 64, 4, 0, -1, "float" },
  { "centerPercentage", 68, 4, 0, -1, "float" },
  { "envelopMin", 72, 4, 0, -1, "float" },
  { "envelopMax", 76, 4, 0, -1, "float" },
  { "envelopPercentage", 80, 4, 0, -1, "float" },
  { "minPriority", 84, 4, 0, -1, "float" },
  { "maxPriority", 88, 4, 0, -1, "float" },
  { "minPriorityThreshold", 92, 4, 0, -1, "float" },
  { "maxPriorityThreshold", 96, 4, 0, -1, "float" },
  { "reverbSend", 100, 4, 0, -1, "float" },
  { "gap2", 104, 4, 0, -1, "int" },
  { "gap3", 108, 4, 0, -1, "int" },
  { "occlusionLevel", 112, 4, 0, -1, "float" },
  { "occlusionWetDry", 116, 4, 0, -1, "float" },
  { "moveTime", 120, 4, 0, -1, "unsigned int" },
  { "startDelay", 124, 4, 0, -1, "unsigned int" },
  { "speakerMap", 128, 4, 0, -1, "unsigned int" },
  { "flags", 132, 4, 0, -1, "int" },
  { "volumeFalloffCurve", 136, 4, 0, -1, "unsigned int" },
  { "reverbFalloffCurve", 140, 4, 0, -1, "unsigned int" },
  { "volumeMinFalloffCurve", 144, 4, 0, -1, "unsigned int" },
  { "reverbMinFalloffCurve", 148, 4, 0, -1, "unsigned int" },
  { "limitType", 152, 4, 0, -1, "snd_limit_type_t" },
  { "gap5", 156, 4, 0, -1, "int" },
  { "entityLimitCount", 160, 4, 0, -1, "unsigned int" },
  { "gap6", 164, 4, 0, -1, "int" },
  { "randomizeType", 168, 4, 0, -1, "snd_randomize_type_t" },
  { "gap7", 172, 4, 0, -1, "int" },
  { "gap8", 176, 4, 0, -1, "int" },
  { "gap9", 180, 4, 0, -1, "int" },
};
static const T4ZField f_202[] = {
  { "data", 0, 4, 1, -1, "char*" },
  { "data_size", 4, 4, 0, -1, "int" },
};
static const T4ZField f_203[] = {
  { "name", 0, 32, 0, 32, "char" },
  { "volumeMod", 32, 4, 0, -1, "float" },
  { "maxVoices", 36, 4, 0, -1, "int" },
  { "restricted", 40, 4, 0, -1, "int" },
  { "pausable", 44, 4, 0, -1, "int" },
  { "stopOnDeath", 48, 4, 0, -1, "int" },
  { "isMusic", 52, 4, 0, -1, "int" },
  { "category", 56, 4, 0, -1, "snd_category_t" },
  { "voiceLimitType", 60, 4, 0, -1, "snd_limit_type_t" },
};
static const T4ZField f_204[] = {
  { "name", 0, 32, 0, 32, "char" },
  { "pointCount", 32, 4, 0, -1, "int" },
  { "points", 36, 64, 0, 16, "float" },
};
static const T4ZField f_205[] = {
  { "name", 0, 32, 0, 32, "char" },
  { "masterRingmod", 32, 4, 0, -1, "float" },
  { "reverbRingmod", 36, 4, 0, -1, "float" },
  { "masterLPRatio", 40, 4, 0, -1, "float" },
  { "masterLPDB", 44, 4, 0, -1, "float" },
  { "masterHPRatio", 48, 4, 0, -1, "float" },
  { "masterHPDB", 52, 4, 0, -1, "float" },
};
static const T4ZField f_206[] = {
  { "name", 0, 32, 0, 32, "char" },
  { "room", 32, 4, 0, -1, "float" },
  { "roomHF", 36, 4, 0, -1, "float" },
  { "roomRolloffFactor", 40, 4, 0, -1, "float" },
  { "decayTime", 44, 4, 0, -1, "float" },
  { "decayHFRatio", 48, 4, 0, -1, "float" },
  { "reflections", 52, 4, 0, -1, "float" },
  { "reflectionsDelay", 56, 4, 0, -1, "float" },
  { "reverb", 60, 4, 0, -1, "float" },
  { "reverbDelay", 64, 4, 0, -1, "float" },
  { "diffusion", 68, 4, 0, -1, "float" },
  { "density", 72, 4, 0, -1, "float" },
  { "HFReference", 76, 4, 0, -1, "float" },
};
static const T4ZField f_207[] = {
  { "input_channel_count", 0, 4, 0, -1, "int" },
  { "output_channel_count", 4, 4, 0, -1, "int" },
  { "volumes", 8, 64, 0, 16, "float" },
};
static const T4ZField f_208[] = {
  { "name", 0, 32, 0, 32, "char" },
  { "channelMaps", 32, 288, 0, 4, "snd_speaker_map" },
};
static const T4ZField f_209[] = {
  { "vertexLayerData", 0, 4, 0, -1, "int" },
  { "firstVertex", 4, 4, 0, -1, "int" },
  { "vertexCount", 8, 2, 0, -1, "uint16_t" },
  { "triCount", 10, 2, 0, -1, "uint16_t" },
  { "baseIndex", 12, 4, 0, -1, "int" },
};
static const T4ZField f_210[] = {
  { "numEntries", 0, 4, 0, -1, "int" },
  { "entries", 4, 4, 1, -1, "expressionEntry**" },
};
static const T4ZField f_211[] = {
  { "hasValidData", 0, 1, 0, -1, "bool" },
  { "spriteMaterial", 4, 4, 1, -1, "Material*" },
  { "flareMaterial", 8, 4, 1, -1, "Material*" },
  { "spriteSize", 12, 4, 0, -1, "float" },
  { "flareMinSize", 16, 4, 0, -1, "float" },
  { "flareMinDot", 20, 4, 0, -1, "float" },
  { "flareMaxSize", 24, 4, 0, -1, "float" },
  { "flareMaxDot", 28, 4, 0, -1, "float" },
  { "flareMaxAlpha", 32, 4, 0, -1, "float" },
  { "flareFadeInTime", 36, 4, 0, -1, "int" },
  { "flareFadeOutTime", 40, 4, 0, -1, "int" },
  { "blindMinDot", 44, 4, 0, -1, "float" },
  { "blindMaxDot", 48, 4, 0, -1, "float" },
  { "blindMaxDarken", 52, 4, 0, -1, "float" },
  { "blindFadeInTime", 56, 4, 0, -1, "int" },
  { "blindFadeOutTime", 60, 4, 0, -1, "int" },
  { "glareMinDot", 64, 4, 0, -1, "float" },
  { "glareMaxDot", 68, 4, 0, -1, "float" },
  { "glareMaxLighten", 72, 4, 0, -1, "float" },
  { "glareFadeInTime", 76, 4, 0, -1, "int" },
  { "glareFadeOutTime", 80, 4, 0, -1, "int" },
  { "sunFxPosition", 84, 12, 0, 3, "float" },
};
static const T4ZField f_212[] = {
  { "firstChildIndex", 0, 0, 0, -1, "int" },
  { "partitionIndex", 0, 0, 0, -1, "int" },
};
static const T4ZField f_213[] = {
  { "handle", 0, 0, 1, -1, "FxEffectDef*" },
  { "name", 0, 0, 1, -1, "char*" },
};
static const T4ZField f_214[] = {
  { "markArray", 0, 0, 1, -1, "FxElemMarkVisuals*" },
  { "array", 0, 0, 1, -1, "FxElemVisuals*" },
  { "instance", 0, 0, 0, -1, "FxElemVisuals" },
};
static const T4ZField f_215[] = {
  { "anonymous", 0, 0, 1, -1, "void*" },
  { "material", 0, 0, 1, -1, "Material*" },
  { "model", 0, 0, 1, -1, "XModel*" },
  { "effectDef", 0, 0, 0, -1, "FxEffectDefRef" },
  { "soundName", 0, 0, 1, -1, "char*" },
};
static const T4ZField f_216[] = {
  { "looping", 0, 0, 0, -1, "FxSpawnDefLooping" },
  { "oneShot", 0, 0, 0, -1, "FxSpawnDefOneShot" },
};
static const T4ZField f_217[] = {
  { "packed", 0, 0, 0, -1, "unsigned int" },
  { "array", 0, 0, 0, -1, "unsigned char" },
};
static const T4ZField f_218[] = {
  { "fields", 0, 0, 0, -1, "GfxDrawSurfFields" },
  { "packed", 0, 0, 0, -1, "uint64_t" },
};
static const T4ZField f_219[] = {
  { "raw", 0, 0, 0, -1, "unsigned int" },
  { "structured", 0, 0, 0, -1, "GfxStateBitsLoadBitsStructured" },
};
static const T4ZField f_220[] = {
  { "loadDef", 0, 0, 1, -1, "GfxImageLoadDef*" },
};
static const T4ZField f_221[] = {
  { "literalConst", 0, 0, 1, -1, "float*" },
  { "codeConst", 0, 0, 0, -1, "MaterialArgumentCodeConst" },
  { "codeSampler", 0, 0, 0, -1, "unsigned int" },
  { "nameHash", 0, 0, 0, -1, "unsigned int" },
};
static const T4ZField f_222[] = {
  { "image", 0, 0, 1, -1, "GfxImage*" },
  { "water", 0, 0, 1, -1, "water_t*" },
};
static const T4ZField f_223[] = {
  { "packed", 0, 0, 0, -1, "unsigned int" },
};
static const T4ZField f_224[] = {
  { "packed", 0, 0, 0, -1, "unsigned int" },
  { "array", 0, 0, 0, -1, "unsigned char" },
};
static const T4ZField f_225[] = {
  { "name", 0, 0, 1, -1, "snd_alias_list_name*" },
  { "sound", 0, 0, 1, -1, "snd_alias_list_t*" },
};
static const T4ZField f_226[] = {
  { "loadSnd", 0, 0, 1, -1, "LoadedSound*" },
  { "streamSnd", 0, 0, 0, -1, "StreamedSound" },
};
static const T4ZField f_227[] = {
  { "frames", 0, 0, 0, -1, "XAnimDeltaPartQuatDataFrames" },
  { "frame0", 0, 0, 0, -1, "XQuat2" },
};
static const T4ZField f_228[] = {
  { "_1", 0, 0, 1, -1, "ByteVec*" },
  { "_2", 0, 0, 1, -1, "UShortVec*" },
};
static const T4ZField f_229[] = {
  { "_1", 0, 0, 0, -1, "unsigned char" },
  { "_2", 0, 0, 0, -1, "uint16_t" },
};
static const T4ZField f_230[] = {
  { "_1", 0, 0, 0, -1, "unsigned char" },
  { "_2", 0, 0, 0, -1, "uint16_t" },
};
static const T4ZField f_231[] = {
  { "_1", 0, 0, 1, -1, "unsigned char*" },
  { "_2", 0, 0, 1, -1, "uint16_t*" },
  { "data", 0, 0, 1, -1, "void*" },
};
static const T4ZField f_232[] = {
  { "frames", 0, 0, 0, -1, "XAnimPartTransFrames" },
  { "frame0", 0, 0, 0, -1, "vec3_t" },
};
static const T4ZField f_233[] = {
  { "physPreset", 0, 0, 1, -1, "PhysPreset*" },
  { "physConstraints", 0, 0, 1, -1, "PhysConstraints*" },
  { "destructibleDef", 0, 0, 1, -1, "DestructibleDef*" },
  { "parts", 0, 0, 1, -1, "XAnimParts*" },
  { "model", 0, 0, 1, -1, "XModel*" },
  { "material", 0, 0, 1, -1, "Material*" },
  { "pixelShader", 0, 0, 1, -1, "MaterialPixelShader*" },
  { "vertexShader", 0, 0, 1, -1, "MaterialVertexShader*" },
  { "techniqueSet", 0, 0, 1, -1, "MaterialTechniqueSet*" },
  { "image", 0, 0, 1, -1, "GfxImage*" },
  { "sound", 0, 0, 1, -1, "snd_alias_list_t*" },
  { "loadSnd", 0, 0, 1, -1, "LoadedSound*" },
  { "clipMap", 0, 0, 1, -1, "clipMap_t*" },
  { "comWorld", 0, 0, 1, -1, "ComWorld*" },
  { "gameWorldSp", 0, 0, 1, -1, "GameWorldSp*" },
  { "gameWorldMp", 0, 0, 1, -1, "GameWorldMp*" },
  { "mapEnts", 0, 0, 1, -1, "MapEnts*" },
  { "gfxWorld", 0, 0, 1, -1, "GfxWorld*" },
  { "lightDef", 0, 0, 1, -1, "GfxLightDef*" },
  { "font", 0, 0, 1, -1, "Font_s*" },
  { "menuList", 0, 0, 1, -1, "MenuList*" },
  { "menu", 0, 0, 1, -1, "menuDef_t*" },
  { "localize", 0, 0, 1, -1, "LocalizeEntry*" },
  { "weapon", 0, 0, 1, -1, "WeaponDef*" },
  { "sndDriverGlobals", 0, 0, 1, -1, "SndDriverGlobals*" },
  { "fx", 0, 0, 1, -1, "FxEffectDef*" },
  { "impactFx", 0, 0, 1, -1, "FxImpactTable*" },
  { "rawfile", 0, 0, 1, -1, "RawFile*" },
  { "stringTable", 0, 0, 1, -1, "StringTable*" },
  { "packIndex", 0, 0, 1, -1, "PackIndex*" },
  { "data", 0, 0, 1, -1, "void*" },
};
static const T4ZField f_234[] = {
  { "leaf", 0, 0, 0, -1, "cLeafBrushNodeLeaf_t" },
  { "children", 0, 0, 0, -1, "cLeafBrushNodeChildren_t" },
};
static const T4ZField f_235[] = {
  { "operand", 0, 0, 0, -1, "Operand" },
  { "op", 0, 0, 0, -1, "operationEnum" },
};
static const T4ZField f_236[] = {
  { "listBox", 0, 0, 1, -1, "listBoxDef_s*" },
  { "editField", 0, 0, 1, -1, "editFieldDef_s*" },
  { "multi", 0, 0, 1, -1, "multiDef_s*" },
  { "enumDvarName", 0, 0, 1, -1, "char*" },
  { "data", 0, 0, 1, -1, "void*" },
};
static const T4ZField f_237[] = {
  { "intVal", 0, 0, 0, -1, "int" },
  { "floatVal", 0, 0, 0, -1, "float" },
  { "stringVal", 0, 0, 1, -1, "char*" },
};
static const T4ZField f_238[] = {
  { "child", 0, 0, 1, -1, "pathnode_tree_t*" },
  { "s", 0, 0, 0, -1, "pathnode_tree_nodes_t" },
};
static const T4ZField f_239[] = {
  { "v", 0, 0, 0, -1, "float" },
};
static const T4ZField f_240[] = {
  { "v", 0, 0, 0, -1, "float" },
};
static const T4ZField f_241[] = {
  { "v", 0, 0, 0, -1, "float" },
};
static const T4ZField f_242[] = {
  { "writable", 0, 4, 0, -1, "WaterWritable" },
  { "H0", 4, 4, 1, -1, "complex_s*" },
  { "wTerm", 8, 4, 1, -1, "float*" },
  { "M", 12, 4, 0, -1, "int" },
  { "N", 16, 4, 0, -1, "int" },
  { "Lx", 20, 4, 0, -1, "float" },
  { "Lz", 24, 4, 0, -1, "float" },
  { "gravity", 28, 4, 0, -1, "float" },
  { "windvel", 32, 4, 0, -1, "float" },
  { "winddir", 36, 8, 0, 2, "float" },
  { "amplitude", 44, 4, 0, -1, "float" },
  { "codeConstant", 48, 16, 0, 4, "float" },
  { "image", 64, 4, 1, -1, "GfxImage*" },
};
static const T4ZField f_243[] = {
  { "name", 0, 4, 1, -1, "char*" },
  { "rect", 4, 24, 0, -1, "rectDef_s" },
  { "rectClient", 28, 24, 0, -1, "rectDef_s" },
  { "group", 52, 4, 1, -1, "char*" },
  { "style", 56, 4, 0, -1, "int" },
  { "border", 60, 4, 0, -1, "int" },
  { "ownerDraw", 64, 4, 0, -1, "int" },
  { "ownerDrawFlags", 68, 4, 0, -1, "int" },
  { "borderSize", 72, 4, 0, -1, "float" },
  { "staticFlags", 76, 4, 0, -1, "unsigned int" },
  { "dynamicFlags", 80, 4, 0, 1, "unsigned int" },
  { "nextTime", 84, 4, 0, -1, "int" },
  { "foreColor", 88, 16, 0, 4, "float" },
  { "backColor", 104, 16, 0, 4, "float" },
  { "borderColor", 120, 16, 0, 4, "float" },
  { "outlineColor", 136, 16, 0, 4, "float" },
  { "background", 152, 4, 1, -1, "Material*" },
};
static const T4ZStruct T4Z_STRUCTS[] = {
  { "BModelDrawInfo", 2, 1, f_0, 0 },
  { "BrushWrapper", 96, 13, f_1, 0 },
  { "CardMemory", 8, 1, f_2, 0 },
  { "CollisionAabbTree", 32, 5, f_3, 0 },
  { "CollisionBorder", 28, 5, f_4, 0 },
  { "CollisionPartition", 20, 6, f_5, 0 },
  { "ComBurnableCell", 12, 3, f_6, 0 },
  { "ComBurnableHeader", 16, 4, f_7, 0 },
  { "ComBurnableSample", 1, 1, f_8, 0 },
  { "ComPrimaryLight", 72, 15, f_9, 0 },
  { "ComWaterCell", 8, 4, f_10, 0 },
  { "ComWaterHeader", 16, 4, f_11, 0 },
  { "ComWorld", 64, 10, f_12, 0 },
  { "DObjAnimMat", 32, 3, f_13, 0 },
  { "DObjSkelMat", 64, 2, f_14, 0 },
  { "DestructibleDef", 20, 5, f_15, 0 },
  { "DestructibleModel", 40, 8, f_16, 0 },
  { "DestructiblePiece", 248, 14, f_17, 0 },
  { "DpvsPlane", 20, 3, f_18, 0 },
  { "DynEntityClient", 16, 6, f_19, 0 },
  { "DynEntityColl", 32, 5, f_20, 0 },
  { "DynEntityDef", 84, 14, f_21, 0 },
  { "DynEntityPose", 32, 2, f_22, 0 },
  { "DynEntityServer", 8, 2, f_23, 0 },
  { "FlameTable", 476, 119, f_24, 0 },
  { "Font_s", 24, 6, f_25, 0 },
  { "FxEffectDef", 36, 9, f_26, 0 },
  { "FxElemAtlas", 8, 7, f_27, 0 },
  { "FxElemDef", 256, 37, f_28, 0 },
  { "FxElemMarkVisuals", 8, 1, f_29, 0 },
  { "FxElemVec3Range", 24, 2, f_30, 0 },
  { "FxElemVelStateInFrame", 48, 2, f_31, 0 },
  { "FxElemVelStateSample", 96, 2, f_32, 0 },
  { "FxElemVisStateSample", 48, 2, f_33, 0 },
  { "FxElemVisualState", 24, 5, f_34, 0 },
  { "FxFloatRange", 8, 2, f_35, 0 },
  { "FxImpactEntry", 140, 2, f_36, 0 },
  { "FxImpactTable", 8, 2, f_37, 0 },
  { "FxIntRange", 8, 2, f_38, 0 },
  { "FxSpawnDefLooping", 8, 2, f_39, 0 },
  { "FxSpawnDefOneShot", 8, 1, f_40, 0 },
  { "FxTrailDef", 28, 7, f_41, 0 },
  { "FxTrailVertex", 20, 3, f_42, 0 },
  { "GameWorldMp", 44, 2, f_43, 0 },
  { "GameWorldSp", 44, 2, f_44, 0 },
  { "GfxAabbTree", 40, 8, f_45, 0 },
  { "GfxBrushModel", 56, 4, f_46, 0 },
  { "GfxBrushModelWritable", 24, 2, f_47, 0 },
  { "GfxCell", 56, 10, f_48, 0 },
  { "GfxCullGroup", 32, 4, f_49, 0 },
  { "GfxDrawSurfFields", 8, 9, f_50, 0 },
  { "GfxImage", 36, 13, f_51, 0 },
  { "GfxImageLoadDef", 16, 6, f_52, 0 },
  { "GfxLight", 64, 12, f_53, 0 },
  { "GfxLightCorona", 32, 4, f_54, 0 },
  { "GfxLightDef", 16, 3, f_55, 0 },
  { "GfxLightGrid", 56, 13, f_56, 0 },
  { "GfxLightGridColors", 168, 1, f_57, 0 },
  { "GfxLightGridEntry", 4, 3, f_58, 0 },
  { "GfxLightImage", 8, 2, f_59, 0 },
  { "GfxLightRegion", 8, 2, f_60, 0 },
  { "GfxLightRegionAxis", 20, 3, f_61, 0 },
  { "GfxLightRegionHull", 80, 4, f_62, 0 },
  { "GfxLightmapArray", 8, 2, f_63, 0 },
  { "GfxPackedPlacement", 52, 3, f_64, 0 },
  { "GfxPackedVertex", 32, 6, f_65, 0 },
  { "GfxPixelShaderLoadDef", 8, 3, f_66, 0 },
  { "GfxPlacement", 28, 2, f_67, 0 },
  { "GfxPortal", 68, 6, f_68, 0 },
  { "GfxPortalWritable", 12, 6, f_69, 0 },
  { "GfxReflectionProbe", 16, 2, f_70, 0 },
  { "GfxSceneDynBrush", 4, 2, f_71, 0 },
  { "GfxSceneDynModel", 6, 2, f_72, 0 },
  { "GfxShadowGeometry", 12, 4, f_73, 0 },
  { "GfxStateBits", 8, 1, f_74, 0 },
  { "GfxStateBitsLoadBitsStructured", 8, 27, f_75, 0 },
  { "GfxStaticModelDrawInst", 92, 11, f_76, 0 },
  { "GfxStaticModelInst", 28, 3, f_77, 0 },
  { "GfxSurface", 52, 8, f_78, 0 },
  { "GfxVertexShaderLoadDef", 8, 3, f_79, 0 },
  { "GfxWaterBuffer", 8, 2, f_80, 0 },
  { "GfxWorld", 800, 66, f_81, 0 },
  { "GfxWorldDpvsDynamic", 48, 4, f_82, 0 },
  { "GfxWorldDpvsPlanes", 16, 4, f_83, 0 },
  { "GfxWorldDpvsStatic", 100, 21, f_84, 0 },
  { "GfxWorldLodChain", 24, 4, f_85, 0 },
  { "GfxWorldLodInfo", 12, 3, f_86, 0 },
  { "GfxWorldVertex", 44, 7, f_87, 0 },
  { "GfxWorldVertexData", 8, 2, f_88, 0 },
  { "GfxWorldVertexLayerData", 8, 2, f_89, 0 },
  { "Glyph", 24, 10, f_90, 0 },
  { "ItemKeyHandler", 12, 3, f_91, 0 },
  { "LoadedSound", 12, 2, f_92, 0 },
  { "LocalizeEntry", 8, 2, f_93, 0 },
  { "MSSChannelMap", 100, 2, f_94, 0 },
  { "MSSSpeakerLevels", 16, 3, f_95, 0 },
  { "MapEnts", 12, 3, f_96, 0 },
  { "Material", 112, 11, f_97, 0 },
  { "MaterialArgumentCodeConst", 4, 3, f_98, 0 },
  { "MaterialConstantDef", 32, 3, f_99, 0 },
  { "MaterialInfo", 32, 9, f_100, 0 },
  { "MaterialMemory", 8, 2, f_101, 0 },
  { "MaterialPass", 20, 8, f_102, 0 },
  { "MaterialPixelShader", 16, 2, f_103, 0 },
  { "MaterialPixelShaderProgram", 12, 2, f_104, 0 },
  { "MaterialShaderArgument", 8, 3, f_105, 0 },
  { "MaterialStreamRouting", 2, 2, f_106, 0 },
  { "MaterialTechnique", 28, 4, f_107, 0 },
  { "MaterialTechniqueSet", 248, 6, f_108, 0 },
  { "MaterialTextureDef", 16, 7, f_109, 0 },
  { "MaterialTextureDefSamplerState", 1, 5, f_110, 0 },
  { "MaterialTypeInfo", 8, 2, f_111, 0 },
  { "MaterialVertexDeclaration", 104, 4, f_112, 0 },
  { "MaterialVertexShader", 16, 2, f_113, 0 },
  { "MaterialVertexShaderProgram", 12, 2, f_114, 0 },
  { "MaterialVertexStreamRouting", 100, 2, f_115, 0 },
  { "MenuList", 12, 3, f_116, 0 },
  { "ModelLodFade", 4, 1, f_117, 0 },
  { "Operand", 8, 2, f_118, 0 },
  { "PackIndex", 28, 3, f_119, 0 },
  { "PackIndexEntry", 12, 3, f_120, 0 },
  { "PackIndexHeader", 20, 5, f_121, 0 },
  { "PathData", 40, 10, f_122, 0 },
  { "PhysConstraint", 152, 28, f_123, 0 },
  { "PhysConstraints", 2440, 3, f_124, 0 },
  { "PhysGeomInfo", 68, 5, f_125, 0 },
  { "PhysGeomList", 20, 3, f_126, 0 },
  { "PhysMass", 12, 1, f_127, 0 },
  { "PhysPreset", 48, 12, f_128, 0 },
  { "PhysPresetInfo", 44, 11, f_129, 0 },
  { "Picmip", 2, 1, f_130, 0 },
  { "PrimedSound", 12, 3, f_131, 0 },
  { "RawFile", 12, 3, f_132, 0 },
  { "SndDriverGlobals", 23556, 6, f_133, 0 },
  { "SoundFile", 20, 3, f_134, 0 },
  { "SpeakerMap", 408, 3, f_135, 0 },
  { "StreamFileName", 12, 3, f_136, 0 },
  { "StreamedSound", 16, 2, f_137, 0 },
  { "StringTable", 16, 4, f_138, 0 },
  { "SunLightParseParams", 136, 11, f_139, 0 },
  { "WaterWritable", 4, 1, f_140, 0 },
  { "WeaponDef", 2476, 425, f_141, 0 },
  { "XAnimDeltaPart", 8, 2, f_142, 0 },
  { "XAnimDeltaPartQuat", 12, 2, f_143, 0 },
  { "XAnimDeltaPartQuatDataFrames", 8, 2, f_144, 0 },
  { "XAnimNotifyInfo", 8, 2, f_145, 0 },
  { "XAnimPartTrans", 36, 3, f_146, 0 },
  { "XAnimPartTransFrames", 32, 4, f_147, 0 },
  { "XAnimParts", 88, 27, f_148, 0 },
  { "XBoneInfo", 40, 3, f_149, 0 },
  { "XModel", 228, 31, f_150, 0 },
  { "XModelCollSurf_s", 44, 7, f_151, 0 },
  { "XModelCollTri_s", 48, 3, f_152, 0 },
  { "XModelDrawInfo", 4, 2, f_153, 0 },
  { "XModelLodInfo", 28, 8, f_154, 0 },
  { "XModelPiece", 16, 2, f_155, 0 },
  { "XModelPieces", 12, 3, f_156, 0 },
  { "XModelQuat", 8, 1, f_157, 0 },
  { "XModelStreamInfo", 1, 1, f_158, 0 },
  { "XQuat", 8, 1, f_159, 0 },
  { "XQuat2", 4, 1, f_160, 0 },
  { "XRigidVertList", 12, 5, f_161, 0 },
  { "XSurface", 64, 15, f_162, 0 },
  { "XSurfaceCollisionAabb", 12, 2, f_163, 0 },
  { "XSurfaceCollisionLeaf", 2, 1, f_164, 0 },
  { "XSurfaceCollisionNode", 16, 3, f_165, 0 },
  { "XSurfaceCollisionTree", 40, 6, f_166, 0 },
  { "XSurfaceTri", 6, 1, f_167, 0 },
  { "XSurfaceVertexInfo", 12, 2, f_168, 0 },
  { "cLeafBrushNodeChildren_t", 12, 3, f_169, 0 },
  { "cLeafBrushNodeLeaf_t", 4, 1, f_170, 0 },
  { "cLeafBrushNode_s", 20, 4, f_171, 0 },
  { "cLeaf_t", 44, 8, f_172, 0 },
  { "cNode_t", 8, 2, f_173, 0 },
  { "cStaticModelWritable", 2, 1, f_174, 0 },
  { "cStaticModel_s", 80, 6, f_175, 0 },
  { "cbrush_t", 80, 11, f_176, 0 },
  { "cbrushside_t", 12, 4, f_177, 0 },
  { "clipMap_t", 332, 58, f_178, 0 },
  { "cmodel_t", 72, 4, f_179, 0 },
  { "columnInfo_s", 16, 4, f_180, 0 },
  { "complex_s", 8, 2, f_181, 0 },
  { "cplane_s", 20, 5, f_182, 0 },
  { "dmaterial_t", 72, 3, f_183, 0 },
  { "editFieldDef_s", 32, 8, f_184, 0 },
  { "expressionEntry", 12, 2, f_185, 0 },
  { "itemDef_s", 376, 43, f_186, 0 },
  { "listBoxDef_s", 364, 19, f_187, 0 },
  { "menuDef_t", 288, 25, f_188, 0 },
  { "multiDef_s", 392, 5, f_189, 0 },
  { "pathbasenode_t", 16, 2, f_190, 0 },
  { "pathlink_s", 12, 5, f_191, 0 },
  { "pathnode_constant_t", 68, 19, f_192, 0 },
  { "pathnode_dynamic_t", 32, 8, f_193, 0 },
  { "pathnode_t", 128, 3, f_194, 0 },
  { "pathnode_transient_t", 28, 7, f_195, 0 },
  { "pathnode_tree_nodes_t", 8, 2, f_196, 0 },
  { "pathnode_tree_t", 16, 3, f_197, 0 },
  { "rectDef_s", 24, 6, f_198, 0 },
  { "snd_alias_list_name", 4, 1, f_199, 0 },
  { "snd_alias_list_t", 12, 3, f_200, 0 },
  { "snd_alias_t", 184, 46, f_201, 0 },
  { "snd_asset", 8, 2, f_202, 0 },
  { "snd_bus_info_t", 64, 9, f_203, 0 },
  { "snd_curve_t", 100, 3, f_204, 0 },
  { "snd_master_effect_t", 56, 7, f_205, 0 },
  { "snd_reverb_patch_t", 80, 13, f_206, 0 },
  { "snd_speaker_map", 72, 3, f_207, 0 },
  { "snd_speaker_map_set", 320, 2, f_208, 0 },
  { "srfTriangles_t", 16, 5, f_209, 0 },
  { "statement_s", 8, 2, f_210, 0 },
  { "sunflare_t", 96, 22, f_211, 0 },
  { "union:CollisionAabbTreeIndex", 4, 2, f_212, 1 },
  { "union:FxEffectDefRef", 4, 2, f_213, 1 },
  { "union:FxElemDefVisuals", 4, 3, f_214, 1 },
  { "union:FxElemVisuals", 4, 5, f_215, 1 },
  { "union:FxSpawnDef", 8, 2, f_216, 1 },
  { "union:GfxColor", 4, 2, f_217, 1 },
  { "union:GfxDrawSurf", 8, 2, f_218, 1 },
  { "union:GfxStateBitsLoadBits", 8, 2, f_219, 1 },
  { "union:GfxTexture", 4, 1, f_220, 1 },
  { "union:MaterialArgumentDef", 4, 4, f_221, 1 },
  { "union:MaterialTextureDefInfo", 4, 2, f_222, 1 },
  { "union:PackedTexCoords", 4, 1, f_223, 1 },
  { "union:PackedUnitVec", 4, 2, f_224, 1 },
  { "union:SndAliasCustom", 4, 2, f_225, 1 },
  { "union:SoundFileRef", 16, 2, f_226, 1 },
  { "union:XAnimDeltaPartQuatData", 8, 2, f_227, 1 },
  { "union:XAnimDynamicFrames", 4, 2, f_228, 1 },
  { "union:XAnimDynamicIndicesQuat", 2, 2, f_229, 1 },
  { "union:XAnimDynamicIndicesTrans", 2, 2, f_230, 1 },
  { "union:XAnimIndices", 4, 3, f_231, 1 },
  { "union:XAnimPartTransData", 32, 2, f_232, 1 },
  { "union:XAssetHeader", 4, 31, f_233, 1 },
  { "union:cLeafBrushNodeData_t", 12, 2, f_234, 1 },
  { "union:entryInternalData", 8, 2, f_235, 1 },
  { "union:itemDefData_t", 4, 5, f_236, 1 },
  { "union:operandInternalDataUnion", 4, 3, f_237, 1 },
  { "union:pathnode_tree_info_t", 8, 2, f_238, 1 },
  { "union:vec2_t", 8, 1, f_239, 1 },
  { "union:vec3_t", 12, 1, f_240, 1 },
  { "union:vec4_t", 16, 1, f_241, 1 },
  { "water_t", 68, 13, f_242, 0 },
  { "windowDef_t", 156, 17, f_243, 0 },
};
#define T4Z_STRUCT_N (sizeof(T4Z_STRUCTS)/sizeof(T4Z_STRUCTS[0]))

/* ---- pointees RUNTIME (sem bytes no stream) ---- */
static const T4ZPairEnt T4Z_RUNTIME[] = {
  { "GfxWorld", "cellCasterBits" },
  { "GfxWorld", "lightmapPrimaryTextures" },
  { "GfxWorld", "lightmapSecondaryTextures" },
  { "GfxWorld", "nonSunPrimaryLightForModelDynEnt" },
  { "GfxWorld", "primaryLightDynEntShadowVis" },
  { "GfxWorld", "primaryLightEntityShadowVis" },
  { "GfxWorld", "reflectionProbeTextures" },
  { "GfxWorld", "sceneDynBrush" },
  { "GfxWorld", "sceneDynModel" },
  { "GfxWorldDpvsDynamic", "dynEntCellBits" },
  { "GfxWorldDpvsDynamic", "dynEntVisData" },
  { "GfxWorldDpvsPlanes", "sceneEntCellBits" },
  { "GfxWorldDpvsStatic", "lodData" },
  { "GfxWorldDpvsStatic", "smodelVisData" },
  { "GfxWorldDpvsStatic", "surfaceCastsSunShadow" },
  { "GfxWorldDpvsStatic", "surfaceMaterials" },
  { "GfxWorldDpvsStatic", "surfaceVisData" },
  { "PathData", "basenodes" },
  { "clipMap_t", "dynEntClientList" },
  { "clipMap_t", "dynEntCollList" },
  { "clipMap_t", "dynEntPoseList" },
  { "clipMap_t", "dynEntServerList" },
};
#define T4Z_RUNTIME_N (sizeof(T4Z_RUNTIME)/sizeof(T4Z_RUNTIME[0]))

/* ---- reorders de pontoe (OAT SequenceReorder) ---- */
static const T4ZOrderEnt T4Z_REORD[] = {
  { "GfxWorld", 1, 3, (const char *const[]){ "materialMemory", "vd", "vld" } },
  { "WeaponDef", 1, 6, (const char *const[]){ "aiVsAiAccuracyGraphName", "aiVsAiAccuracyGraphKnots", "originalAiVsAiAccuracyGraphKnots", "aiVsPlayerAccuracyGraphName", "aiVsPlayerAccuracyGraphKnots", "originalAiVsPlayerAccuracyGraphKnots" } },
  { "clipMap_t", 1, 4, (const char *const[]){ "leafs", "leafbrushes", "leafbrushNodes", "leafsurfaces" } },
};
#define T4Z_REORD_N (sizeof(T4Z_REORD)/sizeof(T4Z_REORD[0]))

static const T4ZOrderEnt T4Z_ORDER[] = {
  { "GfxImage", 0, 2, (const char *const[]){ "name", "texture" } },
  { "MaterialTechnique", 0, 2, (const char *const[]){ "passArray", "name" } },
  { "WeaponDef", 0, 425, (const char *const[]){ "szInternalName", "szDisplayName", "szOverlayName", "gunXModel", "handXModel", "szXAnims", "szModeName", "hideTags", "notetrackSoundMapKeys", "notetrackSoundMapValues", "playerAnimType", "weapType", "weapClass", "penetrateType", "impactType", "inventoryType", "fireType", "clipType", "overheatWeapon", "overheatRate", "cooldownRate", "overheatEndVal", "coolWhileFiring", "offhandClass", "stance", "viewFlashEffect", "worldFlashEffect", "pickupSound", "pickupSoundPlayer", "ammoPickupSound", "ammoPickupSoundPlayer", "projectileSound", "pullbackSound", "pullbackSoundPlayer", "fireSound", "fireSoundPlayer", "fireLoopSound", "fireLoopSoundPlayer", "fireStopSound", "fireStopSoundPlayer", "fireLastSound", "fireLastSoundPlayer", "emptyFireSound", "emptyFireSoundPlayer", "crackSound", "whizbySound", "meleeSwipeSound", "meleeSwipeSoundPlayer", "meleeHitSound", "meleeMissSound", "rechamberSound", "rechamberSoundPlayer", "reloadSound", "reloadSoundPlayer", "reloadEmptySound", "reloadEmptySoundPlayer", "reloadStartSound", "reloadStartSoundPlayer", "reloadEndSound", "reloadEndSoundPlayer", "rotateLoopSound", "rotateLoopSoundPlayer", "deploySound", "deploySoundPlayer", "finishDeploySound", "finishDeploySoundPlayer", "breakdownSound", "breakdownSoundPlayer", "finishBreakdownSound", "finishBreakdownSoundPlayer", "detonateSound", "detonateSoundPlayer", "nightVisionWearSound", "nightVisionWearSoundPlayer", "nightVisionRemoveSound", "nightVisionRemoveSoundPlayer", "altSwitchSound", "altSwitchSoundPlayer", "raiseSound", "raiseSoundPlayer", "firstRaiseSound", "firstRaiseSoundPlayer", "putawaySound", "putawaySoundPlayer", "overheatSound", "overheatSoundPlayer", "bounceSound", "standMountedWeapdef", "crouchMountedWeapdef", "proneMountedWeapdef", "standMountedIndex", "crouchMountedIndex", "proneMountedIndex", "viewShellEjectEffect", "worldShellEjectEffect", "viewLastShotEjectEffect", "worldLastShotEjectEffect", "reticleCenter", "reticleSide", "iReticleCenterSize", "iReticleSideSize", "iReticleMinOfs", "activeReticleType", "vStandMove", "vStandRot", "vDuckedOfs", "vDuckedMove", "duckedSprintOfs", "duckedSprintRot", "duckedSprintBob", "duckedSprintScale", "sprintOfs", "sprintRot", "sprintBob", "sprintScale", "vDuckedRot", "vProneOfs", "vProneMove", "vProneRot", "fPosMoveRate", "fPosProneMoveRate", "fStandMoveMinSpeed", "fDuckedMoveMinSpeed", "fProneMoveMinSpeed", "fPosRotRate", "fPosProneRotRate", "fStandRotMinSpeed", "fDuckedRotMinSpeed", "fProneRotMinSpeed", "worldModel", "worldClipModel", "rocketModel", "knifeModel", "worldKnifeModel", "mountedModel", "hudIcon", "hudIconRatio", "ammoCounterIcon", "ammoCounterIconRatio", "ammoCounterClip", "iStartAmmo", "szAmmoName", "iAmmoIndex", "szClipName", "iClipIndex", "iHeatIndex", "iMaxAmmo", "iClipSize", "shotCount", "szSharedAmmoCapName", "iSharedAmmoCapIndex", "iSharedAmmoCap", "unlimitedAmmo", "damage", "damageDuration", "damageInterval", "playerDamage", "iMeleeDamage", "iDamageType", "iFireDelay", "iMeleeDelay", "meleeChargeDelay", "iDetonateDelay", "iFireTime", "iRechamberTime", "iRechamberBoltTime", "iHoldFireTime", "iDetonateTime", "iMeleeTime", "meleeChargeTime", "iReloadTime", "reloadShowRocketTime", "iReloadEmptyTime", "iReloadAddTime", "reloadEmptyAddTime", "iReloadStartTime", "iReloadStartAddTime", "iReloadEndTime", "iDropTime", "iRaiseTime", "iAltDropTime", "iAltRaiseTime", "quickDropTime", "quickRaiseTime", "iFirstRaiseTime", "iEmptyRaiseTime", "iEmptyDropTime", "sprintInTime", "sprintLoopTime", "sprintOutTime", "deployTime", "breakdownTime", "nightVisionWearTime", "nightVisionWearTimeFadeOutEnd", "nightVisionWearTimePowerUp", "nightVisionRemoveTime", "nightVisionRemoveTimePowerDown", "nightVisionRemoveTimeFadeInStart", "fuseTime", "aiFuseTime", "requireLockonToFire", "noAdsWhenMagEmpty", "avoidDropCleanup", "autoAimRange", "aimAssistRange", "aimAssistRangeAds", "mountableWeapon", "aimPadding", "enemyCrosshairRange", "crosshairColorChange", "moveSpeedScale", "adsMoveSpeedScale", "sprintDurationScale", "fAdsZoomFov", "fAdsZoomInFrac", "fAdsZoomOutFrac", "overlayMaterial", "overlayMaterialLowRes", "overlayReticle", "overlayInterface", "overlayWidth", "overlayHeight", "fAdsBobFactor", "fAdsViewBobMult", "fHipSpreadStandMin", "fHipSpreadDuckedMin", "fHipSpreadProneMin", "hipSpreadStandMax", "hipSpreadDuckedMax", "hipSpreadProneMax", "fHipSpreadDecayRate", "fHipSpreadFireAdd", "fHipSpreadTurnAdd", "fHipSpreadMoveAdd", "fHipSpreadDuckedDecay", "fHipSpreadProneDecay", "fHipReticleSidePos", "iAdsTransInTime", "iAdsTransOutTime", "fAdsIdleAmount", "fHipIdleAmount", "adsIdleSpeed", "hipIdleSpeed", "fIdleCrouchFactor", "fIdleProneFactor", "fGunMaxPitch", "fGunMaxYaw", "swayMaxAngle", "swayLerpSpeed", "swayPitchScale", "swayYawScale", "swayHorizScale", "swayVertScale", "swayShellShockScale", "adsSwayMaxAngle", "adsSwayLerpSpeed", "adsSwayPitchScale", "adsSwayYawScale", "adsSwayHorizScale", "adsSwayVertScale", "bRifleBullet", "armorPiercing", "bBoltAction", "aimDownSight", "bRechamberWhileAds", "adsViewErrorMin", "adsViewErrorMax", "bCookOffHold", "bClipOnly", "canUseInVehicle", "noDropsOrRaises", "adsFireOnly", "cancelAutoHolsterWhenEmpty", "suppressAmmoReserveDisplay", "enhanced", "laserSightDuringNightvision", "bayonet", "killIcon", "killIconRatio", "flipKillIcon", "dpadIcon", "dpadIconRatio", "bNoPartialReload", "bSegmentedReload", "noADSAutoReload", "iReloadAmmoAdd", "iReloadStartAdd", "szAltWeaponName", "altWeaponIndex", "iDropAmmoMin", "iDropAmmoMax", "blocksProne", "silenced", "iExplosionRadius", "iExplosionRadiusMin", "iExplosionInnerDamage", "iExplosionOuterDamage", "damageConeAngle", "iProjectileSpeed", "iProjectileSpeedUp", "iProjectileSpeedForward", "iProjectileActivateDist", "projLifetime", "timeToAccelerate", "projectileCurvature", "projectileModel", "projExplosion", "projExplosionEffect", "projExplosionEffectForceNormalUp", "projDudEffect", "projExplosionSound", "projDudSound", "mortarShellSound", "tankShellSound", "bProjImpactExplode", "stickiness", "hasDetonator", "timedDetonation", "rotate", "holdButtonToThrow", "freezeMovementWhenFiring", "lowAmmoWarningThreshold", "parallelBounce", "perpendicularBounce", "projTrailEffect", "vProjectileColor", "guidedMissileType", "maxSteeringAccel", "projIgnitionDelay", "projIgnitionEffect", "projIgnitionSound", "fAdsAimPitch", "fAdsCrosshairInFrac", "fAdsCrosshairOutFrac", "adsGunKickReducedKickBullets", "adsGunKickReducedKickPercent", "fAdsGunKickPitchMin", "fAdsGunKickPitchMax", "fAdsGunKickYawMin", "fAdsGunKickYawMax", "fAdsGunKickAccel", "fAdsGunKickSpeedMax", "fAdsGunKickSpeedDecay", "fAdsGunKickStaticDecay", "fAdsViewKickPitchMin", "fAdsViewKickPitchMax", "fAdsViewKickYawMin", "fAdsViewKickYawMax", "fAdsViewKickCenterSpeed", "fAdsViewScatterMin", "fAdsViewScatterMax", "fAdsSpread", "hipGunKickReducedKickBullets", "hipGunKickReducedKickPercent", "fHipGunKickPitchMin", "fHipGunKickPitchMax", "fHipGunKickYawMin", "fHipGunKickYawMax", "fHipGunKickAccel", "fHipGunKickSpeedMax", "fHipGunKickSpeedDecay", "fHipGunKickStaticDecay", "fHipViewKickPitchMin", "fHipViewKickPitchMax", "fHipViewKickYawMin", "fHipViewKickYawMax", "fHipViewKickCenterSpeed", "fHipViewScatterMin", "fHipViewScatterMax", "fightDist", "maxDist", "aiVsAiAccuracyGraphName", "aiVsAiAccuracyGraphKnots", "originalAiVsAiAccuracyGraphKnots", "aiVsPlayerAccuracyGraphName", "aiVsPlayerAccuracyGraphKnots", "originalAiVsPlayerAccuracyGraphKnots", "aiVsAiAccuracyGraphKnotCount", "aiVsPlayerAccuracyGraphKnotCount", "originalAiVsAiAccuracyGraphKnotCount", "originalAiVsPlayerAccuracyGraphKnotCount", "iPositionReloadTransTime", "leftArc", "rightArc", "topArc", "bottomArc", "accuracy", "aiSpread", "playerSpread", "minTurnSpeed", "maxTurnSpeed", "pitchConvergenceTime", "yawConvergenceTime", "suppressTime", "maxRange", "fAnimHorRotateInc", "fPlayerPositionDist", "szUseHintString", "dropHintString", "iUseHintStringIndex", "dropHintStringIndex", "horizViewJitter", "vertViewJitter", "szScript", "fOOPosAnimLength", "minDamage", "minPlayerDamage", "fMaxDamageRange", "fMinDamageRange", "destabilizationRateTime", "destabilizationCurvatureMax", "destabilizeDistance", "locationDamageMultipliers", "fireRumble", "meleeImpactRumble", "adsDofStart", "adsDofEnd", "hipDofStart", "hipDofEnd", "flameTableFirstPerson", "flameTableThirdPerson", "flameTableFirstPersonPtr", "flameTableThirdPersonPtr", "tagFx_preparationEffect", "tagFlash_preparationEffect" } },
  { "XAnimDeltaPartQuatDataFrames", 0, 2, (const char *const[]){ "indices", "frames" } },
  { "XAnimPartTransFrames", 0, 2, (const char *const[]){ "indices", "frames" } },
  { "XAnimParts", 0, 11, (const char *const[]){ "name", "names", "notify", "deltaPart", "dataByte", "dataShort", "dataInt", "randomDataShort", "randomDataByte", "randomDataInt", "indices" } },
  { "XSurface", 0, 5, (const char *const[]){ "zoneHandle", "vertInfo", "verts0", "vertList", "triIndices" } },
  { "XSurfaceVertexInfo", 0, 1, (const char *const[]){ "vertsBlend" } },
};
#define T4Z_ORDER_N (sizeof(T4Z_ORDER)/sizeof(T4Z_ORDER[0]))

/* ---- tipo de asset -> struct ---- */
static const T4ZPairEnt T4Z_TYPESTRUCT[] = {
  { "1", "PhysPreset" },
  { "2", "PhysConstraints" },
  { "3", "DestructibleDef" },
  { "4", "XAnimParts" },
  { "5", "XModel" },
  { "6", "Material" },
  { "7", "MaterialTechniqueSet" },
  { "8", "GfxImage" },
  { "9", "snd_alias_list_t" },
  { "10", "LoadedSound" },
  { "11", "clipMap_t" },
  { "12", "clipMap_t" },
  { "13", "ComWorld" },
  { "14", "GameWorldSp" },
  { "15", "GameWorldMp" },
  { "16", "MapEnts" },
  { "17", "GfxWorld" },
  { "18", "GfxLightDef" },
  { "19", "UiMap" },
  { "20", "Font_s" },
  { "21", "MenuList" },
  { "22", "menuDef_t" },
  { "23", "LocalizeEntry" },
  { "24", "WeaponDef" },
  { "25", "SndDriverGlobals" },
  { "26", "FxEffectDef" },
  { "27", "FxImpactTable" },
  { "32", "RawFile" },
  { "33", "StringTable" },
  { "34", "PackIndex" },
};
#define T4Z_TYPESTRUCT_N (sizeof(T4Z_TYPESTRUCT)/sizeof(T4Z_TYPESTRUCT[0]))
