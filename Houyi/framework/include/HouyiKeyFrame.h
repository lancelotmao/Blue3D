#ifndef __Houyi__HouyiKeyFrame__
#define __Houyi__HouyiKeyFrame__

#include <vector>
#include "HouyiTransform.h"
#include "HouyiSerializable.h"

using namespace std;

namespace Houyi
{
    class KeyFrame : public Serializable
    {
    public:
        KeyFrame();
        void addTransform(Transform trans);
        Matrix4 generateMatrix(bool reversely = false);
        Matrix4& getMatrix(bool regenerate = false);
        
        virtual void writeObject(DataOutputStream& dos);
        virtual void readObject(DataInputStream& dis);

    public:
        float mTime;
        vector<Transform> mTransforms;
        Matrix4 mMatrix;

        // assuming all transform in a given key frame use the same interpolation
        int mInterpolation;
    };
}

#endif
