//
//  HouyiDefaultSaver.cpp
//  Houyi
//
//  Created by Lance Mao on 5/6/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiDefaultSaver.h"
#include "HouyiScene.h"

namespace Houyi
{
    const int VERSION = 3;
    const bool DefaultSaver::DEBUG_SAVER = false;
    
    void DefaultSaver::saveScene(Scene* scene)
    {
        mScene = scene;
        DataOutputStream dos(mFileStream);

        saveHeader(scene);
        
        saveSkeletons(scene);
        
        saveMeshes(scene);
        
        saveCameras(scene);
        
        saveMaterials(scene);

        mNodeIndex = 0;
        saveSceneNode(scene->getRootSceneNode());
        
        saveLights(scene);
        
        saveRenderables(scene);
        
        // simple check sum
        dos.writeUTFString("valid");
    }
    
    void DefaultSaver::saveHeader(Scene* scene)
    {
        DataOutputStream dos(mFileStream);
        dos.writeInt(VERSION); // version number
        dos.writeBool(scene->isZUp());
        dos.write(&scene->getStats(), sizeof(scene->getStats()));
    }
    
    void DefaultSaver::saveSkeletons(Scene* scene)
    {
        DataOutputStream dos(mFileStream);
        int skeletonCount = scene->getSkeletonCount();
        if (DEBUG_SAVER)
            LOGD("skeletonCount = %d\n", skeletonCount);
        dos.writeInt(skeletonCount);
        for (int i = 0;i < skeletonCount;++i)
        {
            SkeletonPtr skeleton = scene->getSkeleton(i);
            skeleton->writeObject(dos);
        }
    }
    
    void DefaultSaver::saveMeshes(Scene* scene)
    {
        DataOutputStream dos(mFileStream);
        int meshCount = scene->getMeshCount();
        if (DEBUG_SAVER)
            LOGD("meshCount: %d\n", meshCount);
        dos.writeInt(meshCount);
        for (int i = 0;i < meshCount;++i)
        {
            Mesh* mesh = scene->getMesh(i);
            saveMesh(scene, mesh);
        }
    }
    
    void DefaultSaver::saveMesh(Scene* scene, Mesh* mesh)
    {
        DataOutputStream dos(mFileStream);
        
        bool hasVertex = mesh->getVertexBuffer() != 0;
        dos.writeBool(hasVertex);
        if (hasVertex)
        {
            saveVertexBuffer(mesh->getVertexBuffer());
        }
        
        int surfaceCount = mesh->getSurfaceCount();
        if (DEBUG_SAVER)
            LOGD("hasVertex: %d, surfaceCount = %d\n", hasVertex, surfaceCount);
        dos.writeInt(surfaceCount);
        for (int i = 0; i < surfaceCount;++i)
        {
            SurfacePtr s = mesh->getSurface(i);
            saveSurface(s);
        }
        
        int skeletonIndex = -1;
        for (int i = 0;i < scene->getSkeletonCount();++i)
        {
            if (mesh->getSkeletonRef().mSkeleton == scene->getSkeleton(i))
            {
                skeletonIndex = i;
                break;
            }
        }
        if (DEBUG_SAVER)
            LOGD("skeletonIndex = %d\n", skeletonIndex);
        dos.writeInt(skeletonIndex);
        if (skeletonIndex >= 0)
        {
            int indicesSize = mesh->getSkeletonRef().mIndices->size();
            dos.writeInt(indicesSize);
            dos.write(mesh->getSkeletonRef().mIndices->getData(), sizeof(int) * indicesSize);
            dos.write(&mesh->getBindShapeMatrix(), sizeof(Matrix4));
        }
    }
    
    void DefaultSaver::saveVertexBuffer(VertexBufferPtr vb)
    {
        if (!vb)
        {
            return;
        }
        
        DataOutputStream dos(mFileStream);
        dos.writeBool(vb->useVBO());
        dos.writeInt(vb->getVertexComponentSize());
        dos.writeInt(vb->getPositionLength());
        dos.writeFloatArray(vb->getPosition(), 0, vb->getPositionLength());
        
        // normal
        int normalLength = vb->getNormalLength();
        dos.writeInt(normalLength);
        if (normalLength > 0)
        {
            dos.writeFloatArray(vb->getNormal(), 0, normalLength);
        }
        
        // texture coordinate
        int texcoordLength = vb->getTexcoordLength();
        dos.writeInt(texcoordLength);
        if (texcoordLength > 0)
        {
            dos.writeFloatArray(vb->getTexcoord(), 0, texcoordLength);
        }
        
        // bone indices
        int jointIndexLength = vb->getJointIndexLength();
        dos.writeInt(jointIndexLength);
        if (jointIndexLength > 0)
        {
            dos.writeFloatArray(vb->getJointIndex(), 0, jointIndexLength);
        }
        
        // bone weight
        int jointWeightLength = vb->getJointWeightLength();
        dos.writeInt(jointWeightLength);
        if (jointWeightLength > 0)
        {
            dos.writeFloatArray(vb->getJointWeight(), 0, jointWeightLength);
        }
    }
    
    void DefaultSaver::saveSurface(SurfacePtr surface)
    {
        DataOutputStream dos(mFileStream);
        VertexBufferPtr rawVB = surface->getRawVertexBuffer();
        bool hasVertex = rawVB != 0;
        dos.writeBool(hasVertex);
        if (DEBUG_SAVER)
            LOGD("surface hasVertex: %d\n", hasVertex);
        if (hasVertex)
        {
            saveVertexBuffer(rawVB);
        }
        
        IndexBufferPtr ib = surface->getIndexBuffer();
        if (VERSION >= 3)
        {
            dos.writeBool(ib);
        }
        if (ib)
        {
            saveIndexBuffer(ib);
        }
    }
    
    void DefaultSaver::saveIndexBuffer(IndexBufferPtr ib)
    {
        DataOutputStream dos(mFileStream);
        int lineSize = ib->getLineIndexSize();
        dos.writeInt(lineSize);
        int lineLength = ib->getLineIndicesLength();
        dos.writeInt(lineLength);
        if (lineSize == 4)
            dos.writeUIntArray(ib->getIntLineIndices(), 0, lineLength);
        else
            dos.writeUShortArray(ib->getShortLineIndices(), 0, lineLength);
        
        int triangleSize = ib->getTriangleIndexSize();
        dos.writeInt(triangleSize);
        int triangleLength = ib->getTriangleIndicesLength();
        dos.writeInt(triangleLength);
        if (triangleSize == 4)
            dos.writeUIntArray(ib->getIntTriangleIndices(), 0, triangleLength);
        else
            dos.writeUShortArray(ib->getShortTriangleIndices(), 0, triangleLength);
    }
    
    void DefaultSaver::saveCameras(Scene* scene)
    {
        DataOutputStream dos(mFileStream);
        int cameraCount = scene->getCameraCount();
        if (DEBUG_SAVER)
            LOGD("cameraCount: %d\n", cameraCount);
        dos.writeInt(cameraCount);
        for (int i = 0;i < cameraCount;++i)
        {
            Camera* camera = scene->getCamera(i);
            dos.writeInt(camera->getType());
            camera->writeObject(dos);
        }
    }
    
    void DefaultSaver::saveMaterials(Scene* scene)
    {
        DataOutputStream dos(mFileStream);
        int materialCount = scene->getMaterialManager()->getMaterialCount();
        if (DEBUG_SAVER)
            LOGD("materialCount = %d\n", materialCount);
        dos.writeInt(materialCount);
        for (int i = 0;i < materialCount;++i)
        {
            Material* material = scene->getMaterialManager()->getMaterial(i);
            material->writeObject(dos);
            dos.writeInt(material->getTextureCount());
            for (int j = 0;j < material->getTextureCount();++j)
            {
                dos.writeUTFString(material->getTexture(j)->getName());
            }
        }
    }
    
    void DefaultSaver::saveSceneNode(SceneNode* node)
    {
        DataOutputStream dos(mFileStream);
        
        // name
        dos.writeUTFString(node->getName());
        
        // children count
        dos.writeInt(node->childrenCount());
        if (DEBUG_SAVER)
            LOGD("scene node child count: %d\n", node->childrenCount());
        for (int i = 0;i < node->childrenCount();++i)
        {
            SceneNode* child = static_cast<SceneNode*>(node->getChildAt(i));
            saveSceneNode(child);
        }
        
        // scene node index. used to locate scene node for entity
        node->setId(mNodeIndex);
        dos.writeInt(mNodeIndex);
        if (DEBUG_SAVER)
            LOGD("scene node index: %d\n", mNodeIndex);
        
        // mesh index
        int meshIndex = -1;
        if (node->getEntityCount() > 0)
        {
            for (int i = 0;i < mScene->getMeshCount();++i)
            {
                Mesh* mesh = mScene->getMesh(i);
                if (node->getMesh() == mesh)
                {
                    meshIndex = i;
                    break;
                }
            }
        }
        dos.writeInt(meshIndex);
        if (DEBUG_SAVER)
            LOGD("mesh index: %d\n", meshIndex);
        
        // transform
        dos.write(&node->getLocal(), sizeof(Matrix4));
        
        // key frames requires version 2 and above
        if (VERSION >= 2)
        {
            int kfcnt = node->getKFCount();
            dos.writeInt(kfcnt);
            for (int i = 0;i < kfcnt;++i)
            {
                KeyFrame& kf = node->getKFs()[i];
                kf.writeObject(dos);
            }
        }
        
        mNodeIndex++;
    }
    
    void DefaultSaver::saveLights(Scene* scene)
    {
        DataOutputStream dos(mFileStream);
        int lightCount = scene->getLightCount();
        if (DEBUG_SAVER)
            LOGD("lightCount = %d\n", lightCount);
        dos.writeInt(lightCount);
        for (int i = 0;i < lightCount;++i)
        {
            Light* light = scene->getLight(i);
            light->writeObject(dos);
        }
    }
    
    void DefaultSaver::saveRenderables(Scene* scene)
    {
        DataOutputStream dos(mFileStream);
        int entityCount = scene->getRenderableCount();
        if (DEBUG_SAVER)
            LOGD("entityCount = %d\n", entityCount);
        dos.writeInt(entityCount);
        for (int i = 0;i < entityCount;++i)
        {
            const RenderEntity& entity = scene->getRenderable(i);
            int sid = getEntitySurfaceIndex(entity, scene);
            dos.writeInt(sid);
            
            int nodeId = entity.mNode->getId();
            dos.writeInt(nodeId);
            
            string matName = entity.mMaterial->getName();
            dos.writeUTFString(matName);
            if (DEBUG_SAVER)
                LOGD("sid = %d, nodeId = %d, material = %s\n", sid, nodeId, matName.c_str());
        }
    }
    
    int DefaultSaver::getEntitySurfaceIndex(const RenderEntity& entity, Scene* scene)
    {
        for (int j = 0;j < scene->getMeshCount();++j)
        {
            Mesh* m = scene->getMesh(j);
            for (int k = 0;k < m->getSurfaceCount();++k)
            {
                SurfacePtr s = m->getSurface(k);
                if (s == entity.mSurface)
                {
                    return k;
                }
            }
        }
        return -1;
    }

    int DefaultSaver::getMaterialIndex(const RenderEntity& entity, Scene* scene)
    {
        MaterialManager* matMan = scene->getMaterialManager();
        Material* mat = entity.mMaterial;
        if (mat == &matMan->getDefMaterial())
        {
            return 0;
        }
        else if (mat == &matMan->getWireFrame())
        {
            return -1;
        }

        for (int i = 0;i < matMan->getMaterialCount();++i)
        {
            Material* m = matMan->getMaterial(i);
            if (m == mat)
            {
                return i;
            }
        }
        return 0;
    }
}
