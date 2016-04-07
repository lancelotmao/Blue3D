#ifndef houyi_ObjLoader_h
#define houyi_ObjLoader_h

#include "HouyiLoader.h"
#include "HouyiVector.h"
#include "HouyiScene.h"
#include "HouyiMesh.h"

namespace Houyi
{
    class ObjLoader : public Loader
    {
    public:
    	ObjLoader();
        virtual Scene* loadFromMemory(const char* data, int size);
    private:
        void loadMTLFile(const char* filePath);
        void clearBuffer();
        void addSurface(Mesh* mesh, SceneNode* node, const char* materialName);
        void converVertexIndex(vector<int> &data, int number, int preNumber);
    protected:
    	char mTempCharBuffer[1024][128];
    	int mTempIntBuffer[1024][1];

    	int iVertexNumber;
    	int currentiVertexNumber;
    	int iVertexTextureNumber;
    	int currentiVertexTextureNumber;
    	int iVertexNormalNumber;
    	int currentiVertexNormalNumber;

    	vector<float> vertextArray;
    	vector<float> vertextNormalArray;
    	vector<float> vertextTextureArray;
    	vector<int> vertextIndexArray;
    	vector<int> vertextNormalIndexArray;
    	vector<int> vertextTextureIndexArray;
    };
}
#endif
