//
//  HouyiJointNode.cpp
//  houyi
//
//  Created by Lance Mao on 14/11/11.
//  Copyright (c) 2011 Dong wen. All rights reserved.
//

#include "HouyiLog.h"
#include "HouyiJointNode.h"

namespace Houyi
{
    JointNode::JointNode()
    {
        mParent = 0;
        mKeyFrames = 0;
        mKFCount = 0;
        mCurrentFrame = 0;
        mParentID = -1;
        mChildrenID.clear();
    }

    JointNode::~JointNode()
    {
        delete[] mKeyFrames;
        mChildrenID.clear();
    }

    void JointNode::setParent(JointNode* parent)
    {
        if (mParent)
        {
            LOGE("Joint already has parent!\n");
            return;
        }
        mParent = parent;
    }
    
    JointNode* JointNode::getParent()
    {
        return mParent;
    }
    
    void JointNode::addChild(JointNode* child)
    {
        if (mParent == child)
        {
            LOGE("JointNode::addChild circular adding");
            return;
        }
        mChildren.push_back(child);
    }
    
    JointNode* JointNode::getChildAt(int index)
    {
        return mChildren[index];
    }
    
    int JointNode::getChildrenCount()
    {
        return mChildren.size();
    }
    
    KeyFrame* JointNode::getKeyFrames()
    {
    	return mKeyFrames;
    }

    void JointNode::setKeyFrames(KeyFrame* data)
    {
    	if (mKeyFrames)
    	{
    		delete[] mKeyFrames;
    	}
    	mKeyFrames = data;
    }

    void JointNode::dump(int level)
    {
        LOGD("dump HJoints level = %d; name = %s\n", level, mName.c_str());
        for (int i = 0; i < mChildren.size(); ++i)
        {
            mChildren.at(i)->dump(level + 1);
        }
    }
    
    void JointNode::writeObject(DataOutputStream& dos)
    {
        dos.writeInt(mTreeID);
        dos.writeInt(mParentID);
        dos.write(&mBindPosMatrix, sizeof(mBindPosMatrix));
        dos.write(&mInvBindMatrix, sizeof(mInvBindMatrix));
        dos.write(&mBindSpaceMatrix, sizeof(mBindSpaceMatrix));
        dos.writeInt(mChildrenID.size());
        for (int i = 0;i < mChildrenID.size();++i)
        {
            dos.writeInt(mChildrenID[i]);
        }
        dos.writeInt(mKFCount);
        for (int i = 0;i < mKFCount;++i)
        {
            KeyFrame kf = mKeyFrames[i];
            kf.writeObject(dos);
        }
    }
    
    void JointNode::readObject(DataInputStream& dis)
    {
        mTreeID = dis.readInt();
        mParentID = dis.readInt();
        dis.read(&mBindPosMatrix, sizeof(mBindPosMatrix));
        dis.read(&mInvBindMatrix, sizeof(mInvBindMatrix));
        dis.read(&mBindSpaceMatrix, sizeof(mBindSpaceMatrix));
        int childrenCount = dis.readInt();
        for (int i = 0;i < childrenCount;++i)
        {
            int childId = dis.readInt();
            mChildrenID.push_back(childId);
        }
        mKFCount = dis.readInt();
        mKeyFrames = new KeyFrame[mKFCount];
        for (int i = 0;i < mKFCount;++i)
        {
            mKeyFrames[i].readObject(dis);
        }
    }
}
