//
//  HouyiAdapter.h
//
//  Created by Lance Mao on 15/12/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef HouyiAdapter_h
#define HouyiAdapter_h

#include "HouyiObject.h"
#include "HouyiView.h"

namespace Houyi
{
    class DataListener
    {
    public:
        virtual ~DataListener(){}
        virtual void onDataChanged() = 0;
    };
    
    class Adapter : public Houyi::Object
    {
    public:
        Adapter() : mDataListener(0), mFocusIndex(0)
        {
        }
        
        virtual ~Adapter(){}
        
        virtual int getSize() = 0;
        virtual View* getView(int index, View* reusableView) = 0;
        
        inline void setDataListener(DataListener* listener)
        {
            mDataListener = listener;
        }
        
        virtual void notifyDataChange()
        {
            if (mDataListener)
            {
                mDataListener->onDataChanged();
            }
        }
        
        inline void setFocusIndex(int focus)
        {
            mFocusIndex = focus;
        }

        inline int getFocusIndex()
        {
            return mFocusIndex;
        }

    protected:
        DataListener* mDataListener;
        int mFocusIndex;
    };
}

#endif
