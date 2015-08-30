//
//  HouyiNode.h
//  Houyi
//
//  Created by Lance Mao on 16/2/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef __Houyi__HouyiNode__
#define __Houyi__HouyiNode__

#include "HouyiObject.h"
#include <vector>

namespace Houyi
{
    class Node: public Object
    {
    public:
        Node();
        virtual ~Node();

        virtual Node* getRoot();
        virtual Node* getParent();
        virtual void setParent(Node* parent);
        virtual void addChild(Node* child);
        virtual int childrenCount();
        virtual Node* getChildAt(int index);
        virtual void removeAllChildren();
        virtual void deleteAllChildren();
        virtual void removeChild(int index);
        virtual void removeChild(Node* node);
        virtual void removeFromParent();
        virtual bool contains(Node* node);
        virtual bool isDescendantOf(Node* node);
        virtual Node* findNodeById(int nid);
        virtual Node* findNodeByName(const string& name);
        
    protected:
        Node* mParent;
        vector<Node*> mChildren;
    };
}

#endif /* defined(__Houyi__HouyiNode__) */
