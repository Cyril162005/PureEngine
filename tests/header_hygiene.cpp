/**
 * PureEngine Step 281: the permanent header-hygiene check (the (g)
 * capability). THE INVARIANT: a target compiled with src on its include
 * path can include the standard C/C++ headers that previously collided
 * (the pre-281 src/time.h shadowed <time.h>; the 281 rename removed the
 * collision) without breaking. Returns 0 on success; a reintroduced
 * colliding header name makes this target FAIL to build.
 */
#include <time.h>
#include <ctime>
#include "engine_time.h"

int main() {
    // Both spellings of the standard time header compile and coexist
    // with the engine's timing boundary on the same include path.
    const std::time_t t = std::time(nullptr);
    (void)t;
    return 0;
}
