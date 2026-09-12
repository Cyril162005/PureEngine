/**
 * PureEngine — Step 103: Lightweight component helpers (not ECS)
 * Optional free-function helpers for common data (Velocity, Health, Tag, Timer).
 * Entities remain plain structs (Step 103 fields: health, timer, tag in entity.h);
 * helpers are opt-in, no archetype storage, no mandatory components, no query language.
 * Existing games unchanged (defaults: health 100, timer 0, tag "", velocity 0).
 * Header-only, no CMake change.
 */
#ifndef PUREENGINE_COMPONENTS_H
#define PUREENGINE_COMPONENTS_H

#include <string>
#include "entity.h"
#include "math/vec3.h"

namespace pe {

// Velocity helpers (Entity.velocity already exists, helpers are sugar)
inline void setVelocity(Entity& e, const Vec3& v) { e.velocity = v; }
inline void addVelocity(Entity& e, const Vec3& dv) { e.velocity = e.velocity + dv; }
inline Vec3 getVelocity(const Entity& e) { return e.velocity; }

// Health helpers (Entity.health, default 100)
inline void setHealth(Entity& e, float hp) { e.health = hp < 0.0f ? 0.0f : hp; }
inline float getHealth(const Entity& e) { return e.health; }
inline bool isAlive(const Entity& e) { return e.health > 0.0f; }
inline void damage(Entity& e, float amt) { setHealth(e, e.health - amt); }
inline void heal(Entity& e, float amt) { e.health += amt; }

// Timer helpers (Entity.timer, generic countdown)
inline void setTimer(Entity& e, float t) { e.timer = t; }
inline float getTimer(const Entity& e) { return e.timer; }
inline void tickTimer(Entity& e, float dt) { e.timer -= dt; if (e.timer < 0.0f) e.timer = 0.0f; }
inline bool timerDone(const Entity& e) { return e.timer <= 0.0f; }

// Tag helpers (Entity.tag, free-form string)
inline void setTag(Entity& e, const std::string& t) { e.tag = t; }
inline const std::string& getTag(const Entity& e) { return e.tag; }
inline bool hasTag(const Entity& e, const std::string& t) { return e.tag == t; }
inline void clearTag(Entity& e) { e.tag.clear(); }

} // namespace pe

#endif // PUREENGINE_COMPONENTS_H
