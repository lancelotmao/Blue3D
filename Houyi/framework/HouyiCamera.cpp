//
//  HouyiCamera.cpp
//  Houyi
//
//  Created by Lance Mao on 16/2/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#include "HouyiCamera.h"
#include "math.h"
#include "HouyiMatrix3.h"
#include "HouyiLog.h"
#include "HouyiScene.h"
#include "HouyiWorld.h"
#include <iostream>

namespace Houyi
{
    const float Camera::DEF_FOV = 30;
    
    Camera::~Camera()
    {
        /*for (int i = 0;i < mKFPosLength;i++){
         delete [] mKFPos[i];
         }
         delete [] mKFPos;*/

        for (int i = 0; i < mKFTargetPosLength; i++)
        {
            delete[] mKFTargetPos[i];
        }
        delete[] mKFTargetPos;

        for (int i = 0; i < mKFFOVLength; i++)
        {
            delete[] mKFFOV[i];
        }
        delete[] mKFFOV;

        for (int i = 0; i < mKFRollLength; i++)
        {
            delete[] mKFRoll[i];
        }
        delete[] mKFRoll;
    }

    Camera::Camera(float aspectRatio) : mType(Const::ENormal),
            mPosition(0, 0, 1), mLookAt(0, 0, 0), mUp(0, 1, 0), mRight(1, 0, 0), mForward(0, 0, -1),
            mAspectRatio(aspectRatio), mCamAnim(*this)
    {
        init();
        perspective();
    }

    Camera::Camera(Vector3 position, Vector3 lookat, float roll, float aspectRatio) :
    mType(Const::ENormal), mPosition(position), mLookAt(lookat), mRoll(roll), mAspectRatio(aspectRatio), mCamAnim(*this)
    {
        init();
        lookAt(position, lookat, roll);
        perspective();
    }

    void Camera::init()
    {
        mNearRange = 1;
        mFarRange = 1000;
        mFOV = DEF_FOV;
        mRoll = 0;
        if (mAspectRatio == 0)
        {
            mAspectRatio = 1.0f;
        }
//    mMaxKFCount = 1;

//    mKFPos = NULL;
        mKFTargetPos = NULL;
        mKFFOV = NULL;
        mKFRoll = NULL;
//    mKFPosLength = 0;
        mKFTargetPosLength = 0;
        mKFFOVLength = 0;
        mKFRollLength = 0;
        mPeekSpeed = 1;
        mIsPeekAnimEnabled = true;
        mPeekConstraints = 0;
        mPeekStarted = false;
        mProjectionDirty = true;
    }

    Vector3 Camera::getLookAt()
    {
        return mLookAt;
    }

    void Camera::setLookAt(Vector3 v)
    {
        mLookAt = v;
    }
    
    void Camera::setLookAt(float x, float y, float z)
    {
        mLookAt.x = x;
        mLookAt.y = y;
        mLookAt.z = z;
    }

    Vector3 Camera::getPosition()
    {
        return mPosition;
    }

    void Camera::setPosition(Vector3 position)
    {
        mPosition = position;
    }

    void Camera::setPosition(float x, float y, float z)
    {
        mPosition.x = x;
        mPosition.y = y;
        mPosition.z = z;
    }

    Vector3 Camera::getUp()
    {
        return mUp;
    }
    
    void Camera::setUp(Vector3 up)
    {
        mUp = up;
    }

    void Camera::setUp(float x, float y, float z)
    {
        mUp.x = x;
        mUp.y = y;
        mUp.z = z;
    }
    
    Vector3 Camera::getForward(bool normalize) const
    {
        Vector3 forward = mLookAt - mPosition;
        if (normalize)
        {
            forward.normalize();
        }
        return forward;
    }

    Vector3 Camera::getRight(bool normalize) const {
		Vector3 right = mRight;
    	if(normalize) {
    		right.normalize();
    	}

    	return right;
    }

    float Camera::getBankAngle()
    {
        return mBankAngle;
    }

    void Camera::setBankAngle(float angle)
    {
        mBankAngle = angle;
    }

    float Camera::getFocus()
    {
        return mFocus;
    }

    void Camera::setFocus(float focus)
    {
        mFocus = focus;
    }

    float Camera::getNearRange()
    {
        return mNearRange;
    }

    void Camera::setNearRange(float nearrange)
    {
        mNearRange = nearrange;
        mProjectionDirty = true;
    }

    float Camera::getFarRange()
    {
        return mFarRange;
    }

    void Camera::setFarRange(float farrange)
    {
        mFarRange = farrange;
        mProjectionDirty = true;
    }

    float Camera::getFOV()
    {
        return mFOV;
    }

    void Camera::setFOV(float fov)
    {
        mFOV = fov;
        mProjectionDirty = true;
    }

    float Camera::getRoll()
    {
        return mRoll;
    }
    
    void Camera::roll(float roll)
    {
        mRoll += roll;
        lookAt();
    }

    void Camera::setRoll(float roll)
    {
        if (mRoll != roll)
        {
            mRoll = roll;
            lookAt();
        }
    }

    float Camera::getPeekSpeed()
    {
        return mPeekSpeed;
    }

    void Camera::setPeekSpeed(float peekSpeed)
    {
        mPeekSpeed = peekSpeed;
    }

    Matrix4& Camera::getViewMatrix()
    {
        return mViewMatrix;
    }

    float Camera::getAspectRatio()
    {
        return mAspectRatio;
    }

    void Camera::setAspectRatio(float aspectRatio)
    {
        mAspectRatio = aspectRatio;
        mProjectionDirty = true;
    }

    Matrix4& Camera::getProjectionMatrix()
    {
        return mProjectionMatrix;
    }

    float* Camera::getViewProjectionMatrix()
    {
        return mViewProjectionMatrix;
    }

    Matrix3& Camera::getNormalMatrix()
    {
        return mNormalMatrix;
    }

    void Camera::lookAt()
    {
        lookAt(mPosition, mLookAt, mRoll);
    }

    void Camera::lookAt(Vector3 aPosition, Vector3 aLookAt, float aRoll)
    {
        mPosition = aPosition;
        mLookAt = aLookAt;

        mUp.x = 0;
        mUp.y = 1;
        mUp.z = 0;

        mForward = aLookAt - aPosition;
        mForward.normalise();

        // calculating up vector using the following steps:
        // 1. Calculate angle of mForward & x-z plane: theta1
        // 2. Calculate angle of mForward & -z axis: theta2
        // 3. Rotate (0, 1, 0) along x axis by theta1
        // 4. Rotate then along y axis by theta2 or 180-theta2
        // 5. Rotate then along mForward by aRoll
        double theta1 = asin(mForward.y);

        double theta2;
        if (mForward.z == 0)
        {
            theta2 = 0;
        }
        else
        {
            theta2 = atan(mForward.x / mForward.z);
            if (mForward.z > 0)
            {
                theta2 = 3.1415926 / 2 - theta2;
            }
        }

        mUp.y = (float) cos(theta1);
        mUp.z = (float) sin(theta1);

        mUp.z = (float) (mUp.z * cos(theta2) - mUp.x * sin(theta2));
        mUp.x = (float) (mUp.x * cos(theta2) + mUp.z * sin(theta2));

        mUp.rotate(mForward, aRoll);
        // up.iX = up.iX * Cos(aRoll) - up.iY * Sin(aRoll);
        // up.iY = up.iY * Cos(aRoll) + up.iZ * Sin(aRoll);

        mUp.normalize();

        mRight = mForward.crossProduct(mUp);
        mRight.normalize();

        mUp = mRight.crossProduct(mForward);
        mUp.normalize();

        buildViewMatrix();
    }

    void Camera::update(int keyFrame)
    {
//    mCurFrame = keyFrame % mMaxKFCount;
//    
//    bool lookat = false;
//    if (mKFPos != NULL && mKFPosLength > 1){
//        if (mCurFrame >= 0 && mCurFrame < mKFPosLength){
//            mPosition.x = mKFPos[mCurFrame][0];
//            mPosition.y = mKFPos[mCurFrame][1];
//            mPosition.z = mKFPos[mCurFrame][2];
//        }
//        
//        lookat = true;
//    }
//    
//    if (mKFTargetPos != NULL && mKFTargetPosLength > 1){
//        if (mCurFrame >= 0 && mCurFrame < mKFTargetPosLength){
//            mLookAt.x = mKFTargetPos[mCurFrame][0];
//            mLookAt.y = mKFTargetPos[mCurFrame][1];
//            mLookAt.z = mKFTargetPos[mCurFrame][2];
//        }
//        
//        lookat = true;
//    }
//    
//    if (lookat){
//        lookAt();
//    }else{
//        update();
//    }
    }

    void Camera::update()
    {
        if (mPeekAnim.isActive())
        {
            mPeekAnim.process(getScene()->getWorld()->getRoot(), currentTimeMillis());
            float speed = mPeekAnim.getCurrent();
            startPeek();
            peek(mPeekAnimX1, mPeekAnimY1, mPeekAnimX2, mPeekAnimY2, speed);
            endPeek();
        }

        if (!mCamAnim.isActive() || mCamAnim.getAnimType() != CameraAnimation::ETransform)
        {
            mForward = mLookAt - mPosition;
            mForward.normalize();

            mRight = mForward.crossProduct(mUp);
            mRight.normalize();

            mUp = mRight.crossProduct(mForward);
            mUp.normalize();

            buildViewMatrix();
        }
        else
        {
            // matrix set by controller
        }
        
        if (mProjectionDirty)
        {
            project();
            mProjectionDirty = false;
        }
    }

    void Camera::project()
    {
        perspective();
    }

    void Camera::perspective()
    {
        perspective(mFOV, mAspectRatio, mNearRange, mFarRange);
    }

    void Camera::perspective(float fov, float aspectRatio, float near, float far)
    {
        mNearRange = near;
        mFarRange = far;
        mAspectRatio = aspectRatio;

        float f = (float) (1 / tan(Math::DegreesToRadians(fov / 2)));
        mProjectionMatrix[0][0] = f / aspectRatio;
        mProjectionMatrix[0][1] = 0;
        mProjectionMatrix[0][2] = 0;
        mProjectionMatrix[0][3] = 0;

        mProjectionMatrix[1][0] = 0;
        mProjectionMatrix[1][1] = f;
        mProjectionMatrix[1][2] = 0;
        mProjectionMatrix[1][3] = 0;

        mProjectionMatrix[2][0] = 0;
        mProjectionMatrix[2][1] = 0;
        mProjectionMatrix[2][2] = (near + far) / (near - far);
        mProjectionMatrix[2][3] = 2 * near * far / (near - far);

        mProjectionMatrix[3][0] = 0;
        mProjectionMatrix[3][1] = 0;
        mProjectionMatrix[3][2] = -1;
        mProjectionMatrix[3][3] = 0;

        mInvProjectionMatrix = mProjectionMatrix.inverse();
    }

    void Camera::buildViewMatrix()
    {
        mViewMatrix[0][0] = mNormalMatrix[0][0] = mRight.x;
        mViewMatrix[0][1] = mNormalMatrix[0][1] = mRight.y;
        mViewMatrix[0][2] = mNormalMatrix[0][2] = mRight.z;
        mViewMatrix[0][3] = -mPosition.x * mRight.x - mPosition.y * mRight.y - mPosition.z * mRight.z;

        mViewMatrix[1][0] = mNormalMatrix[1][0] = mUp.x;
        mViewMatrix[1][1] = mNormalMatrix[1][1] = mUp.y;
        mViewMatrix[1][2] = mNormalMatrix[1][2] = mUp.z;
        mViewMatrix[1][3] = -mPosition.x * mUp.x - mPosition.y * mUp.y - mPosition.z * mUp.z;

        mViewMatrix[2][0] = mNormalMatrix[2][0] = -mForward.x;
        mViewMatrix[2][1] = mNormalMatrix[2][1] = -mForward.y;
        mViewMatrix[2][2] = mNormalMatrix[2][2] = -mForward.z;
        mViewMatrix[2][3] = mPosition.x * mForward.x + mPosition.y * mForward.y + mPosition.z * mForward.z;

        mViewMatrix[3][0] = 0;
        mViewMatrix[3][1] = 0;
        mViewMatrix[3][2] = 0;
        mViewMatrix[3][3] = 1;
    }

    void Camera::buildInvViewMatrix()
    {
        mForward = mLookAt - mPosition;
        mForward.normalize();
        mRight = mForward.crossProduct(mUp);
        mRight.normalize();
        mUp = mRight.crossProduct(mForward);
        mUp.normalize();
        mViewInvMatrix[0][0] = mRight.x;
        mViewInvMatrix[0][1] = mUp.x;
        mViewInvMatrix[0][2] = -mForward.x;
        mViewInvMatrix[0][3] = mPosition.x;
        mViewInvMatrix[1][0] = mRight.y;
        mViewInvMatrix[1][1] = mUp.y;
        mViewInvMatrix[1][2] = -mForward.y;
        mViewInvMatrix[1][3] = mPosition.y;
        mViewInvMatrix[2][0] = mRight.z;
        mViewInvMatrix[2][1] = mUp.z;
        mViewInvMatrix[2][2] = -mForward.z;
        mViewInvMatrix[2][3] = mPosition.z;
        mViewInvMatrix[3][0] = 0;
        mViewInvMatrix[3][1] = 0;
        mViewInvMatrix[3][2] = 0;
        mViewInvMatrix[3][3] = 1;
    }
    
    Vector3 Camera::getSpaceDimension(float screenWidth, float screenHeight)
    {
        float fov = getFOV();
        Vector3 forward = mPosition - mLookAt;
        float spaceh = tan(Math::DegreesToRadians(fov/2)) * forward.length();
        float spacew = spaceh * screenWidth / screenHeight;
        return Vector3(spacew, spaceh, 0);
    }
    
    void Camera::pan(float screenWidth, float screenHeight, float dx, float dy)
    {
        Vector3 v = getSpaceDimension(screenWidth, screenHeight);
        float w = v.x;
        float h = v.y;
        // movement on the plane where mLookAt is
        Vector3 delta = Vector3(w * dx, -h * dy, 0);
        float length = delta.length();
        // our delta is actually mRight movement in eye space
        // we need to transform it back to world space
        // since camera rotation is orthogonal and normalized so inverse is transpose
        Matrix3 r;
        mViewMatrix.extract3x3Matrix(r);
        r = r.Transpose();
        delta.normalise();
        delta = r * delta * length;

        // reverse direction as when user pan to the right
        // camera should actually move to the left
        Vector3 lookat = mLookAt - delta;
        setLookAt(lookat);
        Vector3 pos = mPosition - delta;
        setPosition(pos);
    }
    
    void Camera::present(float speed)
    {
        mCamAnim.startPresentation(speed);
    }
    
    void Camera::stopPresent()
    {
        mCamAnim.stopPresentation();
    }

    Vector3 Camera::clipToWorld(float screenX, float screenY)
    {
        // 1. transform screen coordinates into eye coordinates
        Vector3 eyeV = mInvProjectionMatrix * Vector3(screenX, screenY, 0);

        // 2. transform eye coordinates into world coordinates using inverse of view matrix
        // note the view matrix is an orthogonal matrix, so we can get the inverse of
        // this sub matrix immediately because its inverse is its transpose.
        // Then we multiply translation to get the final matrix
        buildInvViewMatrix();
        return mViewInvMatrix * eyeV;
    }

    Ray Camera::castRay(float ndcX, float ndcY)
    {
        Vector3 p = clipToWorld(ndcX, ndcY);
        Vector3 d = p - mPosition;
        d.normalize();
        return Ray(mPosition, d);
    }

    Ray Camera::castRay(float screenX, float screenY, float viewportWidth, float viewportHeight)
    {
        float ndcX = screenX * 2 / viewportWidth - 1;
        float ndcY = 1 - screenY * 2 / viewportHeight;
        return castRay(ndcX, ndcY);
    }

    void Camera::startPeek()
    {
		mPeekStartPosition = mPosition;
		mPeekStartUp = mUp;
		mPeekStarted = true;
    }

    void Camera::peek(float xs1, float ys1, float xs2, float ys2)
    {
        peek(xs1, ys1, xs2, ys2, mPeekSpeed);
    }
    
    void Camera::peek(float xs1, float ys1, float xs2, float ys2, float speed)
    {
        if (!mPeekStarted)
        {
            LOGW("peek not started, call startPeek() first\n");
            return;
        }

        // cannot peek either horizontally or vertically
        if (mPeekConstraints == 3)
        {
            return;
        }

        if ((mPeekConstraints & 1) != 0)
        {
            xs2 = xs1;
        }
        if ((mPeekConstraints & 2) != 0)
        {
            ys2 = ys1;
        }

        // first check if two points are very close, no need to rotate
        float dis = xs1 - xs2 + ys1 - ys2;
        if (dis > -0.0001 && dis < 0.0001)
        {
            return;
        }

        mPosition = mPeekStartPosition;
        mUp = mPeekStartUp;

        // 1&2 transform from clip to eye then eye to world
        Vector3 v1 = clipToWorld(xs1, ys1);
        Vector3 v2 = clipToWorld(xs2, ys2);

        // 3. calculate rotate vector VR which is the cross product of
        // mForward&(P2 - P1)
        Vector3 vr = (mLookAt - mPosition).crossProduct(v2 - v1);
        vr.normalize();

        // 4. rotate camera position along VR by length(P1-P2) * coefficient
        float length = (xs2 - xs1) * (xs2 - xs1) + (ys2 - ys1) * (ys2 - ys1);
        mPosition -= mLookAt;
        mPosition.rotate(vr, length * speed);
        mPosition += mLookAt;

        // 5. up vector rotate in the same way as position
        mUp.rotate(vr, length * speed);

        // 6. update camera. this will update view matrix also
        //update(/**mCurFrame**/);
    }

    void Camera::endPeek()
    {
        mPeekStartPosition = mPosition;
        mPeekStartUp = mUp;
        mPeekStarted = false;
    }
    
    void Camera::startPeekAnim(float xs1, float ys1, float xs2, float ys2, double lastMoveTime, double upTime)
    {
        if (!mIsPeekAnimEnabled)
        {
            return;
        }
        
        stopPeekAnim();

        mPeekAnimX1 = xs1;
        mPeekAnimY1 = ys1;
        mPeekAnimX2 = xs2;
        mPeekAnimY2 = ys2;
        
        double speed = Math::Clamp<double>(100.0 / (upTime - lastMoveTime), 0, 100);
        double duration = Math::Clamp<double>(1000 * speed, 0, 3000);
        mPeekAnim.setDuration(duration);
        mPeekAnim.start(getScene()->getWorld()->getRoot(), speed, 0);
    }
    
    void Camera::startPeekAnim(float vx, float vy)
    {
        if (!mIsPeekAnimEnabled)
        {
            return;
        }
        
        stopPeekAnim();
        
        mPeekAnimX1 = 0;
        mPeekAnimY1 = vy;
        mPeekAnimX2 = vx;
        mPeekAnimY2 = 0;
        
        double v = sqrt(vx*vx + vy*vy);
        double speed = 6 * v;
        double duration = Math::Clamp<double>(v * 3000, 0, 3000);
        mPeekAnim.setDuration(duration);
        mPeekAnim.start(getScene()->getWorld()->getRoot(), speed, 0);
    }
    
    void Camera::stopPeekAnim()
    {
        mPeekAnim.stop();
    }
    
    void Camera::enablePeekAnim(bool enable)
    {
        mIsPeekAnimEnabled = enable;
    }
    
    void Camera::disablePeekAnim()
    {
        mIsPeekAnimEnabled = false;
    }
    
    bool Camera::isPeekAnimEnabled()
    {
        return mIsPeekAnimEnabled;
    }

    int Camera::getPeekConstraint()
    {
        return mPeekConstraints;
    }
    
    void Camera::setPeekConstraint(int constraint)
    {
        mPeekConstraints = constraint;
    }

    void Camera::setKFTargetPos(int index, float x, float y, float z)
    {
        mKFTargetPos[index][0] = x;
        mKFTargetPos[index][1] = y;
        mKFTargetPos[index][2] = z;
    }

    void Camera::setupKeyFrame()
    {
        // interpolate position
//    for (int i = 0, start = 0;i < mKFPosLength;i++){
//        float x = mKFPos[i][0];
//        float y = mKFPos[i][1];
//        float z = mKFPos[i][2];
//        
//        if (start != i && (x != 0 || y != 0 || z != 0)){
//            int len = i - start;
//            float stepX = (mKFPos[i][0] - mKFPos[start][0]) / len;
//            float stepY = (mKFPos[i][1] - mKFPos[start][1]) / len;
//            float stepZ = (mKFPos[i][2] - mKFPos[start][2]) / len;
//            for (int j = start;j < i;j++){
//                float deltaX = (j - start) * stepX;
//                float deltaY = (j - start) * stepY;
//                float deltaZ = (j - start) * stepZ;
//                mKFPos[j][0] = mKFPos[start][0] + deltaX;
//                mKFPos[j][1] = mKFPos[start][1] + deltaY;
//                mKFPos[j][2] = mKFPos[start][2] + deltaZ;
//            }
//            start = i;
//        }
//    }
//    
//    //    cout << "Set camera key. Key frame count = " << mKFPosLength << endl;
//    //    for (int i = 0;i < mKFPosLength;i++){
//    //        cout << "KF " << i
//    //                << ": POSX = "
//    //                << mKFPos[i][0] << "; POSY = "
//    //                << mKFPos[i][1] << "; POSZ = "
//    //                << mKFPos[i][2] << endl;
//    //    }
//    
//    // interpolate target position
//    for (int i = 0, start = 0;i < mKFTargetPosLength;i++){
//        float x = mKFTargetPos[i][0];
//        float y = mKFTargetPos[i][1];
//        float z = mKFTargetPos[i][2];
//        
//        if (start != i && (x != 0 || y != 0 || z != 0)){
//            int len = i - start;
//            float stepX = (mKFTargetPos[i][0] - mKFTargetPos[start][0]) / len;
//            float stepY = (mKFTargetPos[i][1] - mKFTargetPos[start][1]) / len;
//            float stepZ = (mKFTargetPos[i][2] - mKFTargetPos[start][2]) / len;
//            for (int j = start;j < i;j++){
//                float deltaX = (j - start) * stepX;
//                float deltaY = (j - start) * stepY;
//                float deltaZ = (j - start) * stepZ;
//                mKFTargetPos[j][0] = mKFTargetPos[start][0] + deltaX;
//                mKFTargetPos[j][1] = mKFTargetPos[start][1] + deltaY;
//                mKFTargetPos[j][2] = mKFTargetPos[start][2] + deltaZ;
//            }
//            start = i;
//        }
//    }

        //		Log.d(HEngine.TAG, "Set camera key. Key frame count = " + mKFTargetPos.length);
        //		for (int i = 0;i < mKFTargetPos.length;i++){
        //			Log.d(HEngine.TAG, "KF " + i
        //					+ ": POSX = "
        //					+ mKFTargetPos[i][0] + "; POSY = "
        //					+ mKFTargetPos[i][1] + "; POSZ = "
        //					+ mKFTargetPos[i][2]);
        //		}
    }

    void Camera::setViewMatrix(Matrix3 r, Vector3 t)
    {
        mViewMatrix = r;
        mViewMatrix[0][3] = t.x;
        mViewMatrix[1][3] = t.y;
        mViewMatrix[2][3] = t.z;
    }

    void Camera::writeObject(DataOutputStream& dos)
    {
        dos.write(&mPosition, sizeof(Vector3));
        dos.write(&mLookAt, sizeof(Vector3));
        dos.write(&mUp, sizeof(Vector3));
        dos.writeFloat(mAspectRatio);
        dos.writeFloat(mFOV);
        dos.writeFloat(mRoll);
        dos.writeFloat(mNearRange);
        dos.writeFloat(mFarRange);
    }

    void Camera::readObject(DataInputStream& dis)
    {
        dis.read(&mPosition, sizeof(Vector3));
        dis.read(&mLookAt, sizeof(Vector3));
        dis.read(&mUp, sizeof(Vector3));
        dis.read(&mAspectRatio, sizeof(float));
        dis.read(&mFOV, sizeof(float));
        dis.read(&mRoll, sizeof(float));
        dis.read(&mNearRange, sizeof(float));
        dis.read(&mFarRange, sizeof(float));
        mCamAnim.saveForAnim();
    }
}
