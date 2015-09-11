#include "HouyiSceneNode.h"
#include "HouyiMaterialManager.h"
#include "HouyiAABB.h"
#include "HouyiMeshOptimizer.h"
#include "HouyiRoot.h"
#include "HouyiWorld.h"
#include "HouyiScene.h"

namespace Houyi
{
    SceneNode::SceneNode() :
    mScene(0), mMesh(0), mDisplayDirty(true), mVisibility(EVisible), mRenderListener(0),
    mHasNonUniform(false), mHasNegativeScale(false), mAABB(0),
    mLocal(Matrix4::IDENTITY), mLocal2World(Matrix4::IDENTITY), mSelectable(true),
    mZOrder(0), mKFCount(0), mKFs(0), mCurFrame(0)
    {
        setRenderListener(this);
    }

    SceneNode::~SceneNode()
    {
        delete[] mKFs;
    }
    
    void SceneNode::init()
    {
        onInit();
        
        for (int i = 0;i < mChildren.size();++i)
        {
            SceneNode* child = (SceneNode*)mChildren[i];
            if (child)
            {
                child->init();
            }
        }
    }
    
    void SceneNode::onInit()
    {
    }
    
    void SceneNode::postInit()
    {
        onPostInit();
        
        if (mEntities.size() == 0 && mMesh)
        {
            SceneNode* root = static_cast<SceneNode*>(getRoot());
            root->getScene()->addRenderable(this);
        }
        
        for (int i = 0;i < mChildren.size();++i)
        {
            SceneNode* child = (SceneNode*)mChildren[i];
            if (child)
            {
                child->postInit();
            }
        }
    }
    
    void SceneNode::onPostInit()
    {  
    }

    bool SceneNode::update(int frame)
    {
        mCurFrame = frame;
        
        bool ret = onUpdate();
        
        if (mKFs)
        {
            updateViewMatix(frame);
            ret |= mKFCount > 1;
        }
        else if (mDisplayDirty)
        {
            updateViewMatix();
            mDisplayDirty = false;
        }

        for (int i = 0;i < mChildren.size();++i)
        {
            SceneNode* child = static_cast<SceneNode*>(mChildren[i]);
            if (child)
            {
                ret |= child->update(frame);
            }
        }
        return ret;
    }
    
    void SceneNode::setScene(Scene* scene)
    {
        mScene = scene;
    }
    
    Scene* SceneNode::getScene()
    {
        return mScene;
    }

    void SceneNode::dirtyDisplay()
    {
        mDisplayDirty = true;
        for (int i = 0;i < mChildren.size();++i)
        {
            SceneNode* child = static_cast<SceneNode*>(mChildren[i]);
            child->dirtyDisplay();
        }
    }

    void SceneNode::setIdentity()
    {
        mLocal = Matrix4::IDENTITY;
        dirtyDisplay();
    }

    void SceneNode::translate(Vector3 trans)
    {
        translate(trans.x, trans.y, trans.z);
    }

    void SceneNode::translate(float tx, float ty, float tz)
    {
        Matrix4 trans = Matrix4::IDENTITY;
        trans.setTrans(Vector3(tx, ty, tz));
        mLocal = trans * mLocal;
        dirtyDisplay();
    }
    
    void SceneNode::rotate(float x, float y, float z, float radian)
    {
        Vector3 axis = Vector3(x, y, z);
        rotate(axis, radian);
    }
    
    void SceneNode::rotate(Vector3 axis, float radian)
    {
        Radian angle(-radian);
        axis.normalise();
        
        Matrix3 rot;
        rot.FromAngleAxis(axis, angle);
        Matrix4 r4(rot);
        mLocal = r4 * mLocal;
        
        dirtyDisplay();
    }
    
    void SceneNode::rotate(Quaternion q)
    {
        Vector3 axis;
        Radian angle;
        q.ToAngleAxis(angle, axis);
        rotate(axis, angle.valueRadians());
        
        dirtyDisplay();
    }

    void SceneNode::scale(const float sx, const float sy, const float sz)
    {
        scale(Vector3(sx, sy, sz));
    }
    
    void SceneNode::scale(const Vector3 s)
    {
        Matrix4 sm = Matrix4::IDENTITY;
        sm.setScale(s);
        mLocal = sm * mLocal;
        
        dirtyDisplay();
    }
    
    void SceneNode::scale(const float s)
    {
        scale(s, s, s);
    }
    
    void SceneNode::mulMatrix(const Matrix4& m)
    {
        mLocal = m * mLocal;
        dirtyDisplay();
    }

    void SceneNode::updateViewMatix(int frame)
    {
//        LOGE("F %d", (mKFCount));
        // start with local matrix
        mLocal2World = mKFCount != 0 ? mKFs[frame % mKFCount].getMatrix() * mLocal : mLocal;

        // multiply parent's transform
        SceneNode* parent = (SceneNode*) mParent;

        while (parent)
        {
            Matrix4 parentTrans = parent->mKFCount != 0 ? parent->mKFs[frame % parent->mKFCount].getMatrix() : parent->mLocal;
            mLocal2World = parentTrans * mLocal2World;

            parent = (SceneNode*) parent->mParent;
        }
        
        Matrix4 transposeOfInverse = mLocal2World;
        if (mHasNonUniform)
        {
            transposeOfInverse = transposeOfInverse.inverse().transpose();
        }
        transposeOfInverse.extract3x3Matrix(mLocal2WorldNormal);
    }

    const Matrix4& SceneNode::getLocal2World()
    {
        return mLocal2World;
    }

    Matrix4 SceneNode::getLocal2Parent()
    {
        return mKFCount != 0 ? mKFs[mCurFrame % mKFCount].getMatrix() * mLocal : mLocal;
    }
    
    Matrix4& SceneNode::getLocal()
    {
        return mLocal;
    }
    
    Matrix3& SceneNode::getLocal2WorldNormal()
    {
        return mLocal2WorldNormal;
    }

    void SceneNode::setLocal(Matrix4 matrix, bool nonUniform)
    {
        mHasNonUniform = nonUniform;
        mLocal = matrix;
        if (matrix.determinant() < 0)
        {
            mHasNegativeScale = true;
            LOGW("negative scale found for node: %s\n", getName().c_str());
        }
        updateViewMatix();
        dirtyDisplay();
    }

    void SceneNode::setVisibility(Visibility v)
    {
        mVisibility = v;
        for (int i = 0;i < childrenCount();++i)
        {
            static_cast<SceneNode*>(getChildAt(i))->setVisibility(v);
        }
    }

    SceneNode::Visibility SceneNode::getVisibility()
    {
        return mVisibility;
    }
    
    bool SceneNode::isVisible()
    {
        if (mVisibility == EVisible)
        {
            SceneNode* parent = (SceneNode*) mParent;
            while (parent)
            {
                if (parent->getVisibility() != EVisible)
                {
                    return false;
                }
                
                parent = (SceneNode*) parent->mParent;
            }
            return true;
        }
        return false;
    }

    AABB* SceneNode::generateAABB(Scene* scene)
    {
        if (!mAABB)
        {
            mAABB = HouyiNew AABB(scene);
            mAABB->init();
            mAABB->generate(mMesh);
            addChild(mAABB);
        }
        
        for (int i = 0;i < mChildren.size();++i)
        {
            SceneNode* child = static_cast<SceneNode*>(mChildren[i]);
            if (!child->isSelectable())
            {
                // don't generate AABB for any node that are not selectable
                // e.g. AABB itself or Grid Node
                continue;
            }
            
            AABB* childAABB = child->getAABB();
            if (!childAABB)
            {
                childAABB = child->generateAABB(scene);
            }
            if (childAABB->isValid())
            {
                mAABB->merge(child);
            }
        }
        return mAABB;
    }
    
    void SceneNode::showAABB(bool all)
    {
        if (!mAABB)
        {
            generateAABB(mScene);
        }
        
        if (mAABB && mAABB->isValid())
        {
            mAABB->setVisibility(EVisible);
            mScene->addAABBToRenderable(this);
            if (all)
            {
                for (int i = 0; i < childrenCount(); ++i)
                {
                    SceneNode* child = static_cast<SceneNode*>(mChildren[i]);
                    child->showAABB(all);
                }
            }
        }
    }

    void SceneNode::hideAABB(bool all)
    {
        if (mAABB)
        {
            mAABB->setVisibility(EGone);
            if (all)
            {
                for (int i = 0; i < childrenCount(); ++i)
                {
                    SceneNode* child = static_cast<SceneNode*>(mChildren[i]);
                    child->hideAABB(all);
                }
            }
        }
    }

    AABB* SceneNode::getAABB()
    {
        return mAABB;
    }

    bool SceneNode::isAABBVisible()
    {
        return mAABB && mAABB->getVisibility() == EVisible;
    }

    void SceneNode::clearAABB()
    {
        mAABB = 0;
    }

    int SceneNode::getMaterialCount()
    {
        return mMaterials.size();
    }

    void SceneNode::addMaterial(Material* mat)
    {
        mMaterials.push_back(mat);
    }
    
    void SceneNode::deleteMaterial(Material* mat)
    {
        for (int i = 0;i < mMaterials.size();++i)
        {
            if (mMaterials[i] == mat)
            {
                mMaterials.erase(mMaterials.begin() + i);
                break;
            }
        }
    }
    
    void SceneNode::clearMaterial()
    {
        mMaterials.clear();
    }

    Material* SceneNode::getMaterial(int index)
    {
        return mMaterials[index];
    }

    void SceneNode::setColor(Scene* scene, Color color)
    {
        if (mMaterials.size() == 0)
        {
            MaterialManager* matMan = scene->getMaterialManager();
            Material* mat = matMan->createMaterial(MaterialManager::getKeyCounter());
            mat->setAmbientColor(color);
            mat->setDiffuseColor(color);
            mat->setSpecularColor(color);
            mat->getRenderState().disableLighting();
            mMaterials.push_back(mat);
        }
    }

    SceneNode* SceneNode::clone()
    {
        SceneNode* node = HouyiNew SceneNode();
        node->setName(mName);
        node->setMesh(mMesh);
        node->setLocal(mLocal);
        node->setKFs(mKFs, mKFCount);
        for (int i = 0;i < mMaterials.size();++i)
        {
            node->addMaterial(mMaterials[i]);
        }
        for (int i = 0;i < mEntities.size();++i)
        {
            RenderEntity e = mEntities[i];
            e.mNode = node;
            node->addEntity(e);
        }

        for (int i = 0;i < mChildren.size();++i)
        {
            SceneNode* child = (static_cast<SceneNode*>(mChildren[i]))->clone();
            if (child)
            {
                node->addChild(child);
            }
        }

        for (int i = 0;i < node->mChildren.size();++i)
        {
            SceneNode* child = (static_cast<SceneNode*>(node->mChildren[i]));
            if (child->getName().compare("AABB") == 0)
            {
                node->mAABB = static_cast<AABB*>(child);
            }
        }
        return node;
    }

    vector<RenderEntity> SceneNode::getAllEntities()
    {
        vector<RenderEntity> ret;
        for (int i = 0;i < mEntities.size();++i)
        {
            ret.push_back(mEntities[i]);
        }

        for (int i = 0;i < mChildren.size();++i)
        {
            SceneNode* child = static_cast<SceneNode*>(mChildren[i]);
            vector<RenderEntity> childEntities = child->getAllEntities();
            for (int j = 0;j < childEntities.size();++j)
            {
                ret.push_back(childEntities[j]);
            }
        }
        return ret;
    }
    
    void SceneNode::setMeshForChildren(Mesh* mesh)
    {
        for (int i = 0;i < mChildren.size();++i)
        {
            SceneNode* child = static_cast<SceneNode*>(mChildren[i]);
            if (child->getName().compare("AABB") != 0) // skip AABB
            {
                child->setMesh(mesh);
                child->setMeshForChildren(mesh);
            }
        }
    }
}
