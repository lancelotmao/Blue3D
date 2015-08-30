#ifndef HouyiStats_H
#define HouyiStats_H

namespace Houyi
{
    struct SceneStats
    {
        SceneStats() : mRenderableCount(0), mVertexCount(0), mLineCount(0), mTriangleCount(0)
        {
        }

        void print()
        {
            LOGI("vertex count: %d\n", mVertexCount);
            LOGI("line count: %d\n", mLineCount);
            LOGI("triangle count: %d\n", mTriangleCount);
            LOGI("renderable count: %d\n", mRenderableCount);
        }
        int mRenderableCount;
        int mVertexCount;
        int mLineCount;
        int mTriangleCount;
    };
}
#endif
