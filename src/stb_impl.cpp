// --- Step 10: stb_image implementation translation unit ---
// stb libraries are "single-header" libraries: the header carries BOTH
// the declarations and the implementation, and the implementation is
// compiled in exactly ONE translation unit — the one that defines the
// *_IMPLEMENTATION macro before including the header. That unit lives
// HERE, deliberately separate from main.cpp:
//   - main.cpp only needs the DECLARATIONS (plain #include <stb_image.h>),
//   - keeping ~8,000 lines of generated implementation code out of
//     main.cpp keeps the engine's own file readable,
//   - and it mirrors how miniaudio's static lib isolates ITS
//     implementation from our code.
// Same pattern the C standard library uses: declare everywhere,
// define once.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
