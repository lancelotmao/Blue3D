#ifndef Houyi_CubeEnvMap_h
#define Houyi_CubeEnvMap_h

#include "HouyiSceneNode.h"

namespace Houyi
{
    class Scene;
    
    class CubeEnvMap : public SceneNode
    {
    public:
        CubeEnvMap(Scene* scene, const string px, const string py, const string pz,
                const string nx, const string ny, const string nz);
        CubeEnvMap(Scene* scene, const ImagePtr images[6]);
        ~CubeEnvMap();
        
        virtual void onRenderBegin(Pass* pass);
        virtual void onRenderEnd(Pass* pass);

    private:
        void init(Scene* scene);
        
    private:
        Texture* mCubeTexture;
        ImagePtr imgs[6];
    };
}
#endif
