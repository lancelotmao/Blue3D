//
//  HouyiNode.cpp
//  Houyi
//
//  Created by Lance Mao on 16/2/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#include "HouyiNode.h"
#include "HouyiLog.h"

namespace Houyi
{
    Node::Node() : mParent(0)
    {
    }

    Node::~Node()
    {
    }

    Node* Node::getRoot()
    {
        Node* root = this;
        while (root->mParent) {
            root = root->mParent;
        }
        return root;
    }
    
    Node* Node::getParent()
    {
        return mParent;
    }

    void Node::setParent(Node* parent)
    {
        mParent = parent;
    }

    void Node::addChild(Node* child)
    {
        if (child)
        {
        	for (int i = 0;i < mChildren.size();++i)
        	{
        		if (child == mChildren[i])
        		{
        			return;
        		}
        	}
        	if (child->getParent())
        	{
        	    child->getParent()->removeChild(child);
        	}
            child->setParent(this);
            mChildren.push_back(child);
        }
        else
        {
            LOGE("Node::addChild. NULL\n");
        }
    }

    int Node::childrenCount()
    {
        return mChildren.size();
    }

    Node* Node::getChildAt(int index)
    {
        return mChildren[index];
    }

    void Node::removeAllChildren()
    {
        mChildren.clear();
    }
    
    void Node::deleteAllChildren()
    {
        for (int i = 0;i < mChildren.size();++i)
        {
            mChildren[i]->deleteAllChildren();
            HouyiDelete(mChildren[i]);
        }
        removeAllChildren();
    }

    void Node::removeChild(int index)
    {
        mChildren.erase(mChildren.begin() + index);
    }

    void Node::removeChild(Node* node)
    {
        for (int i = 0;i < mChildren.size();++i)
        {
            if (mChildren[i] == node)
            {
                removeChild(i);
                break;
            }
        }
    }

    void Node::removeFromParent()
    {
        if (mParent)
        {
            mParent->removeChild(this);
            mParent = 0;
        }
    }

    bool Node::contains(Node* node)
    {
    	for (int i = 0;i < mChildren.size();++i)
		{
			if (mChildren[i] == node)
			{
				return true;
			}
		}
    	return false;
    }

    bool Node::isDescendantOf(Node* node)
    {
        if (!node)
        {
            return false;
        }

        if (node == this)
        {
            return true;
        }

        if (node->contains(this))
        {
            return true;
        }
        else
        {
            for (int i = 0;i < node->mChildren.size();++i)
            {
                if (isDescendantOf(node->mChildren[i]))
                {
                    return true;
                }
            }
        }
        return false;
    }
    
    Node* Node::findNodeById(int nid)
    {
        for (int i = 0;i < mChildren.size();++i)
        {
            Node* res = mChildren[i]->findNodeById(nid);
            if (res)
            {
                return res;
            }
        }
        return nid == mId ? this : 0;
    }
    
    Node* Node::findNodeByName(const string& name)
    {
        for (int i = 0;i < mChildren.size();++i)
        {
            Node* res = mChildren[i]->findNodeByName(name);
            if (res)
            {
                return res;
            }
        }
        return mName.compare(name) == 0 ? this : 0;
    }
}
