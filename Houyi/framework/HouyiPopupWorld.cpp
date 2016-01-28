#include "HouyiPopupWorld.h"
#include "HouyiRoot.h"

namespace Houyi
{
    PopupWorld::PopupWorld(HRect rect) : mState(EIdle), mRect(rect)
    {

    }

    PopupWorld::~PopupWorld()
    {

    }

    void PopupWorld::show()
    {
        mRoot->addPopupWorld(this);
        mState = EPrepare;
    }

    void PopupWorld::dismiss()
    {
        mState = EIdle;
    }

    void PopupWorld::prepare()
    {
        if (mState != EPrepare)
        {
            LOGW("invalid popup world state. mState = %d", mState);
            return;
        }

        onPrepare();

        mState = EShowing;
    }

    bool PopupWorld::onBackPressed()
    {
        // by default dismiss popup world
        dismiss();
        return true;
    }

    void PopupWorld::clearContext()
    {
        LOGI("PopupWorld::clearing context \n");

        World::clearContext();
        if (mState == EShowing)
        {
            mState = EPrepare;
        }
    }
}
