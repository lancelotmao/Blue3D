#include "HouyiMeshOptimizer.h"
#include "HouyiMesh.h"
#include "HouyiAABB.h"
#include <map>
#include <sstream>

namespace Houyi
{
    void MeshOptimizer::optimizeShadow(Mesh* mesh)
    {
        VertexBufferPtr finalVB = new VertexBuffer();
        map<string, int> vMap;
        vector<float> posVec;
        vector<float> boneIndices;
        vector<float> boneWeights;
        vector<unsigned short> indices;
        int mapIdx = 0;
        for (int i = 0;i < mesh->getSurfaceCount();++i)
        {
            SurfacePtr surface = mesh->getSurface(i);
            VertexBufferPtr vb = surface->getVertexBuffer();

            int componentSize = vb->getVertexComponentSize();
            float* pos = vb->getPosition();
            float* jointIdx = vb->getJointIndex();
            float* jointWeights = vb->getJointWeight();
            int componentCount = vb->getPositionLength() / componentSize;
            for (int j = 0;j < componentCount;++j)
            {
                ostringstream oss;
                for (int k = 0; k < componentSize; ++k)
                {
                    oss << pos[j * componentSize + k];
                }
                string key = oss.str();
                pair<map<string, int>::iterator, bool> p;
                p = vMap.insert(pair<string, int>(key, mapIdx));
                if(p.second)
                {
                    for (int k = 0; k < 3; ++k)
                    {
                        if (k < componentSize)
                        {
                            posVec.push_back(pos[j * componentSize + k]);
                        }
                        else
                        {
                            posVec.push_back(0);
                        }
                    }

                    for (int k = 0;k < 4;++k)
                    {
                        boneIndices.push_back(jointIdx[j * 4 + k]);
                        boneWeights.push_back(jointWeights[j * 4 + k]);
                    }

                    indices.push_back(mapIdx);
                    mapIdx++;
                }
                else
                {
                    indices.push_back(p.first->second);
                }
            }
        }

        finalVB->setVertexComponentSize(3);
        float* pos = new float[posVec.size()];
        for (int i = 0;i < posVec.size();++i)
        {
            pos[i] = posVec[i];
        }

        float* jointIdx = new float[boneIndices.size()];
        float* jointWeights = new float[boneWeights.size()];
        for (int i = 0;i < boneIndices.size();++i)
        {
            jointIdx[i] = boneIndices[i];
            jointWeights[i] = boneWeights[i];
        }

        finalVB->setPosition(pos, posVec.size());
        finalVB->setJointIndex(jointIdx, boneIndices.size());
        finalVB->setJointWeight(jointWeights, boneWeights.size());

        IndexBufferPtr ib = new IndexBuffer();
        uint* idx = new uint[indices.size()];
        for (int i = 0;i < indices.size();++i)
        {
            idx[i] = indices[i];
        }
        ib->setTriangleIndices(idx, indices.size());

        SurfacePtr shadowSurface = new Surface(finalVB, ib);
        mesh->setShadowSurface(shadowSurface);
    }

    void MeshOptimizer::generateWireFrame(Mesh* mesh)
    {
        for (int i = 0;i < mesh->getSurfaceCount();++i)
        {
            SurfacePtr surface = mesh->getSurface(i);
            generateWireFrame(surface);
        }
    }

    void MeshOptimizer::generateWireFrame(SurfacePtr surface)
    {
        VertexBufferPtr vb = surface->getVertexBuffer();
        int componentSize = vb->getVertexComponentSize();

        uint* indices = 0; // only support int index
        int indicesLength = 0;

        IndexBufferPtr indexBufferPtr = surface->getIndexBuffer();
        if (indexBufferPtr)
        {
            int length = indexBufferPtr->getTriangleIndicesLength();
            if (length == 0 || !indexBufferPtr->getIntTriangleIndices())
            {
                LOGW("MeshOptimizer::generateWireFrame, no triangle indices\n");
                return;
            }
            int triangleCount = length / 3;
            uint* triangleIndices = indexBufferPtr->getIntTriangleIndices();
            indicesLength = triangleCount * 6;
            indices = new uint[indicesLength];
            for (int i = 0;i < triangleCount;++i)
            {
                indices[i * 6] = triangleIndices[i * 3];
                indices[i * 6 + 1] = triangleIndices[i * 3 + 1];

                indices[i * 6 + 2] = triangleIndices[i * 3 + 1];
                indices[i * 6 + 3] = triangleIndices[i * 3 + 2];

                indices[i * 6 + 4] = triangleIndices[i * 3 + 2];
                indices[i * 6 + 5] = triangleIndices[i * 3];
            }
        }
        else
        {
            int triangleCount = vb->getPositionLength() / componentSize / 3;
            indicesLength = triangleCount * 6;
            indices = new uint[indicesLength];
            for (int i = 0;i < triangleCount;++i)
            {
                indices[i * 6] = i * 3;
                indices[i * 6 + 1] = i * 3 + 1;

                indices[i * 6 + 2] = i * 3 + 1;
                indices[i * 6 + 3] = i * 3 + 2;

                indices[i * 6 + 4] = i * 3 + 2;
                indices[i * 6 + 5] = i * 3;
            }
        }

        if (indices)
        {
            IndexBufferPtr wireFrame = new IndexBuffer();
            wireFrame->setLineIndices(indices, indicesLength);
            surface->setWireFrameIndexBuffer(wireFrame);
        }
    }

    void MeshOptimizer::generateScifiTangent(VertexBufferPtr vertexBuffer)
    {
        if (!vertexBuffer)
        {
            LOGW("MeshOptimizer::generateScifiTangent. null vertex buffer");
            return;
        }
        
        if (vertexBuffer->getTangent())
        {
            LOGW("MeshOptimizer::generateScifiTangent. tangent exist\n");
            return;
        }

        int tcnt = vertexBuffer->getPositionLength() / 9;
        float* tangent = new float[tcnt * 9];
        for (int i = 0;i < tcnt;++i)
        {
            tangent[i * 9] = 1;
            tangent[i * 9 + 1] = 0;
            tangent[i * 9 + 2] = 0;

            tangent[i * 9 + 3] = 0;
            tangent[i * 9 + 4] = 1;
            tangent[i * 9 + 5] = 0;

            tangent[i * 9 + 6] = 0;
            tangent[i * 9 + 7] = 0;
            tangent[i * 9 + 8] = 1;
        }
        vertexBuffer->setTangent(tangent, tcnt * 9);
    }
    
    void MeshOptimizer::generateNormal(Mesh* mesh)
    {
        if (mesh->getMode() == Mesh::ECompact)
        {
            generateSmoothNormal(mesh);
        }
        else if(mesh->getMode() == Mesh::EExpanded)
        {
            generateFaceNormal(mesh);
        }
        else
        {
            generateFaceNormal(mesh);
        }
    }
    
    void MeshOptimizer::generateSmoothNormal(Mesh* mesh)
    {
        if (!mesh)
        {
            LOGW("MeshOptimizer::generateSmoothNormal. Null Mesh");
            return;
        }
        
        if (mesh->getMode() != Mesh::ECompact)
        {
            mesh->compact();
        }
        
        VertexBufferPtr vb = mesh->getVertexBuffer();
        int vcnt = vb->getVertexCount();
        vector<Vector3>* vnRecord = new vector<Vector3>[vcnt];
        float* position = vb->getPosition();
        int vcs = vb->getVertexComponentSize();
        for (int i = 0;i < mesh->getSurfaceCount();++i)
        {
            SurfacePtr surface = mesh->getSurface(i);
            IndexBufferPtr ib = surface->getIndexBuffer();
            int idxCnt = ib->getTriangleIndicesLength();
            int triangleCnt = idxCnt / 3;
            uint* indices = ib->getIntTriangleIndices();
            for (int j = 0;j < triangleCnt;++j)
            {
                uint v1Idx = indices[j * 3];
                uint v2Idx = indices[j * 3 + 1];
                uint v3Idx = indices[j * 3 + 2];
                
                float v1z = (vcs == 2 ? 0 : position[v1Idx * vcs + 2]);
                float v2z = (vcs == 2 ? 0 : position[v2Idx * vcs + 2]);
                float v3z = (vcs == 2 ? 0 : position[v3Idx * vcs + 2]);
                
                Vector3 v1(position[v1Idx * vcs], position[v1Idx * vcs + 1], v1z);
                Vector3 v2(position[v2Idx * vcs], position[v2Idx * vcs + 1], v2z);
                Vector3 v3(position[v3Idx * vcs], position[v3Idx * vcs + 1], v3z);
                
                Vector3 n = (v3 - v1).crossProduct(v1 - v2);
                n.normalise();
                vnRecord[v1Idx].push_back(n);
                vnRecord[v2Idx].push_back(n);
                vnRecord[v3Idx].push_back(n);
            }
        }
        
        float* normal = new float[vcnt * 3];
        for (int i = 0;i < vcnt;++i)
        {
            vector<Vector3> vn = vnRecord[i];
            Vector3 n = Vector3::ZERO;
            for (int j = 0;j < vn.size();++j)
            {
                n += vn[j];
            }
            n.normalise();
            normal[i * 3] = n.x;
            normal[i * 3 + 1] = n.y;
            normal[i * 3 + 2] = n.z;
        }
        vb->setNormal(normal, vcnt*3);
        delete[] vnRecord;
    }
    
    void MeshOptimizer::generateFaceNormal(Mesh* mesh)
    {
        if (mesh->getMode() != Mesh::EExpanded)
        {
            mesh->expand();
        }
        
        // expanded vertex buffer
        VertexBufferPtr vb = mesh->getVertexBuffer();
        
        int vcs = vb->getVertexComponentSize();
        int tStep = 3 * vcs;
        int triangleCount = vb->getVertexCount() / 3;
        
        int normalLength = triangleCount * 9;
        float* normal = new float[normalLength];
        float* position = vb->getPosition();
        
        for (int i = 0;i < triangleCount;++i)
        {
            float v1z = (vcs == 2 ? 0 : position[i * tStep + 2]);
            float v2z = (vcs == 2 ? 0 : position[i * tStep + 5]);
            float v3z = (vcs == 2 ? 0 : position[i * tStep + 8]);
            
            Vector3 v1(position[i * tStep], position[i * tStep + 1], v1z);
            Vector3 v2(position[i * tStep + vcs], position[i * tStep + vcs + 1], v2z);
            Vector3 v3(position[i * tStep + vcs * 2], position[i * tStep + vcs * 2 + 1], v3z);
            
            Vector3 n = (v3 - v1).crossProduct(v1 - v2);
            n.normalize();
            normal[i * 9] = n.x;
            normal[i * 9 + 1] = n.y;
            if (vcs == 2)
            {
                normal[i * 9 + 2] = 1;
            }
            else
            {
                normal[i * 9 + 2] = n.z;
            }
            
            normal[i * 9 + 3] = n.x;
            normal[i * 9 + 4] = n.y;
            if (vcs == 2)
            {
                normal[i * 9 + 5] = 1;
            }
            else
            {
                normal[i * 9 + 5] = n.z;
            }
            
            normal[i * 9 + 6] = n.x;
            normal[i * 9 + 7] = n.y;
            if (vcs == 2)
            {
                normal[i * 9 + 8] = 1;
            }
            else
            {
                normal[i * 9 + 8] = n.z;
            }
        }
        
        vb->setNormal(normal, normalLength);
    }

    void MeshOptimizer::generateTangent(Mesh* mesh)
    {
        if (mesh->getMode() != Mesh::EExpanded)
        {
            mesh->expand();
        }

        // expanded vertex buffer
        VertexBufferPtr vb = mesh->getVertexBuffer();
        int triangleCount = vb->getVertexCount() / 3;
        float* position = vb->getPosition();
        float* texcoord = vb->getTexcoord();
        int tanLen = triangleCount * 3 * 4;
        float* tangent = new float[tanLen];
        float* binormal = new float[tanLen];
        for (long a = 0; a < triangleCount; a++)
        {
            const Vector3 v1(position[a * 9], position[a * 9 + 1], position[a * 9 + 2]);
            const Vector3 v2(position[a * 9 + 3], position[a * 9 + 4], position[a * 9 + 5]);
            const Vector3 v3(position[a * 9 + 6], position[a * 9 + 7], position[a * 9 + 8]);

            const Vector2 w1(texcoord[a * 6], texcoord[a * 6 + 1]);
            const Vector2 w2(texcoord[a * 6 + 2], texcoord[a * 6 + 3]);
            const Vector2 w3(texcoord[a * 6 + 4], texcoord[a * 6 + 5]);

            float x1 = v2.x - v1.x;
            float x2 = v3.x - v1.x;
            float y1 = v2.y - v1.y;
            float y2 = v3.y - v1.y;
            float z1 = v2.z - v1.z;
            float z2 = v3.z - v1.z;

            float s1 = w2.x - w1.x;
            float s2 = w3.x - w1.x;
            float t1 = w2.y - w1.y;
            float t2 = w3.y - w1.y;

            float r = 1.0F / (s1 * t2 - s2 * t1);
            Vector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
                    (t2 * z1 - t1 * z2) * r);
            Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                    (s1 * z2 - s2 * z1) * r);

            tangent[a * 9] += sdir.x;
            tangent[a * 9 + 1] += sdir.y;
            tangent[a * 9 + 2] += sdir.z;
            tangent[a * 9 + 3] += sdir.x;
            tangent[a * 9 + 4] += sdir.y;
            tangent[a * 9 + 5] += sdir.z;
            tangent[a * 9 + 6] += sdir.x;
            tangent[a * 9 + 7] += sdir.y;
            tangent[a * 9 + 8] += sdir.z;

            binormal[a * 9] += tdir.x;
            binormal[a * 9 + 1] += tdir.y;
            binormal[a * 9 + 2] += tdir.z;
            binormal[a * 9 + 3] += tdir.x;
            binormal[a * 9 + 4] += tdir.y;
            binormal[a * 9 + 5] += tdir.z;
            binormal[a * 9 + 6] += tdir.x;
            binormal[a * 9 + 7] += tdir.y;
            binormal[a * 9 + 8] += tdir.z;
        }

        vb->setTangent(tangent, tanLen);
        vb->setBinormal(binormal, tanLen);
    }
}
