#ifndef __Houyi__HouyiPopupWorld__
#define __Houyi__HouyiPopupWorld__

#include "HouyiWorld.h"

namespace Houyi
{
    class PopupWorld : public World
    {
        friend class Root;

    public:
        enum PopupWorldState
        {
            EIdle,
            EPrepare,
            EShowing
        };

        PopupWorld(HRect rect);
        virtual ~PopupWorld();

        virtual void show();
        virtual void dismiss();
        virtual void prepare();
        virtual bool onBackPressed();
        virtual void clearContext();

        inline HRect& getRect()
        {
            return mRect;
        }
        
        inline bool isShowing()
        {
            return mState == EShowing;
        }

    protected:
        virtual void onPrepare(){}
        virtual void onDestroy(){}

    private:
        int mState;
        HRect mRect;
    };
}
#endif
