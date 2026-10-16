/**
 * PureEngine - Step 324: the prefab I/O split (Entity/Scene/Persistence
 * slice 3). File: prefab_persist.h
 *
 * What moved here (from prefab.h :58-177, the bodies VERBATIM, inline
 * stripped): loadPrefab + the prefab_detail parse helpers
 * (prefabTrim/prefabCandidates/prefabParseFloat/prefabParseVec3/
 * prefabParseBool - now file-local in prefab_persist.cpp; zero external
 * users verified at Step 324). prefab.h keeps the Prefab struct +
 * instantiatePrefab (pure construction, no I/O) and includes THIS header
 * after its namespace close (the guarded-include pattern, the same as the
 * scene_persist pair - both include orders work, no circular dependency).
 *
 * The load contracts (UNCHANGED, pinned by hostile_data_test's checkPrefab*
 * family): the 3-candidate probe (assets/prefabs/, ../assets/prefabs/,
 * ../../assets/prefabs/) + the CWD-relative and absolute-path fallbacks, the
 * strict "# PureEngine prefab v1" header check, the line-by-line key=value
 * parse with the malformed-warn+skip (the valid fields kept), the unknown-key
 * warn+skip, no exceptions, out assigned only on the full success.
 */
#ifndef PUREENGINE_PREFAB_PERSIST_H
#define PUREENGINE_PREFAB_PERSIST_H

#include "prefab.h"

namespace pe {

bool loadPrefab(const std::string& filename, Prefab& out);

} // namespace pe

#endif // PUREENGINE_PREFAB_PERSIST_H
