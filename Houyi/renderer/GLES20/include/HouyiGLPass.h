#ifndef __Houyi__GLPass__
#define __Houyi__GLPass__

#include "HouyiPass.h"
#include <map>

namespace Houyi
{
    class GLPass : public Pass
    {
    public:
        GLPass(int program);
        GLPass(const char* vertex, const char* fragment);
        virtual ~GLPass();

        virtual void beginPass();
        virtual void endPass();
        virtual void uploadMaterial(Material* material, MaterialManager* matMan);
        virtual void invalidate();
        
        virtual int getAtribLocation(string key);
        virtual int getUniformLocation(string key);
        virtual void uploadWorldMat(const Matrix4& m);
        virtual void uploadNormalMat(const Matrix3& m);
        virtual void uploadEyeMat(const Matrix4& m);
        virtual void uploadProjectionMat(const Matrix4& m);
        virtual void uploadMatrix4(const string& uniformName, const Matrix4& m, bool transpose = false);
        virtual void uploadMatrix3(const string& uniformName, const Matrix3& m, bool transpose = false);

        static Pass* createPass(int shaderId);

    protected:
        virtual void onRender(Renderer* renderer, Scene* scene);

        // TODO investigate multi-texture
        virtual void uploadTexture(const Texture* texture, const RenderState& renderState, int index = 0);

        void init();

    protected:
        map<string, int> mAttribMap;
        map<string, int> mUniformMap;
    };
}

#endif
