#ifndef Houyi_HouyiRigidBody_h
#define Houyi_HouyiRigidBody_h

#include "HouyiForce.h"
#include "HouyiSceneNode.h"

namespace Houyi
{
    class RigidBody : public Object
    {
    public:
        RigidBody(SceneNode* node);
        virtual ~RigidBody();

        virtual void update();
        virtual void applyForce(Force force);
        
    protected:
        virtual void calculateMassCenter();
        virtual void calculatePivot(float angle);

    private:
        SceneNode* mSceneNode;
        double mLastUpdatedTime;
        Vector3 mMassCenter;
        Vector3 mPivot;
        Vector3 mTorque;
        float mAgularAccel;
        float mAgularVelocity;
        Vector3 mForceNormal;
        float mForceMag;

        vector<Force> mForces;
    };
}
#endif
