/**
 * PureEngine — Step 77: Shader file boundary
 * File: shader.h
 *
 * GLSL stops living as string literals inside renderer.h: this boundary
 * reads vertex/fragment sources from files, compiles, and links them
 * into a program. The renderer calls one function and keeps every
 * behavior it had — same checks, same stderr messages (plus filenames),
 * same cleanup discipline.
 *
 * Path rule (house convention, tilemap/animation precedent): callers pass
 * BASE filenames; this header probes assets/shaders/<name> from the
 * current directory, then ../ and ../../ (repo root, build/, and
 * build/Release/ launches all resolve). No new dependency, no GL context
 * management here — the caller owns the window, like everywhere else.
 *
 * Failure contract (renderer.h precedent): print to stderr, return 0.
 * Partial state never escapes — a failed second compile deletes the
 * first shader, a failed link deletes the program.
 *
 * Header-only, same discipline as every project module: no shader.cpp,
 * no CMakeLists.txt change for THIS file (bundles copy the GLSL assets).
 */
#ifndef PUREENGINE_SHADER_H
#define PUREENGINE_SHADER_H
// Include guard, same pattern as every other project header.

#include <glad/gl.h>  // every GL call below goes through the GLAD loader
#include <fstream>     // shader source files
#include <iostream>    // compile/link diagnostics (renderer discipline)
#include <sstream>     // file -> string
#include <string>      // paths + sources

namespace pe {

// Read a whole text file; empty string when it cannot be opened. The
// caller distinguishes missing/empty files by warning itself.
inline std::string readShaderFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        return std::string();
    }
    std::ostringstream text;
    text << in.rdbuf();
    return text.str();
}

// Compile one stage; stderr + 0 on failure (shader deleted). The label
// mirrors renderer.h's old messages so logs stay greppable.
inline GLuint compileShader(const std::string& source, GLenum type) {
    const char* kind = (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
    GLuint shader = glCreateShader(type);
    const GLchar* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    int success = 0;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << kind << " shader compilation failed:\n"
                  << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

// Load, compile, and link a program from two GLSL files in
// assets/shaders/. Returns the program, or 0 after printing diagnostics.
inline GLuint loadShader(const std::string& vertFile, const std::string& fragFile) {
    const std::string vertCandidates[3] = {
        std::string("assets/shaders/") + vertFile,
        std::string("../assets/shaders/") + vertFile,
        std::string("../../assets/shaders/") + vertFile
    };
    const std::string fragCandidates[3] = {
        std::string("assets/shaders/") + fragFile,
        std::string("../assets/shaders/") + fragFile,
        std::string("../../assets/shaders/") + fragFile
    };
    std::string vertSource;
    for (const std::string& candidate : vertCandidates) {
        vertSource = readShaderFile(candidate);
        if (!vertSource.empty()) {
            break;
        }
    }
    if (vertSource.empty()) {
        std::cerr << "Vertex shader file not found: " << vertFile
                  << " (tried: assets/shaders/, ../assets/shaders/, ../../assets/shaders/)"
                  << std::endl;
        return 0;
    }
    std::string fragSource;
    for (const std::string& candidate : fragCandidates) {
        fragSource = readShaderFile(candidate);
        if (!fragSource.empty()) {
            break;
        }
    }
    if (fragSource.empty()) {
        std::cerr << "Fragment shader file not found: " << fragFile
                  << " (tried: assets/shaders/, ../assets/shaders/, ../../assets/shaders/)"
                  << std::endl;
        return 0;
    }

    const GLuint vertexShader = compileShader(vertSource, GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        return 0;  // message already printed; nothing else created yet
    }
    const GLuint fragmentShader = compileShader(fragSource, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);  // mirror renderer's old cleanup
        return 0;
    }

    // Linking can fail even when both stages compiled (interface
    // mismatch), so check GL_LINK_STATUS. Shaders are baked into the
    // program either way — delete them on both paths, as before.
    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    int success = 0;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

}  // namespace pe

#endif  // PUREENGINE_SHADER_H
