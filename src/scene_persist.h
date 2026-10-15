/**
 * PureEngine - Step 323: the scene persistence split (Entity/Scene/
 * Persistence slice 2). File: scene_persist.h
 *
 * What moved here (from scene.h :277-816, the bodies VERBATIM, inline
 * stripped): saveSceneToFile, loadSceneFromFile, saveSceneManagerToFile,
 * loadSceneManagerFromFile + the persistence-only parse/quote helpers
 * (sceneTrim/sceneParseFloat/sceneParseInt/sceneSplitComma/sceneQuote/
 * sceneUnquote/sceneSplitCommaQuoted - now file-local in scene_persist.cpp;
 * zero external users verified at Step 323). scene.h keeps the data types +
 * the inline list ops and includes THIS header at its end, so every existing
 * caller compiles unchanged (the guarded-include pattern: this header
 * includes scene.h for the complete types; the guards order the pair - no
 * circular dependency).
 *
 * The contracts (UNCHANGED, pinned by hostile_data_test's
 * checkPersistenceFormatPins + the Step 111/113/282 records): the 4-decimal
 * writer, the dead drop-on-save, the strict v1(14)/v2(34) parse, the
 * unknown-version reject with out untouched, the unknown-prefix strict
 * reject, the atomic tmp+rename (fs::rename), meshId not serialized, the
 * explicit-path + the 3-candidate probe path behavior.
 */
#ifndef PUREENGINE_SCENE_PERSIST_H
#define PUREENGINE_SCENE_PERSIST_H

#include "scene.h"

namespace pe {

bool saveSceneToFile(const Scene& s, const std::string& fileName);
bool loadSceneFromFile(const std::string& fileName, Scene& out);
bool saveSceneManagerToFile(const SceneManager& m, const std::string& fileName);
bool loadSceneManagerFromFile(const std::string& fileName, SceneManager& out);

} // namespace pe

#endif // PUREENGINE_SCENE_PERSIST_H
