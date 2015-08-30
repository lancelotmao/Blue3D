//
//  Collada.h
//  houyi
//
//  Created by Lance Mao on 16/11/11.
//  Copyright (c) 2011 Dong wen. All rights reserved.
//

#ifndef houyi_Collada_h
#define houyi_Collada_h

#include <string>
#include <vector>
#include <map>

#include "RefCount.h"
#include "SmartPtr.h"
#include "HouyiObject.h"
#include "HouyiVector3.h"
#include "HouyiVector4.h"

using namespace std;

namespace collada
{
    // ----------------------------------//
    class AuthoringTool
    {
    public:
        string mTool;
    };
    
    // ----------------------------------//
    class Contributor
    {
    public:
        AuthoringTool mAuthoringTool;
    };
    
    // ----------------------------------//
    class Asset
    {
    public:
        Contributor mContributor;
        string mUpAxis;
    };
    
    class Matrix
    {
    public:
        Matrix();
        explicit Matrix(float data[16], bool copy = true);
        Matrix(const Matrix& m);
        float* getData();

        float& operator[](const int index);
        Matrix operator*(Matrix& m);
        Matrix operator*=(Matrix& m);
        Matrix& operator=(const Matrix& m);
        
        void makeIdentity();
        Matrix multiply(Matrix& m);
        Matrix multiply(float data[16]);
        void translate(float x, float y, float z);
        void scale(float x, float y, float z);
        void rotate(float x, float y, float z, float theta);
        
        void dump();

    private:
        float mData[16];
        float* mDataPtr; // may point to extern data
    };

    // ----------------------------------//
    class FloatArray: public RefCountBase
    {
    public:
        FloatArray();
        FloatArray(int cnt);
        ~FloatArray();
        FloatArray* clone();

        string mID;
        float *mFloats;
        unsigned int mCount;
    };

    // ----------------------------------//
    class NameArray: public RefCountBase
    {
    public:
        NameArray();
        ~NameArray();

        string mID;
        string *mNames;
        unsigned int mCount;
    };

    typedef SmartPtr<FloatArray> FloatArrayPtr;
    typedef SmartPtr<NameArray> NameArrayPtr;

    // ----------------------------------//
    class Param
    {
    public:
        string mName;
        string mType;
    };

    // ----------------------------------//
    class Accessor
    {
    public:
        Accessor() : mCount(0), mStride(0)
        {}
        string mSource;
        unsigned int mCount;
        unsigned int mStride;
        vector<Param> mParams;
    };

    // ----------------------------------//
    class Color
    {
    public:
        string mSID;
        float mR;
        float mG;
        float mB;
        float mA;

        Color()
        {
            mR = 0;
            mG = 0;
            mB = 0;
            mA = 1;
        }

        Color(const Color& rc)
        {
            mR = rc.mR;
            mG = rc.mG;
            mB = rc.mB;
            mA = rc.mA;
            mSID = rc.mSID;
        }
        
        Color(float r, float g, float b, float a) : mR(r), mG(g), mB(b), mA(a)
        {}
        
        inline bool isFullyTransparent()
        {
            return mR == 0 && mG == 0 && mB == 0 && mA == 0;
        }

        void normalize();
    };

    // ----------------------------------//
    class Point // actually point light
    {
    public:
        Color mColor;
    };

    // ----------------------------------//
    class Perspective
    {
    public:
        float mXFOV;
        float mAspectRatio;
        float mNear;
        float mFar;
    };

    // ----------------------------------//
    class TechniqueCommon
    {
    public:
        TechniqueCommon() : mHasAmbient(false), mIsDirectional(false)
        {}
        
        Accessor mAccessor;
        vector<string> mSymbol;
        vector<string> mMaterialTarget;
        Point mPoint;
        Perspective mPerspective;
        
        bool mHasAmbient;
        bool mIsDirectional;
    };

    // ----------------------------------//
    class Source: public RefCountBase
    {
    public:
        string mID;
        FloatArrayPtr mFloatArrayPtr;
        NameArrayPtr mNameArrayPtr;
        TechniqueCommon mTechniqueCommon;
    };

    typedef SmartPtr<Source> SourcePtr;

    // ----------------------------------//
    class Transform
    {
    public:
        enum
        {
            EMatrix,
            ETranslate,
            ERotate,
            EScale
        };

        enum
        {
            EConstInterpolation,
            ELinearInterpolation,
            EBezierInterpolation
        };

        int mType;
        int mInterpolation;
        string mSID;
        float mElements[4];
        float mInTangent[6];
        float mOutTangent[6];

        Transform();
        Transform(float* matrixData);
        void init();

        Matrix& getMatrix();
        void setMatrix(Matrix m);
        void translate(float x, float y, float z);
        void scale(float x, float y, float z);
        void rotate(float x, float y, float z, float degree);
        void generateMatrix();
        
    private:
        Matrix mMatrix;
    };

    // ----------------------------------//
    class KeyFrame
    {
    public:
        void addTransform(Transform trans)
        {
            mTransforms.push_back(trans);
        }

        KeyFrame()
        {}

        KeyFrame(const KeyFrame& src)
        {
            if (this != &src)
            {
                mTime = src.mTime;
                for (int i = 0;i < src.mTransforms.size();++i)
                {
                    mTransforms.push_back(src.mTransforms[i]);
                }
            }
        }

        KeyFrame& operator=(const KeyFrame& src)
        {
            if (this != &src)
            {
                mTime = src.mTime;
                for (int i = 0;i < src.mTransforms.size();++i)
                {
                    mTransforms.push_back(src.mTransforms[i]);
                }
            }
            return *this;
        }

        float mTime;
        vector<Transform> mTransforms;
        int mInterpolation; // assuming all transform use the same interpolation
    };

    // ----------------------------------//
    class Input
    {
    public:
        string mSemantic;
        string mSource;
        string mOffset;
        string mSet;
    };

    // ----------------------------------//
    class Vertices
    {
    public:
        string mID;
        vector<Input> mInputs;
    };

    // ----------------------------------//
    class P: public RefCountBase
    {
    public:
        P()
        {
            mIndices = 0;
            mCount = 0;
        }
        virtual ~P()
        {
            delete[] mIndices;
            mIndices = 0;
        }
        unsigned int* mIndices;
        unsigned int mCount;
    };

    typedef SmartPtr<P> PPtr;

    // ----------------------------------//
    class VCount: public P
    {
    public:
        VCount(int cnt)
        {
            mCount = cnt;
            mIndices = new unsigned int[mCount];
        }
        ~VCount()
        {
        }
    };
    typedef SmartPtr<VCount> VCountPtr;

    // ----------------------------------//
    class Image
    {
    public:
        string mID;
        string mName;
        string mInitFrom;
    };

    // ----------------------------------//
    class Diffuse
    {
    public:
        string mTextureID;
        Color mColor;
    };

    // ----------------------------------//
    class Phong
    {
    public:
        Phong()
        {
            mTransparent.mR = 0;
            mTransparent.mG = 0;
            mTransparent.mB = 0;
            mTransparent.mA = 1;
            mTransparency = 1;
            mSpecular.mA = 0;
            mReflectivity = 0;
        }
    
        Color mEmission;
        Color mAmbient;
        Diffuse mDiffuse;
        Color mSpecular;
        float mShininess;
        Color mTransparent;
        string mOpaque;
        float mTransparency;
        float mReflectivity;
    };

    // ----------------------------------//
    class Technique
    {
    public:
        Technique() : mIsDoubleSided(false)
        {}
        Phong mPhong;
        bool mIsDoubleSided;
    };

    // ----------------------------------//
    class Surface
    {
    public:
        string mType;
        string mInitFrom;
    };

    // ----------------------------------//
    class Sampler2D
    {
    public:
        string mSourceID;
        string mMinFilter;
        string mMagFilter;
    };

    // ----------------------------------//
    class Extra
    {
    public:
        Technique mTechnique;
    };

    // ----------------------------------//
    class NewParam
    {
    public:
        string mSID;
        Surface mSurface;
        Sampler2D mSampler2D;
    };

    // ----------------------------------//
    class ProfileCommon
    {
    public:
        Technique mTechnique;
        vector<NewParam> mNewParams;
    };

    // ----------------------------------//
    class Effect
    {
    public:
        string mID;
        string mName;
        ProfileCommon mProfile;
    };

    // ----------------------------------//
    class Material
    {
    public:
        string mID;
        string mName;
        string mBindTarget;
        string mEffectURL;
        Effect mEffect;
    };

    class Primitive: public Houyi::Object
    {
    public:
        Primitive()
        {
            mCount = 0;
        }
        virtual ~Primitive(){}

        unsigned int mCount;
        string mMaterialStr;
        Material mMaterial;
        vector<Input> mInputs;
    };

    // ----------------------------------//
    class Lines: public Primitive
    {
    public:
        PPtr mPPtr;
    };

    // ----------------------------------//
    class Triangles: public Primitive
    {
    public:
        PPtr mPPtr;
    };

    // ----------------------------------//
    class Polygons: public Primitive
    {
    public:
        vector<PPtr> mPPtrs;
    };

    // ----------------------------------//
    class Polylist: public Primitive
    {
    public:
        VCountPtr mVCountPtr;
        PPtr mPPtr;
    };

    // ----------------------------------//
    class Geometry : public Houyi::Object
    {
    public:
        virtual ~Geometry()
        {
        }
        string mID;
        string mURL;
        TechniqueCommon mTechniqueCommon;
    };

    // ----------------------------------//
    class Mesh: public Geometry
    {
    public:
        virtual ~Mesh()
        {
            for (int i = 0;i < mPrimitives.size();++i)
            {
                HouyiDelete(mPrimitives[i]);
            }
            mPrimitives.clear();
        }

        SourcePtr findSource(string sourceName)
        {
            for (int i = 0; i < mSourcePtrs.size(); ++i)
            {
                SourcePtr srcPtr = mSourcePtrs.at(i);
                if (sourceName.compare(srcPtr->mID) == 0
                        || sourceName.substr(1).compare(srcPtr->mID) == 0)
                {
                    return srcPtr;
                }
            }

            return NULL;
        }

        FloatArrayPtr findFloatArray(string sourceName)
        {
            for (int i = 0; i < mSourcePtrs.size(); ++i)
            {
                SourcePtr srcPtr = mSourcePtrs.at(i);
                if (sourceName.compare(srcPtr->mID) == 0
                        || sourceName.substr(1).compare(srcPtr->mID) == 0)
                {
                    return srcPtr->mFloatArrayPtr;
                }
            }

            return NULL;
        }

        vector<SourcePtr> mSourcePtrs;
        Vertices mVertices;
        vector<Primitive*> mPrimitives;
        vector<Lines*> mLines; // not own
        vector<Triangles*> mTriangles; // not own
        vector<Polygons*> mPolygons; // not own
        vector<Polylist*> mPolylist; // not own
    };
    
    // ----------------------------------//
    class InstanceGeometry
    {
    public:
        InstanceGeometry() : mGeoData(0)
        {}
        
        Geometry* mGeoData; // point to global
        Geometry mGeometry; // contains information for searching
        map<string, string> mMaterialMap; // primitive symbol -> material target
    };

    // ----------------------------------//
    class Sampler
    {
    public:
        Input mInput;
        Input mOutput;
        Input mInterpolation;
        Input mInTangent;
        Input mOutTangent;
    };

    // ----------------------------------//
    class Channel
    {
    public:
        string mSource;
        string mTarget;

        string getTargetID()
        {
            size_t len = mTarget.length();
            char target[len+1];
            strcpy(target, mTarget.c_str());
            char* p = strtok(target, "/\n");
            return string(p);
        }

        string getTargetSID()
        {
            size_t len = mTarget.length();
            char target[len+1];
            strcpy(target, mTarget.c_str());
            strtok(target, "/\n");
            char* p = strtok(NULL, "/\n");
            if (p)
            {
                return string(p);
            }
            else
            {
                return "";
            }
        }
    };

    // ----------------------------------//
    class Animation
    {
    public:
        SourcePtr mInputPtr;
        SourcePtr mOutputPtr;
        SourcePtr mInterpolationPtr;
        SourcePtr mInTangent;
        SourcePtr mOutTangent;
        Sampler mSampler;
        Channel mChannel;
        vector<SourcePtr> mSources;
    };

    // ----------------------------------//
    class AnimationGroup
    {
    public:
        vector<Animation> mAnimations;
        string mID;
        string mName;
    };

    class VertexWeight: public RefCountBase
    {
    public:
        VertexWeight()
        {
            mCount = 0;
            mVLength = 0;
            mVCount = 0;
            mVCountLength = 0;
            mV = 0;
        }
        ~VertexWeight()
        {
            delete[] mVCount;
            mVCount = 0;
            delete[] mV;
            mV = 0;
        }
        unsigned int mCount;
        Input mJoint;
        Input mWeight;
        unsigned int mVCountLength;
        unsigned int* mVCount;
        unsigned int mVLength;
        unsigned int* mV;
    };

    typedef SmartPtr<VertexWeight> VertexWeightPtr;

    class JointNode;

    // ----------------------------------//
    class Skin: public RefCountBase
    {
    public:
        Skin();
        ~Skin();

        Geometry* mGeometry; // not own
        Matrix mBindShapeMatrix;
        vector<SourcePtr> mSourcePtrs;
        Input mJoints;
        Input mInvBindMatrix;
        VertexWeightPtr mVertexWeightPtr;

        // data for mesh
        JointNode** mJNPtrTable; // found & copied from the scene node tree
        unsigned int* mJNIndex;
        float* mJNWeight;
        int mJNCount;
        int mJNIndexCount;
        vector<int> mRoots;
    };

    typedef SmartPtr<Skin> SkinPtr;

    // ----------------------------------//
    class Controller
    {
    public:
        vector<SkinPtr> mSkinPtrs;
        string mID;
        string mURL;
        TechniqueCommon mTechniqueCommon;
    };

    // ----------------------------------//
    class SceneNode: public Houyi::Object
    {
    public:
        SceneNode() : mParentPtr(0), mHasNonUniform(false){}
        virtual ~SceneNode(){}

        Matrix getMatrix(bool recalculate = false);
        void setMatrix(Matrix m);
        void addTransform(Transform transform);
        FloatArrayPtr generateMatrixData(SourcePtr src, string replaceSID, int stride);

        // common data
        string mID;
        string mSID;
        string mType;
        string mURL;
        vector<Transform> mTransforms;

        // tree node data
        SceneNode* mParentPtr;
        vector<SceneNode*> mChildren;

        // custom data
        vector<InstanceGeometry> mGeoInstance;
        Controller mController;
        TechniqueCommon mTechniqueCommon;
        
        bool mHasNonUniform;

    private:
        Matrix mMatrix;
    };

    // ----------------------------------//
    class JointNode: public Houyi::Object
    {
    public:
        JointNode();
        virtual ~JointNode();

        int getChildCount();
        void dump(int level);
        
        Matrix getMatrix(bool recalculate = false);
        void setMatrix(Matrix m);
        void addTransform(Transform transform);
        FloatArrayPtr generateMatrixData(SourcePtr src, string replaceSID, int stride);

        string mID;
        string mSID;
        string mType;
        vector<Transform> mTransforms;
        bool mHasNonUniform;
        
        JointNode* mParentPtr;
        vector<JointNode*> mChildren;
        vector<int> mChildrenID;

        // data for mesh
        string mParentIDStr;
        int mParentID;
        Matrix mInvBindMatrix;
        KeyFrame* mKeyFrames;
        int mKFCount;
        bool mAnimSet;
        
    private:
        Matrix mMatrix;
    };

    // ----------------------------------//
    class Light: public SceneNode
    {
    public:
        TechniqueCommon mTechniqueCommon;
    };

    // ----------------------------------//
    class Optics
    {
    public:
        TechniqueCommon mTechniqueCommon;
    };

    // ----------------------------------//
    class Camera
    {
    public:
        string mID;
        string mName;
        string mSID;
        Optics mOptics;
    };

    // ----------------------------------//
    class VisualScene
    {
    public:
        vector<JointNode*> mJointRootPtrs;
        vector<SceneNode*> mSceneNodePtrs;
        vector<SceneNode*> mLights;
        vector<SceneNode*> mCameras;
        vector<SceneNode*> mCameraTargets;
        string mID;
        string mName;
    };
    
    class Stats
    {
    public:
        Stats() : mNbLine(0), mNbTriangle(0), mNbPolygon(0), mNbPolylist(0), mNodeHasGeoData(0)
        {}
        
        int mNbLine;
        int mNbTriangle;
        int mNbPolygon;
        int mNbPolylist;
        int mNodeHasGeoData;
    };

    // ----------------------------------//
    class Collada
    {
    public:
        ~Collada();
        
        Stats mStats;
        Asset mAsset;
        vector<Image> mImages;
        vector<Material> mMaterials;
        vector<Effect> mEffects;
        vector<Geometry*> mGeometryPtrs;
        vector<Light*> mLights;
        vector<Camera> mCameras;
        vector<Controller> mControllers;
        vector<AnimationGroup> mAnimationGroups;
        vector<VisualScene> mVisualScenes;
        // from library_nodes
        vector<SceneNode*> mNodes;
        // might from visual scene or library_nodes
        vector<SceneNode*> mInstanceNodePtrs;
        // use a single vector to track scene nodes as they might be shared
        vector<SceneNode*> mSceneNodeOwner;
        vector<JointNode*> mJointNodeOwner;
    };
}

#endif
