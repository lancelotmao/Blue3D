#ifndef __Houyi__HouyiCameraAnimation__
#define __Houyi__HouyiCameraAnimation__

#include "HouyiObject.h"
#include "HouyiVector3.h"
#include "HouyiQuaternion.h"
#include "HouyiAnimation.h"

namespace Houyi
{
    class Camera;

    struct CameraAnimData
    {
    public:
        CameraAnimData(){}
        CameraAnimData(Camera* cam);
        Vector3 lookAt;
        Vector3 position;
        float roll;
        Quaternion q; // rotation
        Vector3 t; // translate
    };

    class CameraAnimation : public Animation, public AnimationListener
    {
    public:
        enum
        {
            EPeek,
            ETransform,
            EPresent,
            EComponent
        };
        
        CameraAnimation(Camera& c) : mCamera(c), mPresentationSpeed(0.1)
        {
            mListener = this;
        }

        virtual ~CameraAnimation()
        {
            stop();
        }

        virtual void onAnimation(double ratio);
        virtual void onAnimationStop();

        void saveForAnim();
        void startTransAnim();
        void startComponentAnim(double duration);
        
        void startPresentation(float speed = 0.1);
        void stopPresentation();
        inline bool isPresenting()
        {
            return mAnimType == EPresent && isActive();
        }
        inline int getAnimType()
        {
            return mAnimType;
        }
        
        inline void setAnimType(int type)
        {
            mAnimType = type;
        }
        
        inline void setSourceAnimData(const CameraAnimData data)
        {
            mSourceAnimData = data;
        }
        
        inline const CameraAnimData& getTargetAnimData()
        {
            return mTargetAnimData;
        }
        
        inline void setTargetAnimData(CameraAnimData data)
        {
            mTargetAnimData = data;
        }

    private:
        Camera& mCamera;
        CameraAnimData mTargetAnimData;
        CameraAnimData mSourceAnimData;
        int mAnimType;
        float mPresentationSpeed; // in radian
    };
}

#endif
