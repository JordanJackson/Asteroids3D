#ifndef TEXTURE_ANIMATION_H_
#define TEXTURE_ANIMATION_H_

#include "GLSH.h"

class TextureSheet
{
    GLuint mTex;

    int mWidth, mHeight;

    std::vector<std::vector<glsh::VPT>> mFrames;        // each frame is a VPT quad

    // private constructor, use Create method to instantiate
    TextureSheet()
        : mTex(0)
        , mWidth(0)
        , mHeight(0)
    { }

public:

    static TextureSheet* Create(const std::string& path, int numFrames);

    GLuint GetHandle() const
    {
        return mTex;
    }

    int NumFrames() const
    {
        return (int)mFrames.size();
    }

    const std::vector<glsh::VPT>& GetFrameRect(int frameNo) const
    {
        // assumes there's at least one frame
        if (frameNo <= 0) {
            return mFrames.front();
        } else if (frameNo >= NumFrames()) {
            return mFrames.back();
        } else {
            return mFrames[frameNo];
        }
    }

    void DrawFrame(int frameNo) const
    {
        glBindTexture(GL_TEXTURE_2D, mTex);
        glsh::DrawGeometry(GL_TRIANGLE_STRIP, GetFrameRect(frameNo));
    }
};


class AnimatedEffect
{
    const TextureSheet* mTextureSheet;

    float mDuration;
    float mTime;

public:
    glm::vec2 mPos;
    float mAngle;

    AnimatedEffect(const TextureSheet* texsheet, float duration, const glm::vec2& pos, float angle)
        : mTextureSheet(texsheet)
        , mDuration(duration)
        , mTime(0.0f)
        , mPos(pos)
        , mAngle(angle)
    { }

    void AddTime(float dt)
    {
        mTime += dt;
    }

    bool FinishedPlaying() const
    {
        return mTime >= mDuration;
    }

    void DrawCurrentFrame() const
    {
        int numCells = mTextureSheet->NumFrames();
        int cellIndex;
        if (mTime <= 0.0f) {
            cellIndex = 0;
        } else if (mTime >= mDuration) {
            cellIndex = numCells - 1;
        } else {
            cellIndex = (int)(mTime / mDuration * numCells);
        }

        mTextureSheet->DrawFrame(cellIndex);
    }
};

#endif
