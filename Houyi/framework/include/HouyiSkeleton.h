#ifndef houyi_Skeleton_h
#define houyi_Skeleton_h

#include "SmartPtr.h"
#include "RefCount.h"
#include "HouyiArray.h"
#include "HouyiJointNode.h"
#include "HouyiSerializable.h"
#include <vector>

using namespace std;

namespace Houyi
{
	class Skeleton : public RefCountBase, public Serializable
	{
	public:
	    static const int MAX_BONE_COUNT;

	    Skeleton();
		~Skeleton();

		void update();

		void setJointNodes(JointNode** joints, int count);
		JointNode** getJointNodes();
		void setHardwareEnabled(bool isOn);
		bool isHardwareEnabled();

		void setJointsCount(int count);
		int getJointsCount();
        
        void setUseLinearRatioForBezier(bool isOn);
        bool isUseLinearRatioForBezier();
        
        int findJointIndexByName(string name);
        
        void pause();
        void resume();
        inline bool isPaused()
        {
            return mPausedTime != 0;
        }
        
        inline void setOwnJoints(bool owns)
        {
            mOwnsJoints = owns;
        }
        
        virtual void writeObject(DataOutputStream& dos);
        virtual void readObject(DataInputStream& dis);
        
		// debug
		void dumpJoints();
		void dumpJoint(JointNode* jnPtr, int level);

	private:
		void updateJoints(double delta, JointNode* jnPtr);
		Matrix4 lerp(const Transform& cur, const Transform& next, double ratio);
		Matrix4 berp(const Transform& cur, const Transform& next, double delta, double curFrameTime, double nextFrameTime, double ratio);
		void reset();
		void reset(JointNode* jnPtr);

	public:
		vector<int> mJNRoots; // index of the roots in mJoints array
		int mMaxKFCount;
		int mCurFrame;

	private:
        bool mOwnsJoints;
		JointNode** mJoints; // flat data
		int mTicker;
		bool mHardwareEnabled;
		int mJointsCount; // length of mJoints array
		double mStartTime;
        bool mUseLinearRaioForBezier;
        float mAnimationSpeed;
        double mPausedTime;
	};
    
    typedef SmartPtr<Skeleton> SkeletonPtr;
    
    class SkeletonRef
    {
    public:
        SkeletonRef(SkeletonPtr skeleton) : mSkeleton(skeleton)
        {
            
        }
        
        inline operator bool() const
        {
            return mSkeleton;
        }
        
        inline bool isHardwareEnabled()
        {
            return mSkeleton->isHardwareEnabled();
        }
        
        inline int getMaxKFCount()
        {
            return mSkeleton->mMaxKFCount;
        }
        
        inline void setMaxKFCount(int kfCount)
        {
            mSkeleton->mMaxKFCount = kfCount;
        }
        
        inline int getJointsCount()
        {
            return mIndices->size();
        }
        
        JointNode* getJointNode(int index)
        {
            int jointIndex = mIndices->getData()[index];
            return mSkeleton->getJointNodes()[jointIndex];
        }
        
    public:
        SkeletonPtr mSkeleton;
        SmartPtr<Array <int> > mIndices;
        //ArrayPtr<int> mIndices;
    };
}

#endif
