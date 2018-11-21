#ifndef HouyiBlueAdapter_h
#define HouyiBlueAdapter_h

#include <Houyi/Houyi.h>

using namespace Houyi;

class BlueAdapter : public Adapter, public ListItemClickListener
{
public:
    virtual ~BlueAdapter(){}
	virtual int getSize();
	virtual View* getView(int index, View* reusableView);

	inline void setViewHeight(float h)
	{
	    mViewHeight = h;
	}

	int getFocus();

	virtual void onItemClicked(View* view, int index);

private:
	float mViewHeight;
	ImagePtr mDefThumb;
	ImagePtr mImgFocus;
};

#endif
