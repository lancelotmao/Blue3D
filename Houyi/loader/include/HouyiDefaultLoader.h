//
//  HouyiDefaultLoader.h
//  Houyi
//
//  Created by Lance Mao on 6/6/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiDefaultLoader_h
#define Houyi_HouyiDefaultLoader_h

#include "HouyiLoader.h"
#include "HouyiDataInputStream.h"
#include "HouyiMesh.h"

namespace Houyi
{
    class SceneNode;
    class DefaultLoader : public Loader
    {
    public:
        virtual Scene* loadFromMemory(const char* data, int size);
        
    private:
        static const bool DEBUG_LOADER;
        
        void loadHeader(DataInputStream& dis);
        void loadSkeletons(DataInputStream& dis);
        void loadMeshes(DataInputStream& dis);
        void loadCameras(DataInputStream& dis);
        void loadMaterials(DataInputStream& dis);
        void loadSceneNodes(DataInputStream& dis);
        void loadLights(DataInputStream& dis);
        void loadRenderables(DataInputStream& dis);
        
        Mesh* loadMesh(DataInputStream& dis);
        VertexBufferPtr readVertexBuffer(DataInputStream& dis);
        SurfacePtr readSurface(DataInputStream& dis);
        IndexBufferPtr readIndexBuffer(DataInputStream& dis);
        SceneNode* loadSceneNode(DataInputStream& dis);
        
    private:
        int mVersion;
    };
}

#endif
