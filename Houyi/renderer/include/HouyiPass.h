#ifndef __Houyi__Pass__
#define __Houyi__Pass__

#include "HouyiObject.h"
#include "HouyiScene.h"

namespace Houyi
{
    class PassListener;

    class PassResult
    {
    public:
        vector<int> mTextureIds;
    };

    class Pass : public Object
    {
    public:
        enum
        {
            ENormal = 0,
            EDebug,
            EIdle,
            EInvalid
        };

        Pass(int program);
        Pass(const char* vertex, const char* fragment);
        virtual ~Pass();

        virtual int getProgram();
        virtual void setProgram(int program);
        virtual int getPassState();
        virtual void setPassState(int state);

        // called by Renderer
        virtual void beginPass();
        virtual void renderPass(Scene* scene);
        virtual void endPass();

        virtual PassResult& getMyResult();
        virtual void setMyResult(PassResult result);
        virtual PassResult& getLastPassResult();
        virtual void setLastPassResult(PassResult result);
        virtual void invalidate();

        inline const char* getVertexShader() const
        {
            return mVertex;
        }

        inline const char* getFragmentShader() const
        {
            return mFragment;
        }
        
        // if this returns true we will divide projection matrix by 'far'
        // and in shader, we should have output.z *= output.w
        virtual bool linearizeZ();

        virtual int getAtribLocation(string key) = 0;
        virtual int getUniformLocation(string key) = 0;
        virtual void uploadWorldMat(const Matrix4& m) = 0;
        virtual void uploadNormalMat(const Matrix3& m) = 0;
        virtual void uploadEyeMat(const Matrix4& m) = 0;
        virtual void uploadProjectionMat(const Matrix4& m) = 0;
        virtual void uploadMatrix4(const string& uniformName, const Matrix4& m, bool transpose = false) = 0;
        virtual void uploadMatrix3(const string& uniformName, const Matrix3& m, bool transpose = false) = 0;
        
    protected:
        // one time initialization
        virtual void onCreate();

        // obviously sub class has to provide render functionality
        virtual void onStart();
        virtual void onRender(Scene* scene) = 0;
        virtual void onFinish();
    protected:
        int mProgram;
        PassListener* mPassListener;

        int mPassState;
        PassResult mLastPassResult;
        PassResult mMyResult;

        // mark start time when pass is created so that later we can get delta time
        double mStartTimeMillis;
        double mDeltaTimeMillis;

        // back shader so that when context is lost we will reload from it
        const char* mVertex;
        const char* mFragment;
    };

    class PassListener
    {
    public:
        virtual ~PassListener(){}
        virtual void onPassBegin(const Pass* pass) = 0;
        virtual void onPassEnd(const Pass* pass) = 0;
    };
}

#endif
