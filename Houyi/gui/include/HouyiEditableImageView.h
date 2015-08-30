#ifndef HOUYIEDITABLEIMAGEVIEW_H_
#define HOUYIEDITABLEIMAGEVIEW_H_

#include "HouyiDragImageView.h"

namespace Houyi
{
    class EditableImageView : public DragImageView
    {
    public:
        EditableImageView();
        virtual ~EditableImageView();
        
        virtual bool onScale(const MotionEvent& e, float s);
        virtual bool onRotate(const MotionEvent& e, float radian);

        virtual void setBound(const HRect& rect);
    };
}

#endif
