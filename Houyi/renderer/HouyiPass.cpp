#include "HouyiPass.h"
#include "Platform.h"

namespace Houyi
{
    Pass::Pass(int program) : mProgram(program), mPassListener(0), mPassState(EInvalid), mStartTimeMillis(0)
    {
    }

    Pass::Pass(const char* vertex, const char* fragment)
        : mVertex(vertex), mFragment(fragment), mProgram(-1), mPassListener(0), mPassState(EInvalid), mStartTimeMillis(0)
    {
    }

    Pass::~Pass()
    {
    }

    int Pass::getProgram()
    {
        return mProgram;
    }

    void Pass::setProgram(int program)
    {
        mProgram = program;
    }

    int Pass::getPassState()
    {
        return mPassState;
    }

    void Pass::setPassState(int state)
    {
        mPassState = state;
    }

    void Pass::beginPass()
    {
        if (mStartTimeMillis == 0)
        {
            mStartTimeMillis = currentTimeMillis();
        }
        mDeltaTimeMillis = currentTimeMillis() - mStartTimeMillis;
        onStart();
        if (mPassListener)
        {
            mPassListener->onPassBegin(this);
        }
    }

    void Pass::renderPass(Scene* scene)
    {
        onRender(scene);
    }

    void Pass::endPass()
    {
        onFinish();
        if (mPassListener)
        {
            mPassListener->onPassEnd(this);
        }
    }

    void Pass::onCreate()
    {
    }

    void Pass::onStart()
    {
    }

    void Pass::onFinish()
    {
    }

    bool Pass::linearizeZ()
    {
        return false;
    }

    PassResult& Pass::getMyResult()
    {
        return mMyResult;
    }

    void Pass::setMyResult(PassResult result)
    {
        mMyResult = result;
    }

    PassResult& Pass::getLastPassResult()
    {
        return mLastPassResult;
    }

    void Pass::setLastPassResult(PassResult result)
    {
        mLastPassResult = result;
    }

    void Pass::invalidate()
    {
        if (mPassState != EIdle)
        {
            mPassState = EInvalid;
        }
    }
}
