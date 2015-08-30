//
//  ColladaLoader.cpp
//  houyi
//
//  Created by Lance Mao on 9/11/11.
//  Copyright (c) 2011 Dong wen. All rights reserved.
//

#include <iostream>
#include <assert.h>
#include "HouyiColladaLoader.h"
#include "HouyiUtil.h"
#include "HouyiMaterialManager.h"
#include "HouyiStringUtils.h"

namespace collada
{
    ColladaLoader::~ColladaLoader()
    {
        mJoints.clear();
    }
    
    Scene* ColladaLoader::convert(Collada& collada)
    {
        // prepare joints
        SkeletonPtr skeleton = new Skeleton();
        skeleton->setOwnJoints(true);
        for (int v = 0; v < mCollada.mVisualScenes.size(); ++v)
        {
            VisualScene vs = mCollada.mVisualScenes.at(v);
            
            for (int j = 0; j < vs.mJointRootPtrs.size(); ++j)
            {
                skeleton->mJNRoots.push_back(mJoints.size());
                copyTree(vs.mJointRootPtrs.at(j));
            }
        }
        
        skeleton->setJointsCount(mJoints.size());
        Houyi::JointNode** joints = new Houyi::JointNode*[skeleton->getJointsCount()];
        skeleton->setJointNodes(joints, skeleton->getJointsCount());
        for (int i = 0; i < mJoints.size(); ++i)
        {
            Houyi::JointNode* p = mJoints.at(i);
            skeleton->getJointNodes()[i] = p;
            if (p->mKFCount > skeleton->mMaxKFCount)
            {
                skeleton->mMaxKFCount = p->mKFCount;
            }
        }
        
        if (mJoints.size() > 0)
        {
            if (mCollada.mAsset.mContributor.mAuthoringTool.mTool.find("Maya") == string::npos)
            {
                skeleton->setUseLinearRatioForBezier(false);
            }
            else
            {
                skeleton->setUseLinearRatioForBezier(true);
            }
            mScene->addSkeleton(skeleton);
        }
        
        // because joints goes with mesh
        // we need to prepare joints before convert mesh
        convertMeshes(skeleton);
        
        notifiyProgress(0.3);

        for (int v = 0; v < mCollada.mVisualScenes.size(); ++v)
        {
            VisualScene vs = mCollada.mVisualScenes.at(v);

            // convert lights. Note some default lights might be added so clear
            int lightCount = vs.mLights.size();
            if (lightCount > 0)
            {
                mScene->clearLights();
            }
            for (int g = 0; g < lightCount; ++g)
            {
                SceneNode* lightNode = vs.mLights.at(g);

                collada::Color color = lightNode->mTechniqueCommon.mPoint.mColor;
                Houyi::Light* light = HouyiNew Houyi::Light();
                light->setColor(color.mR, color.mG, color.mB);
                light->setIsDirectional(lightNode->mTechniqueCommon.mIsDirectional);

                light->setLocal(Matrix4(lightNode->getMatrix().getData()));

                light->setIsAmbient(lightNode->mTechniqueCommon.mHasAmbient);

                mScene->addLight(light);
            }

            // convert cameras
            mScene->deleteCameras();
            for (int i = 0; i < vs.mCameras.size(); ++i)
            {
                SceneNode* camera = vs.mCameras.at(i);
                SceneNode* target = 0;
                for (int j = 0; j < vs.mCameraTargets.size(); ++j)
                {
                    SceneNode* t = vs.mCameraTargets.at(j);
                    if (t->getName().find(camera->getName()) != string::npos)
                    {
                        target = t;
                        break;
                    }
                }

                Perspective persp = camera->mTechniqueCommon.mPerspective;

                Houyi::Camera* hcam = HouyiNew Houyi::Camera();
                hcam->setLookAt(0, 0, 0);
                hcam->setFOV(persp.mXFOV);
                hcam->setNearRange(persp.mNear);
                hcam->setFarRange(persp.mFar);

                if (target)
                {
                    Matrix targetMatrix = target->getMatrix();
                    hcam->setLookAt(targetMatrix[12], targetMatrix[13], targetMatrix[14]);
                }

                Matrix4 camTrans = Matrix4(camera->getMatrix().getData());
                Matrix3 r;
                camTrans.extract3x3Matrix(r);
                hcam->setPosition(camTrans[0][3], camTrans[1][3], camTrans[2][3]);
                Vector3 up = r * Vector3(0, 1, 0);
                hcam->setUp(up.x, up.y, up.z);

                mScene->addCamera(hcam);
            }
        }

        // convert scene nodes
        for (int v = 0; v < mCollada.mVisualScenes.size(); ++v)
        {
            VisualScene vs = mCollada.mVisualScenes.at(v);
            for (int g = 0; g < vs.mSceneNodePtrs.size(); g++)
            {
                SceneNode* gnPtr = vs.mSceneNodePtrs.at(g);
                Houyi::SceneNode* node = convertSceneNode(gnPtr);
                mScene->addSceneNode(node);
            }
        }
        
        // scene node might not be linked with geometry
        // in this case add node to wrap each mesh
        if (mScene->getRenderableCount() == 0)
        {
            for (int i = 0; i < mScene->getMeshCount(); ++i)
            {
                Houyi::SceneNode* node = HouyiNew Houyi::SceneNode();
                node->setMesh(mScene->getMesh(i));
                mScene->addSceneNode(node);
                mScene->addRenderable(node);
            }
        }

        notifiyProgress(0.4);

        // prepare world
        if (mCollada.mAsset.mUpAxis.find("Z") != string::npos
                || mCollada.mAsset.mUpAxis.find("z") != string::npos)
        {
            mScene->setZAsUpAxis();
        }
        
        if (mLogLevel >= EStats)
        {
            LOGD("\n-----collada stats-----\n");
            printStats();
            LOGD("there are [%d] renderable meshes\n", mScene->getRenderableCount());
            LOGD("-------collada stats-----\n\n");
        }

        if (mLogLevel == EFull)
        {
            LOGD("--------dumping hierarchy------\n");
            mScene->dumpHierarchy();
            LOGD("--------dumping hierarchy------\n");
            LOGD("--------dumping joints------\n");
            skeleton->dumpJoints();
            LOGD("--------dumping joints------\n");
        }

        return mScene;
    }

    Houyi::SceneNode* ColladaLoader::convertSceneNode(SceneNode* gnPtr)
    {
        Houyi::SceneNode* hnPtr = HouyiNew Houyi::SceneNode();
        hnPtr->setName(gnPtr->getName());

        if (mLogLevel == EFull)
        {
            LOGI("converting SceneNode, name = %s\n", hnPtr->getName().c_str());
        }
        
        // transform data
        const Matrix4& local = Matrix4(gnPtr->getMatrix().getData());
        hnPtr->setLocal(local, gnPtr->mHasNonUniform);
        mScene->setHasNegativeScale(mScene->hasNegativeScale() | (local.determinant() < 0));

        for (int g = 0; g < gnPtr->mGeoInstance.size();++g)
        {
            InstanceGeometry instanceGeo = gnPtr->mGeoInstance[g];
            Mesh* colladaMesh = (Mesh*)instanceGeo.mGeoData;
            if (mLogLevel == EFull)
            {
                LOGI("Has geometry data id = %s\n", colladaMesh->mID.c_str());
            }

            // add material
            for (int p = 0; p < colladaMesh->mPrimitives.size(); ++p)
            {
                Primitive* primitive = colladaMesh->mPrimitives[p];
                const string& symbol = primitive->mMaterialStr;
//                LOGW("%s", symbol.c_str());
                string bindTargetID;
                map<string, string>::iterator it = instanceGeo.mMaterialMap.find(symbol);
                if (it != instanceGeo.mMaterialMap.end())
                {
                    bindTargetID = it->second;
//                    LOGE("%s, %s", symbol.c_str(), bindTargetID.c_str());
                    if (bindTargetID.length() > 0)
                    {
                        for (int i = 0; i < mCollada.mMaterials.size(); ++i)
                        {
                            Material mat = mCollada.mMaterials.at(i);
                            if (bindTargetID.substr(1).compare(mat.mID) == 0)
                            {
                                Houyi::Material* hmat = convertMaterial(mat);
                                hnPtr->addMaterial(hmat);
                                break;
                            }
                        }
                    }
                }
            }

            Houyi::SceneNode* n = HouyiNew Houyi::SceneNode();
            Houyi::Mesh* mesh = findMesh(colladaMesh);
            if (!mesh)
            {
                LOGW("No mesh for scene node: %s, mesh id = %s\n",
                     gnPtr->getName().c_str(), colladaMesh->mID.c_str());
            }
            else
            {
                hnPtr->setMesh(mesh);
                mScene->addRenderable(hnPtr);
                n->setMesh(mesh);
                mScene->addRenderable(n);
                hnPtr->addChild(n);
            }

            if (mLogLevel == EFull)
            {
                LOGI("Done converting SceneNode[%s]\n", hnPtr->getName().c_str());
            }
        }
        
        // convert children as well
        for (int i = 0; i < gnPtr->mChildren.size(); ++i)
        {
            SceneNode* gnchild = gnPtr->mChildren.at(i);
            Node* child = convertSceneNode(gnchild);
            hnPtr->addChild(child);
            child->setParent(hnPtr);
        }
        
        return hnPtr;
    }

    void ColladaLoader::convertMeshes(SkeletonPtr& skeleton)
    {
        for (int i = 0;i < mCollada.mGeometryPtrs.size();++i)
        {
            Mesh* colladaMesh = (Mesh*)(mCollada.mGeometryPtrs[i]);

            Houyi::Mesh* mesh = HouyiNew Houyi::Mesh();
            mesh->setName(colladaMesh->mID); // use name as id

            float* rawVertices = NULL;
            float* rawNormals = NULL;
            float* rawTexcoords = NULL;
            for (int j = 0; j < colladaMesh->mVertices.mInputs.size(); j++)
            {
                Input input = colladaMesh->mVertices.mInputs.at(j);
                string semantic = input.mSemantic;
                if (semantic.compare("POSITION") == 0)
                {
                    string sPos = input.mSource.substr(1);
                    rawVertices = colladaMesh->findFloatArray(sPos)->mFloats;
                }
                else if (semantic.compare("NORMAL") == 0)
                {
                    string sNorm = input.mSource.substr(1);
                    rawNormals = colladaMesh->findFloatArray(sNorm)->mFloats;
                }
            }

            SkinPtr skin;
            for (int j = 0; j < mCollada.mControllers.size(); ++j)
            {
                Controller ctl = mCollada.mControllers.at(j);
                for (int k = 0; k < ctl.mSkinPtrs.size(); ++k)
                {
                    SkinPtr s = ctl.mSkinPtrs.at(k);
                    if (s->mGeometry== colladaMesh)
                    {
                        skin = s;
                        break;
                    }
                }
            }

            if (skin)
            {
                Matrix4 transposed = Matrix4(skin->mBindShapeMatrix.getData());
                mesh->setBindShapeMatrix(transposed);

                SkeletonRef skeletonRef(skeleton);
                ArrayPtr<int> indices = new Array<int>(skin->mJNCount);
                skeletonRef.mIndices = indices;
                for (int k = 0; k < skin->mJNCount; ++k)
                {
                    if (skin->mJNPtrTable && skin->mJNPtrTable[k])
                    {
                        int jointIndex = findJointIndexById(skin->mJNPtrTable[k]->mID);
                        indices->getData()[k] = jointIndex;
                        if (skeleton->getJointNodes()[k]->mKFCount > skeleton->mMaxKFCount)
                        {
                            skeleton->mMaxKFCount = skeleton->getJointNodes()[k]->mKFCount;
                        }
                    }
                    // TODO rare case check
//                    if (skeleton->mJoints[k] && skeleton->mJoints[k]->mKFCount > hnPtr->getMaxKeyFrameCount())
//                    {
//                        hnPtr->setMaxKeyFrameCount(skeleton->mJoints[k]->mKFCount);
//                    }
                }
                mesh->setSkeletonRef(skeletonRef);
            }

            // lines
            for (int j = 0; j < colladaMesh->mLines.size(); ++j)
            {
                VertexBufferPtr vb = new VertexBuffer(VertexBuffer::EExpandAsLines, 0);
                Lines* trianglesPtr = colladaMesh->mLines.at(j);
                int inputCnt = trianglesPtr->mInputs.size();
                int offsetVertex = -1;
                int offsetNormal = -1;
                int offsetTexcoord = -1;
                int attribCnt = 1; // at minimum, there should be position attrib
                int texcoordStride = 2;
                for (int k = 0; k < inputCnt; ++k)
                {
                    Input input = trianglesPtr->mInputs.at(k);
                    string semantic = input.mSemantic;
                    int offset = atoi(input.mOffset.c_str());
                    if (semantic.compare("VERTEX") == 0)
                    {
                        offsetVertex = offset;
                        // raw vertices already find in vertex section
                    }
                    else if (semantic.compare("NORMAL") == 0)
                    {
                        offsetNormal = offset;
                        if (!rawNormals) // normals could be already found in vertices tag
                        {
                            rawNormals = colladaMesh->findFloatArray(input.mSource)->mFloats;
                        }
                    }
                    else if (semantic.compare("TEXCOORD") == 0)
                    {
                        offsetTexcoord = offset;
                        SourcePtr src = colladaMesh->findSource(input.mSource);
                        rawTexcoords = src->mFloatArrayPtr->mFloats;
                        texcoordStride = src->mTechniqueCommon.mAccessor.mStride;
                    }

                    if (offset+1 > attribCnt)
                    {
                        attribCnt = offset+1;
                    }
                }

                int positionCount = trianglesPtr->mCount * 2 * 3;
                int normalCount = trianglesPtr->mCount * 2 * 3;
                int texcoordCount = trianglesPtr->mCount * 2 * 2;
                int jointNodeIndexCount = trianglesPtr->mCount * 2 * 4;
                float* position = new float[positionCount];

                float* normal = NULL;
                if (offsetNormal >= 0)
                {
                    normal = new float[normalCount];
                }
                float* texcoord = NULL;
                if (offsetTexcoord >= 0)
                {
                    texcoord = new float[texcoordCount];
                }

                float* jointNodeIndex = NULL;
                float* jointWeights = NULL;
                if (skin && skin->mJNCount > 0)
                {
                    jointNodeIndex = new float[jointNodeIndexCount];
                    jointWeights = new float[jointNodeIndexCount];
                }
                unsigned int* indices = trianglesPtr->mPPtr->mIndices;
                for (int k = 0; k < trianglesPtr->mCount * 2; ++k)
                {
                    int posIdx = indices[k * attribCnt + offsetVertex];

                    position[k * 3] = rawVertices[posIdx * 3];
                    position[k * 3 + 1] = rawVertices[posIdx * 3 + 1];
                    position[k * 3 + 2] = rawVertices[posIdx * 3 + 2];

                    if (normal)
                    {
                        int normIdx = indices[k * attribCnt + offsetNormal];
                        normal[k * 3] = rawNormals[normIdx * 3];
                        normal[k * 3 + 1] = rawNormals[normIdx * 3 + 1];
                        normal[k * 3 + 2] = rawNormals[normIdx * 3 + 2];
                    }

                    if (texcoord)
                    {
                        int texIdx = indices[k * attribCnt + offsetTexcoord];
                        texcoord[k * 2] = rawTexcoords[texIdx * texcoordStride];
                        texcoord[k * 2 + 1] = rawTexcoords[texIdx * texcoordStride + 1];
                    }

                    if (jointNodeIndex)
                    {
                        for (int m = 0; m < 4; ++m)
                        {
                            // previously we use the following code because skin is independent
                            /* JointNodePtr p = skin->mJNPtrTable[skin->mJNIndex[posIdx * 3 + m]];
                            * jointNodeIndex[k * 3 + m] = findJointNodeIndexByName(p->mName);
                            * */
                            jointNodeIndex[k * 4 + m] = skin->mJNIndex[posIdx * 4 + m];
                        }
                    }

                    if (jointWeights)
                    {
                        jointWeights[k * 4] = skin->mJNWeight[posIdx * 4];
                        jointWeights[k * 4 + 1] = skin->mJNWeight[posIdx * 4 + 1];
                        jointWeights[k * 4 + 2] = skin->mJNWeight[posIdx * 4 + 2];
                        jointWeights[k * 4 + 3] = skin->mJNWeight[posIdx * 4 + 3];
                    }
                }

                vb->setPosition(position, positionCount);

                if (normal)
                {
                    vb->setNormal(normal, normalCount);
                }

                if (texcoord)
                {
                    vb->setTexcoord(texcoord, texcoordCount);
                }

                if (jointNodeIndex)
                {
                    vb->setJointIndex(jointNodeIndex, jointNodeIndexCount);
                }

                if (jointWeights)
                {
                    vb->setJointWeight(jointWeights, jointNodeIndexCount);
                }

                Houyi::SurfacePtr surface = new Houyi::Surface(vb);
                surface->setRenderMode(Houyi::Const::ERenderModeLine);
                mesh->addSurface(surface);

                if (trianglesPtr->mMaterialStr.length() > 0)
                {
                    Houyi::Material* material = convertMaterial(trianglesPtr->mMaterial);
                    if (material)
                    {
                        surface->setMaterial(material);
                    }
                    else
                    {
                        LOGW("cannot find material for triangle name = %s\n", trianglesPtr->mMaterialStr.c_str());
                    }
                }
                else
                {
                    LOGW("triangle has no material, will try Mesh material\n");
                }
            }

            // triangles
            for (int j = 0; j < colladaMesh->mTriangles.size(); ++j)
            {
                VertexBufferPtr vb = new VertexBuffer(VertexBuffer::EExpandAsTriangles, 0);
                Triangles* trianglesPtr = colladaMesh->mTriangles.at(j);
                int inputCnt = trianglesPtr->mInputs.size();
                int offsetVertex = -1;
                int offsetNormal = -1;
                int offsetTexcoord = -1;
                int attribCnt = 1; // at minimum, there should be position attrib
                int texcoordStride = 2;
                for (int k = 0; k < inputCnt; ++k)
                {
                    Input input = trianglesPtr->mInputs.at(k);
                    string semantic = input.mSemantic;
                    int offset = atoi(input.mOffset.c_str());
                    if (semantic.compare("VERTEX") == 0)
                    {
                        offsetVertex = offset;
                        // raw vertices already find in vertex section
                    }
                    else if (semantic.compare("NORMAL") == 0)
                    {
                        offsetNormal = offset;
                        if (!rawNormals) // normals could be already found in vertices tag
                        {
                            rawNormals = colladaMesh->findFloatArray(input.mSource)->mFloats;
                        }
                    }
                    else if (semantic.compare("TEXCOORD") == 0)
                    {
                        offsetTexcoord = offset;
                        SourcePtr src = colladaMesh->findSource(input.mSource);
                        rawTexcoords = src->mFloatArrayPtr->mFloats;
                        texcoordStride = src->mTechniqueCommon.mAccessor.mStride;
                    }

                    if (offset+1 > attribCnt)
                    {
                        attribCnt = offset+1;
                    }
                }

                int positionCount = trianglesPtr->mCount * 3 * 3;
                int normalCount = trianglesPtr->mCount * 3 * 3;
                int texcoordCount = trianglesPtr->mCount * 3 * 2;
                int jointNodeIndexCount = trianglesPtr->mCount * 3 * 4;
                int wireFrameCnt = trianglesPtr->mCount * 6;
                float* position = new float[positionCount];
                uint* wfIndices = new uint[wireFrameCnt];

                float* normal = NULL;
                if (offsetNormal >= 0 || rawNormals)
                {
                    normal = new float[normalCount];
                    if (offsetNormal < 0) offsetNormal = 0;
                }
                float* texcoord = NULL;
                if (offsetTexcoord >= 0)
                {
                    texcoord = new float[texcoordCount];
                }

                float* jointNodeIndex = NULL;
                float* jointWeights = NULL;
                if (skin && skin->mJNCount > 0)
                {
                    jointNodeIndex = new float[jointNodeIndexCount];
                    jointWeights = new float[jointNodeIndexCount];
                }
                unsigned int* indices = trianglesPtr->mPPtr->mIndices;
                for (int k = 0; k < trianglesPtr->mCount * 3; ++k)
                {
                    int posIdx = indices[k * attribCnt + offsetVertex];

                    position[k * 3] = rawVertices[posIdx * 3];
                    position[k * 3 + 1] = rawVertices[posIdx * 3 + 1];
                    position[k * 3 + 2] = rawVertices[posIdx * 3 + 2];

                    if (normal)
                    {
                        int normIdx = indices[k * attribCnt + offsetNormal];
                        normal[k * 3] = rawNormals[normIdx * 3];
                        normal[k * 3 + 1] = rawNormals[normIdx * 3 + 1];
                        normal[k * 3 + 2] = rawNormals[normIdx * 3 + 2];
                    }

                    if (texcoord)
                    {
                        int texIdx = indices[k * attribCnt + offsetTexcoord];
                        texcoord[k * 2] = rawTexcoords[texIdx * texcoordStride];
                        texcoord[k * 2 + 1] = rawTexcoords[texIdx * texcoordStride + 1];
                    }

                    if (jointNodeIndex)
                    {
                        for (int m = 0; m < 4; ++m)
                        {
                            // previously we use the following code because skin is independent
                            /* JointNodePtr p = skin->mJNPtrTable[skin->mJNIndex[posIdx * 3 + m]];
                            * jointNodeIndex[k * 3 + m] = findJointNodeIndexByName(p->mName);
                            * */
                            jointNodeIndex[k * 4 + m] = skin->mJNIndex[posIdx * 4 + m];
                        }
                    }

                    if (jointWeights)
                    {
                        jointWeights[k * 4] = skin->mJNWeight[posIdx * 4];
                        jointWeights[k * 4 + 1] = skin->mJNWeight[posIdx * 4 + 1];
                        jointWeights[k * 4 + 2] = skin->mJNWeight[posIdx * 4 + 2];
                        jointWeights[k * 4 + 3] = skin->mJNWeight[posIdx * 4 + 3];
                    }
                }

                int wfIdx = 0;
                for (int w = 0;w < trianglesPtr->mCount;w++)
                {
                    wfIndices[w * 6] = wfIdx;
                    wfIndices[w * 6 + 1] = wfIdx + 1;
                    wfIndices[w * 6 + 2] = wfIdx + 1;
                    wfIndices[w * 6 + 3] = wfIdx + 2;
                    wfIndices[w * 6 + 4] = wfIdx + 2;
                    wfIndices[w * 6 + 5] = wfIdx;
                    wfIdx += 3;
                }

                vb->setPosition(position, positionCount);

                if (normal)
                {
                    vb->setNormal(normal, normalCount);
                }

                if (texcoord)
                {
                    vb->setTexcoord(texcoord, texcoordCount);
                }

                if (jointNodeIndex)
                {
                    vb->setJointIndex(jointNodeIndex, jointNodeIndexCount);
                }

                if (jointWeights)
                {
                    vb->setJointWeight(jointWeights, jointNodeIndexCount);
                }

                IndexBufferPtr wfIB = new IndexBuffer();
                wfIB->setLineIndices(wfIndices, wireFrameCnt);
                Houyi::SurfacePtr surface = new Houyi::Surface(vb);
                surface->setWireFrameIndexBuffer(wfIB);
                mesh->addSurface(surface);

                if (trianglesPtr->mMaterialStr.length() > 0)
                {
                    Houyi::Material* material = convertMaterial(trianglesPtr->mMaterial);
                    if (material)
                    {
                        surface->setMaterial(material);
                    }
                    else
                    {
                        LOGW("cannot find material for triangle name = %s\n", trianglesPtr->mMaterialStr.c_str());
                    }
                }
                else
                {
                    LOGW("triangle has no material, will try Mesh material\n");
                }
            }

            // polygons
            for (int i = 0, n = colladaMesh->mPolygons.size(); i < n; ++i)
            {
                Polygons* polyPtr = colladaMesh->mPolygons.at(i);
                VertexBufferPtr vb = new VertexBuffer(VertexBuffer::EExpandAsTriangles, 0);
                int inputCnt = polyPtr->mInputs.size();
                int offsetVertex = -1;
                int offsetNormal = -1;
                int offsetTexcoord = -1;
                int attribCnt = 1; // at minimum, there should be position attrib
                int texcoordStride = 2;
                for (int j = 0; j < inputCnt; ++j)
                {
                    Input input = polyPtr->mInputs.at(j);
                    string semantic = input.mSemantic;
                    int offset = atoi(input.mOffset.c_str());
                    if (semantic.compare("VERTEX") == 0)
                    {
                        offsetVertex = offset;
                        // raw vertices already find in vertex section
                    }
                    else if (semantic.compare("NORMAL") == 0)
                    {
                        offsetNormal = offset;
                        if (!rawNormals) // normals could be already found in vertices tag
                        {
                            rawNormals = colladaMesh->findFloatArray(input.mSource)->mFloats;
                        }
                    }
                    else if (semantic.compare("TEXCOORD") == 0)
                    {
                        offsetTexcoord = offset;
                        SourcePtr src = colladaMesh->findSource(input.mSource);
                        rawTexcoords = src->mFloatArrayPtr->mFloats;
                        texcoordStride = src->mTechniqueCommon.mAccessor.mStride;
                    }

                    if (offset+1 > attribCnt)
                    {
                        attribCnt = offset+1;
                    }
                }

                int triangleCnt = 0;
                int wireFrameCnt = 0;
                for (int j = 0; j < polyPtr->mPPtrs.size(); ++j)
                {
                    PPtr p = polyPtr->mPPtrs[j];
                    int polyCnt = p->mCount / attribCnt;
                    triangleCnt += polyCnt - 2;
                    wireFrameCnt += polyCnt * 2;
                }
                int positionCount = triangleCnt * 3 * 3;
                int normalCount = triangleCnt * 3 * 3;
                int texcoordCount = triangleCnt * 3 * 2;
                float* normal = NULL;
                if (offsetNormal >= 0 || rawNormals)
                {
                    normal = new float[normalCount];
                    if (offsetNormal < 0) offsetNormal = 0;
                }
                float* texcoord = NULL;
                if (offsetTexcoord >= 0)
                {
                    texcoord = new float[texcoordCount];
                }
                int jointNodeIndexCount = triangleCnt * 3 * 4;
                float* position = new float[positionCount];
                float* jointNodeIndex = NULL;
                float* jointWeights = NULL;
                if (skin && skin->mJNCount > 0)
                {
                    jointNodeIndex = new float[jointNodeIndexCount];
                    jointWeights = new float[jointNodeIndexCount];
                }
                uint* wfIndices = new uint[wireFrameCnt];
                int wfIdx = 0;
                int wfIdxIdx = 0;
                int triangleIdx = 0;
                unsigned int* indices = 0;
                for (int j = 0, n = polyPtr->mPPtrs.size(); j < n; ++j)
                {
                    PPtr p = polyPtr->mPPtrs[j];
                    int polyCnt = p->mCount / attribCnt;
                    indices = p->mIndices;
                    int wfBegin = wfIdx;
                    wfIndices[wfIdxIdx++] = wfIdx++;
                    wfIndices[wfIdxIdx++] = wfIdx;
                    for (int k = 1; k <= polyCnt - 2; ++k)
                    {
                        int v1 = indices[offsetVertex];
                        int v2 = indices[k * attribCnt + offsetVertex];
                        int v3 = indices[(k + 1) * attribCnt + offsetVertex];

                        position[triangleIdx * 9] = rawVertices[v1 * 3];
                        position[triangleIdx * 9 + 1] = rawVertices[v1 * 3 + 1];
                        position[triangleIdx * 9 + 2] = rawVertices[v1 * 3 + 2];

                        position[triangleIdx * 9 + 3] = rawVertices[v2 * 3];
                        position[triangleIdx * 9 + 4] = rawVertices[v2 * 3 + 1];
                        position[triangleIdx * 9 + 5] = rawVertices[v2 * 3 + 2];

                        position[triangleIdx * 9 + 6] = rawVertices[v3 * 3];
                        position[triangleIdx * 9 + 7] = rawVertices[v3 * 3 + 1];
                        position[triangleIdx * 9 + 8] = rawVertices[v3 * 3 + 2];

                        wfIndices[wfIdxIdx++] = wfIdx++;
                        wfIndices[wfIdxIdx++] = wfIdx;
                        if (k + 1 <= polyCnt - 2) // still more to go then skip 2
                        {
                            wfIdx += 2;
                        }

                        if (normal)
                        {
                            int n1 = indices[offsetNormal];
                            int n2 = indices[k * attribCnt + offsetNormal];
                            int n3 = indices[(k + 1) * attribCnt + offsetNormal];
                            Vector3 v;
                            
                            v.x = rawNormals[n1 * 3];
                            v.y = rawNormals[n1 * 3 + 1];
                            v.z = rawNormals[n1 * 3 + 2];
                            v.normalize();
                            normal[triangleIdx * 9] = v.x;
                            normal[triangleIdx * 9 + 1] = v.y;
                            normal[triangleIdx * 9 + 2] = v.z;

                            v.x = rawNormals[n2 * 3];
                            v.y = rawNormals[n2 * 3 + 1];
                            v.z = rawNormals[n2 * 3 + 2];
                            v.normalize();
                            normal[triangleIdx * 9 + 3] = v.x;
                            normal[triangleIdx * 9 + 4] = v.y;
                            normal[triangleIdx * 9 + 5] = v.z;

                            v.x = rawNormals[n3 * 3];
                            v.y = rawNormals[n3 * 3 + 1];
                            v.z = rawNormals[n3 * 3 + 2];
                            v.normalize();
                            normal[triangleIdx * 9 + 6] = v.x;
                            normal[triangleIdx * 9 + 7] = v.y;
                            normal[triangleIdx * 9 + 8] = v.z;
                        }
                        
                        if (texcoord)
                        {
                            int t1 = indices[offsetTexcoord];
                            texcoord[triangleIdx * 6] = rawTexcoords[t1 * texcoordStride];
                            texcoord[triangleIdx * 6 + 1] = rawTexcoords[t1 * texcoordStride + 1];

                            int t2 = indices[k * attribCnt + offsetTexcoord];
                            texcoord[triangleIdx * 6 + 2] = rawTexcoords[t2 * texcoordStride];
                            texcoord[triangleIdx * 6 + 3] = rawTexcoords[t2 * texcoordStride + 1];

                            int t3 = indices[(k + 1) * attribCnt + offsetTexcoord];
                            texcoord[triangleIdx * 6 + 4] = rawTexcoords[t3 * texcoordStride];
                            texcoord[triangleIdx * 6 + 5] = rawTexcoords[t3 * texcoordStride + 1];
                        }
                        
                        if (jointNodeIndex)
                        {
                            for (int m = 0; m < 4; ++m)
                            {
                                jointNodeIndex[triangleIdx * 12 + m] = skin->mJNIndex[v1 * 4 + m];
                                jointNodeIndex[triangleIdx * 12 + 4 + m] = skin->mJNIndex[v2 * 4 + m];
                                jointNodeIndex[triangleIdx * 12 + 8 + m] = skin->mJNIndex[v3 * 4 + m];
                            }
                        }

                        if (jointWeights)
                        {
                            for (int m = 0; m < 4; ++m)
                            {
                                jointWeights[triangleIdx * 12 + m] = skin->mJNWeight[v1 * 4 + m];
                                jointWeights[triangleIdx * 12 + 4 + m] = skin->mJNWeight[v2 * 4 + m];
                                jointWeights[triangleIdx * 12 + 8 + m] = skin->mJNWeight[v3 * 4 + m];
                            }
                        }

                        triangleIdx++;
                    }

                    wfIndices[wfIdxIdx++] = wfIdx++;
                    wfIndices[wfIdxIdx++] = wfBegin;
                }

                vb->setPosition(position, positionCount);

                if (normal)
                {
                    vb->setNormal(normal, normalCount);
                }

                if (texcoord)
                {
                    vb->setTexcoord(texcoord, texcoordCount);
                }

                if (jointNodeIndex)
                {
                    vb->setJointIndex(jointNodeIndex, jointNodeIndexCount);
                }

                if (jointWeights)
                {
                    vb->setJointWeight(jointWeights, jointNodeIndexCount);
                }

                IndexBufferPtr wfIB = new IndexBuffer();
                wfIB->setLineIndices(wfIndices, wireFrameCnt);
                Houyi::SurfacePtr surface = new Houyi::Surface(vb);
                surface->setWireFrameIndexBuffer(wfIB);
                mesh->addSurface(surface);

                if (polyPtr->mMaterialStr.length() > 0)
                {
                    Houyi::Material* material = convertMaterial(polyPtr->mMaterial);
                    if (material)
                    {
                        surface->setMaterial(material);
                    }
                }
            }

            // polylist
            for (int i = 0, n = colladaMesh->mPolylist.size(); i < n; ++i)
            {
                Polylist* polyPtr = colladaMesh->mPolylist.at(i);
                VertexBufferPtr vb = new VertexBuffer(VertexBuffer::EExpandAsTriangles, 0);
                int inputCnt = polyPtr->mInputs.size();
                int offsetVertex = -1;
                int offsetNormal = -1;
                int offsetTexcoord = -1;
                int attribCnt = 1; // at minimum, there should be position attrib
                int texcoordStride = 2;
                for (int j = 0; j < inputCnt; ++j)
                {
                    Input input = polyPtr->mInputs.at(j);
                    string semantic = input.mSemantic;
                    int offset = atoi(input.mOffset.c_str());
                    if (semantic.compare("VERTEX") == 0)
                    {
                        offsetVertex = offset;
                        // raw vertices already find in vertex section
                    }
                    else if (semantic.compare("NORMAL") == 0)
                    {
                        offsetNormal = offset;
                        if (!rawNormals) // normals could be already found in vertices tag
                        {
                            rawNormals = colladaMesh->findFloatArray(input.mSource)->mFloats;
                        }
                    }
                    else if (semantic.compare("TEXCOORD") == 0)
                    {
                        offsetTexcoord = offset;
                        SourcePtr src = colladaMesh->findSource(input.mSource);
                        rawTexcoords = src->mFloatArrayPtr->mFloats;
                        texcoordStride = src->mTechniqueCommon.mAccessor.mStride;
                    }

                    if (offset+1 > attribCnt)
                    {
                        attribCnt = offset+1;
                    }
                }

                int triangleCnt = 0;
                int wireFrameCnt = 0;
                for (int j = 0; j < polyPtr->mVCountPtr->mCount; ++j)
                {
                    int polyCnt = polyPtr->mVCountPtr->mIndices[j];
                    triangleCnt += polyCnt - 2;
                    wireFrameCnt += polyCnt * 2;
                }
                int positionCount = triangleCnt * 3 * 3;
                int normalCount = triangleCnt * 3 * 3;
                int texcoordCount = triangleCnt * 3 * 2;
                float* normal = NULL;
                if (offsetNormal >= 0 || rawNormals)
                {
                    normal = new float[normalCount];
                    if (offsetNormal < 0) offsetNormal = 0;
                }
                float* texcoord = NULL;
                if (offsetTexcoord >= 0)
                {
                    texcoord = new float[texcoordCount];
                }
                int jointNodeIndexCount = triangleCnt * 3 * 4;
                float* position = new float[positionCount];
                float* jointNodeIndex = NULL;
                float* jointWeights = NULL;
                if (skin && skin->mJNCount > 0)
                {
                    jointNodeIndex = new float[jointNodeIndexCount];
                    jointWeights = new float[jointNodeIndexCount];
                }
                uint* wfIndices = new uint[wireFrameCnt];
                int wfIdx = 0;
                int wfIdxIdx = 0;
                int triangleIdx = 0;
                int indexIdx = 0;
                unsigned int* indices = polyPtr->mPPtr->mIndices;
                for (int j = 0, n = polyPtr->mVCountPtr->mCount; j < n; ++j)
                {
                    int polyCnt = polyPtr->mVCountPtr->mIndices[j];
                    int wfBegin = wfIdx;
                    wfIndices[wfIdxIdx++] = wfIdx++;
                    wfIndices[wfIdxIdx++] = wfIdx;
                    for (int k = 1; k <= polyCnt - 2; ++k)
                    {
                        int v1 = indices[indexIdx * attribCnt + offsetVertex];
                        int v2 = indices[(indexIdx + k) * attribCnt + offsetVertex];
                        int v3 = indices[(indexIdx + k + 1) * attribCnt + offsetVertex];

                        position[triangleIdx * 9] = rawVertices[v1 * 3];
                        position[triangleIdx * 9 + 1] = rawVertices[v1 * 3 + 1];
                        position[triangleIdx * 9 + 2] = rawVertices[v1 * 3 + 2];

                        position[triangleIdx * 9 + 3] = rawVertices[v2 * 3];
                        position[triangleIdx * 9 + 4] = rawVertices[v2 * 3 + 1];
                        position[triangleIdx * 9 + 5] = rawVertices[v2 * 3 + 2];

                        position[triangleIdx * 9 + 6] = rawVertices[v3 * 3];
                        position[triangleIdx * 9 + 7] = rawVertices[v3 * 3 + 1];
                        position[triangleIdx * 9 + 8] = rawVertices[v3 * 3 + 2];

                        wfIndices[wfIdxIdx++] = wfIdx++;
                        wfIndices[wfIdxIdx++] = wfIdx;
                        if (k + 1 <= polyCnt - 2) // still more to go then skip 2
                        {
                            wfIdx += 2;
                        }

                        if (normal)
                        {
                            int n1 = indices[indexIdx * attribCnt + offsetNormal];
                            int n2 = indices[(indexIdx + k) * attribCnt + offsetNormal];
                            int n3 = indices[(indexIdx + k + 1) * attribCnt + offsetNormal];
                            Vector3 v;
                            
                            v.x = rawNormals[n1 * 3];
                            v.y = rawNormals[n1 * 3 + 1];
                            v.z = rawNormals[n1 * 3 + 2];
                            v.normalize();
                            normal[triangleIdx * 9] = v.x;
                            normal[triangleIdx * 9 + 1] = v.y;
                            normal[triangleIdx * 9 + 2] = v.z;

                            v.x = rawNormals[n2 * 3];
                            v.y = rawNormals[n2 * 3 + 1];
                            v.z = rawNormals[n2 * 3 + 2];
                            v.normalize();
                            normal[triangleIdx * 9 + 3] = v.x;
                            normal[triangleIdx * 9 + 4] = v.y;
                            normal[triangleIdx * 9 + 5] = v.z;

                            v.x = rawNormals[n3 * 3];
                            v.y = rawNormals[n3 * 3 + 1];
                            v.z = rawNormals[n3 * 3 + 2];
                            v.normalize();
                            normal[triangleIdx * 9 + 6] = v.x;
                            normal[triangleIdx * 9 + 7] = v.y;
                            normal[triangleIdx * 9 + 8] = v.z;
                        }

                        if (texcoord)
                        {
                            int t1 = indices[indexIdx * attribCnt + offsetTexcoord];
                            texcoord[triangleIdx * 6] = rawTexcoords[t1 * texcoordStride];
                            texcoord[triangleIdx * 6 + 1] = rawTexcoords[t1 * texcoordStride + 1];

                            int t2 = indices[(indexIdx + k) * attribCnt + offsetTexcoord];
                            texcoord[triangleIdx * 6 + 2] = rawTexcoords[t2 * texcoordStride];
                            texcoord[triangleIdx * 6 + 3] = rawTexcoords[t2 * texcoordStride + 1];

                            int t3 = indices[(indexIdx + k + 1) * attribCnt + offsetTexcoord];
                            texcoord[triangleIdx * 6 + 4] = rawTexcoords[t3 * texcoordStride];
                            texcoord[triangleIdx * 6 + 5] = rawTexcoords[t3 * texcoordStride + 1];
                        }

                        if (jointNodeIndex)
                        {
                            for (int m = 0; m < 4; ++m)
                            {
                                jointNodeIndex[triangleIdx * 12 + m] = skin->mJNIndex[v1 * 4 + m];
                                jointNodeIndex[triangleIdx * 12 + 4 + m] = skin->mJNIndex[v2 * 4 + m];
                                jointNodeIndex[triangleIdx * 12 + 8 + m] = skin->mJNIndex[v3 * 4 + m];
                            }
                        }

                        if (jointWeights)
                        {
                            for (int m = 0; m < 4; ++m)
                            {
                                jointWeights[triangleIdx * 12 + m] = skin->mJNWeight[v1 * 4 + m];
                                jointWeights[triangleIdx * 12 + 4 + m] = skin->mJNWeight[v2 * 4 + m];
                                jointWeights[triangleIdx * 12 + 8 + m] = skin->mJNWeight[v3 * 4 + m];
                            }
                        }

                        triangleIdx++;
                    }

                    wfIndices[wfIdxIdx++] = wfIdx++;
                    wfIndices[wfIdxIdx++] = wfBegin;

                    indexIdx += polyCnt;
                }

                vb->setPosition(position, positionCount);

                if (normal)
                {
                    vb->setNormal(normal, normalCount);
                }

                if (texcoord)
                {
                    vb->setTexcoord(texcoord, texcoordCount);
                }

                if (jointNodeIndex)
                {
                    vb->setJointIndex(jointNodeIndex, jointNodeIndexCount);
                }

                if (jointWeights)
                {
                    vb->setJointWeight(jointWeights, jointNodeIndexCount);
                }

                IndexBufferPtr wfIB = new IndexBuffer();
                wfIB->setLineIndices(wfIndices, wireFrameCnt);
                Houyi::SurfacePtr surface = new Houyi::Surface(vb);
                surface->setWireFrameIndexBuffer(wfIB);
                mesh->addSurface(surface);

                if (polyPtr->mMaterialStr.length() > 0)
                {
                    Houyi::Material* material = convertMaterial(polyPtr->mMaterial);
                    if (material)
                    {
                        surface->setMaterial(material);
                    }
                }
            }

            mScene->addMesh(mesh);
        }
    }

    Houyi::Mesh* ColladaLoader::findMesh(Mesh* colladaMesh)
    {
        for (int i = 0;i < mScene->getMeshCount();++i)
        {
            Houyi::Mesh* m = mScene->getMesh(i);
            if (m->getName().compare(colladaMesh->mID) == 0)
            {
                return m;
            }
        }
        return 0;
    }

    Houyi::JointNode* ColladaLoader::copyJointNode(JointNode* jnPtr)
    {
        if (!jnPtr)
        {
            return NULL;
        }

        Houyi::JointNode* hjnPtr = HouyiNew Houyi::JointNode();
        hjnPtr->mID = jnPtr->mID;
        hjnPtr->setName(jnPtr->getName());
        hjnPtr->mSID = jnPtr->mSID;
        hjnPtr->mKFCount = jnPtr->mKFCount;
        Houyi::KeyFrame* keyFrames = new Houyi::KeyFrame[hjnPtr->mKFCount];
        hjnPtr->setKeyFrames(keyFrames);
        hjnPtr->mParentID = jnPtr->mParentID;
        for (int i = 0; i < jnPtr->mChildrenID.size(); ++i)
        {
            hjnPtr->mChildrenID.push_back(jnPtr->mChildrenID.at(i));
        }
        for (int i = 0; i < hjnPtr->mKFCount; ++i)
        {
        	keyFrames[i].mTime = jnPtr->mKeyFrames[i].mTime;
        	keyFrames[i].mInterpolation = getInterpolation(jnPtr->mKeyFrames[i].mInterpolation);
            for (int j = 0, n = jnPtr->mKeyFrames[i].mTransforms.size();j < n;++j)
            {
                Transform trans = jnPtr->mKeyFrames[i].mTransforms[j];
                Houyi::Transform htrans = Houyi::Transform(trans.getMatrix().getData());
                htrans.mType = getType(trans.mType);
                for (int k = 0;k < 4;++k)
                {
                    htrans.mElements[k] = trans.mElements[k];
                }

                htrans.mInterpolation = trans.mInterpolation;
                for (int k = 0;k < 6;++k)
                {
                    htrans.mInTangent[k] = trans.mInTangent[k];
                    htrans.mOutTangent[k] = trans.mOutTangent[k];
                }
                keyFrames[i].addTransform(htrans);
            }
        }

        hjnPtr->mInvBindMatrix = Matrix4(jnPtr->mInvBindMatrix.getData());
        hjnPtr->mBindPosMatrix = Matrix4(jnPtr->getMatrix().getData());

        return hjnPtr;
    }

    Houyi::JointNode* ColladaLoader::copyTree(JointNode* jnRootPtr)
    {
        Houyi::JointNode* parent = copyJointNode(jnRootPtr);
        mJoints.push_back(parent);
        int parentID = mJoints.size() - 1;
        parent->mTreeID = parentID;
        for (int i = 0, n = jnRootPtr->mChildren.size(); i < n; ++i)
        {
            Houyi::JointNode* hjPtr = copyTree(jnRootPtr->mChildren.at(i));
            parent->addChild(hjPtr);
            hjPtr->setParent(parent);
            parent->mChildrenID.push_back(hjPtr->mTreeID);
            hjPtr->mParentID = parentID;
        }

        return parent;
    }

    Houyi::JointNode* ColladaLoader::findJointNode(string nodeId)
    {
        for (int i = 0;i < mJoints.size();++i)
        {
            Houyi::JointNode* child = mJoints[i];
            if (child->mID.compare(nodeId) == 0)
            {
                return child;
            }
        }

        return 0;
    }
    
    int ColladaLoader::findJointIndexById(string nodeId)
    {
        for (int i = 0;i < mJoints.size();++i)
        {
            Houyi::JointNode* child = mJoints[i];
            if (child->mID.compare(nodeId) == 0)
            {
                return i;
            }
        }
        
        return -1;
    }

    int ColladaLoader::findJointNodeIndexByName(string name)
    {
        if (name.length() == 0)
        {
            return -1;
        }

        int res = -1;
        for (int i = 0; i < mJoints.size(); ++i)
        {
            Houyi::JointNode* p = mJoints.at(i);
            if (name.compare(p->getName()) == 0 || name.compare(p->mID) == 0 || name.compare(p->mSID) == 0)
            {
                return i;
            }
        }

        return res;
    }
    
    Houyi::Material* ColladaLoader::convertMaterial(Material material)
    {
        if (material.mID.length() == 0 && material.mName.length() == 0)
        {
            LOGW("ColladaLoader::convertMaterial. invalid material\n");
            return 0;
        }

        MaterialManager* matMan = mScene->getMaterialManager();
        Houyi::Material* hmat = matMan->getMaterial(material.mID);
        if (!hmat)
        {
            hmat = matMan->createMaterial(material.mID);
            setMaterialAttributes(hmat, material);
        }

        return hmat;
    }

    void ColladaLoader::setMaterialAttributes(Houyi::Material* hmat, Material material)
    {
        hmat->setName(material.mName);
        
        Effect effect = material.mEffect;
        Phong phong = effect.mProfile.mTechnique.mPhong;

        Color ambient = phong.mAmbient;
        hmat->setAmbientColor(ambient.mR, ambient.mG, ambient.mB, ambient.mA);

        Diffuse diffuse = phong.mDiffuse;
        Color color = diffuse.mColor;
        hmat->setDiffuseColor(color.mR, color.mG, color.mB, color.mA);

        Color specular = phong.mSpecular;
        hmat->setSpecularColor(specular.mR, specular.mG, specular.mB, specular.mA);

        hmat->setShininess(phong.mShininess);

        hmat->getRenderState().setDoubleSided(effect.mProfile.mTechnique.mIsDoubleSided);

        Color transparent = phong.mTransparent;
        hmat->setTransparent(transparent.mR, transparent.mG, transparent.mB, transparent.mA);

        float transparency = phong.mTransparency;
        hmat->setTransparency(transparency);

        hmat->setReflectivity(phong.mReflectivity);

        string opaque = phong.mOpaque;
        if (opaque.compare("RGB_ZERO") == 0)
        {
            hmat->setTransparentMode(Houyi::Material::ERGBZero);
        }
        else if (opaque.compare("A_ONE") == 0)
        {
            hmat->setTransparentMode(Houyi::Material::EAOne);
        }

        string texname = findTexturePath(diffuse.mTextureID);
        if (texname.length() > 0)
        {
            string paths[3];
            paths[0] = StringUtils::getLastPathComponent(texname);
            paths[1] = StringUtils::getPath(mFilePath) + StringUtils::getLastPathComponent(texname);
            paths[2] = StringUtils::getTexturePath(mFilePath, texname);
            Texture* texture = hmat->createTextureByPath(mScene, paths);
            if(texture)
            {
                if (diffuse.mColor.isFullyTransparent())
                {
                    hmat->setTextureMode(RenderState::EReplace);
                }
                LOGD("Texture loaded for material: %s\n", material.mName.c_str());
            }
        }
        else
        {
            
        }
    }

    int ColladaLoader::getInterpolation(int src)
    {
        if (src == Transform::EConstInterpolation)
        {
            return Houyi::Transform::EConstInterpolation;
        }
        else if (src == Transform::ELinearInterpolation)
        {
            return Houyi::Transform::ELinearInterpolation;
        }
        else if (src == Transform::EBezierInterpolation)
        {
            return Houyi::Transform::EBezierInterpolation;
        }
        return src;
    }
    
    int ColladaLoader::getType(int src)
    {
        if (src == Transform::EMatrix)
        {
            return Houyi::Transform::EMatrix;
        }
        else if (src == Transform::ETranslate)
        {
            return Houyi::Transform::ETranslate;
        }
        else if (src == Transform::ERotate)
        {
            return Houyi::Transform::ERotate;
        }
        else if (src == Transform::EScale)
        {
            return Houyi::Transform::EScale;
        }
        return src;
    }

    string ColladaLoader::findTexturePath(string texname)
    {
        int pos = texname.find_last_of(".");
        // work-around for file exist test
        // in some case . is in the middle
        if (pos == string::npos || texname.length() - 1 - pos > 5)
        {
            for (int i = 0; i < mCollada.mMaterials.size(); ++i)
            {
                Material& mat = mCollada.mMaterials.at(i);
                Effect& effect = mat.mEffect;

                Phong& phong = effect.mProfile.mTechnique.mPhong;
                Diffuse& diffuse = phong.mDiffuse;

                // in this case texture name is from sample 2d->surface->init_from->images
                NewParam samplerParam;
                for (int j = 0;j < effect.mProfile.mNewParams.size();++j)
                {
                    NewParam np = effect.mProfile.mNewParams[j];
                    if (np.mSID.compare(texname) == 0)
                    {
                        samplerParam = np;
                        break;
                    }
                }

                // sampler found, now find surface
                NewParam surfaceParam;
                string surfaceSID = samplerParam.mSampler2D.mSourceID;
                for (int j = 0;j < effect.mProfile.mNewParams.size();++j)
                {
                    NewParam np = effect.mProfile.mNewParams[j];
                    if (np.mSID.compare(surfaceSID) == 0)
                    {
                        surfaceParam = np;
                        break;
                    }
                }

                // get image id from surface's init_from
                string initFrom = surfaceParam.mSurface.mInitFrom;

                // now find image from images
                for (int j = 0;j < mCollada.mImages.size();++j)
                {
                    Image img = mCollada.mImages[j];
                    if (img.mID.compare(initFrom) == 0)
                    {
                        // finally got file path
                        diffuse.mTextureID = img.mInitFrom;
                        return diffuse.mTextureID;
                    }
                }
            }
        }
        return texname;
    }
}
