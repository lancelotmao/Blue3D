#ifndef HOUYIDRAGIMAGEVIEW_H_
#define HOUYIDRAGIMAGEVIEW_H_

#include "HouyiImageView.h"

namespace Houyi
{
    class DragImageView : public ImageView
    {
    public:
        DragImageView();
        virtual bool onTouch(MotionEvent e);

        inline bool isActive()
        {
            return mAcceptOutofBoundEvent;
        }
        
        inline void setActive(bool active)
        {
            mAcceptOutofBoundEvent = active;
        }

        virtual bool onScroll(const MotionEvent& e, float sx, float sy);
    };
}

#endif
