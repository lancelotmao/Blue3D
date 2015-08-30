#include "HouyiCameraAnimation.h"
#include "HouyiCamera.h"
#include "HouyiMatrix3.h"

namespace Houyi
{
    CameraAnimData::CameraAnimData(Camera* cam)
    {
        lookAt = cam->getLookAt();
        position = cam->getPosition();
        roll = cam->getRoll();
    }
    
    void CameraAnimation::onAnimation(double ratio)
    {
        if (mAnimType == ETransform)
        {
            Quaternion q = Quaternion::Slerp(ratio, mSourceAnimData.q, mTargetAnimData.q, true);
            Matrix3 rm;
            q.ToRotationMatrix(rm);

            Vector3 t = mSourceAnimData.t + (mTargetAnimData.t - mSourceAnimData.t) * ratio;
            mCamera.setViewMatrix(rm, t);
        }
        else if (mAnimType == EPresent)
        {
            mCamera.startPeek();
            mCamera.peek(0, 1, mPresentationSpeed, 1);
            mCamera.endPeek();
        }
        else if (mAnimType == EComponent)
        {
            Vector3 pos = mSourceAnimData.position + (mTargetAnimData.position - mSourceAnimData.position) * ratio;
            Vector3 lookat = mSourceAnimData.lookAt + (mTargetAnimData.lookAt - mSourceAnimData.lookAt) * ratio;
            float roll = mSourceAnimData.roll + (mTargetAnimData.roll - mSourceAnimData.roll) * ratio;
            mCamera.setPosition(pos);
            mCamera.setLookAt(lookat);
            mCamera.setRoll(roll);
        }
    }

    void CameraAnimation::onAnimationStop()
    {
    	if (mAnimType == ETransform)
    	{
			mCamera.setLookAt(mTargetAnimData.lookAt);
			mCamera.setPosition(mTargetAnimData.position);
			mCamera.setRoll(mTargetAnimData.roll);
			mCamera.lookAt();
			mCamera.endPeek();
    	}
    }

    void CameraAnimation::saveForAnim()
    {
        mTargetAnimData.lookAt = mCamera.getLookAt();
        mTargetAnimData.position = mCamera.getPosition();
        mTargetAnimData.roll = mCamera.getRoll();

        mCamera.lookAt();
        Matrix3 m(mCamera.getNormalMatrix());
        mTargetAnimData.q.FromRotationMatrix(m);
        const Matrix4& m4 = mCamera.getViewMatrix();
        mTargetAnimData.t.x = m4[0][3];
        mTargetAnimData.t.y = m4[1][3];
        mTargetAnimData.t.z = m4[2][3];
    }

    void CameraAnimation::startTransAnim()
    {
        Matrix3 m(mCamera.getNormalMatrix());
        mSourceAnimData.q.FromRotationMatrix(m);
        const Matrix4& m4 = mCamera.getViewMatrix();
        mSourceAnimData.t.x = m4[0][3];
        mSourceAnimData.t.y = m4[1][3];
        mSourceAnimData.t.z = m4[2][3];

        mAnimType = ETransform;
        setInterpolationType(Animation::EDecelerate);
        setDuration(400);
        start();
    }
    
    void CameraAnimation::startComponentAnim(double duration)
    {
        mSourceAnimData.position = mCamera.getPosition();
        mSourceAnimData.lookAt = mCamera.getLookAt();
        mSourceAnimData.roll = mCamera.getRoll();
        
        mAnimType = EComponent;
        setInterpolationType(Animation::EDecelerate);
        setDuration(duration);
        start();
    }
    
    void CameraAnimation::startPresentation(float speed)
    {
        mPresentationSpeed = speed;
        mAnimType = EPresent;
        setDuration(FLT_MAX);
        start();
    }
    
    void CameraAnimation::stopPresentation()
    {
        stop();
    }
}
