#include "TextureAnimation.h"

TextureSheet* TextureSheet::Create(const std::string& path, int numFrames)
{
    int width, height;

    GLuint tex = glsh::CreateTexture2D(path, false, &width, &height);
    if (!tex) {
        return NULL;
    }

    TextureSheet* texsheet = new TextureSheet();
    texsheet->mTex = tex;
    texsheet->mWidth = width;
    texsheet->mHeight = height;

    int frameWidth = width / numFrames;         // assumes frames are of equal width with no padding
    float uTexelWidth = 1.0f / width;
    float uStep = (frameWidth - 1) * uTexelWidth;

    float u1 = 0.5f * uTexelWidth;
    float u2 = u1 + uStep;

    for (int i = 0; i < numFrames; i++) {
        std::vector<glsh::VPT> rect(4);
        // FIXME: this code assumes that the frames are square
        rect[0].pos = glm::vec3(-0.5f, -0.5f, 0.0f);    rect[0].texcoord = glm::vec2(u1, 0.0f);
        rect[1].pos = glm::vec3( 0.5f, -0.5f, 0.0f);    rect[1].texcoord = glm::vec2(u2, 0.0f);
        rect[2].pos = glm::vec3(-0.5f,  0.5f, 0.0f);    rect[2].texcoord = glm::vec2(u1, 1.0f);
        rect[3].pos = glm::vec3( 0.5f,  0.5f, 0.0f);    rect[3].texcoord = glm::vec2(u2, 1.0f);
        texsheet->mFrames.push_back(rect);
        u1 = u2 + uTexelWidth;
        u2 = u1 + uStep;
    }

    return texsheet;
}
