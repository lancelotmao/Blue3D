#ifndef houyi_3dsLoader_h
#define houyi_3dsLoader_h

#include "HouyiLoader.h"
#include "HouyiDataInputStream.h"
#include "HouyiMaterial.h"
#include "HouyiCamera.h"
#include "HouyiLight.h"

namespace Houyi
{
    class Houyi3dsLoader : public Loader
    {
    public:
        virtual ~Houyi3dsLoader();
        virtual Scene* loadFromMemory(const char* data, int size);

    private:
        // mUseFaceNormal indicates that we stick to face normal for vertices.
        // for those vertices that are shared between faces we expand(duplicate)
        // them and make sure every vertices` s normal is corresponding
        // to the face normal that it belongs to. This is very important for
        // cube because for example if a cube has only 8 vertices we
        // can not use vertex normal average approach
        bool mUseFaceNormal;

        void load3DSFile(DataInputStream* in, int length);
        Material* loadMaterial(DataInputStream* in, int length);

        void loadMaterialMap(Material* mat, DataInputStream* inputStream, int length);
        void loadNamedObject(DataInputStream* inputStream, int length);
        Mesh* loadNamedTriangleObject(DataInputStream* inputStream, int length, string name);
        Camera* loadCamera(DataInputStream* inputStream, int length);
        Light* loadLight(DataInputStream* inputStream, int length);

        // key frame
        void loadKeyFrame(DataInputStream* inputStream, int length);
        void loadObjectNodeTag(DataInputStream* inputStream, int length);
        void loadCameraNodeTag(DataInputStream* inputStream, int length);
        void loadTargetNodeTag(DataInputStream* inputStream, int length);
        void loadLightNodeTag(DataInputStream* inputStream, int length);

        // util
        string load3DSString(DataInputStream* inputStream);
        int loadColor(DataInputStream* inputStream, int length);
        float loadPercentage(DataInputStream* inputStream, int length);
        SceneNode* findNodeByName(const string& name);

    private:
        vector<SceneNode*> mNodes;
    };
}
#endif
