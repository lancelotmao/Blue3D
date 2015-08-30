//
//  HouyiDefaultSaver.h
//  Houyi
//
//  Created by Lance Mao on 5/6/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiDefaultSaver_h
#define Houyi_HouyiDefaultSaver_h

#include "HouyiSaver.h"
#include "HouyiVertexBuffer.h"
#include "HouyiSurface.h"
#include "HouyiRenderEntity.h"

namespace Houyi
{
    class Mesh;
    class SceneNode;
    class DefaultSaver : public Saver
    {
    public:
        virtual void saveScene(Scene* scene);
        
    private:
        void saveHeader(Scene* scene);
        void saveSkeletons(Scene* scene);
        void saveMeshes(Scene* scene);
        void saveMesh(Scene* scene, Mesh* mesh);
        void saveVertexBuffer(VertexBufferPtr vb);
        void saveSurface(SurfacePtr surface);
        void saveIndexBuffer(IndexBufferPtr ib);
        void saveCameras(Scene* scene);
        void saveMaterials(Scene* scene);
        void saveSceneNode(SceneNode* node);
        
        // lights must be saved after scene node
        void saveLights(Scene* scene);
        
        void saveRenderables(Scene* scene);
        
        int getEntitySurfaceIndex(const RenderEntity& entity, Scene* scene);
        int getMaterialIndex(const RenderEntity& entity, Scene* scene);
        
    private:
        static const bool DEBUG_SAVER;
        int mNodeIndex;
        Scene* mScene;
    };
}

#endif
