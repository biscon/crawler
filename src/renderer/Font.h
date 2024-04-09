//
// Created by bison on 01-11-22.
//

#ifndef GAME_FONT_H
#define GAME_FONT_H


#include <defs.h>
#include <unordered_map>
#include "TextureAtlas.h"

extern "C" {
    #include <ft2build.h>
    #include FT_FREETYPE_H
};

namespace Renderer {
    typedef struct                  Glyph Glyph;

    struct Glyph {
        u32                         atlasId;
        u32                         advance;
        i32                         size[2];
        i32                         bearing[2];
    };

    typedef struct                  FT_FaceRec_* FT_Face;

    struct Font {
        FT_Face face;
        u32 size;
        TextureAtlas atlas;
        std::unordered_map<u32, Glyph> glyphs;
    };

    void InitFonts();
    void ShutdownFonts();
    void CreateFont(Font& font, const std::string& path, u32 size);
    void DestroyFont(Font& font);
    u32 MeasureTextWidth(const Font& font, const std::string& text);
}


#endif //GAME_FONT_H
