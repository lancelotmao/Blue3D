#ifndef __Houyi__HouyiSceneManager__
#define __Houyi__HouyiSceneManager__

#include "HouyiScene.h"

namespace Houyi
{
    class World;
    class SceneManager : public Object
    {
    public:
        static SceneManager* getInstance();
        virtual ~SceneManager();

        SceneNode* findSceneNodeByName(string name);
        SceneNode* findSceneNodeByName(string name, SceneNode* node);
        
        // creates a dummy root scene node
        SceneNode* createRootSceneNode();

        void addScene(Scene* scene);
        
        inline int getSceneCount()
        {
            return mScenes.size();
        }
        
        inline Scene* getScene(int index)
        {
            return mScenes[index];
        }
        
        void deleteScene(Scene* scene);
        void deleteSceneDefer(Scene* scene);
        void deleteSceneDefer(int index);
        
        Scene* findSceneByName(const string& name);
        
    private:
        SceneManager();
        void deleteSceneNode(Node* node);
        
        void addDeadObject(Object* obj);
        void deleteDeadObject();

    private:
        static SceneManager* mIns;

        pthread_mutex_t mSceneMutex;
        vector<Scene*> mScenes;
        
        // when exceeded delete the oldest scene
        int mMaxSceneCacheSize;
        
        vector<Object*> mDeadScene;
        pthread_mutex_t mDeadSceneMutex;
    };
}
#endif
