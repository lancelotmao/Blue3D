#include "HouyiMesh.h"
#include "HouyiRoot.h"
#include "HouyiCapability.h"
#include "HouyiMeshOptimizer.h"
#include "HouyiAABB.h"
#include "HouyiScene.h"

namespace Houyi
{
    Mesh::Mesh() :
            mTexture(0), mAABB(0),
            mEdgeLineMesh(0), mIsColorSet(false), mMode(ECompact), mCompact(0), mExpanded(0),
            mSkeleton(0), mMaterial(0), mCastShadowEnabled(true), mShadowFactor(0.9f)
    {
    }

    Mesh::~Mesh()
    {
    }

    Mesh& Mesh::operator=(const Mesh& mesh)
    {
        if (this == &mesh)      // Same object?
            return *this;

        mTexture = mesh.mTexture;
        return *this;
    }

    SurfacePtr Mesh::getSurface(int index)
    {
        if (index >= 0 && index < mSurfaces.size())
        {
            return mSurfaces[index];
        }
        return 0;
    }

    void Mesh::addSurface(SurfacePtr surface)
    {
        if (surface)
        {
            mSurfaces.push_back(surface);
            surface->setMesh(this);
        }
        else
        {
            LOGW("adding null mSurfaces to mesh\n");
        }
    }

    int Mesh::getSurfaceCount()
    {
        return mSurfaces.size();
    }

    void Mesh::setTexture(Texture* texture)
    {
        if (texture != mTexture)
        {
            if (mTexture)
            {
                HouyiDelete (mTexture);
            }
            mTexture = texture;
        }

        if (mMaterial)
        {
            if (mMaterial->getTextureCount() > 0)
            {
                Texture* t = mMaterial->getTexture(0);
//                t->setImage(texture->getImage());
            }
            else
            {
                mMaterial->addTexture(texture);
            }
        }
    }

    Texture* Mesh::getTexture()
    {
        return mTexture;
    }

    int Mesh::getTextureId()
    {
        if (mTexture)
        {
            return mTexture->getTextureId();
        }
        else
        {
            return -1;
        }
    }

    Color& Mesh::getColorRef()
    {
        return mColor;
    }

    void Mesh::setColor(Color color)
    {
        mColor = color;
        mIsColorSet = true;
    }

    bool Mesh::isColorSet()
    {
        return mIsColorSet;
    }

    Material* Mesh::getMaterial()
    {
        return mMaterial;
    }

    void Mesh::setMaterial(Material* material)
    {
        mMaterial = material;
    }

    void Mesh::updateBoneAni()
    {
        SkeletonPtr skeleton = mSkeleton.mSkeleton;
        JointNode** joints = skeleton->getJointNodes();
        int* indices = mSkeleton.mIndices->getData();
        int maxFrame = skeleton->mMaxKFCount;

        // most cases all surface should use mesh vertex
        // so it means there is one vertex buffer per mesh
        // or even one vertex buffer for the whole world
        // anyway we add map for safety
        set<VertexBufferPtr> updatedSet;
        set<VertexBufferPtr>::iterator it;
        
        for (int i = 0; i < getSurfaceCount(); ++i)
        {
            SurfacePtr surface = getSurface(i);
            VertexBufferPtr vb = surface->getVertexBuffer();
            it = updatedSet.find(vb);
            if (it != updatedSet.end())
            {
                continue;
            }

            updatedSet.insert(vb);
            vb->setUseVBO(false);
            vb->setBind(false);
            int vertexCnt = vb->getPositionLength() / 3;
            int jicnt = vb->getJointIndexLength();
            float* postion = vb->getPosition();
            float* jointIndex = vb->getJointIndex();
            float* jointWeight = vb->getJointWeight();
            float* orginalPos = vb->getGeoPos();
            for (int j = 0; j < vertexCnt && j < jicnt; ++j)
            {
                if (maxFrame == 0)
                {
                    Vector3 v = Vector3(orginalPos[j * 3], orginalPos[j * 3 + 1], orginalPos[j * 3 + 2]);
                    v = mBindShapeMatrix * v;

                    postion[j * 3] = v.x;
                    postion[j * 3 + 1] = v.y;
                    postion[j * 3 + 2] = v.z;
                }
                else
                {
                    Vector4 weightedPos(0, 0, 0, 0);

                    float weightTotal = 0;
                    for (int k = 0; k < 4; k++)
                    {
                        float weight = jointWeight ? jointWeight[j * 4 + k] : 1;
                        weightTotal += weight;
                        if (weight == 0)
                        {
                            continue;
                        }

                        int idx = (int) jointIndex[j * 4 + k];
                        int globalIndex = indices[idx];
                        JointNode* jnPtr = joints[globalIndex];
                        if (jnPtr)
                        {
                            Vector4 v = Vector4(orginalPos[j * 3], orginalPos[j * 3 + 1], orginalPos[j * 3 + 2], 1);

                            if (Capability::useNeon())
                            {
                                Vector4::transform(mBindShapeMatrixTranspose.getData(), &v, &v);
                                Vector4::transform(jnPtr->mWorldMatrixTranspose.getData(), &v, &v);
                                Vector4::scalar(&v, &weight);
                            }
                            else
                            {
                                v = mBindShapeMatrix * v ;
                                v = jnPtr->mWorldMatrix * v;
                                v = v * weight;
                            }
                            
                            weightedPos = weightedPos + v;
                        }
                    }

    //					assert (weightTotal > 0.99 && weightTotal < 1.001);
                    postion[j * 3] = weightedPos.x;
                    postion[j * 3 + 1] = weightedPos.y;
                    postion[j * 3 + 2] = weightedPos.z;
                }
            }
        }
    }

    void Mesh::setBindShapeMatrix(Matrix4 m)
    {
        mBindShapeMatrix = m;
        mBindShapeMatrixTranspose = m.transpose();
    }

    Matrix4& Mesh::getBindShapeMatrix()
    {
        return mBindShapeMatrix;
    }
    
    bool Mesh::bindToSkeleton(Skeleton* skeleton, string jointName)
    {
        if (!skeleton)
        {
            LOGE("mesh cannot bind to NULL skeleton\n");
            return false;
        }
        
        int index = skeleton->findJointIndexByName(jointName);
        if (index == -1)
        {
            LOGW("mesh cannot bind to skeleton, jointName = %s\n", jointName.c_str());
            return false;
        }
        
        SkeletonRef skeletonRef(skeleton);
        ArrayPtr<int> indices = new Array<int>(1); // all bind to one joint
        skeletonRef.mIndices = indices;
        indices->getData()[0] = index;
        
        for (int i = 0;i < getSurfaceCount();++i)
        {
            SurfacePtr surface = getSurface(i);
            VertexBufferPtr vb = surface->getVertexBuffer();
            int vcnt = vb->getVertexCount();
            int jcnt = vcnt * 4;
            float* ji = new float[jcnt];
            vb->setJointIndex(ji, jcnt);
            float* jw = new float[jcnt];
            vb->setJointWeight(jw, jcnt);
            for (int j = 0;j < vcnt;++j)
            {
                ji[j * 4] = 0; // pointing to first data in indices which in turn points to joint
                
                jw[j * 4] = 1;
                jw[j * 4 + 1] = 0;
                jw[j * 4 + 2] = 0;
                jw[j * 4 + 3] = 0;
            }
        }
        
        setSkeletonRef(skeletonRef);
        
        return true;
    }

    float* Mesh::getUploadBoneMatrix()
    {
        return mUploadBoneMatrix;
    }
    
    void Mesh::enableShadowCast(bool enable)
    {
        mCastShadowEnabled = enable;
    }
    
    bool Mesh::isShadowCastEnabled()
    {
        return mCastShadowEnabled;
    }

    void Mesh::setShadowFactor(float factor)
    {
        mShadowFactor = factor;
    }

    float Mesh::getShadowFactor()
    {
        return mShadowFactor;
    }

    void Mesh::setShadowSurface(SurfacePtr surface)
    {
        mShadowSurface = surface;
    }

    SurfacePtr Mesh::getShadowSurface()
    {
        return mShadowSurface;
    }

    void Mesh::expand()
    {
        if (mExpanded)
        {
            mMode = EExpanded;
            // already expanded, only change mode
            LOGW("Mesh::expand. already expanded\n");
            return;
        }
        
        if (!mCompact)
        {
            mMode = EExpanded;
            // already expanded, probably in raw mode
            LOGW("Mesh::expand. no compact vertex buffer\n");
            return;
        }

        vector<VertexData> expandedData;
        VertexBufferPtr vb = new VertexBuffer();

        float* pos = mCompact->getGeoPos(); // not position which is chaning during animation
        float* normal = mCompact->getNormal();
        float* texcoord = mCompact->getTexcoord();
        float* boneIdx = mCompact->getJointIndex();
        float* boneWeight = mCompact->getJointWeight();
        for (int i = 0;i < getSurfaceCount();++i)
        {
            SurfacePtr surface = getSurface(i);
            IndexBufferPtr ib = surface->getIndexBuffer();
            int length = ib->getTriangleIndicesLength();
            uint* indices = ib->getIntTriangleIndices();
            uint* expandedIndices = new uint[length];
            for (int j = 0;j < length;++j)
            {
                VertexData data;
                int idx = indices[j];
                data.position.x = pos[idx * 3];
                data.position.y = pos[idx * 3 + 1];
                data.position.z = pos[idx * 3 + 2];
                if (normal)
                {
                    data.normal.x = normal[idx * 3];
                    data.normal.y = normal[idx * 3 + 1];
                    data.normal.z = normal[idx * 3 + 2];
                }
                if (texcoord)
                {
                    data.texcoord.x = texcoord[idx * 2];
                    data.texcoord.y = texcoord[idx * 2 + 1];
                }
                if (boneIdx)
                {
                    data.boneIndex.x = boneIdx[idx * 4];
                    data.boneIndex.y = boneIdx[idx * 4 + 1];
                    data.boneIndex.z = boneIdx[idx * 4 + 2];
                    data.boneIndex.w = boneIdx[idx * 4 + 3];
                }
                if (boneWeight)
                {
                    data.boneWeight.x = boneWeight[idx * 4];
                    data.boneWeight.y = boneWeight[idx * 4 + 1];
                    data.boneWeight.z = boneWeight[idx * 4 + 2];
                    data.boneWeight.w = boneWeight[idx * 4 + 3];
                }

                // re-index to current vertex
                expandedIndices[j] = expandedData.size();
                expandedData.push_back(data);
            }

            IndexBufferPtr expandedIndexBuffer = new IndexBuffer();
            expandedIndexBuffer->setTriangleIndices(expandedIndices, length);
            surface->setExpandedBuffer(expandedIndexBuffer);
        }

        int afterCnt = expandedData.size();
        float* position = new float[afterCnt * 3];
        for(int j = 0;j < afterCnt;++j)
        {
            VertexData data = expandedData[j];
            position[j * 3] = data.position.x;
            position[j * 3 + 1] = data.position.y;
            position[j* 3 + 2] = data.position.z;
        }
        vb->setPosition(position, afterCnt * 3);

        if (normal)
        {
            float* normal = new float[afterCnt * 3];
            for(int j = 0;j < afterCnt;++j)
            {
                VertexData data = expandedData[j];
                normal[j * 3] = data.normal.x;
                normal[j * 3 + 1] = data.normal.y;
                normal[j* 3 + 2] = data.normal.z;
            }
            vb->setNormal(normal, afterCnt * 3);
        }

        if (texcoord)
        {
            float* texcoord = new float[afterCnt * 2];
            for(int j = 0;j < afterCnt;++j)
            {
                VertexData data = expandedData[j];
                texcoord[j * 2] = data.texcoord.x;
                texcoord[j * 2 + 1] = data.texcoord.y;
            }
            vb->setTexcoord(texcoord, afterCnt * 2);
        }

        if (boneIdx)
        {
            float* boneIdx = new float[afterCnt * 4];
            for(int j = 0;j < afterCnt;++j)
            {
                VertexData data = expandedData[j];
                boneIdx[j * 4] = data.boneIndex.x;
                boneIdx[j * 4 + 1] = data.boneIndex.y;
                boneIdx[j * 4 + 2] = data.boneIndex.z;
                boneIdx[j * 4 + 3] = data.boneIndex.w;
            }
            vb->setJointIndex(boneIdx, afterCnt * 4);
        }

        if (boneWeight)
        {
            float* boneWeight = new float[afterCnt * 4];
            for(int j = 0;j < afterCnt;++j)
            {
                VertexData data = expandedData[j];
                boneWeight[j * 4] = data.boneWeight.x;
                boneWeight[j * 4 + 1] = data.boneWeight.y;
                boneWeight[j * 4 + 2] = data.boneWeight.z;
                boneWeight[j * 4 + 3] = data.boneWeight.w;
            }
            vb->setJointWeight(boneWeight, afterCnt * 4);
        }

        setExpandedVertexBuffer(vb);

        mMode = EExpanded;
    }

    void Mesh::compact()
    {
        mMode = ECompact;
    }
    
    AABB* Mesh::getAABB(Scene* scene)
    {
        if (!mAABB)
        {
            mAABB = HouyiNew AABB(scene);
            mAABB->init();
        }
        return mAABB;
    }
    
    void Mesh::setRenderMode(int mode)
    {
        for (int i = 0;i < getSurfaceCount();++i)
        {
            SurfacePtr surface = getSurface(i);
            surface->setRenderMode(mode);
        }
    }
}
