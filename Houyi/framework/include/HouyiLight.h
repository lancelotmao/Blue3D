//
//  Light.h
//  houyi
//
//  Created by Lance Mao on 15/08/2011.
//  Copyright 2011 Dong wen. All rights reserved.
//

#ifndef Light_H
#define Light_H

#include "HouyiSceneNode.h"
#include "HouyiSerializable.h"

namespace Houyi
{
    class Light: public SceneNode, public Serializable
    {
    private:
        float mX;
        float mY;
        float mZ;
        float mR;
        float mG;
        float mB;
        float mMultiplier;

        float mColor[4];
        float mPosition[4];
        
        bool mIsAmbient;
        bool mIsPosFixed;

        Matrix4 mShadowCastViewMatrix;
        Matrix4 mShadowCastProjectionMatrix;

    public:
        Light();
        virtual ~Light();
        
        virtual const Matrix4& getLocal2World();
//        virtual SceneNode* clone();

        void setPosition(float x, float y, float z, float a = 1);
        void setColor(float r, float g, float b);
        void setMultiplier(float multiplier);
        float* getColor();
        float* getPosition();
        float* getWorldPosition(float pos[4]);
        void setIsAmbient(bool ambient);
        bool isAmbient();
        void setPosFixed(bool fixed);
        bool isPosFixed();
        void setShadowCastViewMatrix(Matrix4 mat);
        Matrix4& getShadowCastViewMatrix();
        void setShadowCastProjectionwMatrix(Matrix4 mat);
        Matrix4& getShadowCastProjectionwMatrix();
        void setIsDirectional(bool isDirectional);
        bool isDirectional();
        
        virtual void writeObject(DataOutputStream& dos);
        virtual void readObject(DataInputStream& dis);
    };
}

#endif
