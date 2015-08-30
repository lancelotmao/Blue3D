#include "HouyiEditableImageView.h"

namespace Houyi
{
    EditableImageView::EditableImageView()
    {
        mName = "EditableImageView";
    }

    EditableImageView::~EditableImageView()
    {

    }
    
    void EditableImageView::setBound(const HRect& rect)
    {
        View::setBound(rect);
    }

    bool EditableImageView::onScale(const MotionEvent& e, float s)
    {
        if (getVisibility() == View::EVisible)
        {
            scale(s, s, 1);
            return true;
        }
        return false;
    }

    bool EditableImageView::onRotate(const MotionEvent& e, float radian)
    {
        if (getVisibility() == View::EVisible)
        {
            roll(radian);
            return true;
        }
        return false;
    }
}
