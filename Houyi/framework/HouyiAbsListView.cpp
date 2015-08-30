//
//  HouyiAbsListView.cpp
//  Houyi
//
//  Created by Lance Mao on 18/5/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiAbsListView.h"

namespace Houyi
{
    AbsListView::AbsListView() : mAdapter(0)
    {
        
    }
    
    AbsListView::~AbsListView()
    {
    }
    
    void AbsListView::setAdapter(Adapter* adapter)
    {
        mAdapter = adapter;
        mAdapter->setDataListener(this);
    }
    
    Adapter* AbsListView::getAdapter()
    {
        return mAdapter;
    }
}
