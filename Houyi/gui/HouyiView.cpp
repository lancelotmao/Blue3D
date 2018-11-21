#include "HouyiView.h"
#include "HouyiRenderer.h"
#include "HouyiMaterialManager.h"

namespace Houyi
{
    const bool View::DEBUG_EVENT = false;
    const float View::SPACE_HEIGHT = 2;
    float View::SPACE_WIN_RATIO = 1;
    
	View::View() : mWorld(0), mBound(0, 0, 0, 0), mAttached(false), mRequestLayout(true), mScaleX(1),
    mScaleY(1), mScaleZ(1), mRoll(0), mAcceptOutofBoundEvent(false), mOnClickListener(0),
    mGestureDetector(this), mHAlignment(EHAlignLeft), mVAlignment(EVAlignTop), mIsActive(false),
    mAlphaAnimation(0), mAttachedScene(0), mHGravity(EHAlignLeft), mVGravity(EVAlignTop), mOnTouchListener(0)
	{
		mName = "View";
		mMaterial = HouyiNew Material();
		mMaterial->setTextureMode(RenderState::EModulate);
		mMaterial->setTransparentMode(Material::EView);
		mMaterial->setDiffuseColor(0xffffffff);
		mMaterial->getRenderState().disableLighting();
        mMaterial->getRenderState().disableDepthTest();
        mMaterial->setPassConfigurable(false);
        addMaterial(mMaterial);
	}

	View::~View()
	{
        HouyiDelete(mMaterial);
        HouyiDelete(mAlphaAnimation);
	}

	int View::getWidth(bool includeScale)
	{
        return mBound.mWidth * (includeScale ? mScaleX : 1);
	}

	int View::getHeight(bool includeScale)
	{
		return mBound.mHeight * (includeScale ? mScaleY : 1);
	}
    
    void View::setBound(const HRect& rect, int halign, int valign)
    {
        mHAlignment = halign;
        mVAlignment = valign;
        if (mBound != rect)
        {
            mBound = rect;
            requestLayout();
        }
    }
    
    const HRect& View::getBound() const
    {
        return mBound;
    }
    
    float View::getSpaceWidth(bool includeScale)
    {
        return getWidth(includeScale) * SPACE_WIN_RATIO;
    }

    float View::getSpaceHeight(bool includeScale)
    {
        return getHeight(includeScale) * SPACE_WIN_RATIO;
    }
    
    void View::removeChildView(View* child)
    {
        if (child)
        {
            for (int i = 0, n = mChildren.size();i < n;++i)
            {
                if (mChildren[i] == child)
                {
                    child->mParent = 0;
                    child->detachFromScene(child->mAttachedScene);
                    mChildren.erase(mChildren.begin() + i);
                    LOGD("View::removeChild, child name = %s, this = %s\n", child->getName().c_str(), getName().c_str());
                    break;
                }
            }
        }
    }
    
    void View::removeFromParent()
    {
        if (mParent)
        {
            mParent->removeChild(this);
        }
    }
    
    View* View::getRootView()
    {
        View* p = _getParent();
        while (p && p->mParent)
        {
            p = p->_getParent();
        }
        return p;
    }

    View* View::_getParent()
    {
        return (View*)mParent;
    }

    View* View::_getChildAt(int index)
    {
        return (View*)mChildren[index];
    }

    void View::attachToScene(Scene* scene)
    {
        if (!scene)
        {
            return;
        }

        if (mMesh && !mAttached)
        {
            scene->addMesh(mMesh);
            scene->addRenderable(this);
        }
        
        mAttached = true;
        mAttachedScene = scene;

        for (int i = 0, n = mChildren.size(); i < n; ++i)
        {
            View* v = _getChildAt(i);
            v->attachToScene(scene);
        }
    }
    
    void View::detachFromScene(Scene* scene)
    {
        mAttached = false;
        
        for (int i = scene->getRenderableCount() - 1;i >= 0;--i)
        {
            const RenderEntity& e = scene->getRenderable(i);
            if (e.mNode == this)
            {
                scene->removeRenderable(i);
            }
        }

		for (int i = 0, n = mChildren.size();i < n;++i)
		{
			View* v = _getChildAt(i);
			v->detachFromScene(mAttachedScene);
		}
    }
    
    bool View::onUpdate()
    {
        if (mAlphaAnimation && mAlphaAnimation->isActive())
        {
            float alpha = mAlphaAnimation->getCurrent();
            if (alpha < 0.0001)
            {
                alpha = 0;
            }
            else if (alpha > 1)
            {
                alpha = 1;
            }
            mMaterial->setTransparency(alpha);
            return true;
        }
        return false;
    }

    void View::layout(const HRect& bound, World* world)
    {
        if (!mRequestLayout || !world)
        {
            return;
        }
        
        mWorld = world;
        Scene* scene = world->getDefaultScene();
        Renderer* renderer = world->getRoot()->getRenderer();
        float screenWidth = renderer->getWidth();
        float screenHeight = renderer->getHeight();
        float aspectRatio = screenWidth / screenHeight;
        float spaceHeight = SPACE_HEIGHT;
        SPACE_WIN_RATIO = spaceHeight / screenHeight;
        
        if (mMesh)
        {
            float spaceWidth = spaceHeight * aspectRatio;
            float width = bound.mWidth / screenWidth * spaceWidth;
            float height = bound.mHeight / screenHeight * spaceHeight;
            if (mHAlignment == EHAlignLeft)
            {
                mPos.x = (bound.mLeft / screenWidth - 0.5) * spaceWidth + width/2;
            }
            else if (mHAlignment == EHAlignCenter)
            {
                mPos.x = (bound.mLeft / screenWidth) * spaceWidth;
            }
            else if (mHAlignment == EHAlignRight)
            {
                mPos.x = (-bound.mLeft / screenWidth + 0.5) * spaceWidth - width/2;
            }

            if (mVAlignment == EVAlignTop)
            {
                mPos.y = (0.5 - bound.mTop / screenHeight) * spaceHeight - height/2;
            }
            else if (mVAlignment == EVAlignCenter)
            {
                mPos.y = (-bound.mTop / screenHeight) * spaceHeight;
            }
            else if (mVAlignment == EVAlignBottom)
            {
                mPos.y = (-0.5 + bound.mTop / screenHeight) * spaceHeight + height/2;
            }

            mScale.x = width * mScaleX;
            mScale.y = height * mScaleY;
            mScale.z = 1;

            setIdentity();
            scale(mScale.x, mScale.y, 1);
            rotate(0, 0, 1, mRoll);
            translate(mPos.x, mPos.y, 0);
            
            attachToScene(scene);
        }
        
        bool layoutRequested = onLayout(bound, world);
        
        for (int i = 0, n = mChildren.size();i < n;++i)
		{
			View* v = _getChildAt(i);
//			LOGD("v name = %s, v->mRequestLayout = %d", v->getName().c_str(), v->mRequestLayout);
            if (v->mRequestLayout)
            {
                HRect vBound = HRect(mBound.mLeft + v->mBound.mLeft,
                                     mBound.mTop + v->mBound.mTop,
                                     v->mBound.mWidth, v->mBound.mHeight);
                v->layout(vBound, world);
            }
		}

        mRequestLayout = layoutRequested;
        updateViewMatix();
        mDisplayDirty = false;
    }
    
    bool View::onLayout(const HRect& bound, World* world)
    {
        return false;
    }
    
    void View::requestLayout()
    {
        mRequestLayout = true;
        for (int i = 0, n = mChildren.size();i < n;++i)
		{
			View* v = _getChildAt(i);
            v->requestLayout();
		}
        
        View* p = static_cast<View*>(getParent());
        while (p)
        {
            p->mRequestLayout = true;
            p = static_cast<View*>(p->getParent());
        }
    }

    bool View::dispatchTouchEvent(MotionEvent e)
    {
        if (DEBUG_EVENT)
        {
            LOGD("View::dispatchTouchEvent %s[%f, %f, %d]\n",
                    mName.c_str(), e.getX(), e.getY(), e.mType);
        }
        
        if (!mWorld)
        {
            return false;
        }

        if (mGestureDetector.onTouch(e, mWorld->getRoot()->getRenderer()))
        {
            if (DEBUG_EVENT)
                LOGD("%s::dispatchTouchEvent. Gesture Returned\n", mName.c_str());
            return true;
        }

        Renderer* renderer = mWorld->getRoot()->getRenderer();
        float screenWidth = renderer->getWidth();
        float screenHeight = renderer->getHeight();

        for (int i = mChildren.size() - 1;i >= 0;--i)
        {
            View* v = _getChildAt(i);
            if (v->getVisibility() != EVisible)
            {
                continue;
            }

            float w = v->getWidth();
            float h = v->getHeight();
            HRect bound;
            if (v->mHAlignment == EHAlignLeft)
            {
                bound.mLeft = v->mBound.mLeft;
            }
            else if (v->mHAlignment == EHAlignCenter)
            {
                bound.mLeft = v->mBound.mLeft + screenWidth / 2 - w / 2;
            }
            else if (v->mHAlignment == EHAlignRight)
            {
                bound.mLeft = screenWidth - v->mBound.mLeft - w;
            }
            if (v->mVAlignment == EVAlignTop)
            {
                bound.mTop = v->mBound.mTop;
            }
            else if (v->mVAlignment == EVAlignCenter)
            {
                bound.mTop = v->mBound.mTop + screenHeight / 2 - h / 2;
            }
            else if (v->mVAlignment == EVAlignBottom)
            {
                bound.mTop = screenHeight - v->mBound.mTop - h;
            }

            float cx = bound.mLeft + w/2;
            float cy = bound.mTop + h/2;
            float l = (bound.mLeft - cx) * v->mScaleX + cx;
            float t = (bound.mTop - cy) * v->mScaleY + cy;
            float r = v->getWidth(true) + l;
            float b = v->getHeight(true) + t;
            float x = e.getX();
            float y = e.getY();
            if (DEBUG_EVENT)
                LOGD("Checking child[%s] bound [%f,%f,%f,%f]\n", v->getName().c_str(), l, t, r, b);
            if ((x > l && x < r && y > t && y < b) || v->mAcceptOutofBoundEvent)
            {
                MotionEvent eSub = e;
                eSub.setX(x - l, 0);
                eSub.setY(y - t, 0);
                bool res = v->dispatchTouchEvent(v->mAcceptOutofBoundEvent ? e : eSub);
                if (res)
                {
                    return true;
                }
            }
            else if (v->mIsActive)
            {
                v->mIsActive = false;
                v->cancel();
            }
        }
        
        return onTouch(e);
    }
    
    bool View::onTouch(MotionEvent e)
    {
    	if(mOnTouchListener)
    	{
    		return mOnTouchListener->onTouch(e, this);
    	}
        return false;
    }

    void View::setOnTouchListener(OnTouchListener* listener)
    {
    	mOnTouchListener = listener;
    }

    void View::setOnClickListener(OnClickListener* listener)
    {
        mOnClickListener = listener;
    }

    bool View::onSingleTapUp(const MotionEvent& e)
    {
        if (mOnClickListener)
        {
            if (DEBUG_EVENT)
            {
                LOGD("onSingleTapUp. View = %s\n", mName.c_str());
            }
            mOnClickListener->onClick(this);
        }
        return false;
    }

    bool View::onFling(const MotionEvent& e, float vx, float vy)
    {
        for (int i = mChildren.size() - 1;i >= 0;--i)
        {
            View* v = _getChildAt(i);
            float l = v->mBound.mLeft;
            float t = v->mBound.mTop;
            float r = v->getWidth() + l;
            float b = v->getHeight() + t;
            float x = e.getX();
            float y = e.getY();
            if (x > l && x < r && y > t && y < b)
            {
                MotionEvent eSub(x - l, y - t, e.mType);
                bool res = v->onFling(eSub, vx, vy);
                if (res)
                {
                    return true;
                }
            }
        }

        return false;
    }

    void View::invalidate()
    {
        invalidate(HRect(0, 0, getWidth(), getHeight()));
    }
    
    void View::invalidate(HRect dirty)
    {
        
    }

    void View::setTranslate(Vector3 trans)
    {
        mTranslate = trans;
        setIdentity();
        scale(mScale.x * mScaleX, mScale.y * mScaleY, 1);
        SceneNode::translate(mPos.x, mPos.y, 0);
        SceneNode::translate(trans.x, trans.y, trans.z);
    }

//    void View::translate(Vector3 trans)
//    {
//        mTranslate += trans;
//        SceneNode::translate(trans.x, trans.y, trans.z);
//    }

    void View::roll(float roll)
    {
        mRoll += roll;
        requestLayout();
    }

//    void View::rotate(float x, float y, float z, float radian)
//    {
//        SceneNode::rotate(x, y, z, radian);
//        requestLayout();
//    }
//
//    void View::scale(float sx, float sy, float sz)
//    {
//        mScaleX *= sx;
//        mScaleY *= sy;
//        mScaleZ *= sz;
//        SceneNode::scale(sx, sy, sz);
//        requestLayout();
//    }

    void View::resetTransform()
    {
        mScaleX = 1;
        mScaleY = 1;
        mScaleZ = 1;
        mTranslate.x = 0;
        mTranslate.y = 0;
        mTranslate.z = 0;
        mRoll = 0;
        setIdentity();
        scale(mScale.x, mScale.y, 1);
        SceneNode::translate(mPos.x, mPos.y, 0);
    }
    
    void View::onRenderBegin(Pass* pass)
    {
        if (!mAttachedScene || !mAttachedScene->getCurrentCamera())
        {
            return;
        }
        Camera* worldCamera = mAttachedScene->getCurrentCamera();
        Camera cam;
        cam.setAspectRatio(worldCamera->getAspectRatio());
        cam.setUp(worldCamera->getUp());
        float fov = worldCamera->getFOV();
        cam.setFOV(fov);
        
        cam.setPosition(Vector3(0, 0, 1 / tan(Math::PI * fov / 360)));
        cam.setLookAt(Vector3(0, 0, 0));
        cam.lookAt();
        
        Matrix4 m = cam.getViewMatrix();
        pass->uploadEyeMat(m);
        
        cam.setNearRange(1);
        cam.setFarRange(100);
        cam.perspective();
        pass->uploadProjectionMat(cam.getProjectionMatrix());
    }
    
    void View::onRenderEnd(Pass* pass)
    {
        if (!mAttachedScene || !mAttachedScene->getCurrentCamera())
        {
            return;
        }
        Camera* worldCamera = mAttachedScene->getCurrentCamera();
        pass->uploadEyeMat(worldCamera->getViewMatrix());
        pass->uploadProjectionMat(worldCamera->getProjectionMatrix());
    }

    void View::cancel()
    {

    }

    void View::startAlphaAnimation(float from, float to, double duration, double delay, int interpolateType)
    {
        if (!mWorld)
        {
            return;
        }
        
        if (!mAlphaAnimation)
        {
            mAlphaAnimation = HouyiNew SceneNodeAnimation();
        }
        mAlphaAnimation->setInterpolationType(interpolateType);
        mAlphaAnimation->setDuration(duration);
        mAlphaAnimation->start(mWorld->getRoot(), from, to, delay);
    }

    void View::fadeOut(double duration, double delay, int interpolateType)
    {
        startAlphaAnimation(mMaterial->getTransparency(), 0, duration, delay, interpolateType);
    }
    
    void View::fadeIn(double duration, double delay, int interpolateType)
    {
        setVisibility(EVisible);
        startAlphaAnimation(mMaterial->getTransparency(), 1, duration, delay, interpolateType);
    }
}
