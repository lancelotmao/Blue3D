#include "HouyiRigidBody.h"
#include "HouyiAABB.h"

namespace Houyi
{
    RigidBody::RigidBody(SceneNode* node) : mSceneNode(node), mAgularAccel(0), mAgularVelocity(0)
    {
        mSceneNode->updateViewMatix();
        mSceneNode->generateAABB(static_cast<SceneNode*>(node->getRoot())->getScene());
        mLastUpdatedTime = currentTimeMillis();
    }

    RigidBody::~RigidBody()
    {

    }

    void RigidBody::update()
    {
        // update mass center since mesh might be transformed
        calculateMassCenter();
        
        double now = currentTimeMillis();
        double dt = now - mLastUpdatedTime;
                     
        if (mForces.size() > 0)
        {
            Force f = mForces[0];
            Vector3 d = f.mPosition - mMassCenter;
            mTorque = f.mNormal.crossProduct(d);
            
            mAgularVelocity = f.mMag;
            mForces.clear();
        }

        float angle = mAgularVelocity * dt / 1000;
        
        // calculate pivot point
        calculatePivot(angle);
        
        mSceneNode->translate(-mPivot.x, -mPivot.y, -mPivot.z);
        mSceneNode->rotate(mTorque.x, mTorque.y, mTorque.z, angle);
        mSceneNode->translate(mPivot.x, mPivot.y, mPivot.z);
        
        mLastUpdatedTime = now;
    }

    void RigidBody::applyForce(Force force)
    {
        force.mOnTime = currentTimeMillis();
        mForces.push_back(force);
    }
    
    void RigidBody::calculateMassCenter()
    {
        AABB* aabb = mSceneNode->getAABB();
        float* vertices = aabb->getAABBVertices();
        
        float minX = FLT_MAX;
        float minY = FLT_MAX;
        float minZ = FLT_MAX;
        float maxX = -FLT_MIN;
        float maxY = -FLT_MIN;
        float maxZ = -FLT_MIN;
        for (int i = 0;i < 8;++i)
        {
            Vector3 src(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
            Vector3 vdst = mSceneNode->getLocal2World() * src;
            if (vdst.x < minX){
                minX = vdst.x;
            }
            if (vdst.y < minY){
                minY = vdst.y;
            }
            if (vdst.z < minZ){
                minZ = vdst.z;
            }
            if (vdst.x > maxX){
                maxX = vdst.x;
            }
            if (vdst.y > maxY){
                maxY = vdst.y;
            }
            if (vdst.z > maxZ){
                maxZ = vdst.z;
            }
        }
        
        mMassCenter.x = (maxX + minX) / 2;
        mMassCenter.y = (maxY + minY) / 2;
        mMassCenter.z = (maxZ + minZ) / 2;
    }
    
    void RigidBody::calculatePivot(float angle)
    {
        AABB* aabb = mSceneNode->getAABB();
        float* vertices = aabb->getAABBVertices();
        
        Matrix4 trans;
        trans.translate(-mMassCenter.x, -mMassCenter.y, -mMassCenter.z);
        trans.rotate(mTorque.x, mTorque.y, mTorque.z, angle);
        trans.translate(mMassCenter.x, mMassCenter.y, mMassCenter.z);
        
        float minY = FLT_MAX;
        for (int i = 0;i < 8;++i)
        {
            Vector3 src(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
            Vector3 v = mSceneNode->getLocal2World() * src;
            Vector3 hv(v.x, v.y, v.z);
            Vector3 vd = trans * hv;
            
            // TODO real collision test here
            if (vd.y < minY)
            {
                mPivot = hv;
                minY = vd.y;
            }
        }
    }
}
