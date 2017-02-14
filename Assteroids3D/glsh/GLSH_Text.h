// <barf>

#ifndef GLSH_TEXT_H_
#define GLSH_TEXT_H_

#include "GLSH_Texture.h"
#include "GLSH_Vertex.h"

#include <string>
#include <vector>


namespace glsh {

class Font {

    GLuint                  mTex;

    std::vector<TexRect>    mChars;     // indexed using ASCII codes

    float                   mHeight;    // useful when rendering multiple lines of text
    float                   mWidth;     // useful when rendering fixed-width text

public:
                            Font();
                            ~Font();

    bool                    Load(const std::string& name);
    void                    Unload();

    bool                    IsLoaded() const;

    GLuint                  getTex() const              { return mTex; }

    float                   getHeight() const           { return mHeight; }
    float                   getWidth() const            { return mWidth; }

    const TexRect&          getCharRect(int c) const    { return mChars[c]; }       // UNCHECKED

    bool                    hasChar(int c) const        { return c >= 0 && c < (int)mChars.size() && mChars[c].w != 0.0f && mChars[c].h != 0.0f; }
};


#if _WIN32
// fuck you Win32
#undef CreateFont
#endif

Font* CreateFont(const std::string& name);


class TextBatch {

    const Font*             mFont;
    std::vector<VPT>        mVerts;
    float                   mWidth, mHeight;

public:

                            TextBatch();

    void                    SetText(const Font* font, const std::string& text, bool fixedWidth = false);
    void                    SetText(const Font* font, const std::vector<std::string>& textLines);

    void                    Clear();

    const Font*             GetFont() const         { return mFont; }
    float                   GetWidth() const        { return mWidth; }
    float                   GetHeight() const       { return mHeight; }

    void                    DrawGeometry() const;
};

} // end of namespace

#endif

// </barf>
