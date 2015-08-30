#ifndef Houyi_CubeEnvMap_h
#define Houyi_CubeEnvMap_h

#include "HouyiSceneNode.h"

namespace Houyi
{
    class CubeEnvMap : public SceneNode
    {
    public:
        CubeEnvMap(const string px, const string py, const string pz,
                const string nx, const string ny, const string nz);
        ~CubeEnvMap();
        
        virtual void onRenderBegin(Pass* pass);
        virtual void onRenderEnd(Pass* pass);

    private:
        Texture* mCubeTexture;
    };
}
#endif
