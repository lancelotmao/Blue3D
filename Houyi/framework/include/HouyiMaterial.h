//
//  HouyiMaterial.h
//  houyi
//
//  Created by Lance Mao on 17/08/2011.
//  Copyright 2011 Dong wen. All rights reserved.
//
#ifndef HOUYIMATERIAL_H
#define HOUYIMATERIAL_H

#include "HouyiObject.h"
#include "HouyiColor.h"
#include "HouyiTexture.h"
#include "HouyiRenderState.h"
#include "HouyiSerializable.h"
#include <vector>

namespace Houyi
{
    class Pass;
    class Scene;
    
    class Material: public Object, public Serializable
    {
    public:
        enum TransparentMode
        {
            EAZero,
            EAOne,
            ERGBZero,
            // for views which only has fixed alpha value
            EView
        };

        Material();
        Material(string name);
        virtual ~Material(){}
        // 0 by default, path is under asset for android and main bundle for iOS
        // 1 current folder + texture name
        // 2 current folder + texture full path (might contain ../ or ./)
        Texture* createTextureByPath(Scene* scene, const string paths[3]);
        string getTexName() const;
        void setExternTexID(int id);
        int getExternTexID() const;
        void setAmbientColor(int color);
        void setAmbientColor(float r, float g, float b, float a = 1.0f);
        void setAmbientColor(Color color);
        Color getAmbientColor() const;
        void setDiffuseColor(int color);
        void setDiffuseColor(float r, float g, float b, float a = 1.0f);
        void setDiffuseColor(Color color);
        Color getDiffuseColor() const;
        void setSpecularColor(int color);
        void setSpecularColor(float r, float g, float b, float a = 1.0f);
        void setSpecularColor(Color color);
        Color getSpecularColor() const;
        void setSpecularLevel(float level);
        float getSpecularLevel() const;
        void setShininess(float shininess);
        float getShininess() const;
        void setTransparent(float r, float g, float b, float a);
        void setTransparent(Color color);
        Color getTransparent() const;
        void setTransparentMode(TransparentMode mode);
        void setTransparency(float transparency);
        float getTransparency() const;
        const float* getDiffuseArray() const;
        const float* getSpecularArray() const;

        void addTexture(Texture* texture);
        Texture* getTexture(int index = 0) const;
        int getTextureCount() const;
        void clearTexture();
        
        inline void setReflectivity(float reflectivity)
        {
            mReflectivity = reflectivity;
        }

        inline float getReflectivity() const
        {
            return mReflectivity;
        }
        
        virtual void setPass(Pass* pass);
        virtual Pass* getPass() const;
        
        inline void setPassConfigurable(bool enable)
        {
            mPassConfigurable = enable;
        }
        
        inline bool isPassConfigurable()
        {
            return mPassConfigurable;
        }
        
        inline void setTextureMode(int mode)
        {
            mTextureMode = mode;
        }
        
        inline int getTextureMode()
        {
            return mTextureMode;
        }
        
        inline RenderState& getRenderState()
        {
            return mRenderState;
        }

        inline void setCubeMapTexID(int id)
        {
            mCubeMapTexID = id;
        }

        inline int getCubeMapTexID() const
        {
            return mCubeMapTexID;
        }
        
        virtual void writeObject(DataOutputStream& dos);
        virtual void readObject(DataInputStream& dis);

    private:
        void init();

    private:
        string mTextureName;
        int mTextureMode;
        int mExternTexID;

        Color mAmbientColor;
        Color mDiffuseColor;
        Color mSpecularColor;

        float mSpecularLevel;
        float mShininess;
        
        Color mTransparent;
        TransparentMode mTranparentMode;
        float mTransparency;
        float mReflectivity;

        vector<Texture*> mTextures;

        RenderState mRenderState;
        
        // pass or technique depending on different terminology
        Pass* mPass;
        // for some, we want to set it only once and won't respond
        // to shader change e.g. user change shader in setting
        bool mPassConfigurable;

        int mCubeMapTexID;
    };
}

#endif
