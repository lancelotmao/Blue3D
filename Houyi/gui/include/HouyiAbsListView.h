//
//  HouyiAbsListView.h
//
//  Created by Lance Mao on 15/12/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef HouyiAbsListView_h
#define HouyiAbsListView_h

#include "HouyiView.h"
#include "HouyiAdapter.h"

namespace Houyi
{
    class AbsListView : public View, public DataListener
    {
    public:
        AbsListView();
        virtual ~AbsListView();

        virtual void setAdapter(Adapter* adapter);
        
        virtual Adapter* getAdapter();
        
        virtual void scroll(float delta) = 0;

    protected:
        Adapter* mAdapter;
    };
}

#endif
