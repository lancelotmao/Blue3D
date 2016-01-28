#ifndef Houyi_HouyiShaderManager_h
#define Houyi_HouyiShaderManager_h

#include <vector>
#include "HouyiPass.h"

namespace Houyi
{
    class Renderer;
    class Scene;
    class Root;

    class ShaderManager : public Object
    {
    public:
//        static ShaderManager* getInstance();
        ShaderManager(Renderer* renderer);
        ~ShaderManager();

        void addPass(Pass* pass);
        Pass* getPass(int index = 0);
        void insertPass(Pass* pass, int index);
        void setPass(Pass* pass);
        void deleteAll();
        int getPassCount();

        Pass* begineRender(Scene* scene);
        bool render(Renderer* renderer, Scene* scene);
        void endRender(Scene* scene);

        void addDefaultShader(Root* root, Scene* scene);
        Pass* getDefaultShader(Renderer* renderer);
        inline Pass* getStaticPPLShader()
        {
            return mStaticPPL;
        }
        void updateShader(Scene* scene);
        void invalidateAll();

        Pass* createShader(int pid, Renderer* renderer);

    private:
        int getStaticDefault();
        int getStaticPPL();
        int getDefault();
        int getPPL();

    private:
        static ShaderManager* mIns;

        vector<Pass*> mPasses;
        vector<Pass*>::iterator mPassIterator;

        Pass* mStaticDefault;
        Pass* mStaticPPL;
        Pass* mDynamicDefault;
        Pass* mDynamicPPL;

        Pass* mDefault;
    };
}

#endif
