//
//  HColladaLoader.h
//  houyi
//
//  Created by Lance Mao on 9/11/11.
//  Copyright (c) 2011 Dong wen. All rights reserved.
//

// v 0.1 The idea is to abstract a layer that is implemented in C++
// and have one sub class to convert the intermediate data to specific engine

#ifndef houyi_HColladaLoader_h
#define houyi_HColladaLoader_h

#include "HouyiAbsColladaLoader.h"
#include "HouyiSceneManager.h"
#include "HouyiJointNode.h"

using namespace Houyi;

namespace collada
{
    // concrete class for Houyi engine that convert data into its own format
    class ColladaLoader: public AbsColladaLoader
    {
    public:
        virtual ~ColladaLoader();
        
        ColladaLoader(const string fp) : AbsColladaLoader(fp)
        {

        }

        Scene* convert(Collada& collada);

    private:
        void convertMeshes(SkeletonPtr& skeleton);
        Houyi::Mesh* findMesh(Mesh* colladaMesh);
        
        Houyi::SceneNode* convertSceneNode(SceneNode* gnPtr);
        
        Houyi::JointNode* copyJointNode(JointNode* jnPtr);
        Houyi::JointNode* copyTree(JointNode* jnPtr);
        Houyi::JointNode* findJointNode(string nodeId);
        int findJointIndexById(string nodeId);
        int findJointNodeIndexByName(string name);
        Houyi::Material* convertMaterial(Material material);
        void setMaterialAttributes(Houyi::Material* hmat, Material material);
        string findTexturePath(string texname);
        
        int getInterpolation(int src);
        int getType(int src);

        // all joints of all scenes
        vector<Houyi::JointNode*> mJoints;
    };
}

#endif
