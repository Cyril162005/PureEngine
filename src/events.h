/**
 * PureEngine — Step 67: Event system foundation
 * File: events.h
 *
 * A tiny synchronous event bus so systems can communicate WITHOUT hard
 * dependencies (the stated future uses: "collision happened", "scene
 * changed"). Old-school and minimal on purpose: a fixed array of
 * handler lists, integer tokens, direct calls — no priorities, no
 * delayed/queued events, no game wiring yet. Nothing in the engine
 * emits or subscribes in Step 67; adoption is a later step, so Arcade
 * and Pong are unaffected by construction (they do not include this
 * header).
 *
 * Payload philosophy (Step 54's roleId rule, reused): GameEvent carries
 * opaque ints the bus never interprets — the EMITTER defines what a/b
 * mean per EventType. New event types append ABOVE the Count sentinel;
 * nothing else in this file changes.
 *
 * Delivery contract:
 *  - emit() calls handlers in subscription order, synchronously.
 *  - emit() iterates a SNAPSHOT: subscribing/unsubscribing (or emitting
 *    again) from inside a handler is safe — membership changes take
 *    effect on the NEXT emit, and each nesting level snapshots
 *    independently, so reentrant emit works.
 *  - Tokens are monotonic per bus and NEVER reused (not even by
 *    clear()): a stale token can never alias a future subscription.
 *  - Single-threaded ONLY — the engine is single-threaded, and this
 *    bus has no locking. Concurrent use is a caller bug.
 *
 * Header-only, same discipline as every project module: no events.cpp,
 * no CMakeLists.txt change. std::function is pure stdlib (C++17 is
 * already set) — no new dependency.
 */
#ifndef PUREENGINE_EVENTS_H
#define PUREENGINE_EVENTS_H

#include <cstddef>     // std::size_t for handlerCount / indexing
#include <functional>  // EventHandler (stdlib — see note above)
#include <vector>      // per-type handler lists

namespace pe {

// Core event types. Meanings of GameEvent::a/b per type are documented
// on each enumerator; the bus itself assigns no meaning.
enum class EventType {
    Collision,    // two entities touched: a/b = entity indices (emitter-defined)
    SceneChanged, // current scene switched: a = previous index, b = new index (-1 = none)
    Count         // sentinel — new types go ABOVE this line
};

struct GameEvent {
    EventType type;
    int a = -1;
    int b = -1;
};

using EventHandler = std::function<void(const GameEvent&)>;

struct EventBus {
    // Register handler for type. Returns a token for unsubscribe, or -1
    // on refusal (empty handler, or type out of range incl. Count).
    int subscribe(EventType type, EventHandler handler) {
        if (!handler || !valid(type)) {
            return -1;
        }
        slots_[index(type)].push_back(HandlerSlot{nextToken_, handler});
        return nextToken_++;
    }

    // Remove a registration. False = unknown type or unknown token
    // (including already-removed or never-issued tokens).
    bool unsubscribe(EventType type, int token) {
        if (!valid(type)) {
            return false;
        }
        std::vector<HandlerSlot>& list = slots_[index(type)];
        for (std::size_t i = 0; i < list.size(); ++i) {
            if (list[i].token == token) {
                list.erase(list.begin() +
                           static_cast<std::ptrdiff_t>(i));
                return true;
            }
        }
        return false;
    }

    // Deliver event to the type's handlers in subscription order (see
    // the snapshot contract in the file header). Unknown type: no-op.
    void emit(const GameEvent& event) {
        if (!valid(event.type)) {
            return;
        }
        const std::vector<HandlerSlot> snapshot = slots_[index(event.type)];
        for (std::size_t i = 0; i < snapshot.size(); ++i) {
            snapshot[i].handler(event);
        }
    }

    // Drop every handler of every type. Tokens stay retired (see the
    // file header): post-clear subscriptions get fresh tokens.
    void clear() {
        for (std::size_t t = 0; t < COUNT; ++t) {
            slots_[t].clear();
        }
    }

    // Registrations currently held for type (0 for unknown types).
    std::size_t handlerCount(EventType type) const {
        if (!valid(type)) {
            return 0;
        }
        return slots_[index(type)].size();
    }

private:
    struct HandlerSlot {
        int token;
        EventHandler handler;
    };

    static constexpr std::size_t COUNT =
        static_cast<std::size_t>(EventType::Count);

    static std::size_t index(EventType type) {
        return static_cast<std::size_t>(type);
    }

    // Negative underlying values wrap huge as size_t, so one upper-
    // bound check covers every out-of-range encoding.
    static bool valid(EventType type) { return index(type) < COUNT; }

    std::vector<HandlerSlot> slots_[COUNT];
    int nextToken_ = 0;
};

}  // namespace pe

#endif  // PUREENGINE_EVENTS_H
