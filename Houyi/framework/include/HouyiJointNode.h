//
//  HouyiJointNode.h
//  houyi
//
//  Created by Lance Mao on 13/11/11.
//  Copyright (c) 2011 Dong wen. All rights reserved.
//

#ifndef houyi_HouyiJointNode_h
#define houyi_HouyiJointNode_h

#include "HouyiObject.h"
#include "HouyiMatrix4.h"
#include "HouyiKeyFrame.h"

namespace Houyi
{
    class JointNode: public Object, public Serializable
    {
    public:
        JointNode();
        ~JointNode();
        void dump(int level);

        void setParent(JointNode* parent);
        JointNode* getParent();
        
        void addChild(JointNode* child);
        JointNode* getChildAt(int index);
        int getChildrenCount();
        
        KeyFrame* getKeyFrames();
        void setKeyFrames(KeyFrame* data);
        
        virtual void writeObject(DataOutputStream& dos);
        virtual void readObject(DataInputStream& dis);

    public:
        int mTreeID;
        int mParentID;
        Matrix4 mBindPosMatrix;
        Matrix4 mInvBindMatrix;
        Matrix4 mBindSpaceMatrix;
        Matrix4 mWorldMatrix;
        Matrix4 mWorldMatrixTranspose;
        string mID;
        string mSID;
        vector<int> mChildrenID;
        
        int mKFCount;
        int mCurrentFrame;
        
    private:
        JointNode* mParent;
        vector<JointNode*> mChildren;
        KeyFrame* mKeyFrames;

    };
}

#endif
