//
//  HouyiDefaultLoader.cpp
//  Houyi
//
//  Created by Lance Mao on 6/6/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiDefaultLoader.h"
#include "HouyiScene.h"
#include "HouyiLog.h"
#include "HouyiTextureManager.h"
#include "HouyiAABB.h"
#include "HouyiModelViewCamera.h"
#include "HouyiFPCamera.h"
#include "HouyiStringUtils.h"

namespace Houyi
{
    const bool DefaultLoader::DEBUG_LOADER = false;
    
    Scene* DefaultLoader::loadFromMemory(const char* data, int size)
    {
        DataInputStream dis(data, size);
        
        loadHeader(dis);
        
        loadSkeletons(dis);
        
        loadMeshes(dis);

        loadCameras(dis);
        
        loadMaterials(dis);
        
        loadSceneNodes(dis);
        
        loadLights(dis);
        
        loadRenderables(dis);
        
        // simple check sum
        string s = dis.readUTFString();
        if (DEBUG_LOADER)
            LOGD("last saved string = %s\n", s.c_str());
        
        return mScene;
    }
    
    void DefaultLoader::loadHeader(DataInputStream& dis)
    {
        mVersion = dis.readInt();
        LOGD(".houyi file version: %d\n", mVersion);
        bool zup = dis.readBool();
        if (zup)
            mScene->setZAsUpAxis();
        dis.read(&mScene->getStats(), sizeof(mScene->getStats()));
    }
    
    void DefaultLoader::loadSkeletons(DataInputStream& dis)
    {
        int skeletonCount = dis.readInt();
        if (DEBUG_LOADER)
            LOGD("skeletonCount = %d\n", skeletonCount);
        for (int i = 0;i < skeletonCount;++i)
        {
            SkeletonPtr skeleton = new Skeleton();
            skeleton->readObject(dis);
            mScene->addSkeleton(skeleton);
        }
    }
    
    void DefaultLoader::loadMeshes(DataInputStream& dis)
    {
        int meshCount = dis.readInt();
        if (DEBUG_LOADER)
            LOGD("meshCount = %d\n", meshCount);
        for (int i = 0;i < meshCount;++i)
        {
            Mesh* mesh = loadMesh(dis);
            if (mesh)
            {
                mScene->addMesh(mesh);
            }
        }
    }
    
    void DefaultLoader::loadCameras(DataInputStream& dis)
    {
        int cameraCount = dis.readInt();
        if (DEBUG_LOADER)
            LOGD("cameraCount = %d\n", cameraCount);
        for (int i = 0;i < cameraCount;++i)
        {
            int type = dis.readInt();
            Camera* camera = 0;
            if (type == Const::EModelView)
            {
                camera = HouyiNew ModelViewCamera();
            }
            else if(type == Const::EFirstPerson)
            {
            	FPCamera* fpCamera = HouyiNew FPCamera();
            	fpCamera->addVirtualPan();
            	camera = fpCamera;
            }
            else
            {
                camera = HouyiNew Camera();
            }
            camera->readObject(dis);
            camera->lookAt();
            camera->project();
            mScene->addCamera(camera);
        }
    }
    
    void DefaultLoader::loadMaterials(DataInputStream& dis)
    {
        int materialCount = dis.readInt();
        if (DEBUG_LOADER)
            LOGD("materialCount = %d\n", materialCount);
        for (int i = 0;i < materialCount;++i)
        {
            Material* material = HouyiNew Material();
            material->readObject(dis);
            int textureSize = dis.readInt();
            for (int j = 0;j < textureSize;++j)
            {
                string texname = dis.readUTFString();
                string paths[3];
                paths[0] = StringUtils::getLastPathComponent(texname);
                paths[1] = StringUtils::getPath(mFilePath) + StringUtils::getLastPathComponent(texname);
                paths[2] = StringUtils::getTexturePath(mFilePath, texname);
                Texture* texture = material->createTextureByPath(mScene, paths);
                if (texture)
                {
                    material->addTexture(texture);
                }
            }
            mScene->getMaterialManager()->addMaterial(material);
        }
    }
    
    void DefaultLoader::loadSceneNodes(DataInputStream& dis)
    {
        mScene->setRootSceneNode(loadSceneNode(dis));
    }
    
    void DefaultLoader::loadLights(DataInputStream& dis)
    {
        int lightCount = dis.readInt();
        if (DEBUG_LOADER)
            LOGD("lightCount = %d\n", lightCount);
        for (int i = 0;i < lightCount;++i)
        {
            Light* light = HouyiNew Light();
            light->readObject(dis);
            mScene->addLight(light);
        }
    }
    
    void DefaultLoader::loadRenderables(DataInputStream& dis)
    {
        int entityCount = dis.readInt();
        if (DEBUG_LOADER)
            LOGD("entityCount = %d\n", entityCount);
        for (int i = 0;i < entityCount;++i)
        {
            int sid = dis.readInt();
            int nodeId = dis.readInt();
            string matName = dis.readUTFString();
            if (DEBUG_LOADER)
                LOGD("surface id = %d, nodeId = %d, material = %s\n", sid, nodeId, matName.c_str());
            SceneNode* node = mScene->findSceneNodeById(nodeId);
            if (node && sid >= 0)
            {
                Mesh* mesh = node->getMesh();
                SurfacePtr s;
                if (mesh)
                {
                    s = mesh->getSurface(sid);
                }
                
                // find material
                Material* mat = mScene->getMaterialManager()->getMaterial(matName);
                if (!mat)
                {
                    mat = &mScene->getMaterialManager()->getDefMaterial();
                }
                
                RenderEntity entity(node, s, mat);
                node->addEntity(entity);
                mScene->addRenderEntity(entity);
            }
        }
    }
    
    Mesh* DefaultLoader::loadMesh(DataInputStream& dis)
    {
        Mesh* mesh = HouyiNew Mesh();
        bool hasVertex = dis.readByte() != 0;
        if (hasVertex)
        {
            VertexBufferPtr vb = readVertexBuffer(dis);
            // TODO change hasVertex to 0, 1, 2 to indicate no vertex, compact or expanded
            mesh->setCompactVertexBuffer(vb);
        }
        
        int surfaceCount = dis.readInt();
        if (DEBUG_LOADER)
            LOGD("surfaceCount = %d\n", surfaceCount);
        for (int i = 0;i < surfaceCount;++i)
        {
            SurfacePtr s = readSurface(dis);
            mesh->addSurface(s);
        }
        
        int skeletonIndex = dis.readInt();
        if (DEBUG_LOADER)
            LOGD("skeletonIndex = %d\n", skeletonIndex);
        if (skeletonIndex >= 0)
        {
            SkeletonPtr skeleton = mScene->getSkeleton(skeletonIndex);
            SkeletonRef skeletonRef(skeleton);
            int indicesSize = dis.readInt();
            ArrayPtr<int> indices = new Array<int>(indicesSize);
            dis.read(indices->getData(), sizeof(int) * indicesSize);
            skeletonRef.mIndices = indices;
            mesh->setSkeletonRef(skeletonRef);
            Matrix4 bindShape;
            dis.read(&bindShape, sizeof(Matrix4));
            mesh->setBindShapeMatrix(bindShape);
        }
        
        return mesh;
    }
    
    VertexBufferPtr DefaultLoader::readVertexBuffer(DataInputStream& dis)
    {
        VertexBufferPtr vb = new VertexBuffer();
        vb->setUseVBO(dis.readBool());
        int componentSize = dis.readInt();
        if (DEBUG_LOADER) LOGD("componentSize = %d\n", componentSize);
        vb->setVertexComponentSize(componentSize);
        
        int posLength = dis.readInt();
        if (DEBUG_LOADER) LOGD("posLength = %d\n", posLength);
        float* pos = new float[posLength];
        dis.read(pos, sizeof(float) * posLength);
        vb->setPosition(pos, posLength);
        
        int normalLength = dis.readInt();
        if(normalLength > 0)
        {
            if (DEBUG_LOADER) LOGD("normalLength = %d\n", normalLength);
            float* normal = new float[normalLength];
            dis.read(normal, sizeof(float) * normalLength);
            vb->setNormal(normal, normalLength);
        }
        
        int texcoordLength = dis.readInt();
        if (texcoordLength > 0)
        {
            if (DEBUG_LOADER) LOGD("texcoordLength = %d\n", texcoordLength);
            float* texcoord = new float[texcoordLength];
            dis.read(texcoord, sizeof(float) * texcoordLength);
            vb->setTexcoord(texcoord, texcoordLength);
        }
        
        int jointIndexLength = dis.readInt();
        if (jointIndexLength > 0)
        {
            if (DEBUG_LOADER) LOGD("jointIndexLength = %d\n", jointIndexLength);
            float* jointIndex = new float[jointIndexLength];
            dis.read(jointIndex, sizeof(float) * jointIndexLength);
            vb->setJointIndex(jointIndex, jointIndexLength);
        }
        
        int jointWeightLength = dis.readInt();
        if (jointWeightLength > 0)
        {
            if (DEBUG_LOADER) LOGD("jointWeightLength = %d\n", jointWeightLength);
            float* jointWeight = new float[jointWeightLength];
            dis.read(jointWeight, sizeof(float) * jointWeightLength);
            vb->setJointWeight(jointWeight, jointWeightLength);
        }
        
        return vb;
    }
    
    SurfacePtr DefaultLoader::readSurface(DataInputStream& dis)
    {
        SurfacePtr s = new Surface();
        bool hasRawVertex = dis.readByte() != 0;
        if (hasRawVertex)
        {
            VertexBufferPtr vb = readVertexBuffer(dis);
            s->setRawVertexBuffer(vb);
        }
        bool hasIndexBuffer = mVersion >= 3 ? dis.readBool() : true;
        if (hasIndexBuffer)
        {
            IndexBufferPtr ib = readIndexBuffer(dis);
            s->setIndexBuffer(ib);
            if (ib->getLineIndicesLength() > 0 && ib->getTriangleIndicesLength() == 0)
                s->setRenderMode(Const::ERenderModeLine);
        }
        return s;
    }
    
    IndexBufferPtr DefaultLoader::readIndexBuffer(DataInputStream& dis)
    {
        IndexBufferPtr ib = new IndexBuffer();
        int lineSize = dis.readInt();
        int lineLength = dis.readInt();
        if (DEBUG_LOADER)
            LOGD("lineSize = %d, lineLength = %d\n", lineSize, lineLength);
        if (lineLength > 0)
        {
            if (lineSize == 2)
            {
                ushort* indices = new ushort[lineLength];
                dis.read(indices, sizeof(ushort) * lineLength);
                ib->setLineIndices(indices, lineLength);
            }
            else
            {
                uint* indices = new uint[lineLength];
                dis.read(indices, sizeof(uint) * lineLength);
                ib->setLineIndices(indices, lineLength);
            }
        }
        
        int triangleSize = dis.readInt();
        int triangleLength = dis.readInt();
        if (DEBUG_LOADER)
            LOGD("triangleSize = %d, triangleLength = %d\n", triangleSize, triangleLength);
        if (triangleLength > 0)
        {
            if (triangleSize == 2)
            {
                ushort* indices = new ushort[triangleLength];
                dis.read(indices, sizeof(ushort) * triangleLength);
                ib->setTriangleIndices(indices, triangleLength);
            }
            else
            {
                uint* indices = new uint[triangleLength];
                dis.read(indices, sizeof(uint) * triangleLength);
                ib->setTriangleIndices(indices, triangleLength);
            }
        }
        
        return ib;
    }
    
    SceneNode* DefaultLoader::loadSceneNode(DataInputStream& dis)
    {
        SceneNode* node = 0;
        string name = dis.readUTFString();
        if (name.compare("AABB") == 0)
        {
            node = HouyiNew AABB(mScene);
            node->setVisibility(SceneNode::EInvisible);
        }
        else
        {
            node = HouyiNew SceneNode();
        }
        node->setName(name);
        
        int childCount = dis.readInt();
        if (DEBUG_LOADER)
            LOGD("node child count = %d\n", childCount);
        for (int i = 0;i < childCount;++i)
        {
            SceneNode* child = loadSceneNode(dis);
            if (child->getName().compare("AABB") == 0)
            {
                if (loadAABB())
                {
                    node->setAABB((AABB*)child);
                }
                else
                {
                    HouyiDelete(child);
                    child = 0;
                }
            }
            if (child)
            {
                node->addChild(child);
            }
        }
        
        int index = dis.readInt();
        if (DEBUG_LOADER)
            LOGD("scene node index = %d\n", index);
        node->setId(index);
        
        int meshIndex = dis.readInt();
        if (DEBUG_LOADER)
            LOGD("meshIndex = %d\n", meshIndex);
        if (meshIndex >= 0 && meshIndex < mScene->getMeshCount())
        {
            Mesh* mesh = mScene->getMesh(meshIndex);
            node->setMesh(mesh);
        }
        else
            node->setMesh(0);
        
        dis.read(&node->getLocal(), sizeof(Matrix4));
        
        // key frame data requires version 2 and above
        if (mVersion >= 2)
        {
            int kfcnt = dis.readInt();
            if (kfcnt > 0)
            {
                KeyFrame* kfs = new KeyFrame[kfcnt];
                for (int i = 0;i < kfcnt;++i)
                {
                    kfs[i].readObject(dis);
                }
                node->setKFs(kfs, kfcnt);
            }
        }
        
        return node;
    }
}
