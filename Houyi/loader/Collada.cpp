//
//  Collada.cpp
//  houyi
//
//  Created by Lance Mao on 16/11/11.
//  Copyright (c) 2011 Dong wen. All rights reserved.
//

#include <iostream>

#include "Collada.h"
#include "SmartPtr.h"
#include "HouyiLog.h"

using namespace collada;

#pragma mark Float Array
FloatArray::FloatArray()
{
    mFloats = 0;
    mCount = 0;
}

FloatArray::FloatArray(int cnt) : mCount(cnt)
{
    mFloats = new float[cnt];
}

FloatArray::~FloatArray()
{
    delete[] mFloats;
    mFloats = 0;
}

FloatArray* FloatArray::clone()
{
    FloatArray* res = new FloatArray();
    res->mID = mID;
    res->mCount = mCount;
    res->mFloats = new float[mCount];
    for (int i = 0; i < mCount; ++i)
    {
        res->mFloats[i] = mFloats[i];
    }

    return res;
}

#pragma mark Matrix
Matrix::Matrix()
{
    mDataPtr = mData;
    makeIdentity();
}

Matrix::Matrix(float data[16], bool copy)
{
    if (copy)
    {
        memcpy(mData, data, 16 * sizeof(float));
        mDataPtr = mData;
    }
    else
    {
        mDataPtr = data;
    }
}

float* Matrix::getData()
{
    return mDataPtr;
}

float& Matrix::operator[](const int index)
{
    return mDataPtr[index];
}

void Matrix::makeIdentity()
{
    mDataPtr[0] = 1;mDataPtr[4] = 0;mDataPtr[8] = 0;mDataPtr[12] = 0;
    mDataPtr[1] = 0;mDataPtr[5] = 1;mDataPtr[9] = 0;mDataPtr[13] = 0;
    mDataPtr[2] = 0;mDataPtr[6] = 0;mDataPtr[10]= 1;mDataPtr[14] = 0;
    mDataPtr[3] = 0;mDataPtr[7] = 0;mDataPtr[11]= 0;mDataPtr[15] = 1;
}

Matrix Matrix::operator*(Matrix& m)
{
    return multiply(m);
}

Matrix Matrix::operator*=(Matrix& m)
{
    Matrix res = multiply(m);
    memcpy(mDataPtr, res.mDataPtr, 16 * sizeof(float));
    return res;
}

Matrix& Matrix::operator=(const Matrix& m)
{
    if (this != &m)
    {
        memcpy(this->mData, m.mData, 16 * sizeof(float));
        this->mDataPtr = this->mData;
    }
    return *this;
}

Matrix::Matrix(const Matrix& m)
{
    if (this != &m)
    {
        memcpy(this->mData, m.mData, 16 * sizeof(float));
        this->mDataPtr = this->mData;
    }
}

Matrix Matrix::multiply(float data[16])
{
    Matrix m(data);
    return multiply(m);
}

Matrix Matrix::multiply(Matrix& m)
{
    Matrix res;
    res.mDataPtr[0] = mDataPtr[0] * m.mDataPtr[0] + mDataPtr[1] * m.mDataPtr[4] + mDataPtr[2] * m.mDataPtr[8] + mDataPtr[3] * m.mDataPtr[12];
    res.mDataPtr[1] = mDataPtr[0] * m.mDataPtr[1] + mDataPtr[1] * m.mDataPtr[5] + mDataPtr[2] * m.mDataPtr[9] + mDataPtr[3] * m.mDataPtr[13];
    res.mDataPtr[2] = mDataPtr[0] * m.mDataPtr[2] + mDataPtr[1] * m.mDataPtr[6] + mDataPtr[2]* m.mDataPtr[10] + mDataPtr[3] * m.mDataPtr[14];
    res.mDataPtr[3] = mDataPtr[0] * m.mDataPtr[3] + mDataPtr[1] * m.mDataPtr[7] + mDataPtr[2]* m.mDataPtr[11] + mDataPtr[3] * m.mDataPtr[15];
    res.mDataPtr[4] = mDataPtr[4] * m.mDataPtr[0] + mDataPtr[5] * m.mDataPtr[4] + mDataPtr[6] * m.mDataPtr[8] + mDataPtr[7] * m.mDataPtr[12];
    res.mDataPtr[5] = mDataPtr[4] * m.mDataPtr[1] + mDataPtr[5] * m.mDataPtr[5] + mDataPtr[6] * m.mDataPtr[9] + mDataPtr[7] * m.mDataPtr[13];
    res.mDataPtr[6] = mDataPtr[4] * m.mDataPtr[2] + mDataPtr[5] * m.mDataPtr[6] + mDataPtr[6]* m.mDataPtr[10] + mDataPtr[7] * m.mDataPtr[14];
    res.mDataPtr[7] = mDataPtr[4] * m.mDataPtr[3] + mDataPtr[5] * m.mDataPtr[7] + mDataPtr[6]* m.mDataPtr[11] + mDataPtr[7] * m.mDataPtr[15];
    res.mDataPtr[8] = mDataPtr[8] * m.mDataPtr[0] + mDataPtr[9] * m.mDataPtr[4] + mDataPtr[10] * m.mDataPtr[8]+ mDataPtr[11] * m.mDataPtr[12];
    res.mDataPtr[9] = mDataPtr[8] * m.mDataPtr[1] + mDataPtr[9] * m.mDataPtr[5] + mDataPtr[10] * m.mDataPtr[9]+ mDataPtr[11] * m.mDataPtr[13];
    res.mDataPtr[10]= mDataPtr[8] * m.mDataPtr[2] + mDataPtr[9] * m.mDataPtr[6] + mDataPtr[10]* m.mDataPtr[10]+ mDataPtr[11] * m.mDataPtr[14];
    res.mDataPtr[11]= mDataPtr[8] * m.mDataPtr[3] + mDataPtr[9] * m.mDataPtr[7] + mDataPtr[10]* m.mDataPtr[11]+ mDataPtr[11] * m.mDataPtr[15];
    res.mDataPtr[12]= mDataPtr[12] * m.mDataPtr[0]+ mDataPtr[13] * m.mDataPtr[4]+ mDataPtr[14] * m.mDataPtr[8]+ mDataPtr[15] * m.mDataPtr[12];
    res.mDataPtr[13]= mDataPtr[12] * m.mDataPtr[1]+ mDataPtr[13] * m.mDataPtr[5]+ mDataPtr[14] * m.mDataPtr[9]+ mDataPtr[15] * m.mDataPtr[13];
    res.mDataPtr[14]= mDataPtr[12] * m.mDataPtr[2]+ mDataPtr[13] * m.mDataPtr[6]+ mDataPtr[14]* m.mDataPtr[10]+ mDataPtr[15] * m.mDataPtr[14];
    res.mDataPtr[15]= mDataPtr[12] * m.mDataPtr[3]+ mDataPtr[13] * m.mDataPtr[7]+ mDataPtr[14]* m.mDataPtr[11]+ mDataPtr[15] * m.mDataPtr[15];
    
    return res;
}

void Matrix::translate(float x, float y, float z)
{
    Matrix t;
    t.makeIdentity();
    
    t[3] = x;
    t[7] = y;
    t[11] = z;
    (*this) *= t;
}

void Matrix::scale(float x, float y, float z)
{
    Matrix s;
    s.makeIdentity();

    s[0] = x;
    s[5] = y;
    s[10] = z;
    (*this) *= s;
}

void Matrix::rotate(float x, float y, float z, float theta)
{
    if (theta == 0) {
		return;
	}
    
	/*
	 * This function performs an axis/angle rotation. (x,y,z) is any vector
	 * on the axis. For greater speed, always use a unit vector (length =
	 * 1). In this version, we will assume an arbitrary length and
	 * normalize.
	 */
    
	double length;
	double c, s, t;
    
	// normalize
	length = sqrt(x * x + y * y + z * z);
    
	// too close to 0, can't make a normalized vector
	if (length < .000001)
		return;
    
	x /= length;
	y /= length;
	z /= length;
    
	// do the trig
	c = cos(theta);
	s = sin(theta);
	t = 1 - c;
    
	// build the rotation matrix
	Matrix m;
	float* ViewRotationMatrix = m.getData();
	ViewRotationMatrix[0] = (float) (t * x * x + c);
	ViewRotationMatrix[4] = (float) (t * x * y - s * z);
	ViewRotationMatrix[8] = (float) (t * x * z + s * y);
	ViewRotationMatrix[12] = 0;
    
	ViewRotationMatrix[1] = (float) (t * x * y + s * z);
	ViewRotationMatrix[5] = (float) (t * y * y + c);
	ViewRotationMatrix[9] = (float) (t * y * z - s * x);
	ViewRotationMatrix[13] = 0;
    
	ViewRotationMatrix[2] = (float) (t * x * z - s * y);
	ViewRotationMatrix[6] = (float) (t * y * z + s * x);
	ViewRotationMatrix[10] = (float) (t * z * z + c);
	ViewRotationMatrix[14] = 0;

	ViewRotationMatrix[3] = 0;
	ViewRotationMatrix[7] = 0;
	ViewRotationMatrix[11] = 0;
	ViewRotationMatrix[15] = 1;
    
	(*this) *= m;
}

void Matrix::dump()
{
    float* m = mDataPtr;
    for (int i = 0; i < 4; ++i)
    {
        LOGD("M(%d) = %f, %f, %f, %f\n", i, m[i*4], m[i*4+1], m[i*4+2], m[i*4+3]);
    }
}

#pragma mark Name Array
NameArray::NameArray()
{
    mNames = 0;
    mCount = 0;
}
NameArray::~NameArray()
{
    delete[] mNames;
    mNames = 0;
}

#pragma mark Joint Node
JointNode::JointNode() : mAnimSet(false)
{
    mKeyFrames = 0;
    mKFCount = 0;
    mParentID = -1;
    mParentPtr = 0;
}

JointNode::~JointNode()
{
    delete[] mKeyFrames;
    mKeyFrames = 0;
}

int JointNode::getChildCount()
{
    int res = mChildren.size();
    for (int i = 0; i < mChildren.size(); ++i)
    {
        res += mChildren.at(i)->getChildCount();
    }

    return res;
}

void JointNode::dump(int level)
{
    for (int i = 0; i < level; ++i)
    {
        printf("    ");
    }

    printf("%s\n", mName.c_str());
    for (int i = 0; i < mChildren.size(); ++i)
    {
        mChildren.at(i)->dump(level + 1);
    }
}

Matrix JointNode::getMatrix(bool recalculate)
{
    if (recalculate)
    {
        mMatrix.makeIdentity();
        for (int i = 0;i < mTransforms.size(); ++i)
        {
            Matrix m = mTransforms[i].getMatrix();
            mMatrix *= m;
        }
    }
    return mMatrix;
}

void JointNode::setMatrix(Matrix m)
{
    mMatrix = m;
}

void JointNode::addTransform(Transform transform)
{
    if (transform.mType == Transform::EScale)
    {
        float sx = transform.mElements[0];
        float sy = transform.mElements[1];
        float sz = transform.mElements[2];
        if (sx != sy || sy != sz)
        {
            mHasNonUniform = true;
        }
    }
    mMatrix *= transform.getMatrix();
    mTransforms.push_back(transform);
}

FloatArrayPtr JointNode::generateMatrixData(SourcePtr src, string replaceSID, int stride)
{
    int pos = replaceSID.find('.');
    if (pos != string::npos)
    {
        replaceSID = replaceSID.substr(0, pos);
    }
    
    int matCnt = src->mFloatArrayPtr->mCount / stride;
    int dataCnt = matCnt * 16;
    FloatArrayPtr ret = new FloatArray(dataCnt);
    float* input = src->mFloatArrayPtr->mFloats;
    float* output = ret->mFloats;
    
    for (int i = 0, j = 0; i < matCnt; ++i, j += stride)
    {
        Matrix m(output + i * 16, false);
        m.makeIdentity();
        
        for (int k = 0, n = mTransforms.size();k < n;++k)
        {
            Transform trans = mTransforms[k];
            if (trans.mSID.compare(replaceSID) == 0)
            {
                if (trans.mType == Transform::ETranslate)
                {
                    Matrix t;
                    t.translate(input[j], input[j + 1], input[j + 2]);
                    m *= t;
                }
                else if (trans.mType == Transform::ERotate)
                {
                    Matrix r;
                    r.rotate(trans.mElements[0], trans.mElements[1], trans.mElements[2],
                             -Houyi::Math::DegreesToRadians(input[j]));
                    m *= r;
                }
                else
                {
                    memcpy(m.getData(), &input[i * 16], 16 * sizeof(float));
                }
            }
            else
            {
                m *= trans.getMatrix();
            }
        }
    }
    
    return ret;
}

#pragma mark Skin
Skin::Skin()
{
    mJNPtrTable = 0;
    mJNCount = 0;
    mJNIndexCount = 0;
    mJNIndex = 0;
    mJNWeight = 0;
    mGeometry = 0;
}

Skin::~Skin()
{
    delete[] mJNPtrTable;
    mJNPtrTable = 0;

    delete[] mJNIndex;
    mJNIndex = 0;

    delete[] mJNWeight;
    mJNWeight = 0;

    mRoots.clear();
}

#pragma mark Transform
Transform::Transform()
{
    init();
}

Transform::Transform(float* matrixData)
{
    init();
    mMatrix = Matrix(matrixData);
}

void Transform::init()
{
    mInterpolation = EConstInterpolation;
    for (int i = 0;i < 6;++i)
    {
        mInTangent[i] = 0;
        mOutTangent[i] = 0;
    }
}

Matrix& Transform::getMatrix()
{
    return mMatrix;
}

void Transform::setMatrix(Matrix m)
{
    mMatrix = m;
}

void Transform::translate(float x, float y, float z)
{
    mElements[0] = x;
    mElements[1] = y;
    mElements[2] = z;
    mMatrix.translate(x, y, z);
}

void Transform::scale(float x, float y, float z)
{
    mElements[0] = x;
    mElements[1] = y;
    mElements[2] = z;
    mMatrix.scale(x, y, z);
}

void Transform::rotate(float x, float y, float z, float degree)
{
    // TODO find rotate angle sign
    mElements[0] = x;
    mElements[1] = y;
    mElements[2] = z;
    mElements[3] = -Houyi::Math::DegreesToRadians(degree);
    mMatrix.rotate(x, y, z, mElements[3]);
}

void Transform::generateMatrix()
{
    if (mType == ETranslate)
    {
        mMatrix.makeIdentity();
        translate(mElements[0], mElements[1], mElements[2]);
    }
    else if (mType == ERotate)
    {
        mMatrix.makeIdentity();
        rotate(mElements[0], mElements[1], mElements[2], mElements[3]);
    }
    else if (mType == EScale)
    {
        mMatrix.makeIdentity();
        scale(mElements[0], mElements[1], mElements[2]);
    }
}

#pragma mark SceneNode
Matrix SceneNode::getMatrix(bool recalculate)
{
    if (recalculate)
    {
        mMatrix.makeIdentity();
        for (int i = 0;i < mTransforms.size(); ++i)
        {
            Matrix m = mTransforms[i].getMatrix();
            mMatrix *= m;
        }
    }
    return mMatrix;
}

void SceneNode::setMatrix(Matrix m)
{
    mMatrix = m;
}

void SceneNode::addTransform(Transform transform)
{
    if (transform.mType == Transform::EScale)
    {
        float sx = transform.mElements[0];
        float sy = transform.mElements[1];
        float sz = transform.mElements[2];
        if (sx != sy || sy != sz)
        {
            mHasNonUniform = true;
        }
    }
    mMatrix *= transform.getMatrix();
    mTransforms.push_back(transform);
}

FloatArrayPtr SceneNode::generateMatrixData(SourcePtr src, string replaceSID, int stride)
{
    int pos = replaceSID.find('.');
    if (pos != string::npos)
    {
        replaceSID = replaceSID.substr(0, pos);
    }

    int matCnt = src->mFloatArrayPtr->mCount / stride;
    int dataCnt = matCnt * 16;
    FloatArrayPtr ret = new FloatArray(dataCnt);
    float* input = src->mFloatArrayPtr->mFloats;
    float* output = ret->mFloats;

    for (int i = 0, j = 0; i < matCnt; ++i, j += stride)
    {
        Matrix m(output + i * 16, false);
        m.makeIdentity();

        for (int k = 0, n = mTransforms.size();k < n;++k)
        {
            Transform trans = mTransforms[k];
            if (trans.mSID.compare(replaceSID) == 0)
            {
                if (trans.mType == Transform::ETranslate)
                {
                    Matrix t;
                    t.translate(input[j], input[j + 1], input[j + 2]);
                    m *= t;
                }
                else if (trans.mType == Transform::ERotate)
                {
                    Matrix r;
                    r.rotate(trans.mElements[0], trans.mElements[1], trans.mElements[2],
                            -Houyi::Math::DegreesToRadians(input[j]));
                    m *= r;
                }
                else
                {
                    memcpy(m.getData(), &input[i * 16], 16 * sizeof(float));
                }
            }
            else
            {
                m *= trans.getMatrix();
            }
        }
    }

    return ret;
}

void Color::normalize()
{
    Houyi::Vector3 v = Houyi::Vector3(mR, mG, mB);
    v.normalise();
    mR = v.x;
    mG = v.y;
    mB = v.z;
}

#pragma mark Collada
Collada::~Collada()
{
    for (int i = 0;i < mGeometryPtrs.size();++i)
    {
        HouyiDelete(mGeometryPtrs[i]);
    }
    
    for (int i = 0, n = mSceneNodeOwner.size();i < n;++i)
    {
        HouyiDelete(mSceneNodeOwner[i]);
    }
    
    for (int i = 0, n = mJointNodeOwner.size();i < n;++i)
    {
        HouyiDelete(mJointNodeOwner[i]);
    }

    for (int i = 0, n = mLights.size();i < n;++i)
    {
        HouyiDelete(mLights[i]);
    }
}
