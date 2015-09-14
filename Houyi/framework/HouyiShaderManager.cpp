#include "HouyiShaderManager.h"
#include "HouyiScene.h"
#include "HouyiRoot.h"
#include "HIShaders.h"
#include "HouyiSettings.h"
#include "HouyiMaterialManager.h"
#include "HouyiRenderer.h"

namespace Houyi
{
    ShaderManager* ShaderManager::mIns = 0;

    ShaderManager* ShaderManager::getInstance()
    {
        if (!mIns)
        {
            mIns = HouyiNew ShaderManager();
        }
        return mIns;
    }

    ShaderManager::ShaderManager() : mStaticDefault(0), mStaticPPL(0),
    mDynamicDefault(0), mDynamicPPL(0), mDefault(0)
    {
        Root* root = Root::getInstance();
        Renderer* renderer = root->getRenderer();
        if (!renderer)
        {
            return;
        }
        mStaticDefault = renderer->loadShader(getStaticDefault());
        mStaticDefault->setName("Static Default");
        mStaticPPL = renderer->loadShader(getStaticPPL());
        mStaticPPL->setName("Static PPL");
        mDefault = mStaticDefault;
    }

    ShaderManager::~ShaderManager()
    {
        deleteAll();
        mIns = 0;
    }

    void ShaderManager::addPass(Pass* pass)
    {
    	for (int i = 0;i < mPasses.size();++i)
    	{
    		if (pass == mPasses[i])
    		{
    			return;
    		}
    	}
        mPasses.push_back(pass);
    }

    Pass* ShaderManager::getPass(int index)
    {
        return mPasses[index];
    }

    void ShaderManager::insertPass(Pass* pass, int index)
    {
        if (index >= mPasses.size())
        {
            mPasses.push_back(pass);
        }
        else
        {
            mPasses.insert(mPasses.begin() + index, pass);
        }
    }

    void ShaderManager::setPass(Pass* pass)
    {
        for (int i = 0;i < mPasses.size();++i)
        {
            HouyiDelete(mPasses[i]);
        }
        mPasses.clear();

        mPasses.push_back(pass);
    }

    void ShaderManager::deleteAll()
    {
        for (int i = 0;i < mPasses.size();++i)
        {
            HouyiDelete(mPasses[i]);
        }
        mPasses.clear();
    }

    int ShaderManager::getPassCount()
    {
        return mPasses.size();
    }

    Pass* ShaderManager::begineRender(Scene* scene)
    {
        // find the first active pass
        mPassIterator = mPasses.begin();
        vector<Pass*>::iterator end = mPasses.end();
        while (mPassIterator != end && (*mPassIterator)->getPassState() == Pass::EIdle)
        {
            mPassIterator++;
        }

        if (mPassIterator == end)
        {
            LogOnce("No active pass\n");
            return 0;
        }
        return *mPassIterator;
    }

    bool ShaderManager::render(Scene* scene)
    {
        mPassIterator = mPasses.begin();
        vector<Pass*>::iterator end = mPasses.end();
        LogOnce("Start render, pass count = %d\n", (int)mPasses.size());
        string passNames;
        while (mPassIterator != end)
        {
            Pass* pass = *mPassIterator;
            if (pass)
            {
                passNames += pass->getName();

                if (pass->getPassState() == Pass::EIdle)
                {
                    mPassIterator++;
                    continue;
                }

                if (pass->getPassState() == Pass::EInvalid)
                {
                    Root* root = Root::getInstance();
                    Renderer* renderer = root->getRenderer();
                    if (renderer->uploadShader(pass))
                    {
                        pass->setPassState(Pass::ENormal);
                    }
                }

                pass->beginPass();
                pass->renderPass(scene);
                pass->endPass();

                if (pass->getPassState() == Pass::ENormal)
                {
                    while (++mPassIterator != end && (*mPassIterator)
                           && (*mPassIterator)->getPassState() == Pass::EIdle);
                    if (mPassIterator != end)
                    {
                        Pass* nextPass = *mPassIterator;
                        nextPass->setLastPassResult(pass->getMyResult());
                        passNames += ", ";
                    }
                }
                else
                {
                    return false;
                }
            }
        }
        LogOnce("Pass Names = %s\n", passNames.c_str());
        return true;
    }

    void ShaderManager::endRender(Scene* scene)
    {

    }

    Pass* ShaderManager::createShader(int pid)
    {
        Root* root = Root::getInstance();
        Renderer* renderer = root->getRenderer();
        if (!renderer)
        {
            return 0;
        }

        Pass* pass = renderer->loadShader(pid);
        addPass(pass);
        return pass;
    }

    void ShaderManager::addDefaultShader(Scene* scene)
    {
        Root* root = Root::getInstance();
        Renderer* renderer = root->getRenderer();
        if (!renderer)
        {
            return;
        }

        Pass* pass = 0;
        int sm = Settings::getInstance()->getShadingMode();

        static int THRESHOLD = -1; // use default as PPL is too slow
        if (scene->getSkeletonCount() == 0 || !scene->getSkeleton(0)->isHardwareEnabled())
        {
            if (!mStaticDefault)
            {
                mStaticDefault = renderer->loadShader(getStaticDefault());
                mStaticDefault->setName("Static Default");
                addPass(mStaticDefault);
            }
            if (!mStaticPPL)
            {
                mStaticPPL = renderer->loadShader(getStaticPPL());
                mStaticPPL->setName("Static PPL");
                addPass(mStaticPPL);
            }
            pass = sm == Settings::ESM_PPL ? mStaticPPL : mStaticDefault;
        }
        else
        {
            if (!mDynamicDefault)
            {
                mDynamicDefault = renderer->loadShader(getDefault());
                mDynamicDefault->setName("Dynamic Default");
                addPass(mDynamicDefault);
            }

            if (!mDynamicPPL)
            {
                mDynamicPPL = renderer->loadShader(getPPL());
                mDynamicPPL->setName("Dynamic PPL");
                addPass(mDynamicPPL);
            }

            pass = scene->getStats().mTriangleCount > THRESHOLD ? mDynamicDefault : mDynamicPPL;
        }

        for (int i = 0, n = scene->getRenderableCount(); i < n;++i)
        {
            const RenderEntity& entity = scene->getRenderable(i);
            Material* mat = entity.mMaterial;
            if (mat && !mat->getPass())
            {
                mat->setPass(pass);
            }
        }

        mDefault = pass;
    }
    
    Pass* ShaderManager::getDefaultShader()
    {
        if (!mDefault)
        {
            Root* root = Root::getInstance();
            Renderer* renderer = root->getRenderer();
            if (!renderer)
            {
                return 0;
            }
            if (!mStaticDefault)
            {
                mStaticDefault = renderer->loadShader(getStaticDefault());
                mStaticDefault->setName("Static Default");
            }
            if (!mStaticPPL)
            {
                mStaticPPL = renderer->loadShader(getStaticPPL());
                mStaticPPL->setName("Static PPL");
            }
            mDefault = mStaticDefault;
        }
        return mDefault;
    }

    void ShaderManager::updateShader(Scene* scene)
    {
        Pass* pass = 0;
        int sm = Settings::getInstance()->getShadingMode();
        if (scene->getSkeletonCount() == 0 || !scene->getSkeleton(0)->isHardwareEnabled())
        {
            if (mStaticDefault && mStaticPPL)
            {
                pass = sm == Settings::ESM_PPL ? mStaticPPL : mStaticDefault;
            }
        }
        else
        {
            if (mDynamicDefault && mDynamicPPL)
            {
                pass = sm == Settings::ESM_PPL ? mDynamicPPL : mDynamicDefault;
            }
        }
        
        for (int i = 0, n = scene->getRenderableCount(); i < n;++i)
        {
            const RenderEntity& entity = scene->getRenderable(i);
            Material* mat = entity.mMaterial;
            if (mat)
            {
                mat->setPass(pass);
            }
        }
        
        mDefault = pass;

        scene->getMaterialManager()->setLastBound(0, 0);
    }

    void ShaderManager::invalidateAll()
    {
        for (int i = 0;i < mPasses.size();++i)
        {
            mPasses[i]->invalidate();
        }
    }

    int ShaderManager::getStaticDefault()
    {
        return HSHADER_STATIC_DEFAULT;
    }

    int ShaderManager::getStaticPPL()
    {
        return HSHADER_STATIC_PPL;
    }

    int ShaderManager::getDefault()
    {
    	return HSHADER_DEFAULT;
    }

    int ShaderManager::getPPL()
    {
    	return HSHADER_PPL;
    }
}
