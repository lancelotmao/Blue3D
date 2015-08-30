//
//  HouyiCamera.h
//  Houyi
//
//  Created by Lance Mao on 16/2/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef __Houyi__HouyiCamera__
#define __Houyi__HouyiCamera__

#include "HouyiQuaternion.h"
#include "HouyiAnimation.h"
#include "HouyiSceneNode.h"
#include "HouyiRay.h"
#include "HouyiCameraAnimation.h"
#include "HouyiSerializable.h"

namespace Houyi
{
    class Camera: public SceneNode, public Serializable
    {
    public:
        static const float DEF_FOV;

    public:
        virtual ~Camera();
        Camera(float aspectRatio = 0);
        Camera(Vector3 position, Vector3 lookat, float roll, float aspectRatio = 0);
        void init();

        virtual void prepare(Scene* scene){}

        inline int getType()
        {
            return mType;
        }

        Vector3 getLookAt();
        void setLookAt(Vector3 v);
        void setLookAt(float x, float y, float z);
        Vector3 getPosition();
        void setPosition(Vector3 position);
        void setPosition(float x, float y, float z);
        Vector3 getUp();
        void setUp(Vector3 up);
        void setUp(float x, float y, float z);
        Vector3 getForward(bool normalize) const;
        Vector3 getRight(bool normalize) const;
        float getBankAngle();
        void setBankAngle(float angle);
        float getFocus();
        void setFocus(float focus);
        float getNearRange();
        void setNearRange(float nearrange);
        float getFarRange();
        void setFarRange(float farrange);
        float getFOV();
        void setFOV(float fov);
        float getRoll();
        void roll(float roll);
        void setRoll(float roll);
        float getPeekSpeed();
        void setPeekSpeed(float peekSpeed);
        float getAspectRatio();
        void setAspectRatio(float aspectRatio);
        
        inline void setProjectionDirty(bool isDirty)
        {
            mProjectionDirty = isDirty;
        }

        Matrix4& getViewMatrix();
        Matrix4& getProjectionMatrix();
        float* getViewProjectionMatrix();
        Matrix3& getNormalMatrix();

        virtual void lookAt();
        virtual void lookAt(Vector3 aPosition, Vector3 aLookAt, float aRoll);

        virtual void update(int keyFrame);
        virtual void update();

        virtual void project();
        virtual void perspective();
        virtual void perspective(float fov, float aspectRatio, float near, float far);

        virtual Vector3 clipToWorld(float screenX, float screenY);
        virtual Ray castRay(float ndcX, float ndcY);
        virtual Ray castRay(float screenX, float screenY, float viewportWidth, float viewportHeight);

        virtual void startPeek();
        virtual void peek(float xs1, float ys1, float xs2, float ys2);
        virtual void endPeek();
        virtual void startPeekAnim(float xs1, float ys1, float xs2, float ys2, double lastMoveTime, double upTime);
        virtual void startPeekAnim(float vx, float vy);
        virtual void stopPeekAnim();
        virtual void enablePeekAnim(bool enable = true);
        virtual void disablePeekAnim();
        virtual bool isPeekAnimEnabled();
        virtual int getPeekConstraint();
        virtual void setPeekConstraint(int constraint);

        //virtual void setTransform(HTransform trans);

        virtual void setKFTargetPos(int index, float x, float y, float z);
        virtual void setupKeyFrame();
        
        void buildViewMatrix();
        void buildInvViewMatrix();
        
        // assuming a plane cross look at and perpendicular to forward
        // this plane will be screen width and height in 3D space
        Vector3 getSpaceDimension(float screenWidth, float screenHeight);
        
        // dx & dy are normalized value. e.g. dx = (deltaX touch event) / w
        virtual void pan(float screenWidth, float screenHeight, float dx, float dy);
        virtual void present(float speed = 0.1);
        virtual void stopPresent();
        
        inline CameraAnimation& getAnimController()
        {
            return mCamAnim;
        }

        void setViewMatrix(Matrix3 r, Vector3 t);

        virtual void writeObject(DataOutputStream& dos);
        virtual void readObject(DataInputStream& dis);

    public:
        float** mKFTargetPos;
        int mKFTargetPosLength;
        float** mKFFOV;
        int mKFFOVLength;
        float** mKFRoll;
        int mKFRollLength;
    protected:
        virtual void peek(float xs1, float ys1, float xs2, float ys2, float speed);
        
    protected:
        int mType;

        /** Position of the camera's eye point. */
        Vector3 mPosition;
        /** Position of the "look-at" direction point. */
        Vector3 mLookAt;
        /** Direction of the up vector. */
        Vector3 mUp;

        Vector3 mForward;
        Vector3 mRight;
        Matrix4 mViewMatrix;
        Matrix4 mViewInvMatrix;
        Matrix4 mProjectionMatrix;
        Matrix4 mInvProjectionMatrix;
        Matrix3 mNormalMatrix;
        
        // project * view matrix
        float mViewProjectionMatrix[16];
        
        float mBankAngle;
        float mFocus;
        float mNearRange;
        float mFarRange;
        float mRoll;
        float mFOV;
        float mAspectRatio;
        bool mProjectionDirty;
        
        // check demo to see touch position is mapped to [-1,1]
        float mPeekSpeed;
        Vector3 mPeekStartPosition;
        Vector3 mPeekStartUp;
        bool mIsPeekAnimEnabled;
        Animation mPeekAnim;
        float mPeekAnimX1;
        float mPeekAnimY1;
        float mPeekAnimX2;
        float mPeekAnimY2;
        int mPeekConstraints;
        bool mPeekStarted;
        
        CameraAnimation mCamAnim;
    };
}

#endif /* defined(__Houyi__HouyiCamera__) */
