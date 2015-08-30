#include "HouyiSceneOptimizer.h"
#include "HouyiMeshOptimizer.h"
#include "HouyiScene.h"
#include "HouyiUtil.h"
#include "HouyiSettings.h"
#include "HouyiSceneManager.h"
#include <map>

namespace Houyi
{
    void SceneOptimizer::optimize(Scene* scene, Loader* loader)
    {
        // first move mesh to separate vector
        vector<Mesh*> unprepared;
        for (int m = 0;m < scene->getMeshCount();++m)
        {
            unprepared.push_back(scene->getMesh(m));
        }
        // start with empty mesh
        scene->clearMeshes();
        
        scene->getRootSceneNode()->generateAABB(scene);
        
        SceneStats& stats = scene->getStats();
        stats = SceneStats(); // clear

        int vBeforeTotal = 0;
        
        // merge all static meshes into a single mesh
        map<uint, uint> vStaticMap;
        vector<VertexData> vStaticData;
        map<uint, uint> wfStaticMap;
        
        int staticFlag = 0; // vertex attribute flag for static mesh
        Mesh* mergedMesh = HouyiNew Mesh();

        // dynamics is added separately without surface merging
        // TODO dynamics should also merge surface
        vector<Mesh*> dynamics;

        // delete mesh for static data
        vector<Mesh*> dead;

        for (int m = 0;m < unprepared.size();++m)
        {
            Mesh* mesh = unprepared[m];
            
            map<uint, uint> vMapDynamic;
            vector<VertexData> vDataDynamic;

            bool isDynamic = mesh->getSkeletonRef();
            map<uint, uint>& vMap = isDynamic ? vMapDynamic : vStaticMap;
            vector<VertexData>& vData = isDynamic ? vDataDynamic : vStaticData;
            int flag = 0;
            if (isDynamic)
            {
                dynamics.push_back(mesh);
            }

            int beforeCnt = 0;
            for (int i = 0;i < mesh->getSurfaceCount();++i)
            {
                map<uint, uint> wfMapDynamic; // per surface
                map<uint, uint>& wfMap = isDynamic ? wfMapDynamic : wfStaticMap;
                int wfOffset = isDynamic ? 0 : wfStaticMap.size(); // start from current map size for static
                SurfacePtr surface = mesh->getSurface(i);
                VertexBufferPtr vb = surface->getRawVertexBuffer();
                IndexBufferPtr ib = surface->getIndexBuffer();
                int vcnt = ib ? ib->getTriangleIndicesLength() : vb->getPositionLength() / 3;
                beforeCnt += vcnt;
                float* pos = vb->getPosition();
                float* normal = vb->getNormal();
                float* texcoord = vb->getTexcoord();
                float* boneIndex = vb->getJointIndex();
                float* boneWeight = vb->getJointWeight();
                float* tangent = vb->getTangent();
                float* binormal = vb->getBinormal();
                uint* indices = new uint[vcnt];
                uint* rawIndices = ib ? ib->getIntTriangleIndices() : 0;
                for (int j = 0;j < vcnt;++j)
                {
                    int idx = rawIndices ? rawIndices[j] : j;
                    float x = pos[idx * 3];
                    float y = pos[idx * 3 + 1];
                    float z = pos[idx * 3 + 2];
                    VertexData data;
                    data.position = Vector3(x, y, z);
                    if (normal)
                    {
                        data.normal = Vector3(normal[idx * 3], normal[idx * 3 + 1], normal[idx * 3 + 2]);
                        flag |= VertexBuffer::Attribute_Normal;
                    }
                    if (texcoord)
                    {
                        data.texcoord = Vector2(texcoord[idx * 2], texcoord[idx * 2 + 1]);
                        flag |= VertexBuffer::Attribute_Texcoord;
                    }
                    if (boneIndex)
                    {
                        data.boneIndex = Vector4(boneIndex[idx * 4], boneIndex[idx * 4 + 1], boneIndex[idx * 4 + 2], boneIndex[idx * 4 + 3]);
                        flag |= VertexBuffer::Attribute_Skeleton;
                    }
                    if (boneWeight)
                    {
                        data.boneWeight = Vector4(boneWeight[idx * 4], boneWeight[idx * 4 + 1], boneWeight[idx * 4 + 2], boneWeight[idx * 4 + 3]);
                    }
                    if (tangent)
                    {
                        data.tangent = Vector3(tangent[idx * 3], tangent[idx * 3 + 1], tangent[idx * 3 + 2]);
                        flag |= VertexBuffer::Attribute_Tangent;
                    }
                    if (binormal)
                    {
                        data.binormal = Vector3(binormal[idx * 3], binormal[idx * 3 + 1], binormal[idx * 3 + 2]);
                        flag |= VertexBuffer::Attribute_Binormal;
                    }
                    
                    uint hash = Util::hashVertex(data);
                    map<uint, uint>::iterator it = vMap.find(hash);
                    if (it != vMap.end())
                    {
                        uint index = it->second;
                        VertexData& dataInMap = vData[index];
                        if (data != dataInMap)
                        {
                            indices[j] = vData.size();
                            vMap.insert(pair<uint, uint>(hash, indices[j]));
                            vData.push_back(data);
                            wfMap.insert(pair<uint, uint>(j + wfOffset, indices[j]));
                        }
                        else
                        {
                            indices[j] = index;
                            wfMap.insert(pair<uint, uint>(j + wfOffset, index));
                        }
                    }
                    else
                    {
                        indices[j] = vData.size();
                        vMap.insert(pair<uint, uint>(hash, indices[j]));
                        vData.push_back(data);
                        wfMap.insert(pair<uint, uint>(j + wfOffset, indices[j]));
                    }
                }
                
                // now we are pointing to global vertex, clear raw buffer
                surface->setRawVertexBuffer(0);
                IndexBufferPtr indexBuffer = new IndexBuffer();
                if (surface->getRenderMode() == Const::ERenderModeLine)
                {
                    indexBuffer->setLineIndices(indices, vcnt);
                }
                else
                {
                    indexBuffer->setTriangleIndices(indices, vcnt);
                }
                surface->setIndexBuffer(indexBuffer);
                if (!isDynamic)
                {
                    // change mesh for static surface point to merged one
                    surface->setMesh(mergedMesh);
                }
                else
                {
                    // for dynamics, keep the same
                }
                
                IndexBufferPtr wfib = surface->getWireFrameIndexBuffer();
                if (wfib)
                {
                    uint* wireframe = wfib->getIntLineIndices();
                    int wireframeLen = wfib->getLineIndicesLength();
                    for (int j = 0;j < wireframeLen;++j)
                    {
                        uint idx = wireframe[j] + wfOffset;
                        map<uint, uint>::iterator it = wfMap.find(idx);
                        int nIdx = it->second;
                        wireframe[j] = nIdx;
                    }
                }
            }

            if (isDynamic)
            {
                VertexBufferPtr result = generateVertexBuffer(vDataDynamic, flag);
                mesh->setCompactVertexBuffer(result);
            }
            else
            {
                staticFlag |= flag;
                dead.push_back(mesh);
            }

            if (loader && loader->isCanceled())
            {
                // clean up
                for (int i = 0;i < unprepared.size();++i)
                {
                    HouyiDelete(unprepared[i]);
                }
                HouyiDelete(mergedMesh);
                return;
            }
            else if (loader)
            {
                loader->notifiyProgress(0.5 + (1 - 0.5) * m / unprepared.size());
            }
            vBeforeTotal += beforeCnt;
        }
        
        // ------DONE merging vertices-----
        // now we don't have any duplicate vertices

        // only delete merged ones
        for (int i = 0;i < dead.size();++i)
        {
            HouyiDelete(dead[i]);
        }

        // prepare merged VertexBuffer for all static meshes
        if (vStaticData.size() > 0)
        {
            // clear mesh for all node. later we will point to merged mesh
            scene->getRootSceneNode()->setMeshForChildren(0);
            
            VertexBufferPtr result = generateVertexBuffer(vStaticData, staticFlag);
            mergedMesh->setCompactVertexBuffer(result);
            scene->addMesh(mergedMesh);

            // merge surface to reduce draw calls
            int sameMatCount = 0;
            vector< vector<RenderEntity> > ordered;
            for (int i = 0;i < scene->getRenderableCount();++i)
            {
                const RenderEntity& entity = scene->getRenderable(i);
                const SurfacePtr entiySurface = entity.mSurface;
                SceneNode* node = entity.mNode;
                Matrix4 nodeTrans = node->getLocal2World();

                bool sameFound = false;
                for (int j = 0;j < ordered.size();++j)
                {
                    vector<RenderEntity>& group = ordered[j];
                    RenderEntity& added = group[0];
                    const SurfacePtr addedSurface = added.mSurface;
                    SceneNode* addedNode = added.mNode;
                    Matrix4 addTrans = addedNode->getLocal2World();
                    if (entity.mMaterial == added.mMaterial && nodeTrans == addTrans
                        && entiySurface->getRenderMode() == addedSurface->getRenderMode())
                    {
                        sameFound = true;
                        sameMatCount++;
                        group.push_back(entity);
                        break;
                    }
                }

                if (!sameFound)
                {
                    vector<RenderEntity> newGroup;
                    newGroup.push_back(entity);
                    ordered.push_back(newGroup);
                }
            }

            vector<RenderEntity> transparents;
            for (int i = 0, groupCnt = ordered.size();i < groupCnt;++i)
            {
                // a group must contain either all triangles or all lines
                vector<RenderEntity> group = ordered[i];

                // get index count
                int groupIndexCount = 0;
                int groupWFIndexCount = 0;
                int renderMode = Const::ERenderModeTriangle;
                for (int j = 0, itemCountInGroup = group.size();j < itemCountInGroup;++j)
                {
                    RenderEntity& entity = group[j];
                    SurfacePtr s = entity.mSurface;
                    IndexBufferPtr ib = s->getIndexBuffer();
                    if (s->getRenderMode() == Const::ERenderModeLine)
                    {
                        groupIndexCount += ib->getLineIndicesLength();
                        renderMode = Const::ERenderModeLine;
                    }
                    else
                    {
                        groupIndexCount += ib->getTriangleIndicesLength();
                        IndexBufferPtr wfib = s->getWireFrameIndexBuffer();
                        if (wfib)
                        {
                            groupWFIndexCount += wfib->getLineIndicesLength();
                        }
                    }
                }

                // copy to final index buffer
                IndexBufferPtr groupIB = new IndexBuffer();
                uint* groupIndices = new uint[groupIndexCount];
                if (renderMode == Const::ERenderModeLine) // lines
                {
                    groupIB->setLineIndices(groupIndices, groupIndexCount);
                }
                else
                {
                    groupIB->setTriangleIndices(groupIndices, groupIndexCount);
                }
                int groupIdxIdx = 0;
                for (int j = 0;j < group.size();++j)
                {
                    RenderEntity& entity = group[j];
                    SurfacePtr s = entity.mSurface;
                    IndexBufferPtr ib = s->getIndexBuffer();
                    uint* indices = renderMode == Const::ERenderModeLine ? ib->getIntLineIndices() : ib->getIntTriangleIndices();
                    int indicesLength = renderMode == Const::ERenderModeLine ? ib->getLineIndicesLength() : ib->getTriangleIndicesLength();
                    for (int k = 0;k < indicesLength; ++k)
                    {
                        groupIndices[groupIdxIdx++] = indices[k];
                    }
                }
                
                // copy to final wireframe buffer
                IndexBufferPtr groupIBWF;
                if (groupWFIndexCount > 0)
                {
                    groupIBWF = new IndexBuffer();
                    uint* groupWFIndices = new uint[groupWFIndexCount];
                    groupIBWF->setLineIndices(groupWFIndices, groupWFIndexCount);
                    int groupWFIdxIdx = 0;
                    for (int j = 0;j < group.size();++j)
                    {
                        RenderEntity& entity = group[j];
                        SurfacePtr s = entity.mSurface;
                        IndexBufferPtr ib = s->getWireFrameIndexBuffer();
                        uint* indices = ib->getIntLineIndices();
                        for (int k = 0, n = ib->getLineIndicesLength();k < n; ++k)
                        {
                            groupWFIndices[groupWFIdxIdx++] = indices[k];
                        }
                    }
                }

                SceneNode* node = group[0].mNode; // node unchanged
                SurfacePtr s = new Surface(); // single surface
                s->setIndexBuffer(groupIB);
                s->setWireFrameIndexBuffer(groupIBWF);
                s->setMesh(mergedMesh);
                s->setRenderMode(group[0].mSurface->getRenderMode());
                mergedMesh->addSurface(s);
                Material* material = group[0].mMaterial; // material same
                node->setMesh(mergedMesh);
                RenderEntity groupEntity(node, s, material);
                node->addEntity(groupEntity);
                if (material->getDiffuseColor().mA < 1)
                {
                    transparents.push_back(groupEntity);
                }
                else
                {
                    scene->mRenderable.push_back(groupEntity);
                }
            }

            for (int i = 0;i < transparents.size();++i)
            {
                scene->mRenderable.push_back(transparents[i]);
            }

            LOGI("Merged same material: %d\n", sameMatCount);

            // remove from scene renderables
            for (int i = 0, groupCnt = ordered.size();i < groupCnt;++i)
            {
                vector<RenderEntity> group = ordered[i];
                for (int j = 0;j < group.size();++j)
                {
                    RenderEntity& entity = group[j];
                    for (int k = scene->getRenderableCount() - 1;k >= 0;--k)
                    {
                        if (entity == scene->getRenderable(k))
                        {
                            scene->mRenderable.erase(scene->mRenderable.begin() + k);
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            // all dynamic
            HouyiDelete(mergedMesh);
        }
        
        for (int i = 0;i < dynamics.size();++i)
        {
            Mesh* mesh = dynamics[i];
            scene->addMesh(mesh);
        }

        for (int m = 0;m < scene->getMeshCount();++m)
        {
            Mesh* mesh = scene->getMesh(m);
            VertexBufferPtr vb = mesh->getVertexBuffer();

            // no normal cause black. in such we generate default normal
            if (vb && !vb->getNormal())
            {
                MeshOptimizer::generateNormal(mesh);
            }

            // generate tangent for special effect
            if (Settings::getInstance()->getShadingMode() == Settings::ESM_Hologram)
            {
                mesh->expand();
                MeshOptimizer::generateScifiTangent(mesh->getVertexBuffer());
                for (int i = 0;i < mesh->getSurfaceCount();++i)
                {
                    SurfacePtr surface = mesh->getSurface(i);
                    surface->setBufferValid(false);
                }
            }
        }
        
        scene->addAABBToRenderable(scene->getRootSceneNode());

        // statistics
        stats.mVertexCount = vStaticData.size();
        for (int i = 0;i < dynamics.size();++i)
        {
            Mesh* mesh = dynamics[i];
            stats.mVertexCount += mesh->getVertexBuffer()->getVertexCount();
        }

        if (vBeforeTotal != 0)
        {
            LOGI("Merge result. Before: %d, After: %d:, Ratio: %.2f%%\n",
                vBeforeTotal, stats.mVertexCount, 100 * (1-(float)stats.mVertexCount/vBeforeTotal));
        }

        stats.mRenderableCount = scene->getRenderableCount();
        for (int i = 0;i < scene->getRenderableCount();++i)
        {
            const RenderEntity& entity = scene->getRenderable(i);
            SurfacePtr s = entity.mSurface;
            if (entity.mNode->getName().compare("AABB") != 0)
            {
                stats.mLineCount += s->getLineCount();
            }
            stats.mTriangleCount += s->getTriangleCount();
        }
        stats.print();
    }

    void SceneOptimizer::prepareSceneMesh(Scene* scene, Loader* loader)
    {
        SceneStats& stats = scene->getStats();
        for (int m = 0;m < scene->getMeshCount();++m)
        {
            Mesh* mesh = scene->getMesh(m);
            
            map<uint, uint> wfStaticMap;
            
            bool isDynamic = mesh->getSkeletonRef();
            map<uint, uint> vMap;
            vector<VertexData> vData;
            int flag = 0;
            
            for (int i = 0;i < mesh->getSurfaceCount();++i)
            {
                map<uint, uint> wfMapDynamic; // per surface
                map<uint, uint>& wfMap = isDynamic ? wfMapDynamic : wfStaticMap;
                int wfOffset = isDynamic ? 0 : wfStaticMap.size(); // start from current map size for static
                SurfacePtr surface = mesh->getSurface(i);
                VertexBufferPtr vb = surface->getRawVertexBuffer();
                IndexBufferPtr ib = surface->getIndexBuffer();
                int vcnt = ib ? ib->getTriangleIndicesLength() : vb->getPositionLength() / 3;
                float* pos = vb->getPosition();
                float* normal = vb->getNormal();
                float* texcoord = vb->getTexcoord();
                float* boneIndex = vb->getJointIndex();
                float* boneWeight = vb->getJointWeight();
                float* tangent = vb->getTangent();
                float* binormal = vb->getBinormal();
                uint* indices = new uint[vcnt];
                uint* rawIndices = ib ? ib->getIntTriangleIndices() : 0;
                for (int j = 0;j < vcnt;++j)
                {
                    int idx = rawIndices ? rawIndices[j] : j;
                    float x = pos[idx * 3];
                    float y = pos[idx * 3 + 1];
                    float z = pos[idx * 3 + 2];
                    VertexData data;
                    data.position = Vector3(x, y, z);
                    if (normal)
                    {
                        data.normal = Vector3(normal[idx * 3], normal[idx * 3 + 1], normal[idx * 3 + 2]);
                        flag |= VertexBuffer::Attribute_Normal;
                    }
                    if (texcoord)
                    {
                        data.texcoord = Vector2(texcoord[idx * 2], texcoord[idx * 2 + 1]);
                        flag |= VertexBuffer::Attribute_Texcoord;
                    }
                    if (boneIndex)
                    {
                        data.boneIndex = Vector4(boneIndex[idx * 4], boneIndex[idx * 4 + 1], boneIndex[idx * 4 + 2], boneIndex[idx * 4 + 3]);
                        flag |= VertexBuffer::Attribute_Skeleton;
                    }
                    if (boneWeight)
                    {
                        data.boneWeight = Vector4(boneWeight[idx * 4], boneWeight[idx * 4 + 1], boneWeight[idx * 4 + 2], boneWeight[idx * 4 + 3]);
                    }
                    if (tangent)
                    {
                        data.tangent = Vector3(tangent[idx * 3], tangent[idx * 3 + 1], tangent[idx * 3 + 2]);
                        flag |= VertexBuffer::Attribute_Tangent;
                    }
                    if (binormal)
                    {
                        data.binormal = Vector3(binormal[idx * 3], binormal[idx * 3 + 1], binormal[idx * 3 + 2]);
                        flag |= VertexBuffer::Attribute_Binormal;
                    }
                    
                    uint hash = Util::hashVertex(data);
                    map<uint, uint>::iterator it = vMap.find(hash);
                    if (it != vMap.end())
                    {
                        uint index = it->second;
                        VertexData& dataInMap = vData[index];
                        if (data != dataInMap)
                        {
                            indices[j] = vData.size();
                            vMap.insert(pair<uint, uint>(hash, indices[j]));
                            vData.push_back(data);
                            wfMap.insert(pair<uint, uint>(j + wfOffset, indices[j]));
                        }
                        else
                        {
                            indices[j] = index;
                            wfMap.insert(pair<uint, uint>(j + wfOffset, index));
                        }
                    }
                    else
                    {
                        indices[j] = vData.size();
                        vMap.insert(pair<uint, uint>(hash, indices[j]));
                        vData.push_back(data);
                        wfMap.insert(pair<uint, uint>(j + wfOffset, indices[j]));
                    }
                }
                
                // now we are pointing to global vertex, clear raw buffer
                surface->setRawVertexBuffer(0);
                IndexBufferPtr indexBuffer = new IndexBuffer();
                if (surface->getRenderMode() == Const::ERenderModeLine)
                {
                    indexBuffer->setLineIndices(indices, vcnt);
                }
                else
                {
                    indexBuffer->setTriangleIndices(indices, vcnt);
                }
                surface->setIndexBuffer(indexBuffer);
                
                IndexBufferPtr wfib = surface->getWireFrameIndexBuffer();
                if (wfib)
                {
                    uint* wireframe = wfib->getIntLineIndices();
                    int wireframeLen = wfib->getLineIndicesLength();
                    for (int j = 0;j < wireframeLen;++j)
                    {
                        uint idx = wireframe[j] + wfOffset;
                        map<uint, uint>::iterator it = wfMap.find(idx);
                        int nIdx = it->second;
                        wireframe[j] = nIdx;
                    }
                }
            }
            
            VertexBufferPtr result = generateVertexBuffer(vData, flag);
            mesh->setCompactVertexBuffer(result);
            
            // no normal cause black. in such we generate default normal
            if (!result->getNormal())
            {
                MeshOptimizer::generateNormal(mesh);
            }
            
            stats.mVertexCount += vData.size();
        }
        
        stats.mRenderableCount = scene->getRenderableCount();
        for (int i = 0;i < scene->getRenderableCount();++i)
        {
            const RenderEntity& entity = scene->getRenderable(i);
            SurfacePtr s = entity.mSurface;
            stats.mLineCount += s->getLineCount();
            stats.mTriangleCount += s->getTriangleCount();
        }

        if (loader->loadAABB())
        {
            scene->getRootSceneNode()->generateAABB(scene);
            scene->addAABBToRenderable(scene->getRootSceneNode());
        }
    }

    VertexBufferPtr SceneOptimizer::generateVertexBuffer(vector<VertexData>& vData, int flag)
    {
        VertexBufferPtr result = new VertexBuffer();;
        int afterCnt = vData.size();
        if (afterCnt == 0)
        {
            LOGE("SceneOptimizer::generateVertexBuffer. No vertex data\n");
            return result;
        }
        float* position = new float[afterCnt * 3];
        for(int i = 0;i < afterCnt;++i)
        {
            VertexData data = vData[i];
            position[i * 3] = data.position.x;
            position[i * 3 + 1] = data.position.y;
            position[i * 3 + 2] = data.position.z;
        }
        result->setPosition(position, afterCnt * 3);

        if ((flag & VertexBuffer::Attribute_Normal) != 0)
        {
            float* normal = new float[afterCnt * 3];
            for(int i = 0;i < afterCnt;++i)
            {
                VertexData data = vData[i];
                normal[i * 3] = data.normal.x;
                normal[i * 3 + 1] = data.normal.y;
                normal[i * 3 + 2] = data.normal.z;
            }
            result->setNormal(normal, afterCnt * 3);
        }

        if ((flag & VertexBuffer::Attribute_Texcoord) != 0)
        {
            float* texcoord = new float[afterCnt * 2];
            for(int i = 0;i < afterCnt;++i)
            {
                VertexData data = vData[i];
                texcoord[i * 2] = data.texcoord.x;
                texcoord[i * 2 + 1] = data.texcoord.y;
            }
            result->setTexcoord(texcoord, afterCnt * 2);
        }

        if ((flag & VertexBuffer::Attribute_Skeleton) != 0)
        {
            float* boneIndex = new float[afterCnt * 4];
            float* boneWeight = new float[afterCnt * 4];
            for(int i = 0;i < afterCnt;++i)
            {
                VertexData data = vData[i];
                boneIndex[i * 4] = data.boneIndex.x;
                boneIndex[i * 4 + 1] = data.boneIndex.y;
                boneIndex[i * 4 + 2] = data.boneIndex.z;
                boneIndex[i * 4 + 3] = data.boneIndex.w;
                boneWeight[i * 4] = data.boneWeight.x;
                boneWeight[i * 4 + 1] = data.boneWeight.y;
                boneWeight[i * 4 + 2] = data.boneWeight.z;
                boneWeight[i * 4 + 3] = data.boneWeight.w;
            }
            result->setJointIndex(boneIndex, afterCnt * 4);
            result->setJointWeight(boneWeight, afterCnt * 4);
        }

        if ((flag & VertexBuffer::Attribute_Tangent) != 0)
        {
            float* tangent = new float[afterCnt * 3];
            for(int i = 0;i < afterCnt;++i)
            {
                VertexData data = vData[i];
                tangent[i * 3] = data.tangent.x;
                tangent[i * 3 + 1] = data.tangent.y;
                tangent[i * 3 + 2] = data.tangent.z;
            }
            result->setTangent(tangent, afterCnt * 3);
        }

        if ((flag & VertexBuffer::Attribute_Binormal) != 0)
        {
            float* binormal = new float[afterCnt * 3];
            for(int i = 0;i < afterCnt;++i)
            {
                VertexData data = vData[i];
                binormal[i * 3] = data.binormal.x;
                binormal[i * 3 + 1] = data.binormal.y;
                binormal[i * 3 + 2] = data.binormal.z;
            }
            result->setBinormal(binormal, afterCnt * 3);
        }

        return result;
    }
}
