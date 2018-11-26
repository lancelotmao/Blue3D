#ifndef __Houyi__HouyiBlueWorld__
#define __Houyi__HouyiBlueWorld__

#include <Houyi/Houyi.h>
#include "BlueAdapter.h"

using namespace Houyi;

class BlueWorld : public World
{
public:
	BlueWorld();
	virtual ~BlueWorld();

	virtual void onCreate();
	virtual void onPostCreate();
	virtual void onWindowChanged();

    inline void setViewMode(int mode)
    {
        mViewMode = mode;
    }

    inline BlueAdapter* getAdapter()
    {
        return mAdapter;
    }
    
    inline ListView* getFilmstrip()
    {
        if (mFilmstrip) {
            mFilmstrip->setWorld(this);
        }
        return mFilmstrip;
    }

private:
    void setFilmstripLayout();

private:
	ListView* mFilmstrip;
	BlueAdapter* mAdapter;
    int mViewMode;
};

#endif
