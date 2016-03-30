#include "HouyiSceneManager.h"
#include "HouyiWorld.h"
#include "HouyiRoot.h"

namespace Houyi
{
    SceneManager* SceneManager::mIns = 0;

    SceneManager* SceneManager::getInstance()
    {
        if (!mIns)
        {
            mIns = HouyiNew SceneManager();
        }
        return mIns;
    }

    SceneManager::SceneManager() : mMaxSceneCacheSize(1)
    {
        pthread_mutex_init(&mSceneMutex, 0);
    }

    SceneManager::~SceneManager()
    {
        for (int i = 0;i < mScenes.size();++i)
        {
            Scene* scene = mScenes[i];
            HouyiDelete (scene);
        }
        mScenes.clear();

        pthread_mutex_destroy(&mSceneMutex);
        mIns = 0;
    }

    void SceneManager::deleteSceneNode(Node* node)
	{
		if (node)
		{
			for (int i = 0, n = node->childrenCount(); i < n; ++i)
			{
				deleteSceneNode(node->getChildAt(i));
			}
			HouyiDelete(node);
		}
		else
		{
			LOGW("SceneManager::deleteSceneNode null node\n");
		}
	}

    SceneNode* SceneManager::createRootSceneNode()
    {
    	SceneNode* node = HouyiNew SceneNode();
        node->setName("Root Scene Node");
    	return node;
    }
    
    void SceneManager::deleteScene(Scene* scene)
    {
        for (int i = 0;i < mScenes.size();++i)
        {
            if (mScenes[i] == scene)
            {
                mScenes.erase(mScenes.begin() + i);
                HouyiDelete(scene);
                break;
            }
        }
    }
    
    void SceneManager::deleteSceneDefer(Scene* scene)
    {
        // to avoid inner mutex
        Scene* dead = 0;
        
        pthread_mutex_lock (&mSceneMutex);
        for (int i = 0;i < mScenes.size();++i)
        {
            if (mScenes[i] == scene)
            {
                dead = scene;
                mScenes.erase(mScenes.begin() + i);
                break;
            }
        }
        pthread_mutex_unlock (&mSceneMutex);
        
        if (dead)
        {
            dead->getWorld()->getRoot()->addDeadObject(dead);
        }
    }
    
    void SceneManager::deleteSceneDefer(int index)
    {
        // to avoid inner mutex
        Scene* dead = 0;
        
        pthread_mutex_lock (&mSceneMutex);
        if (index >= 0 && index < mScenes.size())
        {
            dead = mScenes[index];
            mScenes.erase(mScenes.begin() + index);
        }
        pthread_mutex_unlock (&mSceneMutex);
        
        if (dead)
        {
            dead->getWorld()->getRoot()->addDeadObject(dead);
            LOGD("Scene added for deletion: %s\n", dead->getName().c_str());
        }
    }
    
    Scene* SceneManager::findSceneByName(const string& name)
    {
        string cacheName = name + ".houyi";
        Scene* res = 0;
        pthread_mutex_lock (&mSceneMutex);
        for (int i = 0;i < mScenes.size();++i)
        {
            Scene* scene = mScenes[i];
            if (scene->getName().compare(name) == 0 || scene->getName().compare(cacheName) == 0)
            {
                res = scene;
                break;
            }
        }
        pthread_mutex_unlock (&mSceneMutex);
        return res;
    }
    
    void SceneManager::addScene(Scene* scene)
    {
        pthread_mutex_lock (&mSceneMutex);
        mScenes.push_back(scene);
        pthread_mutex_unlock (&mSceneMutex);
    }
}
