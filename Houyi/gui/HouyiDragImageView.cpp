#include "HouyiDragImageView.h"

namespace Houyi
{
    DragImageView::DragImageView()
    {
        mAcceptOutofBoundEvent = false;
    }

    bool DragImageView::onTouch(MotionEvent e)
    {
        if (e.mType == MotionEvent::TYPE_DOWN && e.mTouchCount == 1)
        {
            if (DEBUG_EVENT)
            {
                LOGD("DragImageView::onTouch set active to true\n");
            }

            mAcceptOutofBoundEvent = true;
        }
        else if (e.mType == MotionEvent::TYPE_UP)
        {
            if (DEBUG_EVENT)
            {
                LOGD("DragImageView::drawg set active to false\n");
            }
            mAcceptOutofBoundEvent = false;
        }
        else if (e.mType == MotionEvent::TYPE_MOVE)
        {
            if (isActive())
            {
                return View::onTouch(e);
            }
            else
            {
                return false;
            }
        }
        return false;
    }

    bool DragImageView::onScroll(const MotionEvent& e, float sx, float sy)
    {
        if (isActive())
        {
            setBound(HRect(mBound.mLeft + sx, mBound.mTop + sy, mBound.mWidth, mBound.mHeight));
            return true;
        }
        return false;
    }
}
