//
//  Material.cpp
//  houyi
//
//  Created by Lance Mao on 17/08/2011.
//  Copyright 2011 Dong wen. All rights reserved.
//

#include "HouyiMaterial.h"
#include "HouyiImageFactory.h"
#include "HouyiTextureManager.h"
#include "HouyiPass.h"
#include "HouyiScene.h"
#include "HouyiStringUtils.h"

namespace Houyi
{
    Material::Material()
    {
        init();
    }

    Material::Material(string name)
    {
        mName = name;
        init();
    }
    
    void Material::init()
    {
        mTranparentMode = EAZero;
        mTextureMode = RenderState::EUnspecified;
        mExternTexID = 0;
        mTransparency = 1;
        mReflectivity = 0;
        mShininess = 0;
        mSpecularLevel = 1.0;
        mSpecularColor.mA = 0;
        mTransparent.mR = 0;
        mTransparent.mG = 0;
        mTransparent.mB = 0;
        mTransparent.mA = 1;
        mTextureName = "";
        mPass = 0;
        mPassConfigurable = true;
        mCubeMapTexID = 0;
    }

    Texture* Material::createTextureByPath(Scene* scene, const string paths[3])
    {
        TextureManager* tm = scene->getTextureManager();
        Texture* tex = 0;
        for (int i = 0; i < 3;++i)
        {
            const string& path = paths[i];
            tex = tm->createTexture(scene, path);
            if (tex)
            {
                mTextureName = path;
                break;
            }
        }

        if (tex)
        {
            mTextures.push_back(tex);
        }
        else
        {
            if (mDiffuseColor.mR == 0 && mDiffuseColor.mG == 0 && mDiffuseColor.mB == 0)
            {
                mDiffuseColor.mR = 0.5;
                mDiffuseColor.mG = 0.5;
                mDiffuseColor.mB = 0.5;
            }
        }
        return tex;
    }

    string Material::getTexName() const
    {
        return mTextureName;
    }

    void Material::setExternTexID(int id)
    {
        mExternTexID = id;
    }

    int Material::getExternTexID() const
    {
        return mExternTexID;
    }

    void Material::setAmbientColor(int color)
    {
        mAmbientColor.mR = ((color >> 16) & 0xff) / (float) 255.0;
        mAmbientColor.mG = ((color >> 8) & 0xff) / (float) 255.0;
        mAmbientColor.mB = ((color) & 0xff) / (float) 255.0;
        mAmbientColor.mA = 1.0f;
    }
    
    void Material::setAmbientColor(float r, float g, float b, float a)
    {
        mAmbientColor.mR = r;
        mAmbientColor.mG = g;
        mAmbientColor.mB = b;
        mAmbientColor.mA = a;
    }
    
    void Material::setAmbientColor(Color color)
    {
        mAmbientColor = color;
    }

    Color Material::getAmbientColor() const
    {
        Color res = mAmbientColor;
        res.mA = 0;
        return res;
    }

    void Material::setDiffuseColor(int color)
    {
        mDiffuseColor.mA = ((color >> 24) & 0xff) / (float) 255.0;
        mDiffuseColor.mR = ((color >> 16) & 0xff) / (float) 255.0;
        mDiffuseColor.mG = ((color >> 8) & 0xff) / (float) 255.0;
        mDiffuseColor.mB = ((color) & 0xff) / (float) 255.0;
    }

    void Material::setDiffuseColor(float r, float g, float b, float a)
    {
        mDiffuseColor.mR = r;
        mDiffuseColor.mG = g;
        mDiffuseColor.mB = b;
        mDiffuseColor.mA = a;
    }

    void Material::setDiffuseColor(Color color)
    {
        mDiffuseColor = color;
    }

    Color Material::getDiffuseColor() const
    {
        Color res = mDiffuseColor;
        if (mTranparentMode == EAOne || mTranparentMode == EView)
        {
            res.mA = mDiffuseColor.mA * mTransparent.mA * mTransparency;
        }
        else if (mTranparentMode == ERGBZero)
        {
            float luminance = mTransparent.luminance();
            res.mA = mDiffuseColor.mA * (1 - luminance) * mTransparency;
        }
        else if (mTranparentMode == EAZero)
        {
            if (mTransparency != 0)
            {
                res.mA = 1 - mTransparency;
            }
        }

        if (res.mA == 0 && mTranparentMode != EView)
        {
            // no point to have full transparent for our app
            res.mA = 1;
        }
        return res;
    }

    void Material::setSpecularColor(int color)
    {
        mSpecularColor = Color(color);
    }

    void Material::setSpecularColor(float r, float g, float b, float a)
    {
        mSpecularColor.mR = r;
        mSpecularColor.mG = g;
        mSpecularColor.mB = b;
        mSpecularColor.mA = a;
    }

    void Material::setSpecularColor(Color color)
    {
        mSpecularColor = color;
    }

    Color Material::getSpecularColor() const
    {
        Color res = mSpecularColor;
        if (mTranparentMode == EAOne)
        {
            res.mA = mSpecularColor.mA * mTransparent.mA * mTransparency;
        }
        else if (mTranparentMode == ERGBZero)
        {
            float luminance = mTransparent.luminance();
            res.mA = mSpecularColor.mA * (1 - luminance) * mTransparency;
        }
        else
        {
            if (mTransparency != 0)
            {
                res.mA = mTransparency;
            }
        }
        return res;
    }

    void Material::setSpecularLevel(float level)
    {
        mSpecularLevel = level;
        mSpecularColor.mR = mSpecularColor.mR * mSpecularLevel;
        mSpecularColor.mG = mSpecularColor.mG * mSpecularLevel;
        mSpecularColor.mB = mSpecularColor.mB * mSpecularLevel;
    }

    float Material::getSpecularLevel() const
    {
        return mSpecularLevel;
    }

    void Material::setShininess(float shininess)
    {
        mShininess = shininess;
    }

    float Material::getShininess() const
    {
        return mShininess;
    }
    
    void Material::setTransparent(float r, float g, float b, float a)
    {
        mTransparent.mR = r;
        mTransparent.mG = g;
        mTransparent.mB = b;
        mTransparent.mA = a;
    }
    
    void Material::setTransparent(Color color)
    {
        mTransparent = color;
    }
    
    Color Material::getTransparent() const
    {
        return mTransparent;
    }

    void Material::setTransparentMode(TransparentMode mode)
    {
        mTranparentMode = mode;
    }

    void Material::setTransparency(float transparency)
    {
        mTransparency = transparency;
    }

    float Material::getTransparency() const
    {
        return mTransparency;
    }

    void Material::addTexture(Texture* texture)
    {
        mTextures.push_back(texture);
    }

    Texture* Material::getTexture(int index) const
    {
        if (index >= 0 && index < mTextures.size())
            return mTextures[index];
        return 0;
    }

    int Material::getTextureCount() const
    {
        return mTextures.size();
    }

    void Material::clearTexture()
    {
        mTextures.clear();
    }
    
    void Material::setPass(Pass* pass)
    {
        if (mPassConfigurable || !mPass)
        {
            mPass = pass;
        }
    }
    
    Pass* Material::getPass() const
    {
        return mPass;
    }
    
    void Material::writeObject(DataOutputStream& dos)
    {
        dos.writeUTFString(getName());
        dos.write(&mTextureMode, sizeof(mTextureMode));
        dos.write(&mAmbientColor, sizeof(mDiffuseColor));
        dos.write(&mDiffuseColor, sizeof(mDiffuseColor));
        dos.write(&mSpecularColor, sizeof(mDiffuseColor));
        dos.write(&mShininess, sizeof(mShininess));
        dos.write(&mSpecularLevel, sizeof(mSpecularLevel));
        dos.write(&mTransparent, sizeof(mTransparent));
        dos.write(&mTranparentMode, sizeof(mTranparentMode));
        dos.write(&mTransparency, sizeof(mTransparency));
        dos.write(&mReflectivity, sizeof(mReflectivity));
        dos.write(&mRenderState, sizeof(mRenderState));
        dos.writeBool(mPassConfigurable);
    }
    
    void Material::readObject(DataInputStream& dis)
    {
        setName(dis.readUTFString());
        dis.read(&mTextureMode, sizeof(mTextureMode));
        dis.read(&mAmbientColor, sizeof(mAmbientColor));
        dis.read(&mDiffuseColor, sizeof(mDiffuseColor));
        dis.read(&mSpecularColor, sizeof(mSpecularColor));
        dis.read(&mShininess, sizeof(mShininess));
        dis.read(&mSpecularLevel, sizeof(mSpecularLevel));
        dis.read(&mTransparent, sizeof(mTransparent));
        dis.read(&mTranparentMode, sizeof(mTranparentMode));
        dis.read(&mTransparency, sizeof(mTransparency));
        dis.read(&mReflectivity, sizeof(mReflectivity));
        dis.read(&mRenderState, sizeof(mRenderState));
        mPassConfigurable = dis.readBool();
    }
}
