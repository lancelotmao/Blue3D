#ifndef Houyi_HouyiSceneOptimizer_h
#define Houyi_HouyiSceneOptimizer_h

#include "HouyiObject.h"
#include "HouyiLoader.h"
#include "HouyiVertexBuffer.h"

namespace Houyi
{
    class Scene;
    class Mesh;

    class SceneOptimizer : Object
    {
    public:
        // merges as many mesh as possible to reduce draw calls
        static void optimize(Scene* scene, Loader* loader);
        
        // make sure all meshes have valid vertex buffer and surfaces have index buffer
        static void prepareSceneMesh(Scene* scene, Loader* loader);

    private:
        static VertexBufferPtr generateVertexBuffer(vector<VertexData>& vData, int flag);
    };
}
#endif
