/**
 * PureEngine — Step 13: Renderer Module Boundary
 * File: renderer.h
 *
 * The engine's first SYSTEM boundary. Before this step, the ENTIRE
 * rendering flow lived directly in main.cpp: shader compile/link,
 * uniform lookups, two VAO/VBO pairs, five texture objects, the entity
 * draw loop, the Phase 3/4 UI digit path, and FOUR duplicated GL
 * cleanup chains on the error paths. main.cpp was simultaneously the
 * window owner, the audio owner, the game-logic owner, AND the renderer.
 *
 * Step 13 moves the rendering responsibility OUT into this class, and
 * nothing else. main.cpp now asks the renderer to initialize GPU
 * resources and to submit frames; HOW the pixels are produced is the
 * renderer's business. The boundary is justified by the existing code,
 * not by engine fashion: every resource below already existed, every
 * draw call below already ran — they are RELOCATED, byte-compatible,
 * not redesigned.
 *
 * What this class deliberately does NOT introduce (the project's
 * kill criterion applies to architecture too):
 *   - no scene graph, no material system, no render graph
 *   - no general graphics abstraction over OpenGL
 *   - no interfaces/virtuals, no renderer registry, no ECS
 * One concrete shader, one concrete triangle, one concrete digit font.
 *
 * Seams that intentionally stay INSIDE for later continuation steps:
 *   - texture LOADING was split into its own boundary by Step 14
 *     (src/resources.h — this class now OBTAINS textures through it
 *     and never touches a file itself);
 *   - camera STATE and MATH were split into their own boundary by
 *     Step 15 (src/camera.h — drawWorld now RECEIVES the view matrix
 *     and constructs nothing camera-related);
 *   - the digit path stays a renderer method; Step 21 gave the UI
 *     its own boundary (src/ui.h — it formats the numbers and hands
 *     them to drawDigitString, owning no GL objects itself).
 *
 * Header-only, same as src/math/, entity.h, collision.h, gamestate.h:
 * every method is defined here, no renderer.cpp exists, and
 * CMakeLists.txt needs no change.
 *
 * Behavior preservation checklist (the step's contract):
 *   - shader sources, uniform names, and locations: unchanged
 *   - vertex layout (position xyz + UV st, stride 5 floats): unchanged
 *   - texture sampling parameters (CLAMP_TO_EDGE + LINEAR): unchanged
 *   - per-entity texture selection by index convention
 *     (0 = player, 1..2 = scenery, 3+ = hostiles): unchanged
 *   - collision tint (white normally, red (1,0,0) when colliding): unchanged
 *   - screen-space digits (projection * model, NO view; blending ON
 *     for text only): unchanged
 */
#ifndef PUREENGINE_RENDERER_H
#define PUREENGINE_RENDERER_H
// Include guard, same pattern as entity.h and the math headers.

#include <algorithm>     // Step 49: std::stable_sort for depth-based draw order
#include <glad/gl.h>     // every GL call below goes through the GLAD loader
#include <iostream>      // the same stderr diagnostics main.cpp always used
#include <string>        // drawDigitString takes formatted game text
#include <vector>        // entity list + collision flags arrive by reference

#include "resources.h"   // Step 14: texture LOADING lives in the resource
                         // boundary now — this class obtains textures from
                         // it and never calls stb_image itself (stb's ONE
                         // implementation stays in src/stb_impl.cpp).
#include "math/vec3.h"   // Vec3 types used by the entity/math interfaces
#include "math/mat4.h"   // view/MVP construction
#include "entity.h"      // drawWorld reads pe::Entity data

namespace pe {

class Renderer {
public:
    // --- Step 13: create every GPU resource the game renders with ---
    // One call replaces main.cpp's entire pre-loop rendering section:
    // shader compile/link, uniform lookups, the triangle and text
    // VAO/VBO pairs, all five textures (checker legacy, player,
    // scenery, hostile, font atlas), and the sampler-uniform bind.
    // Returns false on ANY failure — with the exact stderr message the
    // old inline code printed — after deleting every GL object it had
    // already created (deleting GL name 0 is a safe no-op, so ONE
    // cleanup path serves every failure point). main.cpp then handles
    // the NON-rendering teardown (audio, window, GLFW).
    bool init() {
        // --- Step 4: shader sources, byte-identical to the originals ---
        // VERTEX SHADER: position (location 0) + UV (location 1, Step 10)
        // in, one 'transform' uniform (Step 5) applied to every vertex.
        const char* vertexShaderSource =
            "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "layout (location = 1) in vec2 aTexCoord;\n"
            "uniform mat4 transform;\n"
            "out vec2 TexCoord;\n"
            "void main() {\n"
            "    gl_Position = transform * vec4(aPos, 1.0);\n"
            "    TexCoord = aTexCoord;\n"
            "}\n";

        // FRAGMENT SHADER: Step 10's texture lookup with the Step 8
        // 'color' uniform surviving as a TINT multiplier (white leaves
        // texels untouched; red zeroes green+blue — collision feedback).
        // Phase 3: output alpha comes from the texture — RGB world
        // textures carry alpha 1.0 and blending stays OFF for the world,
        // so only the RGBA font atlas ever uses the channel.
        const char* fragmentShaderSource =
            "#version 330 core\n"
            "uniform sampler2D tex;\n"
            "uniform vec3 color;\n"
            "in vec2 TexCoord;\n"
            "out vec4 FragColor;\n"
            "void main() {\n"
            "    vec4 texel = texture(tex, TexCoord);\n"
            "    FragColor = vec4(texel.rgb * color, texel.a);\n"
            "}\n";

        // Compile the vertex shader — with the Step 4 error check. A
        // broken shader must fail LOUDLY, never as a silent black screen.
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
            glDeleteShader(vertexShader);
            return false;   // nothing else was created yet
        }

        // Compile the fragment shader — same three calls, same check.
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return false;
        }

        // Link both into the program; linking can fail even when both
        // shaders compiled (interface mismatch), so check GL_LINK_STATUS.
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        // The individual shader objects are baked into the program now —
        // delete them either way (same as the original code).
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
            destroyAll();
            return false;
        }

        // --- Step 5 / Step 8: uniform locations, checked like always ---
        // -1 means the name is missing (typo or optimized away); without
        // the check the per-frame uploads would fail SILENTLY.
        transformLocation = glGetUniformLocation(shaderProgram, "transform");
        if (transformLocation < 0) {
            std::cerr << "Uniform 'transform' not found in shader program" << std::endl;
            destroyAll();
            return false;
        }
        colorLocation = glGetUniformLocation(shaderProgram, "color");
        if (colorLocation < 0) {
            std::cerr << "Uniform 'color' not found in shader program" << std::endl;
            destroyAll();
            return false;
        }

        // --- Step 4: triangle geometry, VBO + VAO ---
        // Interleaved position (xyz) + UV (st), same array the world has
        // rendered since Step 10: bottom corners take the texture's
        // bottom UV corners, the apex takes (0.5, 1).
        float vertices[] = {
            // position              // UV
            -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,   // bottom-left
             0.5f, -0.5f, 0.0f,     1.0f, 0.0f,   // bottom-right
             0.0f,  0.5f, 0.0f,     0.5f, 1.0f    // top
        };
        glGenVertexArrays(1, &worldVAO);
        glGenBuffers(1, &worldVBO);
        glBindVertexArray(worldVAO);
        glBindBuffer(GL_ARRAY_BUFFER, worldVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Attribute 0 = position: 3 floats, stride 5 floats, offset 0.
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Attribute 1 = UV: 2 floats, same stride, offset 3 floats in.
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // --- Step 10: the checker texture (legacy asset since Phase 5) ---
        // Same 3-candidate CWD probe every asset uses: relative paths
        // resolve against wherever the exe was launched from (repo root,
        // build/, or build/Release/). Step 14: the load/upload pattern
        // itself moved to the resource boundary (pe::loadRgbTexture).
        const char* checkerCandidates[3] = {
            "assets/checker.png", "../assets/checker.png", "../../assets/checker.png"
        };
        checkerTexture = pe::loadRgbTexture(checkerCandidates);
        if (checkerTexture == 0) {
            std::cerr << "Failed to load assets/checker.png (tried: assets/, ../assets/, ../../assets/)" << std::endl;
            destroyAll();
            return false;
        }

        // --- Game Build Phase 5: the three per-type entity textures ---
        // pe::loadRgbTexture is the extracted third copy of the checker's
        // load/upload pattern (Phase 5's ruling), relocated to the
        // resource boundary by Step 14. Tint rule reminder:
        // every palette color keeps red-channel content, or it would
        // render BLACK under the collision tint.
        playerTexture = loadRgbAsset("tex_player.png");
        sceneryTexture = loadRgbAsset("tex_scenery.png");
        hostileTexture = loadRgbAsset("tex_hostile.png");
        hostileTextureAlt = loadRgbAsset("tex_hostile_alt.png");
        if (playerTexture == 0 || sceneryTexture == 0 || hostileTexture == 0 || hostileTextureAlt == 0) {
            std::cerr << "Failed to load Phase 5 entity textures (tried: assets/, ../assets/, ../../assets/)" << std::endl;
            destroyAll();
            return false;
        }

        // --- Game Build Phase 3: the digit font atlas ---
        // assets/font_digits.png, generated by make_font.ps1: one row of
        // eleven 16x16 cells (digits 0-9 then '.'), white 5x7 glyphs on
        // a TRANSPARENT background. Four FORCED CHANNELS — the alpha is
        // what lets glyphs blend over the scene instead of painting solid
        // backing rectangles. Step 14: the load/upload moved to the
        // resource boundary (pe::loadRgbaTexture).
        const char* fontCandidates[3] = {
            "assets/font_digits.png", "../assets/font_digits.png", "../../assets/font_digits.png"
        };
        fontTexture = pe::loadRgbaTexture(fontCandidates);
        if (fontTexture == 0) {
            std::cerr << "Failed to load assets/font_digits.png (tried: assets/, ../assets/, ../../assets/)" << std::endl;
            destroyAll();
            return false;
        }

        // --- Phase 3: quad geometry for text glyphs ---
        // The triangle's pattern repeated for a unit QUAD (two triangles),
        // SAME interleaved layout so the SAME shader consumes it. The
        // initial upload only establishes the buffer's SIZE with NULL
        // data; the real per-glyph vertices ride in every draw.
        // GL_DYNAMIC_DRAW: re-uploaded every text draw — tiny (six
        // vertices), honest about its use.
        glGenVertexArrays(1, &textVAO);
        glGenBuffers(1, &textVBO);
        glBindVertexArray(textVAO);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferData(GL_ARRAY_BUFFER, 6 * 5 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // --- Step 42: debug AABB line-loop geometry (unit square, 4 verts) ---
        // Same interleaved 5-float layout so the SAME shader + attribute
        // pointers consume it. UVs are set to (0,0) — sampled, irrelevant.
        // The four corners visit a +1/-1 unit square in order, which the
        // draw method scales by halfExtents*2 to get the real box size.
        float aabbVertices[] = {
            // position              // UV
            -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,   // bottom-left
             1.0f, -1.0f, 0.0f,     0.0f, 0.0f,   // bottom-right
             1.0f,  1.0f, 0.0f,     0.0f, 0.0f,   // top-right
            -1.0f,  1.0f, 0.0f,     0.0f, 0.0f    // top-left
        };
        glGenVertexArrays(1, &aabbVAO);
        glGenBuffers(1, &aabbVBO);
        glBindVertexArray(aabbVAO);
        glBindBuffer(GL_ARRAY_BUFFER, aabbVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(aabbVertices), aabbVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // --- Step 10: bind the sampler uniform to TEXTURE UNIT 0 ---
        // A sampler2D uniform holds a unit INDEX, not image data; set
        // once (uniforms persist in the program) while the program is
        // current. Every per-frame texture switch is then just a rebind
        // of unit 0's contents.
        glUseProgram(shaderProgram);
        GLint texLocation = glGetUniformLocation(shaderProgram, "tex");
        if (texLocation < 0) {
            std::cerr << "Uniform 'tex' not found in shader program" << std::endl;
            destroyAll();
            return false;
        }
        glUniform1i(texLocation, 0);   // sampler reads from GL_TEXTURE0

        return true;
    }

    // --- Step 13: release every GPU resource, reverse order of creation ---
    // Replaces the four duplicated cleanup chains that used to follow
    // every error path in main.cpp. GL delete calls on name 0 are safe
    // no-ops, so one method serves both the fatal-exit and the
    // normal-shutdown case. Called exactly once by main.cpp after the
    // loop (audio teardown stays there — it is not rendering).
    void shutdown() {
        destroyAll();
    }

    // --- Per-frame: clear the color buffer ---
    // The STATE-DEPENDENT choice of color stays in main.cpp (it is game
    // logic: menu purple, game-over dark red, the Step 3 black/blue
    // toggle); the renderer only performs the clear it is given.
    void clear(float r, float g, float b) {
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    // --- Per-frame: the world pass (PLAYING, PAUSED, GAME_OVER) ---
    // The Step 7 draw loop, relocated whole. projection AND view
    // arrive as DATA from main.cpp — since Step 15 the view comes
    // prebuilt from pe::Camera, and this method performs no camera
    // math at all; it only submits.
    void drawWorld(const Mat4& projection, const Mat4& view,
                   const std::vector<Entity>& entities,
                   const std::vector<char>& colliding) {
        glUseProgram(shaderProgram);

        // Bind the world VAO ONCE: every entity shares this vertex data —
        // only the transform differs per instance.
        glBindVertexArray(worldVAO);

        // --- Step 10: select the texture unit for this frame ---
        // The sampler uniform points at unit 0 (set once in init), so
        // whichever texture is bound below is what every draw samples.
        glActiveTexture(GL_TEXTURE0);

        // --- Step 7: ONE draw loop for ALL entities ---
        // projection * view * model, acting RIGHT-TO-LEFT on the vertex.
        // The loop neither knows nor cares how many entities exist.
        //
        // Step 49: draw order is now explicit, not accidental. Previously
        // (Step 45's comment, relocated below) this relied entirely on
        // buildInitialEntities() constructing the vector in depth order.
        // A permutation of INDICES is sorted by entity.depth (stable, so
        // entities sharing a depth keep their existing relative order) —
        // neither entities nor colliding is reordered in place, because
        // both the colliding[] lookup and the texture-select branches
        // below still address by ORIGINAL index (i == 0, i < 3), not by
        // sorted position. On the CURRENT entity set this permutation is
        // a no-op (construction order already matches depth order), so
        // today's visual draw order is unchanged; it becomes load-bearing
        // only if a future step reorders the underlying vector (dynamic
        // spawn, removal).
        std::vector<size_t> drawOrder(entities.size());
        for (size_t i = 0; i < entities.size(); ++i) {
            drawOrder[i] = i;
        }
        std::stable_sort(drawOrder.begin(), drawOrder.end(),
            [&entities](size_t a, size_t b) {
                return entities[a].depth < entities[b].depth;
            });

        for (size_t k = 0; k < drawOrder.size(); ++k) {
            const size_t i = drawOrder[k];
            const Entity& entity = entities[i];
            // --- Game Build Phase 5: per-entity texture selection ---
            // Index 0 is the player, 1..2 are the scenery pair, 3 onward
            // are hostiles — the exact same index convention the collision
            // loops in main.cpp use. One bind per entity is cheap (six
            // tiny textures, no state thrash). The checker is kept as the
            // fallback default even though every branch overrides it —
            // legacy asset, sampled by no entity anymore.
            GLuint entityTexture = checkerTexture;   // legacy default, never sampled now
            if (i == 0) {
                entityTexture = playerTexture;
            } else if (i < 3) {
                entityTexture = sceneryTexture;
            } else {
                const int textureId = entity.textureId;
                entityTexture = (textureId == 1) ? hostileTextureAlt : hostileTexture;
            }
            glBindTexture(GL_TEXTURE_2D, entityTexture);
            // Build this entity's MVP from its own data.
            Mat4 mvp = projection * view * entity.modelMatrix();
            // Upload to the 'transform' uniform (GL_FALSE: our Mat4 is
            // already column-major, the layout OpenGL expects).
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &mvp.m[0][0]);
            // Step 10: per-draw TINT — white (1,1,1) leaves the texture
            // untouched; red (1,0,0) zeroes green and blue, so a
            // colliding entity renders red-tinted. Step 8's collision
            // feedback, unchanged.
            if (colliding[i]) {
                glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f);
            } else {
                glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);
            }
            // One draw call for this entity.
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }

    // --- Per-frame: one digit string in SCREEN SPACE (Phase 3/4 UI) ---
    // The Phase 4-extracted glyph path, relocated whole — with one
    // ownership improvement the boundary makes natural: the UI state
    // setup that main.cpp used to perform around the lambda (white
    // tint, blending ON, font atlas bound, text VAO bound, blending
    // OFF again after) now lives HERE with the only code that needs
    // it. Behavior is identical: the world never renders with blending,
    // and the digits are projection * model with NO VIEW, so they stay
    // fixed to the window while the camera pans. Step 21 gave this
    // path its UI boundary (src/ui.h formats the numbers and calls
    // here); Step 13 moved the glyph mechanics intact.
    void drawDigitString(const std::string& text, float originX, float originY,
                         const Mat4& projection) {
        // Screen-space layout constants: glyph quad 0.7 units square
        // (~47 px at 66.7 px/unit — comfortably readable); advance 0.52
        // leaves a small gap between cells. Same values as Phases 3/4.
        const float glyphSize = 0.7f;
        const float glyphAdvance = 0.52f;

        glUseProgram(shaderProgram);
        // The last world draw may have left the tint uniform RED
        // (collision feedback) — text must start from white.
        glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);
        // Alpha blending: fragment alpha (the atlas is transparent
        // between glyph pixels) mixes the glyph over the finished
        // scene. Enabled for text ONLY, switched back off before return.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // The atlas on the sampler's unit — the sampler uniform still
        // points at unit 0, so rebinding the unit's contents is the
        // ENTIRE texture switch.
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        glBindVertexArray(textVAO);

        for (size_t c = 0; c < text.size(); ++c) {
            // Character -> atlas cell: '0'..'9' -> 0..9, '.' -> 10;
            // anything else is skipped (digit-only font).
            int cell = -1;
            if (text[c] >= '0' && text[c] <= '9') {
                cell = text[c] - '0';
            } else if (text[c] == '.') {
                cell = 10;
            }
            if (cell < 0) {
                continue;   // digit-only font: anything else is skipped
            }
            // This cell's horizontal slice of the atlas. V FLIP:
            // stb_image decodes PNG rows TOP-down and GL texel row 0 is
            // v = 0, so v = 0 addresses the image's TOP edge — the
            // bottom corners of the quad take v = 1, or the glyphs
            // would render upside down.
            const float u0 = static_cast<float>(cell) / FONT_CELL_COUNT;
            const float u1 = static_cast<float>(cell + 1) / FONT_CELL_COUNT;
            // Six vertices: unit quad as two triangles, UVs set for THIS
            // cell. Re-uploaded per glyph — six vertices, and clarity
            // beats a cleverer mechanism at this scale.
            float quadVertices[6][5] = {
                { -0.5f, -0.5f, 0.0f, u0, 1.0f },
                {  0.5f, -0.5f, 0.0f, u1, 1.0f },
                {  0.5f,  0.5f, 0.0f, u1, 0.0f },
                { -0.5f, -0.5f, 0.0f, u0, 1.0f },
                {  0.5f,  0.5f, 0.0f, u1, 0.0f },
                { -0.5f,  0.5f, 0.0f, u0, 0.0f }
            };
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_DYNAMIC_DRAW);
            // projection * model — NO VIEW (the screen-space rule).
            // Model = translate to this character's slot, then scale
            // the unit quad to glyph size (right-to-left, Steps 5/6).
            const Mat4 uiMvp = projection
                * Mat4::translation(Vec3(originX + static_cast<float>(c) * glyphAdvance,
                                         originY, 0.0f))
                * Mat4::scale(Vec3(glyphSize, glyphSize, 1.0f));
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &uiMvp.m[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // UI state back off — the next frame's world pass expects the
        // pipeline exactly as Steps 1-12 left it.
        glDisable(GL_BLEND);
    }

    // --- Step 42: TEMPORARY debug AABB wireframe overlay ---
    // Draws each entity's ACTUAL collision AABB as a thin line loop —
    // the EXACT scaled, NON-rotated rectangle that aabbOverlap() tests
    // against in collision.h. Purpose: visually resolve the Step 37
    // open question (is collision perception wrong, or is the sprite
    // visually smaller than the rotation-safe hitbox?).
    // Design rules obeyed: no new shader, no new texture asset, no new
    // uniforms — reuse the existing pipeline with a solid-color tint
    // and the existing checker texture (sampled, irrelevant because
    // the tint saturates everything visible).
    void drawAABBs(const Mat4& projection, const Mat4& view,
                   const std::vector<Entity>& entities) {
        glUseProgram(shaderProgram);
        glBindVertexArray(aabbVAO);
        // Checker texture on sampler unit 0 — any valid texture works;
        // the fragment shader samples it regardless, but the strong
        // uniform tint below dominates the visual output completely.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, checkerTexture);

        for (size_t i = 0; i < entities.size(); ++i) {
            const Entity& entity = entities[i];
            // Model transform for the AABB: translate to entity origin,
            // SCALE to (halfExtent*2, halfExtent*2, 1) so the unit square
            // (-1..+1) maps to the actual full-sized box, NO rotation —
            // AABBs are axis-aligned by definition. This is the exact
            // same box collision.h uses: halfExtents * scale, doubled.
            const Mat4 aabbModel =
                Mat4::translation(entity.position)
                * Mat4::scale(Vec3(entity.halfExtents.x * entity.scale.x * 2.0f,
                                   entity.halfExtents.y * entity.scale.y * 2.0f,
                                   1.0f));
            const Mat4 aabbMvp = projection * view * aabbModel;
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &aabbMvp.m[0][0]);

            // Tint: index 0 = player = bright orange (distinguish from
            // collision-red 1,0,0), everything else = yellow. Two hues
            // are enough: scenery and hostiles share yellow, since the
            // debug question is "does this rotating triangle fill its
            // square hitbox?", not "which type is which?" — textures
            // already show that.
            if (i == 0) {
                glUniform3f(colorLocation, 1.0f, 0.5f, 0.0f);   // orange — player
            } else {
                glUniform3f(colorLocation, 1.0f, 1.0f, 0.0f);   // yellow — scenery / hostiles
            }

            glDrawArrays(GL_LINE_LOOP, 0, 4);
        }
    }

private:
    GLuint loadRgbAsset(const char* baseFilename) {
        const std::string candidate0 = "assets/" + std::string(baseFilename);
        const std::string candidate1 = "../assets/" + std::string(baseFilename);
        const std::string candidate2 = "../../assets/" + std::string(baseFilename);
        const char* candidates[3] = {
            candidate0.c_str(), candidate1.c_str(), candidate2.c_str()
        };
        return pe::loadRgbTexture(candidates);
    }

    // Delete every owned GL object. All names default to 0 and GL
    // delete calls on 0 are no-ops, so this is safe at ANY point of a
    // partial init — the property that replaces main.cpp's four
    // hand-written cleanup chains with one.
    void destroyAll() {
        glDeleteVertexArrays(1, &textVAO);
        glDeleteBuffers(1, &textVBO);
        glDeleteVertexArrays(1, &aabbVAO);
        glDeleteBuffers(1, &aabbVBO);
        glDeleteVertexArrays(1, &worldVAO);
        glDeleteBuffers(1, &worldVBO);
        glDeleteProgram(shaderProgram);
        glDeleteTextures(1, &fontTexture);
        glDeleteTextures(1, &playerTexture);
        glDeleteTextures(1, &sceneryTexture);
        glDeleteTextures(1, &hostileTexture);
        glDeleteTextures(1, &hostileTextureAlt);
        glDeleteTextures(1, &checkerTexture);
        textVAO = 0;
        textVBO = 0;
        worldVAO = 0;
        worldVBO = 0;
        shaderProgram = 0;
        fontTexture = 0;
        playerTexture = 0;
        sceneryTexture = 0;
        hostileTexture = 0;
        hostileTextureAlt = 0;
        checkerTexture = 0;
    }

    // --- GPU resources owned by the renderer ---
    GLuint shaderProgram = 0;        // Step 4: the ONE program everything uses
    GLint transformLocation = -1;    // Step 5: per-draw MVP upload target
    GLint colorLocation = -1;        // Step 8/10: per-draw tint upload target
    GLuint worldVAO = 0, worldVBO = 0;  // Step 4/10: the triangle geometry
    GLuint textVAO = 0, textVBO = 0;    // Phase 3: the glyph quad geometry
    GLuint aabbVAO = 0, aabbVBO = 0;    // Step 42: debug unit-square line loop
    GLuint checkerTexture = 0;       // Step 10: legacy, sampled by no entity
    GLuint playerTexture = 0;        // Phase 5: warm green
    GLuint sceneryTexture = 0;       // Phase 5: steel blue
    GLuint hostileTexture = 0;       // Phase 5: crimson (never tinted)
    GLuint hostileTextureAlt = 0;    // Optional hostile variant texture
    GLuint fontTexture = 0;          // Phase 3: the RGBA digit atlas

    // The atlas's geometry, known FROM THE GENERATOR (not queried):
    // eleven equal cells across the atlas width (digits 0-9 plus '.').
    static const int FONT_CELL_COUNT = 11;
};

} // namespace pe

#endif // PUREENGINE_RENDERER_H
