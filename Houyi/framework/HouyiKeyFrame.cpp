#include "HouyiKeyFrame.h"

namespace Houyi
{
    KeyFrame::KeyFrame()
    {
        mInterpolation = Transform::EBezierInterpolation;
    }

    void KeyFrame::addTransform(Transform trans)
    {
        mTransforms.push_back(trans);
    }
    
    Matrix4 KeyFrame::generateMatrix(bool reversely)
    {
        mMatrix = Matrix4::IDENTITY;
        for (int i = 0;i < mTransforms.size();++i)
        {
            Transform& trans = mTransforms[reversely ? mTransforms.size() - 1 - i : i];
            mMatrix = trans.getMatrix() * mMatrix;
        }
        return mMatrix;
    }

    Matrix4& KeyFrame::getMatrix(bool regenerate)
    {
        if (regenerate)
        {
            generateMatrix();
        }
        return mMatrix;
    }

    void KeyFrame::writeObject(DataOutputStream& dos)
    {
        dos.write(&mTime, sizeof(mTime));
        dos.writeInt(mTransforms.size());
        for (int i = 0;i < mTransforms.size();++i)
        {
            Transform& trans = mTransforms[i];
            dos.writeInt(trans.mType);
            dos.write(&trans.mMatrix, sizeof(trans.mMatrix));
            dos.write(&trans.mElements, sizeof(trans.mElements));
            dos.writeInt(trans.mInterpolation);
            dos.write(&trans.mInTangent, sizeof(trans.mInTangent));
            dos.write(&trans.mOutTangent, sizeof(trans.mOutTangent));
        }
        dos.writeInt(mInterpolation);
    }
    
    void KeyFrame::readObject(DataInputStream& dis)
    {
        dis.read(&mTime, sizeof(mTime));
        int transSize = dis.readInt();
        for (int i = 0;i < transSize;++i)
        {
            int type = dis.readInt();
            Transform trans(type, 0, 0, 0, 0);
            dis.read(&trans.mMatrix, sizeof(trans.mMatrix));
            dis.read(&trans.mElements, sizeof(trans.mElements));
            trans.mInterpolation = dis.readInt();
            dis.read(&trans.mInTangent, sizeof(trans.mInTangent));
            dis.read(&trans.mOutTangent, sizeof(trans.mOutTangent));
            mTransforms.push_back(trans);
        }
        generateMatrix();
        mInterpolation = dis.readInt();
    }
}
