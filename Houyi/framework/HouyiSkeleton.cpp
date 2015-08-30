//
//  Skeleton.cpp
//  houyi
//
//  Created by Lance Mao on 21/11/11.
//  Copyright (c) 2011 Dong wen. All rights reserved.
//

#include "HouyiSkeleton.h"
#include "HouyiUtil.h"
#include "HouyiLog.h"
#include "HouyiSettings.h"

namespace Houyi
{
    const int Skeleton::MAX_BONE_COUNT = 48;
    
    Skeleton::~Skeleton()
    {
        if (mOwnsJoints)
        {
            for (int i = 0;i < mJointsCount;++i)
            {
                HouyiDelete(mJoints[i]);
            }
        }
        delete[] mJoints;
        mJoints = 0;
        mJNRoots.clear();
        mMaxKFCount = 0;
    }

    Skeleton::Skeleton()
    {
        mOwnsJoints = false;
        mJoints = NULL;
        mJNRoots.clear();
        mCurFrame = 0;
        mMaxKFCount = 0;
        mStartTime = -1;
        mHardwareEnabled = true;
        mUseLinearRaioForBezier = true;
        mAnimationSpeed = 1;
        mPausedTime = 0;
    }

    void Skeleton::update()
    {
        if (mPausedTime != 0)
        {
            return;
        }
        
        int rootSize = mJNRoots.size();
        if (rootSize == 0)
        {
            LogOnce("Skeleton update, no root!\n");
            return;
        }

        double now = currentTimeMillis() * mAnimationSpeed;
        if (mStartTime < 0)
        {
            mStartTime = now;
        }

        double delta = (now - mStartTime) / 1000;
        for (int i = 0; i < rootSize; ++i)
        {
            JointNode* root = mJoints[mJNRoots.at(i)];
            updateJoints(delta, root);
        }
        
        if (mCurFrame >= mMaxKFCount - 1)
        {
            reset();
        }
    }

    void Skeleton::updateJoints(double delta, JointNode* jnPtr)
    {
        Matrix4 matBindSpace = Matrix4::IDENTITY;

        if (jnPtr->mKFCount > 0 && mMaxKFCount > 0 && delta >= 0)
        {
            int curFrame = jnPtr->mCurrentFrame;
            int nextFrame = -1;
            for (int i = curFrame;i < jnPtr->mKFCount;++i)
            {
            	KeyFrame* keyFrames = jnPtr->getKeyFrames();
                if (keyFrames[i].mTime > delta)
                {
                    nextFrame = i;
                    if (i == 0)
                    {
                        curFrame = 0;
                    }
                    else
                    {
                        curFrame = i - 1;
                        jnPtr->mCurrentFrame = curFrame;
                    }
                    break;
                }
            }

            // last key frame reached
            if (nextFrame == -1)
            {
                curFrame = jnPtr->mKFCount - 1;
                nextFrame = curFrame;
                if (curFrame > mCurFrame)
                {
                    mCurFrame = curFrame;
                }
            }

            KeyFrame* keyFrames = jnPtr->getKeyFrames();
            KeyFrame& kfCur = keyFrames[curFrame];
            KeyFrame& kfNext = keyFrames[nextFrame];
            double curFrameTime = kfCur.mTime;
            double nextFrameTime = kfNext.mTime;

            double ratio = 0;
            double deltaTime = 0;
            double deltaTimeBetweenTwoFrame = nextFrameTime - curFrameTime;
            if (deltaTimeBetweenTwoFrame > 0)
            {
                deltaTime = delta - curFrameTime;
                if (kfCur.mInterpolation == Transform::EConstInterpolation)
                {
                    ratio = (deltaTime < nextFrameTime - delta) ? 0 : 1;
                }
                else if (kfCur.mInterpolation == Transform::ELinearInterpolation)
                {
                    ratio = deltaTime / deltaTimeBetweenTwoFrame;
                }
                else if (kfCur.mInterpolation == Transform::EBezierInterpolation)
                {
                    ratio = deltaTime / deltaTimeBetweenTwoFrame;
                }
            }

            // need to ensure the order of cur and next is the same
            for (int i = 0; i < kfCur.mTransforms.size(); ++i)
            {
                Transform& cur = kfCur.mTransforms[i];
                Transform& next = kfNext.mTransforms[i];

                if (cur.mType == Transform::EMatrix)
                {
                    // TODO interpolate matrix. should use quaternion
                    matBindSpace = matBindSpace * cur.getMatrix();
                }
                else if (ratio == 0 || ratio == 1
                         || cur.mInterpolation == Transform::EConstInterpolation
                         || cur.mInterpolation == Transform::ELinearInterpolation)
                {
                    Matrix4 mat = lerp(cur, next, ratio);
                    matBindSpace = matBindSpace * mat;
                }
                else if (kfCur.mInterpolation == Transform::EBezierInterpolation)
                {
                    Matrix4 mat = berp(cur, next, deltaTime, curFrameTime, nextFrameTime, ratio);
                    matBindSpace = matBindSpace * mat;
                }
            }
        }
        else
        {
            matBindSpace = jnPtr->mBindPosMatrix;
        }

        int parent = jnPtr->mParentID;
        if (parent >= 0)
        {
            jnPtr->mBindSpaceMatrix = mJoints[parent]->mBindSpaceMatrix * matBindSpace;
        }
        else
        {
            jnPtr->mBindSpaceMatrix = matBindSpace;
        }
        jnPtr->mWorldMatrix = jnPtr->mBindSpaceMatrix * jnPtr->mInvBindMatrix;
        jnPtr->mWorldMatrixTranspose = jnPtr->mWorldMatrix.transpose();

        for (int i = 0; i < jnPtr->mChildrenID.size(); ++i)
        {
            JointNode* childPtr = mJoints[jnPtr->mChildrenID.at(i)];
            updateJoints(delta, childPtr);
        }
    }

    Matrix4 Skeleton::lerp(const Transform& cur, const Transform& next, double ratio)
    {
        Matrix4 mat = Matrix4::IDENTITY;
        if (cur.mType == Transform::ETranslate)
        {
            float translate[3];
            for (int j = 0;j < 3;++j)
            {
                translate[j] = cur.mElements[j] + (next.mElements[j] - cur.mElements[j]) * ratio;
            }
            mat.translate(translate[0], translate[1], translate[2]);
        }
        else if (cur.mType == Transform::ERotate)
        {
            float radian;
            radian = cur.mElements[3] + (next.mElements[3] - cur.mElements[3]) * ratio;
            mat.rotate(radian, cur.mElements[0], cur.mElements[1], cur.mElements[2]);
        }
        else if (cur.mType == Transform::EScale)
        {
            float scale[3];
            for (int j = 0;j < 3;++j)
            {
                scale[j] = cur.mElements[j] + (next.mElements[j] - cur.mElements[j]) * ratio;
            }
            mat.scale(scale[0], scale[1], scale[2]);
        }
        return mat;
    }

    Matrix4 Skeleton::berp(const Transform& cur, const Transform& next, double deltaTime, double curFrameTime, double nextFrameTime, double ratio)
    {
        // bezier interpolation formular:
        // B = P0 * (1-s)^3 + 3 * C0 * s(1-s)^2 + 3 * C1 * s^2(1-s) + P1 * s^3
        Matrix4 mat = Matrix4::IDENTITY;
        if (cur.mType == Transform::ETranslate)
        {
            float translate[3];
            for (int j = 0;j < 3;++j)
            {
                float s = mUseLinearRaioForBezier ? ratio : Util::ApproximateCubicBezierParameter(deltaTime, curFrameTime, cur.mOutTangent[j * 2], nextFrameTime, next.mInTangent[j * 2]);

                float P0 = cur.mElements[j];
                float P1 = next.mElements[j];
                float C0 = cur.mOutTangent[j * 2 + 1];
                float C1 = next.mInTangent[j * 2 + 1];

                float oneMinusS = 1-s;
                translate[j] = P0 * oneMinusS*oneMinusS*oneMinusS + 3 * C0 * s* oneMinusS*oneMinusS + 3 * C1 * s*s * oneMinusS + P1 * s*s*s;

                translate[j] = cur.mElements[j] + (next.mElements[j] - cur.mElements[j]) * s;
            }
            mat.translate(translate[0], translate[1], translate[2]);
        }
        else if (cur.mType == Transform::ERotate)
        {
            float radian;
            float s = mUseLinearRaioForBezier ? ratio : Util::ApproximateCubicBezierParameter(deltaTime, curFrameTime, cur.mOutTangent[0], nextFrameTime, next.mInTangent[0]);

            float P0 = cur.mElements[3];
            float P1 = next.mElements[3];
            float C0 = cur.mOutTangent[1];
            float C1 = next.mInTangent[1];
            float oneMinusS = 1-s;
            if (C0 == 0 && C1 == 0)
            {
                radian = cur.mElements[3] + (next.mElements[3] - cur.mElements[3]) * ratio;
            }
            else
            {
                radian = P0 * oneMinusS*oneMinusS*oneMinusS + 3 * C0 * s* oneMinusS*oneMinusS + 3 * C1 * s*s * oneMinusS + P1 * s*s*s;
            }
            mat.rotate(radian, cur.mElements[0], cur.mElements[1], cur.mElements[2]);
        }
        else if (cur.mType == Transform::EScale)
        {
            float scale[3];
            for (int j = 0;j < 3;++j)
            {
                float s = mUseLinearRaioForBezier ? ratio : Util::ApproximateCubicBezierParameter(deltaTime, curFrameTime, cur.mOutTangent[j * 2], nextFrameTime, next.mInTangent[j * 2]);

                float P0 = cur.mElements[j];
                float P1 = next.mElements[j];
                float C0 = cur.mOutTangent[j * 2 + 1];
                float C1 = next.mInTangent[j * 2 + 1];

                float oneMinusS = 1-s;
                scale[j] = P0 * oneMinusS*oneMinusS*oneMinusS + 3 * C0 * s* oneMinusS*oneMinusS + 3 * C1 * s*s * oneMinusS + P1 * s*s*s;
            }
            mat.scale(scale[0], scale[1], scale[2]);
        }
        return mat;
    }

    void Skeleton::reset()
    {
        mCurFrame = 0;
        mStartTime = -1;
        for (int i = 0; i < mJNRoots.size(); ++i)
        {
            JointNode* root = mJoints[mJNRoots.at(i)];
            reset(root);
        }
    }

    void Skeleton::reset(JointNode* jnPtr)
    {
        jnPtr->mCurrentFrame = 0;
        for (int i = 0; i < jnPtr->mChildrenID.size(); ++i)
        {
            JointNode* childPtr = mJoints[jnPtr->mChildrenID.at(i)];
            reset(childPtr);
        }
    }

    void Skeleton::setJointNodes(JointNode** joints, int count)
    {
        if (mJoints)
        {
            delete[] mJoints;
            mJoints = NULL;
        }
        mJoints = joints;
    }
    
    JointNode** Skeleton::getJointNodes()
    {
        return mJoints;
    }
    
    void Skeleton::setHardwareEnabled(bool isOn)
    {
        mHardwareEnabled = isOn;
    }
    
    bool Skeleton::isHardwareEnabled()
    {
        return Settings::getInstance()->useHardwareBone() && mHardwareEnabled && getJointsCount() <= MAX_BONE_COUNT;
    }
    
    void Skeleton::setJointsCount(int count)
    {
        mJointsCount = count;
        if (count > mJointsCount)
        {
            mHardwareEnabled = false;
        }
    }
    
    int Skeleton::getJointsCount()
    {
        return mJointsCount;
    }
    
    void Skeleton::setUseLinearRatioForBezier(bool isOn)
    {
        mUseLinearRaioForBezier = isOn;
    }
    
    bool Skeleton::isUseLinearRatioForBezier()
    {
        return mUseLinearRaioForBezier;
    }
    
    int Skeleton::findJointIndexByName(string name)
    {
        for (int i = 0, n = mJointsCount;i < n;++i)
        {
            string jn = mJoints[i]->getName();
            if (jn.compare(name) == 0)
            {
                return i;
            }
        }
        return -1;
    }
    
    void Skeleton::pause()
    {
        if (mPausedTime == 0)
        {
            mPausedTime = currentTimeMillis();
        }
        else
        {
            // already paused
        }
    }
    
    void Skeleton::resume()
    {
        mStartTime += currentTimeMillis() - mPausedTime;
        mPausedTime = 0;
    }
    
    void Skeleton::writeObject(DataOutputStream& dos)
    {
        dos.writeInt(mJNRoots.size());
        for (int i = 0;i < mJNRoots.size();++i)
        {
            dos.writeInt(mJNRoots[i]);
        }
        
        dos.writeInt(mMaxKFCount);
        dos.writeBool(mOwnsJoints);
        dos.writeBool(mHardwareEnabled);
        dos.writeInt(mJointsCount);
        for (int i = 0;i < mJointsCount;++i)
        {
            mJoints[i]->writeObject(dos);
        }
    }
    
    void Skeleton::readObject(DataInputStream& dis)
    {
        int JNRootSize = dis.readInt();
        for (int i = 0;i < JNRootSize;++i)
        {
            int rootId = dis.readInt();
            mJNRoots.push_back(rootId);
        }
        
        mMaxKFCount = dis.readInt();
        mOwnsJoints = dis.readBool();
        mHardwareEnabled = dis.readBool();
        mJointsCount = dis.readInt();
        mJoints = new JointNode*[mJointsCount];
        for (int i = 0;i < mJointsCount;++i)
        {
            mJoints[i] = HouyiNew JointNode();
            mJoints[i]->readObject(dis);
        }
    }
    
    void Skeleton::dumpJoints()
    {
        if (!mJoints || mJNRoots.size() == 0)
        {
            LOGD("Skeleton has no joints\n");
            return;
        }

        for (int i = 0; i < mJNRoots.size(); ++i)
        {
            JointNode* p = mJoints[mJNRoots.at(i)];
            dumpJoint(p, 0);
        }
    }

    void Skeleton::dumpJoint(JointNode* jnPtr, int level)
    {
        string tab = "";
        for (int i = 0; i < level; ++i)
        {
            tab += "    ";
        }
        LOGD("%s%s, %s\n", tab.c_str(), jnPtr->getName().c_str(), jnPtr->mSID.c_str());

        // bind pose matrix
        Matrix4 m(jnPtr->mBindPosMatrix);
        for (int i = 0; i < 4; ++i)
        {
            LOGD("%s%.4f, %.4f, %.4f, %.4f\n", tab.c_str(), m[i][0], m[i][1], m[i][2], m[i][3]);
        }

        // inverse bind matrix
        m = jnPtr->mInvBindMatrix;
        for (int i = 0; i < 4; ++i)
        {
            LOGD("%s%.4f, %.4f, %.4f, %.4f\n", tab.c_str(), m[i][0], m[i][1], m[i][2], m[i][3]);
        }

        for (int i = 0; i < jnPtr->mChildrenID.size(); ++i)
        {
            dumpJoint(mJoints[jnPtr->mChildrenID.at(i)], level + 1);
        }
    }
}
