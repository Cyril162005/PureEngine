/**
 * PureEngine — Step 66: Font/text logic boundary
 * File: font.h
 *
 * Pure LOGIC for bitmap text: which atlas cell a character lives in,
 * how wide a string runs, where an alignment starts. NO GL here — the
 * B5-A ruling stands (all glyph GL stays in pe::Renderer, which
 * consumes fontCellFor() inside drawTextString). Because nothing here
 * touches GL, every function below is directly CTest-provable without
 * a graphics context.
 *
 * Atlas contract (assets/font_digits.png, one row of 16x16 cells):
 * cells 0-10 are the frozen Phase-3 digit cells ('0'-'9' -> 0-9,
 * '.' -> 10); Step 66 appends 'A'-'Z' at cells 11-36. Lowercase folds
 * to upper. Anything without a glyph yields -1: the CALLER skips the
 * draw but still consumes the advance slot — the established
 * drawDigitString rule, so spaces gap correctly.
 *
 * Header-only, same discipline as every project module: no font.cpp,
 * no CMakeLists.txt change.
 */
#ifndef PUREENGINE_FONT_H
#define PUREENGINE_FONT_H

#include <string>  // textWidth consumes formatted game text

namespace pe {

// Left: the string starts AT the origin. Center: the string is centered
// ON the origin (the caller shifts left by half the width).
enum class TextAlign { Left, Center };

// Character -> atlas cell (see the contract above). -1 = no glyph.
inline int fontCellFor(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c == '.') {
        return 10;
    }
    if (c >= 'a' && c <= 'z') {
        c = static_cast<char>(c - 'a' + 'A');
    }
    if (c >= 'A' && c <= 'Z') {
        return 11 + (c - 'A');
    }
    return -1;
}

// Total advance of a string at the given per-glyph advance. Every
// character — drawn or skipped — consumes one slot, exactly like
// drawDigitString's index-based slots.
inline float textWidth(const std::string& text, float advance) {
    return static_cast<float>(text.size()) * advance;
}

// Left draws from the origin; Center shifts left by half the width.
inline float alignOffsetX(TextAlign align, float width) {
    return (align == TextAlign::Center) ? -width * 0.5f : 0.0f;
}

}  // namespace pe

#endif  // PUREENGINE_FONT_H
