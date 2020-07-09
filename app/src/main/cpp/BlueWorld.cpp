#include "BlueWorld.h"

BlueWorld::BlueWorld() : mAdapter(0), mFilmstrip(0), mViewMode(0)
{
	mName = "Blue3D World";
}

BlueWorld::~BlueWorld()
{
    HouyiDelete(mAdapter);
}

void BlueWorld::onCreate()
{
    if (mViewMode == 0) // normal
    {
//        mAdapter = HouyiNew BlueAdapter();
//        mFilmstrip = HouyiNew ListView(ListView::EHorizontal);
//        mFilmstrip->setItemClickListener(mAdapter);
//        mFilmstrip->setAdapter(mAdapter);
//        mAdapter->setFocusIndex(mAdapter->getFocus());
////        mFilmstrip->requestCenterFocus();
//        mFilmstrip->setGravity(View::EHAlignCenter, View::EVAlignTop);
//        addView(mFilmstrip);
    }
    else
    {
        // single. e.g. from email attachment. no filmstrip in this case
    }
}

void BlueWorld::onPostCreate()
{
    setFilmstripLayout();
}

void BlueWorld::onWindowChanged()
{
    setFilmstripLayout();
}

void BlueWorld::setFilmstripLayout()
{
    if (mAdapter)
    {
        float h = mRenderer->getHeight() / 10;
        LOGD("BlueWorld::setFilmstripLayout() (%f, %f)\n", mRenderer->getWidth(), h);
        mAdapter->setViewHeight(h);
        mFilmstrip->setBound(HRect(0, 0, mRenderer->getWidth(), h), View::EHAlignLeft, View::EVAlignBottom);
        mFilmstrip->requestCenterFocus();
        mAdapter->notifyDataChange();
    }
}
