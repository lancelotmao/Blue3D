//
//  RubikResolver.h
//  Houyi
//
//  Created by Lance Mao on 3/3/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef __Houyi__RubikResolver__
#define __Houyi__RubikResolver__

#include <string>
#include <vector>
#include "HouyiAllocator.h"

using namespace std;

namespace Houyi
{
	class RNode : public Allocator
	{
	public:
        
		RNode(float rotateEffort, int distanceEffort, int op, char state[20][6])
		{
			mParent = 0;
			mLeft = 0;
			mRight = 0;
			mRotateEffort = rotateEffort;
			mDistanceEffort = distanceEffort;
			mTotalEffort = mRotateEffort + mDistanceEffort;
			mOp = op;
			mOpen = true;
            mSearchedCount = 0;
            mResultParent = 0;

			for (int i = 0;i < 20;++i)
			{
				for (int j = 0;j < 6;++j)
				{
					mState[i][j] = state[i][j];
				}
			}
		}
        
        ~RNode()
        {
            if (mLeft)
            {
                HouyiDelete(mLeft);
            }
            else if (mRight)
            {
                HouyiDelete(mRight);
            }
        }

		RNode* mParent;
		RNode* mLeft;
		RNode* mRight;
		float mTotalEffort; // = rotateEffort + distanceEffort
		float mRotateEffort;
		int mDistanceEffort;
		int mOp;
		char mState[20][6];
        bool mOpen;
        int mSearchedCount;
        RNode* mResultParent;
	};

	class RubikResult : public Allocator
	{
	public:
		RubikResult(int* steps, int count) : mSteps(steps), mStepCount(count)
		{

		}

		virtual ~RubikResult()
		{
			if (mSteps)
			{
				delete[] mSteps;
				mSteps = 0;
			}
		}

		int* mSteps;
		int mStepCount;
	};

	class Face
	{
	public:
		int mMCIndex[8];
	};

	class MCState
	{
	public:
		char mColor[6];
		char mKey;
		int mEffort;
		int mStep[16];
	};

    class RubikResolver : public Allocator
    {
    public:
    	static const char GOAL[20][6];
    	static const char FACE_COLOR[6];
    	enum
		{
			FRONT,
			TOP,
			LEFT,
			BACK,
			BOTTOM,
			RIGHT
		};

    	enum
    	{
    		CG, CW, CO, CY, CB, CR
    	};

    	enum
    	{
    		GR,
    		GL,
    		WR,
    		WL,
    		OR,
    		OL,
    		YR,
    		YL,
    		BR,
    		BL,
    		RR,
    		RL,
    		OP_COUNT
    	};
        
        enum
        {
            ES_PLAIN, // use difference which is often wrong
            ES_FACE_MOST, // if one face has more solved color, we mark higher score
            ES_COLOR_MOST, // for a given face more color with same value, more score
            ES_FACE_COVARIANCE, // face tend to have same color groups
            ES_SINGLE_MC_EFFORT, // simply add every mc's effort
            ES_GROUP_MC_EFFORT // rollback if lowest mc effort increase group effort
        };

        RubikResolver(char init[20][6]);
        virtual  ~RubikResolver();

        char getState(int cubeIndex, int orientationIndex) const;
        void setState(char (&state)[20][6]);
        
        RubikResult* resolve();

        static void updateState(char state[20][6], int op);
        static bool isSolved(const char (&state)[20][6], const char (&goal)[20][6]);
        static bool stateEqual(const char* s1, const char* s2);
        static int color2Int(char c);
        static string toString(const char (&state)[20][6]);

    private:
		RNode* lestEffort(RNode* cur);
		void addNode(RNode* cur, RNode* node);
        void disable(RNode* node);
        void remove(RNode* node);
        void printNode(RNode* node);
		const MCState& getMCState(char (&state)[6]);
		int calculateDistance(char (&state)[20][6], char (&goal)[20][6]);
        RubikResult* generateResult(RNode* solved, int lastStep);

        // manual resolvers
        void findGCross(char (&state)[20][6]);
        void firstLayer(char (&state)[20][6]);
        void secondLayer(char (&state)[20][6]);
        void bottomCross(char (&state)[20][6]);
        void bottomFace(char (&state)[20][6]);
        void bottomCorner(char (&state)[20][6]);
        void finalTouch(char (&state)[20][6]);
        void insertStep(char (&state)[20][6], char step);
        void insertSteps(char (&state)[20][6], int count, char* steps);
        
    private:
        static const MCState MCSS[20][24]; // mini cube state set
        static const int CORNER_IDX[8];
        static const int EDGE_IDX[12];

        static const char STAGE0_MOVES[4][24][24];
        static const char STAGE1_MOVES[4][24][24];
        static const char STAGE2_MOVES[4][24][32];
        
    private:
        char mState[20][6];
        char mGoal[20][6];
        RNode* mHead;
        int mEstimateStrategy;
        int mStageIndex;
        bool mManualEnabled;
        Face G;
        Face W;
        Face O;
        Face Y;
        Face B;
        Face R;
        Face mFaces[6];
        
        double mStartingTime;

        vector<int> mManualSteps;
    };
}

#endif /* defined(__Houyi__RubikResolver__) */
