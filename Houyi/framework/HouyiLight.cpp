//
//  Light.cpp
//  houyi
//
//  Created by Lance Mao on 15/08/2011.
//  Copyright 2011 Dong wen. All rights reserved.
//

#include "HouyiLight.h"

namespace Houyi
{
    Light::Light() : mIsAmbient(false), mIsPosFixed(false)
    {
        mR = 1.0;
        mG = 1.0;
        mB = 1.0;
        mMultiplier = 1.0;
        mPosition[0] = 0;
        mPosition[1] = 0;
        mPosition[2] = 0;
        mPosition[3] = 1.0;
        mSelectable = false;
    }
    
    Light::~Light()
    {

    }

    const Matrix4& Light::getLocal2World()
    {
        return mIsPosFixed ? Matrix4::IDENTITY : SceneNode::getLocal2World();
    }
    
//    SceneNode* Light::clone()
//    {
//        Light* light = HouyiNew Light();
//        (*light) = *this;
//        return light;
//    }

    void Light::setPosition(float x, float y, float z, float a)
    {
        mPosition[0] = x;
        mPosition[1] = y;
        mPosition[2] = z;
        mPosition[3] = a;
    }

    void Light::setColor(float r, float g, float b)
    {
        mR = r;
        mG = g;
        mB = b;
    }

    void Light::setMultiplier(float multiplier)
    {
        mMultiplier = multiplier;
    }

    float* Light::getColor()
    {
        mColor[0] = mR * mMultiplier;
        mColor[1] = mG * mMultiplier;
        mColor[2] = mB * mMultiplier;
        mColor[3] = 1.0;
        return mColor;
    }

    float* Light::getPosition()
    {
        return mPosition;
    }
    
    float* Light::getWorldPosition(float pos[4])
    {
        Vector3 v = getLocal2World() * Vector3(mPosition[0], mPosition[1], mPosition[2]);
        pos[0] = v.x;
        pos[1] = v.y;
        pos[2] = v.z;
        pos[3] = mPosition[3];
        return pos;
    }

    void Light::setIsAmbient(bool ambient)
    {
        mIsAmbient = ambient;
    }
    
    bool Light::isAmbient()
    {
        return mIsAmbient;
    }
    
    void Light::setPosFixed(bool fixed)
    {
        mIsPosFixed = fixed;
    }
    
    bool Light::isPosFixed()
    {
        return mIsPosFixed;
    }

    void Light::setShadowCastViewMatrix(Matrix4 mat)
    {
        mShadowCastViewMatrix = mat;
    }

    Matrix4& Light::getShadowCastViewMatrix()
    {
        return mShadowCastViewMatrix;
    }

    void Light::setShadowCastProjectionwMatrix(Matrix4 mat)
    {
        mShadowCastProjectionMatrix = mat;
    }

    Matrix4& Light::getShadowCastProjectionwMatrix()
    {
        return mShadowCastProjectionMatrix;
    }
    
    void Light::setIsDirectional(bool isDirectional)
    {
        if (isDirectional)
        {
            mPosition[3] = 0;
        }
        else
        {
            mPosition[3] = 1;
        }
    }
    
    bool Light::isDirectional()
    {
        return mPosition[3] == 0;
    }
    
    void Light::writeObject(DataOutputStream& dos)
    {
        dos.writeBool(mIsPosFixed);
        dos.writeBool(mIsAmbient);
        dos.write(mPosition, sizeof(mPosition));
        dos.write(&mR, sizeof(float));
        dos.write(&mG, sizeof(float));
        dos.write(&mB, sizeof(float));
        dos.write(&mLocal, sizeof(Matrix4));
    }
    
    void Light::readObject(DataInputStream& dis)
    {
        mIsPosFixed = dis.readBool();
        mIsAmbient = dis.readBool();
        dis.read(mPosition, sizeof(mPosition));
        dis.read(&mR, sizeof(float));
        dis.read(&mG, sizeof(float));
        dis.read(&mB, sizeof(float));
        dis.read(&mLocal, sizeof(mLocal));
    }
}
