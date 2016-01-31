#ifndef HOUYIVIEW_H_
#define HOUYIVIEW_H_

#include <vector>

#include "HouyiObject.h"
#include "HouyiGestureDetector.h"
#include "HRect.h"
#include "HouyiSceneNode.h"
#include "HouyiRoot.h"
#include "HouyiSceneNodeAnimation.h"

namespace Houyi
{
    class Scene;
    class OnClickListener;
    class OnTouchListener;
    class DragImageView;
    
    class View : public SceneNode, public GestureListener
	{
	public:
        enum
        {
            EHAlignLeft, EHAlignCenter, EHAlignRight
        };

        enum
        {
            EVAlignTop, EVAlignCenter, EVAlignBottom
        };

        static const bool DEBUG_EVENT;

        // typically in GL, space is inside a normalized cube with
        // x [-1, 1]; y [-1, 1]; z [-1, 1]. Thus height is 2
        static const float SPACE_HEIGHT;
        
        static float SPACE_WIN_RATIO;
        
		View();
		virtual ~View();

        // screen space coordinate
		virtual int getWidth(bool includeScale = false);
		virtual int getHeight(bool includeScale = false);
        virtual void setBound(const HRect& rect, int halign = EHAlignLeft, int valign = EVAlignTop);
        virtual const HRect& getBound() const;
        
        // 3D space
        virtual float getSpaceWidth(bool includeScale = false);
        virtual float getSpaceHeight(bool includeScale = false);

        virtual void removeChildView(View* child);
        virtual void removeFromParent() override;
        virtual View* getRootView();
        View* _getParent();
        View* _getChildAt(int index);

		virtual void attachToScene(Scene* scene);
        virtual void detachFromScene(Scene* scene);
        
        virtual bool onUpdate() override;

        inline bool isLayoutRequested()
        {
            return mRequestLayout;
        }
        
        virtual void layout(const HRect& bound, Scene* scene);
        virtual bool onLayout(const HRect& bound, Scene* scene);
        virtual void requestLayout();

        virtual bool dispatchTouchEvent(MotionEvent e);
        virtual bool onTouch(MotionEvent e);
        virtual void setOnClickListener(OnClickListener* listener);
        virtual void setOnTouchListener(OnTouchListener* listener);
        virtual bool onSingleTapUp(const MotionEvent& e) override;
        virtual bool onFling(const MotionEvent& e, float vx, float vy) override;

        virtual void invalidate();
        virtual void invalidate(HRect dirty);

        inline float getScaleX()
        {
            return mScaleX;
        }
        
        inline float getScaleY()
        {
            return mScaleY;
        }
        
        inline float getScaleZ()
        {
            return mScaleZ;
        }
        
        void setTranslate(Vector3 translate);

        inline Vector3 getTranslate()
        {
            return mTranslate;
        }

        inline void setScale(float sx, float sy, float sz)
        {
            mScaleX = sx;
            mScaleY = sy;
            mScaleZ = sz;
        }
        
        inline float getRoll()
        {
            return mRoll;
        }

        inline void setRoll(float roll)
        {
            mRoll = roll;
        }

//        virtual void translate(Vector3 trans);
        virtual void roll(float roll);
//        virtual void rotate(float x, float y, float z, float radian);
//        virtual void scale(float sx, float sy, float sz);
        virtual void resetTransform();
        
        virtual void onRenderBegin(Pass* pass);
        virtual void onRenderEnd(Pass* pass);

        virtual void cancel();

        virtual void startAlphaAnimation(float from, float to, double duration, double delay, int interpolateType = Animation::ELinear);
        virtual void fadeOut(double duration, double delay, int interpolateType = Animation::ELinear);
        virtual void fadeIn(double duration, double delay, int interpolateType = Animation::ELinear);
        
        inline void setGravity(int h, int v)
        {
            mHGravity = h;
            mVGravity = v;
        }

	protected:
		Root* mRoot;
        Scene* mAttachedScene;
		HRect mBound;
        bool mAttached;
        bool mRequestLayout;

        // set after layout
        Vector3 mPos;
        Vector3 mScale;

        // extra transforms
        float mScaleX;
        float mScaleY;
        float mScaleZ;
        Vector3 mTranslate;
        float mRoll;

        // for some view after activated
        bool mAcceptOutofBoundEvent;
        
        OnClickListener* mOnClickListener;
        OnTouchListener* mOnTouchListener;

        GestureDetector mGestureDetector;
        Material* mMaterial;

        // alignment on parent view (or layout)
        int mHAlignment;
        int mVAlignment;

        // alignment for sub views
        int mHGravity;
        int mVGravity;

        bool mIsActive;

        SceneNodeAnimation* mAlphaAnimation;
	};
    
    class OnClickListener
    {
    public:
        virtual ~OnClickListener(){}
        virtual void onClick(View* v) = 0;
    };

    class OnTouchListener
    {
    public:
        virtual ~OnTouchListener(){}
        virtual bool onTouch(MotionEvent event, View* v) = 0;
    };
}
#endif
