//
//  AbsColladaLoader.cpp
//  houyi
//
//  Created by Lance Mao on 13/11/11.
//  Copyright (c) 2011 Dong wen. All rights reserved.
//

#include <iostream>
#include <assert.h>
#include "HouyiAbsColladaLoader.h"
#include "HouyiSceneManager.h"
#include "HouyiUtil.h"

namespace collada
{
    // used to separate data
    static const char* SPACE = " \n\t";
    
    const xmlChar* AbsColladaLoader::ATTR_ID = (const xmlChar*) "id";
    const xmlChar* AbsColladaLoader::ATTR_SID = (const xmlChar*) "sid";
    const xmlChar* AbsColladaLoader::ATTR_NAME = (const xmlChar*) "name";
    const xmlChar* AbsColladaLoader::ATTR_COUNT = (const xmlChar*) "count";
    const xmlChar* AbsColladaLoader::ATTR_SEMANTIC = (const xmlChar*) "semantic";
    const xmlChar* AbsColladaLoader::ATTR_OFFSET = (const xmlChar*) "offset";
    const xmlChar* AbsColladaLoader::ATTR_SET = (const xmlChar*) "set";
    const xmlChar* AbsColladaLoader::ATTR_SOURCE = (const xmlChar*) "source";
    const xmlChar* AbsColladaLoader::ATTR_TYPE = (const xmlChar*) "type";
    const xmlChar* AbsColladaLoader::ATTR_TARGET = (const xmlChar*) "target";
    const xmlChar* AbsColladaLoader::ATTR_URL = (const xmlChar*) "url";
    const xmlChar* AbsColladaLoader::ATTR_TEXTURE = (const xmlChar*) "texture";
    const xmlChar* AbsColladaLoader::ATTR_SYMBOL = (const xmlChar*) "symbol";
    const xmlChar* AbsColladaLoader::ATTR_MATERIAL = (const xmlChar*) "material";
    const xmlChar* AbsColladaLoader::ATTR_STRIDE = (const xmlChar*) "stride";
    const xmlChar* AbsColladaLoader::ATTR_OPAQUE = (const xmlChar*) "opaque";
    
    // misc
    const xmlChar* AbsColladaLoader::MISC_JOINT = (const xmlChar*) "JOINT";
    
    // core tags
    const xmlChar* AbsColladaLoader::TAG_SOURCE = (const xmlChar*) "source";
    const xmlChar* AbsColladaLoader::TAG_FLOAT_ARRAY = (const xmlChar*) "float_array";
    const xmlChar* AbsColladaLoader::TAG_NAME_ARRAY = (const xmlChar*) "Name_array";
    const xmlChar* AbsColladaLoader::TAG_INPUT = (const xmlChar*) "input";
    const xmlChar* AbsColladaLoader::TAG_MATRIX = (const xmlChar*) "matrix";
    const xmlChar* AbsColladaLoader::TAG_TRANSLATE = (const xmlChar*) "translate";
    const xmlChar* AbsColladaLoader::TAG_ROTATE = (const xmlChar*) "rotate";
    const xmlChar* AbsColladaLoader::TAG_SCALE = (const xmlChar*) "scale";
    const xmlChar* AbsColladaLoader::TAG_FLOAT = (const xmlChar*) "float";
    const xmlChar* AbsColladaLoader::TAG_ACCESSOR = (const xmlChar*) "accessor";
    const xmlChar* AbsColladaLoader::TAG_PARAM = (const xmlChar*) "param";
    
    // asset tags
    const xmlChar* AbsColladaLoader::TAG_ASSET = (const xmlChar*) "asset";
    const xmlChar* AbsColladaLoader::TAG_CONTRIBUTOR = (const xmlChar*) "contributor";
    const xmlChar* AbsColladaLoader::TAG_AUTHORING_TOOL = (const xmlChar*) "authoring_tool";
    const xmlChar* AbsColladaLoader::TAG_UP_AXIS = (const xmlChar*) "up_axis";
    
    // image tags
    const xmlChar* AbsColladaLoader::TAG_LIBRARY_IMAGES = (const xmlChar*) "library_images";
    const xmlChar* AbsColladaLoader::TAG_IMAGE = (const xmlChar*) "image";
    const xmlChar* AbsColladaLoader::TAG_INIT_FROM = (const xmlChar*) "init_from";
    
    // material tags
    const xmlChar* AbsColladaLoader::TAG_LIBRARY_MATERIALS = (const xmlChar*) "library_materials";
    const xmlChar* AbsColladaLoader::TAG_MATERIAL = (const xmlChar*) "material";
    const xmlChar* AbsColladaLoader::TAG_INSTANCE_EFFECT = (const xmlChar*) "instance_effect";
    
    // effect tags
    const xmlChar* AbsColladaLoader::TAG_LIBRARY_EFFECTS = (const xmlChar*) "library_effects";
    const xmlChar* AbsColladaLoader::TAG_EFFECT = (const xmlChar*) "effect";
    const xmlChar* AbsColladaLoader::TAG_PROFILE_COMMON = (const xmlChar*) "profile_COMMON";
    const xmlChar* AbsColladaLoader::TAG_NEW_PARAM = (const xmlChar*) "newparam";
    const xmlChar* AbsColladaLoader::TAG_EXTRA = (const xmlChar*) "extra";
    const xmlChar* AbsColladaLoader::TAG_SURFACE = (const xmlChar*) "surface";
    const xmlChar* AbsColladaLoader::TAG_SAMPLER2D = (const xmlChar*) "sampler2D";
    const xmlChar* AbsColladaLoader::TAG_TECHNIQUE = (const xmlChar*) "technique";
    const xmlChar* AbsColladaLoader::TAG_PHONG = (const xmlChar*) "phong";
    const xmlChar* AbsColladaLoader::TAG_LAMBERT = (const xmlChar*) "lambert";
    const xmlChar* AbsColladaLoader::TAG_BLINN = (const xmlChar*) "blinn";
    const xmlChar* AbsColladaLoader::TAG_DOUBLE_SIDED = (const xmlChar*) "double_sided";
    const xmlChar* AbsColladaLoader::TAG_COLOR = (const xmlChar*) "color";
    const xmlChar* AbsColladaLoader::TAG_EMISSION = (const xmlChar*) "emission";
    const xmlChar* AbsColladaLoader::TAG_AMBIENT = (const xmlChar*) "ambient";
    const xmlChar* AbsColladaLoader::TAG_DIFFUSE = (const xmlChar*) "diffuse";
    const xmlChar* AbsColladaLoader::TAG_TEXTURE = (const xmlChar*) "texture";
    const xmlChar* AbsColladaLoader::TAG_MINFILTER = (const xmlChar*) "minfilter";
    const xmlChar* AbsColladaLoader::TAG_MAGFILTER = (const xmlChar*) "magfilter";
    const xmlChar* AbsColladaLoader::TAG_SPECULAR = (const xmlChar*) "specular";
    const xmlChar* AbsColladaLoader::TAG_SHININESS = (const xmlChar*) "shininess";
    const xmlChar* AbsColladaLoader::TAG_TRANSPARENT = (const xmlChar*) "transparent";
    const xmlChar* AbsColladaLoader::TAG_TRANSPARENCY = (const xmlChar*) "transparency";
    const xmlChar* AbsColladaLoader::TAG_REFLECTIVE = (const xmlChar*)"reflectivity";
    
    // geometry tags
    const xmlChar* AbsColladaLoader::TAG_LIBRARY_GEOMETRIES = (const xmlChar*) "library_geometries";
    const xmlChar* AbsColladaLoader::TAG_GEOMETRY = (const xmlChar*) "geometry";
    const xmlChar* AbsColladaLoader::TAG_INSTANCE_GEOMETRY = (const xmlChar*) "instance_geometry";
    const xmlChar* AbsColladaLoader::TAG_MESH = (const xmlChar*) "mesh";
    const xmlChar* AbsColladaLoader::TAG_VERTICES = (const xmlChar*) "vertices";
    const xmlChar* AbsColladaLoader::TAG_LINES = (const xmlChar*) "lines";
    const xmlChar* AbsColladaLoader::TAG_TRIAGNLES = (const xmlChar*) "triangles";
    const xmlChar* AbsColladaLoader::TAG_POLYGONS = (const xmlChar*) "polygons";
    const xmlChar* AbsColladaLoader::TAG_POLYLIST = (const xmlChar*) "polylist";
    const xmlChar* AbsColladaLoader::TAG_P = (const xmlChar*) "p";
    
    // lighting tags
    const xmlChar* AbsColladaLoader::TAG_LIBRARY_LIGHTS = (const xmlChar*) "library_lights";
    const xmlChar* AbsColladaLoader::TAG_LIGHT = (const xmlChar*) "light";
    const xmlChar* AbsColladaLoader::TAG_INSTANCE_LIGHT = (const xmlChar*) "instance_light";
    const xmlChar* AbsColladaLoader::TAG_POINT = (const xmlChar*) "point";
    const xmlChar* AbsColladaLoader::TAG_DIRECTIONAL = (const xmlChar*) "directional";
    
    // camera tags
    const xmlChar* AbsColladaLoader::TAG_LIBRARY_CAMERAS = (const xmlChar*) "library_cameras";
    const xmlChar* AbsColladaLoader::TAG_CAMERA = (const xmlChar*) "camera";
    const xmlChar* AbsColladaLoader::TAG_INSTANCE_CAMERA = (const xmlChar*) "instance_camera";
    const xmlChar* AbsColladaLoader::TAG_OPTICS = (const xmlChar*) "optics";
    const xmlChar* AbsColladaLoader::TAG_PERSPECTIVE = (const xmlChar*) "perspective";
    const xmlChar* AbsColladaLoader::TAG_XFOV = (const xmlChar*) "xfov";
    const xmlChar* AbsColladaLoader::TAG_YFOV = (const xmlChar*) "yfov";
    const xmlChar* AbsColladaLoader::TAG_ASPECT_RATIO = (const xmlChar*) "aspect_ratio";
    const xmlChar* AbsColladaLoader::TAG_NEAR = (const xmlChar*) "znear";
    const xmlChar* AbsColladaLoader::TAG_FAR = (const xmlChar*) "zfar";
    
    // visual scene tags
    const xmlChar* AbsColladaLoader::TAG_LIBRARY_VISUAL_SCENES = (const xmlChar*) "library_visual_scenes";
    const xmlChar* AbsColladaLoader::TAG_LIBRARY_NODES = (const xmlChar*) "library_nodes";
    const xmlChar* AbsColladaLoader::TAG_VISUAL_SCENE = (const xmlChar*) "visual_scene";
    const xmlChar* AbsColladaLoader::TAG_NODE = (const xmlChar*) "node";
    const xmlChar* AbsColladaLoader::TAG_INSTANCE_CONTROLLER = (const xmlChar*) "instance_controller";
    const xmlChar* AbsColladaLoader::TAG_BIND_MATERIAL = (const xmlChar*) "bind_material";
    const xmlChar* AbsColladaLoader::TAG_INSTANCE_MATERIAL = (const xmlChar*) "instance_material";
    const xmlChar* AbsColladaLoader::TAG_TECHNIQUE_COMMON = (const xmlChar*) "technique_common";
    const xmlChar* AbsColladaLoader::TAG_INSTANCE_NODE = (const xmlChar*) "instance_node";
    
    // controller tags
    const xmlChar* AbsColladaLoader::TAG_LIBRARY_CONTROLLERS = (const xmlChar*) "library_controllers";
    const xmlChar* AbsColladaLoader::TAG_CONTROLLER = (const xmlChar*) "controller";
    const xmlChar* AbsColladaLoader::TAG_SKIN = (const xmlChar*) "skin";
    const xmlChar* AbsColladaLoader::TAG_BIND_SHAPE_MATRIX = (const xmlChar*) "bind_shape_matrix";
    const xmlChar* AbsColladaLoader::TAG_JOINTS = (const xmlChar*) "joints";
    const xmlChar* AbsColladaLoader::TAG_VERTEX_WEIGHTS = (const xmlChar*) "vertex_weights";
    const xmlChar* AbsColladaLoader::TAG_VCOUNT = (const xmlChar*) "vcount";
    const xmlChar* AbsColladaLoader::TAG_V = (const xmlChar*) "v";
    
    // animation tags
    const xmlChar* AbsColladaLoader::TAG_LIBRARY_ANIMATIONS = (const xmlChar*) "library_animations";
    const xmlChar* AbsColladaLoader::TAG_ANIMATION = (const xmlChar*) "animation";
    const xmlChar* AbsColladaLoader::TAG_SAMPLER = (const xmlChar*) "sampler";
    const xmlChar* AbsColladaLoader::TAG_CHANNEL = (const xmlChar*) "channel";
    
    Scene* AbsColladaLoader::loadFromMemory(const char* data, int size)
    {
        mLogLevel = EStats;
        
        xmlDoc* doc = xmlParseMemory(data, size);
        if (!doc)
        {
            LOGD("COLLADA file not parsed correctly. Abort...");
            return NULL;
        }

        xmlNode* root = xmlDocGetRootElement(doc);
        if (!root)
        {
            LOGD("COLLADA file is empty. Abort...");
            xmlFreeDoc(doc);
            return NULL;
        }
        
        notifiyProgress(0);
        
        xmlNode* cur = root;

        // first load xml data into intermediate layer
        loadElement(cur);
        if (mIsCanceled) return loadingCanceled(doc);
        notifiyProgress(0.1);

        // arrange data
        prepareMaterial();
        if (mIsCanceled) return loadingCanceled(doc);
        prepareNode();
        if (mIsCanceled) return loadingCanceled(doc);
        prepareLights();
        if (mIsCanceled) return loadingCanceled(doc);
        prepareGeometry();
        if (mIsCanceled) return loadingCanceled(doc);
        updateJointsAni();
        if (mIsCanceled) return loadingCanceled(doc);
        prepareSkins();
        
        if (mLogLevel == EFull)
        {
            dumpScene();
        }

        xmlFreeDoc(doc);

        notifiyProgress(0.2);
        if (mIsCanceled)
        {
            return loadingCanceled(0);
        }
        
        // call pure virtual convert which will be implemented by sub class
        Scene* ret = convert(mCollada);
        return ret;
    }

    void AbsColladaLoader::loadElement(xmlNode * a_node)
    {
        assert(a_node);

        xmlNode *cur_node = a_node->children;

        while (cur_node)
        {
            if (cur_node->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur_node->name, TAG_ASSET))
                {
                    loadAsset(cur_node);
                }
                else if (!xmlStrcmp(cur_node->name, TAG_LIBRARY_IMAGES))
                {
                    loadLibraryImages(cur_node);
                }
                else if (!xmlStrcmp(cur_node->name, TAG_LIBRARY_MATERIALS))
                {
                    loadLibraryMaterials(cur_node);
                }
                else if (!xmlStrcmp(cur_node->name, TAG_LIBRARY_EFFECTS))
                {
                    loadLibraryEffects(cur_node);
                }
                else if (!xmlStrcmp(cur_node->name, TAG_LIBRARY_GEOMETRIES))
                {
                    bool res = loadLibraryGeometries(cur_node);
                    if (!res) return;
                }
                else if (!xmlStrcmp(cur_node->name, TAG_LIBRARY_LIGHTS))
                {
                    loadLibraryLights(cur_node);
                }
                else if (!xmlStrcmp(cur_node->name, TAG_LIBRARY_CAMERAS))
                {
                    loadLibraryCameras(cur_node);
                }
                else if (!xmlStrcmp(cur_node->name, TAG_LIBRARY_VISUAL_SCENES))
                {
                    loadLibraryVisualScenes(cur_node);
                }
                else if (!xmlStrcmp(cur_node->name, TAG_LIBRARY_NODES))
                {
                    loadLibraryNodes(cur_node);
                }
                else if (!xmlStrcmp(cur_node->name, TAG_LIBRARY_CONTROLLERS))
                {
                    loadLibraryControllers(cur_node);
                }
                else if (!xmlStrcmp(cur_node->name, TAG_LIBRARY_ANIMATIONS))
                {
                    loadLibraryAnimations(cur_node);
                }
                else
                {
                    LOGW("Unsupported Collada library tag: %s\n", cur_node->name);
                }
            }

            cur_node = cur_node->next;
            if (mIsCanceled) return;
        }
    }

#pragma mark Core

    FloatArrayPtr AbsColladaLoader::loadFloatArray(xmlNode* node)
    {
        FloatArrayPtr floatArrayPtr = new FloatArray();

        xmlChar* dataId = xmlGetProp(node, ATTR_ID);
        floatArrayPtr->mID = xmlChar2string(dataId);
        xmlFree(dataId);

        xmlChar* count = xmlGetProp(node, ATTR_COUNT);
        assert(count);
        int cnt = atoi((const char*) count);
        xmlFree(count);

        floatArrayPtr->mCount = cnt;
        if (cnt == 0)
        {
            LOGE("AbsColladaLoader::loadFloatArray. 0 float");
            return floatArrayPtr;
        }

        floatArrayPtr->mFloats = new float[cnt];

        xmlChar* xmlRaw = node->children->content;
        char* raw = (char*) xmlRaw;
        char* saveptr;
        char* p = strtok_r(raw, SPACE, &saveptr);
        int i = 0;
        while (p)
        {
            floatArrayPtr->mFloats[i] = atof(p);
            p = strtok_r(NULL, SPACE, &saveptr);
            i++;
        }

        return floatArrayPtr;
    }

    FloatArrayPtr AbsColladaLoader::loadFloatArray(xmlNode* node, int count)
    {
        FloatArrayPtr floatArrayPtr = new FloatArray();

        xmlChar* dataId = xmlGetProp(node, ATTR_SID);
        floatArrayPtr->mID = xmlChar2string(dataId);
        xmlFree(dataId);

        floatArrayPtr->mCount = count;
        if (count == 0)
        {
            return floatArrayPtr;
        }
        floatArrayPtr->mFloats = new float[count];

        xmlChar* xmlRaw = node->children->content;
        char* raw = (char*) xmlRaw;
        char* saveptr;
        char* p = strtok_r(raw, SPACE, &saveptr);
        int i = 0;
        while (p)
        {
            floatArrayPtr->mFloats[i] = atof(p);
            p = strtok_r(NULL, SPACE, &saveptr);
            i++;
        }

        return floatArrayPtr;
    }

    NameArrayPtr AbsColladaLoader::loadNameArray(xmlNode* node)
    {
        NameArrayPtr arrayPtr = new NameArray();

        xmlChar* dataId = xmlGetProp(node, ATTR_ID);
        arrayPtr->mID = xmlChar2string(dataId);
        xmlFree(dataId);

        xmlChar* count = xmlGetProp(node, ATTR_COUNT);
        assert(count);
        int cnt = atoi((const char*) count);
        xmlFree(count);

        arrayPtr->mCount = cnt;
        arrayPtr->mNames = new string[cnt];

        xmlChar* xmlRaw = node->children->content;
        char* raw = (char*) xmlRaw;
        char* saveptr;
        char* p = strtok_r(raw, SPACE, &saveptr);
        int i = 0;
        while (p)
        {
            arrayPtr->mNames[i] = (p);
            p = strtok_r(NULL, SPACE, &saveptr);
            i++;
        }

        return arrayPtr;
    }

    SourcePtr AbsColladaLoader::loadSource(xmlNode* node)
    {
        SourcePtr srcPtr = new Source();
        xmlChar* dataId = xmlGetProp(node, ATTR_ID);
        srcPtr->mID = xmlChar2string(dataId);
        xmlFree(dataId);

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_FLOAT_ARRAY))
                {
                    FloatArrayPtr fa = loadFloatArray(cur);
                    srcPtr->mFloatArrayPtr = fa;
                }
                else if (!xmlStrcmp(cur->name, TAG_NAME_ARRAY))
                {
                    NameArrayPtr na = loadNameArray(cur);
                    srcPtr->mNameArrayPtr = na;
                }
                else if (!xmlStrcmp(cur->name, TAG_TECHNIQUE_COMMON))
                {
                    srcPtr->mTechniqueCommon = loadTechniqueCommon(cur);
                }
            }

            cur = cur->next;
        }

        return srcPtr;
    }

    Input AbsColladaLoader::loadInput(xmlNode* node)
    {
        Input input;
        xmlChar* dataId = xmlGetProp(node, ATTR_SEMANTIC);
        input.mSemantic = xmlChar2string(dataId);
        xmlFree(dataId);

        xmlChar* offset = xmlGetProp(node, ATTR_OFFSET);
        input.mOffset = xmlChar2string(offset);
        xmlFree(offset);

        xmlChar* set = xmlGetProp(node, ATTR_SET);
        input.mSet = xmlChar2string(set);
        xmlFree(set);

        xmlChar* source = xmlGetProp(node, ATTR_SOURCE);
        input.mSource = xmlChar2string(source);
        xmlFree(source);
        return input;
    }

    Matrix AbsColladaLoader::loadMatrix(xmlNode* node)
    {
        Matrix m;
        xmlChar* xmlRaw = node->children->content;
        char* raw = (char*) xmlRaw;
        char *saveptr;
        char* p = strtok_r(raw, SPACE, &saveptr);
        int i = 0;
        while (p)
        {
            m[i] = atof(p);
            p = strtok_r(NULL, SPACE, &saveptr);
            i++;
        }

        return m;
    }

    float AbsColladaLoader::loadFloat(xmlNode* node)
    {
        xmlChar* xmlRaw = node->children->content;
        char* raw = (char*) xmlRaw;
        char *saveptr;
        char* p = strtok_r(raw, SPACE, &saveptr);
        return atof(p);
    }
    
    float AbsColladaLoader::loadInt(xmlNode* node)
    {
        xmlChar* xmlRaw = node->children->content;
        char* raw = (char*) xmlRaw;
        char *saveptr;
        char* p = strtok_r(raw, SPACE, &saveptr);
        return atoi(p);
    }

    Transform AbsColladaLoader::loadTransform(xmlNode* node)
    {
        xmlChar* sid = xmlGetProp(node, ATTR_SID);
        Transform transform;
        transform.mSID = xmlChar2string(sid);
        xmlFree(sid);
        
        if (!xmlStrcmp(node->name, TAG_MATRIX))
        {
            transform.mType = Transform::EMatrix;
            transform.setMatrix(loadMatrix(node));
        }
        else if (!xmlStrcmp(node->name, TAG_TRANSLATE))
        {
            transform.mType = Transform::ETranslate;
            FloatArrayPtr fa = loadFloatArray(node, 3);
            float* fdata = fa->mFloats;
            if (fdata)
            {
                transform.translate(fdata[0], fdata[1], fdata[2]);
            }
        }
        else if (!xmlStrcmp(node->name, TAG_ROTATE))
        {
            transform.mType = Transform::ERotate;
            FloatArrayPtr fa = loadFloatArray(node, 4);
            float* fdata = fa->mFloats;
            if (fdata)
            {
                transform.rotate(fdata[0], fdata[1], fdata[2], fdata[3]);
            }
        }
        else if (!xmlStrcmp(node->name, TAG_SCALE))
        {
            transform.mType = Transform::EScale;
            FloatArrayPtr fa = loadFloatArray(node, 3);
            float* fdata = fa->mFloats;
            if (fdata)
            {
                transform.scale(fdata[0], fdata[1], fdata[2]);
            }
        }
        else
        {
            LOGW("Unsupported Collada tag: %s\n", node->name);
        }
        return transform;
    }

    Accessor AbsColladaLoader::loadAccessor(xmlNode* node)
    {
        Accessor accessor;

        xmlChar* src = xmlGetProp(node, ATTR_SOURCE);
        if (src)
        {
            accessor.mSource = xmlChar2string(src);
            xmlFree(src);
        }

        xmlChar* str_count = xmlGetProp(node, ATTR_COUNT);
        if (str_count)
        {
            unsigned int cnt = atoi((const char*) str_count);
            accessor.mCount = cnt;
            xmlFree(str_count);
        }

        xmlChar* str_stride = xmlGetProp(node, ATTR_STRIDE);
        if (str_stride)
        {
            unsigned int stride = atoi((const char*) str_stride);
            accessor.mStride = stride;
            xmlFree(str_stride);
        }

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_PARAM))
                {
                    Param param = loadParam(cur);
                    accessor.mParams.push_back(param);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }
        return accessor;
    }

    Param AbsColladaLoader::loadParam(xmlNode* node)
    {
        Param param;
        xmlChar* name = xmlGetProp(node, ATTR_NAME);
        param.mName = xmlChar2string(name);
        xmlFree(name);

        xmlChar* type = xmlGetProp(node, ATTR_TYPE);
        param.mType = xmlChar2string(type);
        xmlFree(type);
        return param;
    }

#pragma mark Visual Scene
    void AbsColladaLoader::loadLibraryVisualScenes(xmlNode* node)
    {
        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                assert(!xmlStrcmp(cur->name, TAG_VISUAL_SCENE));
                loadVisualScene(cur);
            }
            cur = cur->next;
        }
    }

    void AbsColladaLoader::loadLibraryNodes(xmlNode* node)
    {
        VisualScene dummy;
        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                assert(!xmlStrcmp(cur->name, TAG_NODE));
                SceneNode* nodePtr = loadSceneNode(dummy, cur);
                mCollada.mNodes.push_back(nodePtr);
            }
            cur = cur->next;
        }
    }

    void AbsColladaLoader::loadVisualScene(xmlNode* node)
    {
        VisualScene vs;

        xmlChar* id = xmlGetProp(node, ATTR_ID);
        vs.mID = xmlChar2string(id);
        xmlFree(id);

        xmlChar* name = xmlGetProp(node, ATTR_NAME);
        vs.mName = xmlChar2string(name);
        xmlFree(name);

        xmlNode* child = node->children;
        while (child)
        {
            if (child->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(child->name, TAG_NODE))
                {
                    xmlChar* type = xmlGetProp(child, ATTR_TYPE);
                    if (xmlStrcmp(type, MISC_JOINT) == 0)
                    {
                        vs.mJointRootPtrs.push_back(loadJointNode(child));
                    }
                    else if (isLightNode(child))
                    {
                        vs.mLights.push_back(loadLightNode(child));
                    }
                    else if (isCameraNode(child))
                    {
                        vs.mCameras.push_back(loadCameraNode(child));
                    }
                    else if (isCameraTargetNode(child))
                    {
                        vs.mCameraTargets.push_back(loadSceneNode(vs, child));
                    }
                    else // geometry or geometry groups
                    {
                        SceneNode* nodePtr = loadSceneNode(vs, child);
                        vs.mSceneNodePtrs.push_back(nodePtr);
                    }
                    xmlFree(type);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", child->name);
                }
            }
            child = child->next;
        }

        mCollada.mVisualScenes.push_back(vs);
    }

    JointNode* AbsColladaLoader::loadJointNode(xmlNode* node)
    {
        JointNode* jnPtr = HouyiNew JointNode();
        mCollada.mJointNodeOwner.push_back(jnPtr);
        xmlChar* id = xmlGetProp(node, ATTR_ID);
        jnPtr->mID = xmlChar2string(id);
        xmlFree(id);
        xmlChar* name = xmlGetProp(node, ATTR_NAME);
        jnPtr->setName(xmlChar2string(name));
        xmlFree(name);
        xmlChar* sid = xmlGetProp(node, ATTR_SID);
        jnPtr->mSID = xmlChar2string(sid);
        xmlFree(sid);
        xmlChar* type = xmlGetProp(node, ATTR_TYPE);
        jnPtr->mType = xmlChar2string(type);
        xmlFree(type);

        xmlNode* child = node->children;
        while (child)
        {
            if (child->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(child->name, TAG_NODE))
                {
                    JointNode* jnChildPtr = loadJointNode(child);
                    jnChildPtr->mParentPtr = jnPtr;
                    jnChildPtr->mParentIDStr = jnPtr->mID;
                    jnPtr->mChildren.push_back(jnChildPtr);
                }
                else if (!xmlStrcmp(child->name, TAG_MATRIX)
                        || !xmlStrcmp(child->name, TAG_TRANSLATE)
                        || !xmlStrcmp(child->name, TAG_ROTATE)
                        || !xmlStrcmp(child->name, TAG_SCALE))
                {
                    jnPtr->addTransform(loadTransform(child));
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", child->name);
                }
            }
            child = child->next;
        }

        return jnPtr;
    }

    SceneNode* AbsColladaLoader::loadSceneNode(VisualScene& vs, xmlNode* node)
    {
        SceneNode* nodePtr = HouyiNew SceneNode();
        mCollada.mSceneNodeOwner.push_back(nodePtr);
        
        xmlChar* id = xmlGetProp(node, ATTR_ID);
        nodePtr->mID = xmlChar2string(id);
        xmlFree(id);
        xmlChar* name = xmlGetProp(node, ATTR_NAME);
        nodePtr->setName(xmlChar2string(name));
        xmlFree(name);
        xmlChar* sid = xmlGetProp(node, ATTR_SID);
        nodePtr->mSID = xmlChar2string(sid);
        xmlFree(sid);
        xmlChar* type = xmlGetProp(node, ATTR_TYPE);
        nodePtr->mType = xmlChar2string(type);
        xmlFree(type);
        if (nodePtr->getName().length() == 0)
        {
            nodePtr->setName(nodePtr->mID.length() == 0 ? nodePtr->mSID : nodePtr->mID);
        }

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_NODE))
                {
                    xmlChar* type = xmlGetProp(cur, ATTR_TYPE);
                    if (!xmlStrcmp(type, MISC_JOINT))
                    {
                        vs.mJointRootPtrs.push_back(loadJointNode(cur));
                    }
                    else
                    {
                        SceneNode* gnChildPtr = loadSceneNode(vs, cur);
                        gnChildPtr->mParentPtr = nodePtr;
                        nodePtr->mChildren.push_back(gnChildPtr);
                    }
                    xmlFree(type);
                }
                else if (isLightNode(node))
                {
                    // at this point light is a sub node in some group
                    // need to consider parents' matrix
                    SceneNode* light = loadLightNode(node);
                    vs.mLights.push_back(light);
                }
                else if (isCameraNode(node))
                {
                    SceneNode* camera = loadCameraNode(node);
                    vs.mCameras.push_back(camera);
                    return camera;
                }
                else if (!xmlStrcmp(cur->name, TAG_INSTANCE_CONTROLLER))
                {
                    nodePtr->mController = loadInstanceController(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_MATRIX)
                        || !xmlStrcmp(cur->name, TAG_TRANSLATE)
                        || !xmlStrcmp(cur->name, TAG_ROTATE)
                        || !xmlStrcmp(cur->name, TAG_SCALE))
                {
                    nodePtr->addTransform(loadTransform(cur));
                }
                else if (!xmlStrcmp(cur->name, TAG_INSTANCE_GEOMETRY))
                {
                    InstanceGeometry instanceGeo = loadInstanceGeo(cur);
                    for(int i = 0;i < instanceGeo.mGeometry.mTechniqueCommon.mSymbol.size();++i)
                    {
                        string symbol = instanceGeo.mGeometry.mTechniqueCommon.mSymbol[i];
                        string target = instanceGeo.mGeometry.mTechniqueCommon.mMaterialTarget[i];
                        instanceGeo.mMaterialMap.insert(pair<string, string>(symbol, target));
                    }
                    nodePtr->mGeoInstance.push_back(instanceGeo);
                }
                else if (!xmlStrcmp(cur->name, TAG_INSTANCE_NODE))
                {
                    SceneNode* nodeIns = HouyiNew SceneNode();
                    mCollada.mSceneNodeOwner.push_back(nodeIns);
                    nodePtr->mChildren.push_back(nodeIns);
                    nodeIns->mParentPtr = nodePtr;
                    xmlChar* url = xmlGetProp(cur, ATTR_URL);
                    nodeIns->mURL = xmlChar2string(url).substr(1);
                    xmlFree(url);
                    mCollada.mInstanceNodePtrs.push_back(nodeIns);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        return nodePtr;
    }

    InstanceGeometry AbsColladaLoader::loadInstanceGeo(xmlNode* node)
    {
        InstanceGeometry instanceGeo;
        Geometry geoPtr;
        xmlChar* id = xmlGetProp(node, ATTR_ID);
        geoPtr.mID = xmlChar2string(id);
        xmlFree(id);
        xmlChar* name = xmlGetProp(node, ATTR_NAME);
        geoPtr.setName(xmlChar2string(name));
        xmlFree(name);
        xmlChar* url = xmlGetProp(node, ATTR_URL);
        geoPtr.mURL = xmlChar2string(url);
        xmlFree(url);

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_BIND_MATERIAL))
                {
                    xmlNode* child = cur->children;
                    while (child)
                    {
                        if (child->type == XML_ELEMENT_NODE)
                        {
                            if (!xmlStrcmp(child->name, TAG_TECHNIQUE_COMMON))
                            {
                                geoPtr.mTechniqueCommon = loadTechniqueCommon(child);
                            }
                            else
                            {
                                LOGW("Unsupported Collada tag: %s\n", child->name);
                            }
                        }

                        child = child->next;
                    }
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }

            cur = cur->next;
        }

        instanceGeo.mGeometry = geoPtr;
        return instanceGeo;
    }

    SceneNode* AbsColladaLoader::loadLightNode(xmlNode* node)
    {
        SceneNode* snPtr = HouyiNew SceneNode();
        mCollada.mSceneNodeOwner.push_back(snPtr);
        
        xmlChar* id = xmlGetProp(node, ATTR_ID);
        snPtr->mID = xmlChar2string(id);
        xmlFree(id);
        xmlChar* name = xmlGetProp(node, ATTR_NAME);
        snPtr->setName(xmlChar2string(name));
        xmlFree(name);
        xmlChar* sid = xmlGetProp(node, ATTR_SID);
        snPtr->mSID = xmlChar2string(sid);
        xmlFree(sid);

        xmlNode* child = node->children;
        while (child)
        {
            if (child->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(child->name, TAG_MATRIX)
                        || !xmlStrcmp(child->name, TAG_TRANSLATE)
                        || !xmlStrcmp(child->name, TAG_ROTATE)
                        || !xmlStrcmp(child->name, TAG_SCALE))
                {
                    snPtr->addTransform(loadTransform(child));
                }
                else if (!xmlStrcmp(child->name, TAG_INSTANCE_LIGHT))
                {
                    xmlChar* url = xmlGetProp(child, ATTR_URL);
                    for (int i = 0; i < mCollada.mLights.size(); ++i)
                    {
                        Light* l = mCollada.mLights.at(i);
                        if (xmlChar2string(url).substr(1).compare(l->mID) == 0)
                        {
                            snPtr->mTechniqueCommon = l->mTechniqueCommon;
                            break;
                        }
                    }
                    xmlFree(url);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", child->name);
                }
            }
            child = child->next;
        }

        return snPtr;
    }

    SceneNode* AbsColladaLoader::loadCameraNode(xmlNode* node)
    {
        SceneNode* snPtr = HouyiNew SceneNode();
        mCollada.mSceneNodeOwner.push_back(snPtr);
        
        xmlChar* id = xmlGetProp(node, ATTR_ID);
        snPtr->mID = xmlChar2string(id);
        xmlFree(id);
        xmlChar* name = xmlGetProp(node, ATTR_NAME);
        snPtr->setName(xmlChar2string(name));
        xmlFree(name);
        xmlChar* sid = xmlGetProp(node, ATTR_SID);
        snPtr->mSID = xmlChar2string(sid);
        xmlFree(sid);

        xmlNode* child = node->children;
        while (child)
        {
            if (child->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(child->name, TAG_MATRIX)
                        || !xmlStrcmp(child->name, TAG_TRANSLATE)
                        || !xmlStrcmp(child->name, TAG_ROTATE)
                        || !xmlStrcmp(child->name, TAG_SCALE))
                {
                    snPtr->addTransform(loadTransform(child));
                }
                else if (!xmlStrcmp(child->name, TAG_INSTANCE_CAMERA))
                {
                    xmlChar* url = xmlGetProp(child, ATTR_URL);
                    for (int i = 0; i < mCollada.mCameras.size(); ++i)
                    {
                        Camera cam = mCollada.mCameras.at(i);
                        if (xmlChar2string(url).substr(1).compare(cam.mID) == 0)
                        {
                            snPtr->mTechniqueCommon = cam.mOptics.mTechniqueCommon;
                            break;
                        }
                    }
                    xmlFree(url);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", child->name);
                }
            }
            child = child->next;
        }

        return snPtr;
    }

    Controller AbsColladaLoader::loadInstanceController(xmlNode* node)
    {
        Controller controller;

        xmlChar* url = xmlGetProp(node, ATTR_URL);
        controller.mURL = xmlChar2string(url);
        xmlFree(url);

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_BIND_MATERIAL))
                {
                    xmlNode* child = cur->children;
                    while (child)
                    {
                        if (cur->type == XML_ELEMENT_NODE)
                        {
                            if (!xmlStrcmp(child->name, TAG_TECHNIQUE_COMMON))
                            {
                                controller.mTechniqueCommon = loadTechniqueCommon(child);
                            }
                        }

                        child = child->next;
                    }
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }

            cur = cur->next;
        }

        return controller;
    }

    TechniqueCommon AbsColladaLoader::loadTechniqueCommon(xmlNode* node)
    {
        TechniqueCommon tc;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_INSTANCE_MATERIAL))
                {
                    xmlChar* symbol = xmlGetProp(cur, ATTR_SYMBOL);
                    string strSymbol = xmlChar2string(symbol);
                    tc.mSymbol.push_back(strSymbol);
                    xmlFree(symbol);

                    xmlChar* matTarget = xmlGetProp(cur, ATTR_TARGET);
                    string strTarget = xmlChar2string(matTarget);
                    tc.mMaterialTarget.push_back(strTarget);
                    xmlFree(matTarget);
                    
                    mMaterialSymbolMap.insert(pair<string, string>(strSymbol, strTarget));
                }
                else if (!xmlStrcmp(cur->name, TAG_AMBIENT))
                {
                    xmlNode* child = cur->children;
                    while (child)
                    {
                        if (!xmlStrcmp(child->name, TAG_COLOR))
                        {
                            tc.mPoint.mColor = loadColor(child);
                            tc.mHasAmbient = true;
                        }
                        child = child->next;
                    }
                }
                else if (!xmlStrcmp(cur->name, TAG_POINT))
                {
                    tc.mPoint = loadPoint(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_DIRECTIONAL))
                {
                    tc.mPoint = loadPoint(cur);
                    tc.mIsDirectional = true;
                }
                else if (!xmlStrcmp(cur->name, TAG_PERSPECTIVE))
                {
                    tc.mPerspective = loadPerspective(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_ACCESSOR))
                {
                    tc.mAccessor = loadAccessor(cur);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }

            cur = cur->next;
        }

        return tc;
    }

#pragma mark Asset
    void AbsColladaLoader::loadAsset(xmlNode *node)
    {
        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_CONTRIBUTOR))
                {
                    mCollada.mAsset.mContributor = loadContributor(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_UP_AXIS))
                {
                    xmlChar* xmlRaw = cur->children->content;
                    mCollada.mAsset.mUpAxis = xmlChar2string(xmlRaw);
                }
            }
            cur = cur->next;
        }
    }
    
    Contributor AbsColladaLoader::loadContributor(xmlNode* node)
    {
        Contributor contributor;
        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_AUTHORING_TOOL))
                {
                    xmlChar* xmlRaw = cur->children->content;
                    contributor.mAuthoringTool.mTool = xmlChar2string(xmlRaw);
                }
            }
            cur = cur->next;
        }
        return contributor;
    }

#pragma mark Image
    void AbsColladaLoader::loadLibraryImages(xmlNode* node)
    {
        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                assert(!xmlStrcmp(cur->name, TAG_IMAGE));
                mCollada.mImages.push_back(loadImage(cur));
            }
            cur = cur->next;
        }
    }

    Image AbsColladaLoader::loadImage(xmlNode* node)
    {
        Image image;
        xmlChar* id = xmlGetProp(node, ATTR_ID);
        image.mID = xmlChar2string(id);
        xmlFree(id);

        xmlChar* name = xmlGetProp(node, ATTR_NAME);
        image.mName = xmlChar2string(name);
        xmlFree(name);

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_INIT_FROM))
                {
                    xmlChar* xmlRaw = cur->children->content;
                    image.mInitFrom = xmlChar2string(xmlRaw);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }

            cur = cur->next;
        }

        return image;
    }

#pragma mark Material
    void AbsColladaLoader::loadLibraryMaterials(xmlNode* node)
    {
        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                assert(!xmlStrcmp(cur->name, TAG_MATERIAL));
                mCollada.mMaterials.push_back(loadMaterial(cur));
            }
            cur = cur->next;
        }
    }

    Material AbsColladaLoader::loadMaterial(xmlNode* node)
    {
        Material material;

        xmlChar* id = xmlGetProp(node, ATTR_ID);
        material.mID = xmlChar2string(id);
        xmlFree(id);

        xmlChar* name = xmlGetProp(node, ATTR_NAME);
        material.mName = xmlChar2string(name);
        xmlFree(name);

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_INSTANCE_EFFECT))
                {
                    xmlChar* xmlRaw = xmlGetProp(cur, ATTR_URL);
                    material.mEffectURL = xmlChar2string(xmlRaw);
                    xmlFree(xmlRaw);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        return material;
    }

#pragma mark Effect
    void AbsColladaLoader::loadLibraryEffects(xmlNode* node)
    {
        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                assert(!xmlStrcmp(cur->name, TAG_EFFECT));
                mCollada.mEffects.push_back(loadEffect(cur));
            }
            cur = cur->next;
        }
    }

    Effect AbsColladaLoader::loadEffect(xmlNode* node)
    {
        Effect effect;

        xmlChar* id = xmlGetProp(node, ATTR_ID);
        effect.mID = xmlChar2string(id);
        xmlFree(id);

        xmlChar* name = xmlGetProp(node, ATTR_NAME);
        effect.mName = xmlChar2string(name);
        xmlFree(name);

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_PROFILE_COMMON))
                {
                    effect.mProfile = loadProfileCommon(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_EXTRA))
                {
                    Extra extra = loadExtra(cur);
                    effect.mProfile.mTechnique.mIsDoubleSided = extra.mTechnique.mIsDoubleSided;
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        return effect;
    }

    ProfileCommon AbsColladaLoader::loadProfileCommon(xmlNode* node)
    {
        ProfileCommon pc;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_TECHNIQUE))
                {
                    pc.mTechnique = loadTechnique(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_NEW_PARAM))
                {
                    pc.mNewParams.push_back(loadNewParam(cur));
                }
                else if (!xmlStrcmp(cur->name, TAG_EXTRA))
                {
                    Extra extra = loadExtra(cur);
                    pc.mTechnique.mIsDoubleSided = extra.mTechnique.mIsDoubleSided;
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        return pc;
    }

    Extra AbsColladaLoader::loadExtra(xmlNode* node)
    {
        Extra extra;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_TECHNIQUE))
                {
                    extra.mTechnique = loadTechnique(cur);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        return extra;
    }

    NewParam AbsColladaLoader::loadNewParam(xmlNode* node)
    {
        NewParam newParam;
        xmlChar* sid = xmlGetProp(node, ATTR_SID);
        newParam.mSID = xmlChar2string(sid);
        xmlFree(sid);

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_SURFACE))
                {
                    newParam.mSurface = loadSurface(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_SAMPLER2D))
                {
                    newParam.mSampler2D = loadSampler2D(cur);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }
        return newParam;
    }

    Surface AbsColladaLoader::loadSurface(xmlNode* node)
    {
        Surface surface;

        xmlChar* type = xmlGetProp(node, ATTR_TYPE);
        surface.mType = xmlChar2string(type);
        xmlFree(type);

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_INIT_FROM))
                {
                    xmlChar* xmlRaw = cur->children->content;
                    surface.mInitFrom = xmlChar2string(xmlRaw);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }
        return surface;
    }

    Sampler2D AbsColladaLoader::loadSampler2D(xmlNode* node)
    {
        Sampler2D sampler2d;
        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_SOURCE))
                {
                    xmlChar* xmlRaw = cur->children->content;
                    sampler2d.mSourceID = xmlChar2string(xmlRaw);
                }
                else if (!xmlStrcmp(cur->name, TAG_MINFILTER))
                {
                    xmlChar* xmlRaw = cur->children->content;
                    sampler2d.mMinFilter = xmlChar2string(xmlRaw);
                }
                else if (!xmlStrcmp(cur->name, TAG_MAGFILTER))
                {
                    xmlChar* xmlRaw = cur->children->content;
                    sampler2d.mMagFilter = xmlChar2string(xmlRaw);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }
        return sampler2d;
    }

    Technique AbsColladaLoader::loadTechnique(xmlNode* node)
    {
        Technique tech;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_PHONG))
                {
                    tech.mPhong = loadPhong(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_LAMBERT))
                {
                    tech.mPhong = loadPhong(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_BLINN))
                {
                    tech.mPhong = loadPhong(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_DOUBLE_SIDED))
                {
                    tech.mIsDoubleSided = loadInt(cur) == 1 ? true : false;
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        return tech;
    }

    Phong AbsColladaLoader::loadPhong(xmlNode* node)
    {
        Phong phong;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_EMISSION))
                {
                    xmlNode* child = cur->children;
                    while (child)
                    {
                        if (child->type == XML_ELEMENT_NODE)
                        {
                            if (!xmlStrcmp(child->name, TAG_COLOR))
                            {
                                phong.mEmission = loadColor(child);
                            }
                            else
                            {
                                LOGW("Unsupported Collada tag: %s\n", child->name);
                            }
                        }
                        child = child->next;
                    }
                }
                else if (!xmlStrcmp(cur->name, TAG_AMBIENT))
                {
                    xmlNode* child = cur->children;
                    while (child)
                    {
                        if (child->type == XML_ELEMENT_NODE)
                        {
                            if (!xmlStrcmp(child->name, TAG_COLOR))
                            {
                                phong.mAmbient = loadColor(child);
                            }
                            else
                            {
                                LOGW("Unsupported Collada tag: %s\n", child->name);
                            }
                        }
                        child = child->next;
                    }
                }
                else if (!xmlStrcmp(cur->name, TAG_DIFFUSE))
                {
                    phong.mDiffuse = loadDiffuse(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_SPECULAR))
                {
                    xmlNode* child = cur->children;
                    while (child)
                    {
                        if (child->type == XML_ELEMENT_NODE)
                        {
                            if (!xmlStrcmp(child->name, TAG_COLOR))
                            {
                                phong.mSpecular = loadColor(child);
                            }
                            else
                            {
                                LOGW("Unsupported Collada tag: %s\n", child->name);
                            }
                        }
                        child = child->next;
                    }
                }
                else if (!xmlStrcmp(cur->name, TAG_SHININESS))
                {
                    xmlNode* child = cur->children;
                    while (child)
                    {
                        if (child->type == XML_ELEMENT_NODE)
                        {
                            if (!xmlStrcmp(child->name, TAG_FLOAT))
                            {
                                float shine = loadFloat(child);
                                if (shine >= 0 && shine < 1)
                                {
                                    // Blinn-Torrance-Sparrow Mode
                                    shine *= 256;
                                }
                                phong.mShininess = Math::Clamp<float>(shine, 0, 128);
                            }
                            else
                            {
                                LOGW("Unsupported Collada tag: %s\n", child->name);
                            }
                        }
                        child = child->next;
                    }
                }
                else if (!xmlStrcmp(cur->name, TAG_TRANSPARENT))
                {
                    xmlNode* child = cur->children;
                    xmlChar* opaque = xmlGetProp(cur, ATTR_OPAQUE);
                    phong.mOpaque = xmlChar2string(opaque);
                    xmlFree(opaque);
                    while (child)
                    {
                        if (child->type == XML_ELEMENT_NODE)
                        {
                            if (!xmlStrcmp(child->name, TAG_COLOR))
                            {
                                phong.mTransparent = loadColor(child);
                            }
                            else
                            {
                                LOGW("Unsupported Collada tag: %s\n", child->name);
                            }
                        }
                        child = child->next;
                    }
                }
                else if (!xmlStrcmp(cur->name, TAG_TRANSPARENCY))
                {
                    xmlNode* child = cur->children;
                    while (child)
                    {
                        if (child->type == XML_ELEMENT_NODE)
                        {
                            if (!xmlStrcmp(child->name, TAG_FLOAT))
                            {
                                phong.mTransparency = loadFloat(child);
                            }
                            else
                            {
                                LOGW("Unsupported Collada tag: %s\n", child->name);
                            }
                        }
                        child = child->next;
                    }
                }
                else if (!xmlStrcmp(cur->name, TAG_REFLECTIVE))
                {
                    xmlNode* child = cur->children;
                    while (child)
                    {
                        if (child->type == XML_ELEMENT_NODE)
                        {
                            if (!xmlStrcmp(child->name, TAG_FLOAT))
                            {
                                phong.mReflectivity = loadFloat(child);
                            }
                            else
                            {
                                LOGW("Unsupported Collada tag: %s\n", child->name);
                            }
                        }
                        child = child->next;
                    }
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        return phong;
    }

    Color AbsColladaLoader::loadColor(xmlNode* node)
    {
        Color color;

        xmlChar* sid = xmlGetProp(node, ATTR_SID);
        color.mSID = xmlChar2string(sid);
        xmlFree(sid);

        xmlChar* xmlRaw = node->children->content;
        char* raw = (char*) xmlRaw;

        if (raw)
        {
            char* saveptr;
            char* p = strtok_r(raw, SPACE, &saveptr);

            if (p)
            {
                color.mR = atof(p);
                p = strtok_r(NULL, SPACE, &saveptr);
            }

            if (p)
            {
                color.mG = atof(p);
                p = strtok_r(NULL, SPACE, &saveptr);
            }

            if (p)
            {
                color.mB = atof(p);
                p = strtok_r(NULL, SPACE, &saveptr);
            }

            // could be only RGB, no alpha value
            if (p)
            {
                color.mA = atof(p);
                strtok_r(NULL, SPACE, &saveptr);
            }
        }

        return color;
    }

    Diffuse AbsColladaLoader::loadDiffuse(xmlNode* node)
    {
        Diffuse diffuse;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_TEXTURE))
                {
                    xmlChar* tex = xmlGetProp(cur, ATTR_TEXTURE);
                    diffuse.mTextureID = xmlChar2string(tex);
                    xmlFree(tex);
                }
                else if (!xmlStrcmp(cur->name, TAG_COLOR))
                {
                    diffuse.mColor = loadColor(cur);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        return diffuse;
    }

#pragma mark Geometry
    bool AbsColladaLoader::loadLibraryGeometries(xmlNode* node)
    {
        xmlNode* geometry = node->children;
        while (geometry)
        {
            if (geometry->type == XML_ELEMENT_NODE)
            {
                assert(!xmlStrcmp(geometry->name, TAG_GEOMETRY));
                loadGeometry(geometry);
            }
            geometry = geometry->next;
            if (mIsCanceled) return false;
        }
        return true;
    }

    bool AbsColladaLoader::loadGeometry(xmlNode* node)
    {
        xmlChar* id = xmlGetProp(node, ATTR_ID);
        string strId = xmlChar2string(id);
        xmlFree(id);
        xmlChar* name = xmlGetProp(node, ATTR_NAME);
        string strName = xmlChar2string(name);
        xmlFree(name);

        xmlNode* child = node->children;
        while (child)
        {
            if (child->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(child->name, TAG_MESH))
                {
                    Mesh* mesh = loadMesh(child);
                    mesh->mID = strId;
                    mesh->setName(strName);
                    mCollada.mGeometryPtrs.push_back(mesh);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", child->name);
                }
            }
            child = child->next;
            if (mIsCanceled) return false;
        }
        return true;
    }

    Mesh* AbsColladaLoader::loadMesh(xmlNode* node)
    {
        Mesh* meshPtr = HouyiNew Mesh();

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_SOURCE))
                {
                    meshPtr->mSourcePtrs.push_back(loadSource(cur));
                }
                else if (!xmlStrcmp(cur->name, TAG_VERTICES))
                {
                    meshPtr->mVertices = loadVertices(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_LINES))
                {
                    Lines* lines = loadLines(cur);
                    meshPtr->mLines.push_back(lines);
                    meshPtr->mPrimitives.push_back(lines);
                }
                else if (!xmlStrcmp(cur->name, TAG_TRIAGNLES))
                {
                    Triangles* triangles = loadTriangles(cur);
                    meshPtr->mTriangles.push_back(triangles);
                    meshPtr->mPrimitives.push_back(triangles);
                }
                else if (!xmlStrcmp(cur->name, TAG_POLYGONS))
                {
                    Polygons* polygons = loadPolygons(cur);
                    meshPtr->mPolygons.push_back(polygons);
                    meshPtr->mPrimitives.push_back(polygons);
                }
                else if (!xmlStrcmp(cur->name, TAG_POLYLIST))
                {
                    Polylist* polylist = loadPolylist(cur);
                    meshPtr->mPolylist.push_back(polylist);
                    meshPtr->mPrimitives.push_back(polylist);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        return meshPtr;
    }

    Vertices AbsColladaLoader::loadVertices(xmlNode* node)
    {
        Vertices vertices;
        xmlChar* dataId = xmlGetProp(node, ATTR_ID);
        vertices.mID = xmlChar2string(dataId);
        xmlFree(dataId);

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_INPUT))
                {
                    vertices.mInputs.push_back(loadInput(cur));
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }

            cur = cur->next;
        }

        return vertices;
    }

    Lines* AbsColladaLoader::loadLines(xmlNode* node)
    {
        Lines* lines = HouyiNew Lines();

        xmlChar* material = xmlGetProp(node, ATTR_MATERIAL);
        lines->mMaterialStr = xmlChar2string(material);
        xmlFree(material);

        // this cnt is the number of lines
        xmlChar* count = xmlGetProp(node, ATTR_COUNT);
        assert(count);
        unsigned int cnt = atoi((const char*) count);
        xmlFree(count);

        lines->mCount = cnt;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_INPUT))
                {
                    lines->mInputs.push_back(loadInput(cur));
                }
                else if (!xmlStrcmp(cur->name, TAG_P))
                {
                    xmlChar* xmlRaw = cur->children->content;
                    char* raw = (char*) xmlRaw;
                    char* saveptr;
                    char* p = strtok_r(raw, SPACE, &saveptr);
                    lines->mPPtr = new P();
                    int pcnt = 2 * cnt * lines->mInputs.size();
                    lines->mPPtr->mCount = pcnt;
                    lines->mPPtr->mIndices = new unsigned int[pcnt];
                    int i = 0;
                    while (p)
                    {
                        lines->mPPtr->mIndices[i] = atoi(p);
                        p = strtok_r(NULL, SPACE, &saveptr);
                        i++;
                    }
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        mCollada.mStats.mNbLine++;
        return lines;
    }

    Triangles* AbsColladaLoader::loadTriangles(xmlNode* node)
    {
        Triangles* trianglesPtr = HouyiNew Triangles();

        xmlChar* material = xmlGetProp(node, ATTR_MATERIAL);
        trianglesPtr->mMaterialStr = xmlChar2string(material);
        xmlFree(material);

        // this cnt is the number of triangles
        xmlChar* count = xmlGetProp(node, ATTR_COUNT);
        assert(count);
        unsigned int cnt = atoi((const char*) count);
        xmlFree(count);

        trianglesPtr->mCount = cnt;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_INPUT))
                {
                    trianglesPtr->mInputs.push_back(loadInput(cur));
                }
                else if (!xmlStrcmp(cur->name, TAG_P))
                {
                    xmlChar* xmlRaw = cur->children->content;
                    char* raw = (char*) xmlRaw;
                    char* saveptr;
                    char* p = strtok_r(raw, SPACE, &saveptr);
                    trianglesPtr->mPPtr = new P();
                    int pcnt = 3 * cnt * trianglesPtr->mInputs.size();
                    trianglesPtr->mPPtr->mCount = pcnt;
                    trianglesPtr->mPPtr->mIndices = new unsigned int[pcnt];
                    int i = 0;
                    while (p)
                    {
                        trianglesPtr->mPPtr->mIndices[i] = atoi(p);
                        p = strtok_r(NULL, SPACE, &saveptr);
                        i++;
                    }
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        mCollada.mStats.mNbTriangle++;
        return trianglesPtr;
    }

    Polygons* AbsColladaLoader::loadPolygons(xmlNode* node)
    {
        Polygons* polyPtr = HouyiNew Polygons();

        xmlChar* material = xmlGetProp(node, ATTR_MATERIAL);
        polyPtr->mMaterialStr = xmlChar2string(material);
        xmlFree(material);

        xmlChar* count = xmlGetProp(node, ATTR_COUNT);
        assert(count);
        // this cnt is the number of polygons
        unsigned int cnt = atoi((const char*) count);
        xmlFree(count);

        polyPtr->mCount = cnt;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_INPUT))
                {
                    polyPtr->mInputs.push_back(loadInput(cur));
                }
                else if (!xmlStrcmp(cur->name, TAG_P))
                {
                    xmlChar* xmlRaw = cur->children->content;
                    char* raw = (char*) xmlRaw;
                    char* saveptr;
                    char* p = strtok_r(raw, SPACE, &saveptr);
                    PPtr primitive = new P();
                    vector<float> indices;
                    int i = 0;
                    while (p)
                    {
                        indices.push_back(atoi(p));
                        p = strtok_r(NULL, SPACE, &saveptr);
                        i++;
                    }
                    int cnt = indices.size();
                    primitive->mIndices = new unsigned int[cnt];
                    for (int i = 0;i < cnt;++i)
                    {
                        primitive->mIndices[i] = indices[i];
                    }
                    primitive->mCount = cnt;
                    polyPtr->mPPtrs.push_back(primitive);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        mCollada.mStats.mNbPolygon++;
        return polyPtr;
    }

    Polylist* AbsColladaLoader::loadPolylist(xmlNode* node)
    {
        Polylist* polyPtr = HouyiNew Polylist();

        xmlChar* material = xmlGetProp(node, ATTR_MATERIAL);
        polyPtr->mMaterialStr = xmlChar2string(material);
        xmlFree(material);

        xmlChar* count = xmlGetProp(node, ATTR_COUNT);
        assert(count);
        // this cnt is the number of polygons
        unsigned int cnt = atoi((const char*) count);
        xmlFree(count);
        polyPtr->mCount = cnt;
        if (cnt <= 0)
        {
            // mal-format dae file
            return polyPtr;
        }

        unsigned int expandedVCnt = 0;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_INPUT))
                {
                    polyPtr->mInputs.push_back(loadInput(cur));
                }
                else if (!xmlStrcmp(cur->name, TAG_VCOUNT))
                {
                    polyPtr->mVCountPtr = new VCount(polyPtr->mCount);
                    xmlChar* xmlRaw = cur->children->content;
                    char* raw = (char*) xmlRaw;
                    char* saveptr;
                    char* p = strtok_r(raw, SPACE, &saveptr);
                    expandedVCnt = 0;
                    int i = 0;
                    while (p)
                    {
                        int v = atoi(p);
                        expandedVCnt += v;
                        polyPtr->mVCountPtr->mIndices[i] = v;
                        p = strtok_r(NULL, SPACE, &saveptr);
                        i++;
                    }
                }
                else if (!xmlStrcmp(cur->name, TAG_P))
                {
                    xmlChar* xmlRaw = cur->children->content;
                    char* raw = (char*) xmlRaw;
                    char* saveptr;
                    char* p = (strtok_r(raw,SPACE, &saveptr));
                    polyPtr->mPPtr = new P();
                    int pcnt = expandedVCnt * polyPtr->mInputs.size();
                    polyPtr->mPPtr->mCount = pcnt;
                    polyPtr->mPPtr->mIndices = new unsigned int[pcnt];
                    memset(polyPtr->mPPtr->mIndices, 0, pcnt * sizeof(unsigned int));
                    int i = 0;
                    while (p)
                    {
                        polyPtr->mPPtr->mIndices[i] = atoi(p);
                        p = (strtok_r(NULL, SPACE, &saveptr));
                        i++;
                    }
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        mCollada.mStats.mNbPolylist++;
        return polyPtr;
    }

#pragma mark Lights
    void AbsColladaLoader::loadLibraryLights(xmlNode* node)
    {
        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                assert(!xmlStrcmp(cur->name, TAG_LIGHT));
                loadLight(cur);
            }
            cur = cur->next;
        }
    }

    void AbsColladaLoader::loadLight(xmlNode* node)
    {
        Light* light = HouyiNew Light();

        xmlChar* id = xmlGetProp(node, ATTR_ID);
        light->mID = xmlChar2string(id);
        xmlFree(id);

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_TECHNIQUE_COMMON))
                {
                    TechniqueCommon tc = loadTechniqueCommon(cur);
                    light->mTechniqueCommon = tc;
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        mCollada.mLights.push_back(light);
    }

    Point AbsColladaLoader::loadPoint(xmlNode* node)
    {
        Point point;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_COLOR))
                {
                    point.mColor = loadColor(cur);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        return point;
    }

#pragma mark Cameras
    void AbsColladaLoader::loadLibraryCameras(xmlNode* node)
    {
        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                assert(!xmlStrcmp(cur->name, TAG_CAMERA));
                loadCamera(cur);
            }
            cur = cur->next;
        }
    }

    void AbsColladaLoader::loadCamera(xmlNode* node)
    {
        Camera camera;

        xmlChar* id = xmlGetProp(node, ATTR_ID);
        camera.mID = xmlChar2string(id);
        xmlFree(id);

        xmlChar* name = xmlGetProp(node, ATTR_NAME);
        camera.mName = xmlChar2string(name);
        xmlFree(name);
        
        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_OPTICS))
                {
                    Optics optics = loadOptics(cur);
                    camera.mOptics = optics;
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        mCollada.mCameras.push_back(camera);
    }

    Optics AbsColladaLoader::loadOptics(xmlNode* node)
    {
        Optics optics;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_TECHNIQUE_COMMON))
                {
                    TechniqueCommon tc = loadTechniqueCommon(cur);
                    optics.mTechniqueCommon = tc;
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        return optics;
    }

    Perspective AbsColladaLoader::loadPerspective(xmlNode* node)
    {
        Perspective persp;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_XFOV))
                {
                    persp.mXFOV = loadFloat(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_YFOV))
                {
                    // support only one FOV for now
                    persp.mXFOV = loadFloat(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_ASPECT_RATIO))
                {
                    persp.mAspectRatio = loadFloat(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_NEAR))
                {
                    persp.mNear = loadFloat(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_FAR))
                {
                    persp.mFar = loadFloat(cur);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        return persp;
    }

#pragma mark Controllers
    void AbsColladaLoader::loadLibraryControllers(xmlNode* node)
    {
        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                assert(!xmlStrcmp(cur->name, TAG_CONTROLLER));
                loadController(cur);
            }
            cur = cur->next;
        }
    }

    void AbsColladaLoader::loadController(xmlNode* node)
    {
        Controller controller;

        xmlChar* id = xmlGetProp(node, ATTR_ID);
        controller.mID = xmlChar2string(id);
        xmlFree(id);

        xmlNode* child = node->children;
        while (child)
        {
            if (child->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(child->name, TAG_SKIN))
                {
                    SkinPtr skinPtr = loadSkin(child);
                    controller.mSkinPtrs.push_back(skinPtr);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", child->name);
                }
            }
            child = child->next;
        }

        mCollada.mControllers.push_back(controller);
    }

    SkinPtr AbsColladaLoader::loadSkin(xmlNode* node)
    {
        SkinPtr skinPtr = new Skin();

        xmlChar* xmlRaw = xmlGetProp(node, ATTR_SOURCE);
        string srcGeometry = xmlChar2string(xmlRaw).substr(1);
        xmlFree(xmlRaw);
        for (int i = 0; i < mCollada.mGeometryPtrs.size(); ++i)
        {
            Geometry* geoPtr = mCollada.mGeometryPtrs.at(i);
            if (srcGeometry.compare(geoPtr->mID) == 0)
            {
                skinPtr->mGeometry = geoPtr;
                break;
            }
        }

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_BIND_SHAPE_MATRIX))
                {
                    skinPtr->mBindShapeMatrix = loadMatrix(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_SOURCE))
                {
                    skinPtr->mSourcePtrs.push_back(loadSource(cur));
                }
                else if (!xmlStrcmp(cur->name, TAG_JOINTS))
                {
                    xmlNode* nodeJ = cur->children;
                    while (nodeJ)
                    {
                        if (nodeJ->type == XML_ELEMENT_NODE)
                        {
                            if (!xmlStrcmp(nodeJ->name, TAG_INPUT))
                            {
                                xmlChar* semantic = xmlGetProp(nodeJ, ATTR_SEMANTIC);
                                string semanticStr = xmlChar2string(semantic);
                                xmlFree(semantic);
                                if (semanticStr.compare("JOINT") == 0)
                                {
                                    skinPtr->mJoints = loadInput(nodeJ);
                                }
                                else if (semanticStr.compare("INV_BIND_MATRIX") == 0)
                                {
                                    skinPtr->mInvBindMatrix = loadInput(nodeJ);
                                }
                            }
                        }
                        nodeJ = nodeJ->next;
                    }
                }
                else if (!xmlStrcmp(cur->name, TAG_VERTEX_WEIGHTS))
                {
                    skinPtr->mVertexWeightPtr = loadVertexWeight(cur);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        return skinPtr;
    }

    VertexWeightPtr AbsColladaLoader::loadVertexWeight(xmlNode* node)
    {
        VertexWeightPtr vwPtr = new VertexWeight();
        xmlChar* count = xmlGetProp(node, ATTR_COUNT);
        vwPtr->mCount = atoi(xmlChar2string(count).c_str());
        xmlFree(count);

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_INPUT))
                {
                    Input input = loadInput(cur);
                    if (input.mSemantic.compare("JOINT") == 0)
                    {
                        vwPtr->mJoint = input;
                    }
                    else if (input.mSemantic.compare("WEIGHT") == 0)
                    {
                        vwPtr->mWeight = input;
                    }
                }
                else if (!xmlStrcmp(cur->name, TAG_VCOUNT))
                {
                    vwPtr->mVCountLength = vwPtr->mCount;
                    vwPtr->mVCount = new unsigned int[vwPtr->mVCountLength];
                    xmlChar* xmlRaw = cur->children->content;
                    char* raw = (char*) xmlRaw;
                    char* saveptr;
                    char* p = strtok_r(raw, SPACE, &saveptr);
                    int i = 0;
                    while (p)
                    {
                        vwPtr->mVCount[i] = atoi(p);
                        p = strtok_r(NULL, SPACE, &saveptr);
                        i++;
                    }
                }
                else if (!xmlStrcmp(cur->name, TAG_V))
                {
                    // find length first
                    int length = 0;
                    for (int i = 0; i < vwPtr->mVCountLength; ++i)
                    {
                        length += vwPtr->mVCount[i];
                    }

                    vwPtr->mVLength = length * 2;
                    vwPtr->mV = new unsigned int[vwPtr->mVLength];
                    xmlChar* xmlRaw = cur->children->content;
                    char* raw = (char*) xmlRaw;
                    char* saveptr;
                    char* p = strtok_r(raw, SPACE, &saveptr);
                    int i = 0;
                    while (p)
                    {
                        vwPtr->mV[i] = atoi(p);
                        p = strtok_r(NULL, SPACE, &saveptr);
                        i++;
                    }
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }

            cur = cur->next;
        }

        return vwPtr;
    }

#pragma mark Animation
    void AbsColladaLoader::loadLibraryAnimations(xmlNode* node)
    {
        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                assert(!xmlStrcmp(cur->name, TAG_ANIMATION));
                loadAnimationGroup(cur);
            }
            cur = cur->next;
        }
    }

    void AbsColladaLoader::loadAnimationGroup(xmlNode* node)
    {
        AnimationGroup animationGroup;

        xmlChar* id = xmlGetProp(node, ATTR_ID);
        animationGroup.mID = xmlChar2string(id);
        xmlFree(id);

        xmlChar* name = xmlGetProp(node, ATTR_NAME);
        animationGroup.mName = xmlChar2string(name);
        xmlFree(name);

        Animation animation;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_ANIMATION))
                {
                    Animation animation = loadAnimation(cur);
                    animationGroup.mAnimations.push_back(animation);
                }
                else if (!xmlStrcmp(cur->name, TAG_SOURCE))
                {
                    SourcePtr srcPtr = loadSource(cur);
                    animation.mSources.push_back(srcPtr);
                }
                else if (!xmlStrcmp(cur->name, TAG_SAMPLER))
                {
                    animation.mSampler = loadSampler(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_CHANNEL))
                {
                    xmlChar* src = xmlGetProp(cur, ATTR_SOURCE);
                    animation.mChannel.mSource = xmlChar2string(src);
                    xmlFree(src);

                    xmlChar* target = xmlGetProp(cur, ATTR_TARGET);
                    animation.mChannel.mTarget = xmlChar2string(target);
                    xmlFree(target);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        // organize data
        Sampler& sampler = animation.mSampler;
        for (int i = 0;i < animation.mSources.size();++i)
        {
            SourcePtr srcPtr = animation.mSources[i];
            if (sampler.mInput.mSource.length() > 0
                && srcPtr->mID.compare(sampler.mInput.mSource.substr(1)) == 0)
            {
                animation.mInputPtr = srcPtr;
            }
            else if (sampler.mOutput.mSource.length() > 0
                     && srcPtr->mID.compare(sampler.mOutput.mSource.substr(1)) == 0)
            {
                animation.mOutputPtr = srcPtr;
            }
            else if (sampler.mInterpolation.mSource.length() > 0
                     && srcPtr->mID.compare(sampler.mInterpolation.mSource.substr(1)) == 0)
            {
                animation.mInterpolationPtr = srcPtr;
            }
            else if (sampler.mInTangent.mSource.length() > 0
                     && srcPtr->mID.compare(sampler.mInTangent.mSource.substr(1)) == 0)
            {
                animation.mInTangent = srcPtr;
            }
            else if (sampler.mOutTangent.mSource.length() > 0
                     && srcPtr->mID.compare(sampler.mOutTangent.mSource.substr(1)) == 0)
            {
                animation.mOutTangent = srcPtr;
            }
        }

        if (animation.mInputPtr)
        {
            animationGroup.mAnimations.push_back(animation);
        }
        mCollada.mAnimationGroups.push_back(animationGroup);
    }

    Animation AbsColladaLoader::loadAnimation(xmlNode* node)
    {
        Animation animation;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_SOURCE))
                {
                    SourcePtr srcPtr = loadSource(cur);
                    if (srcPtr->mID.find("input") != srcPtr->mID.npos)
                    {
                        animation.mInputPtr = srcPtr;
                    }
                    else if (srcPtr->mID.find("output") != srcPtr->mID.npos)
                    {
                        animation.mOutputPtr = srcPtr;
                    }
                    else if (srcPtr->mID.find("Interpolation") != srcPtr->mID.npos)
                    {
                        animation.mInterpolationPtr = srcPtr;
                    }
                }
                else if (!xmlStrcmp(cur->name, TAG_SAMPLER))
                {
                    animation.mSampler = loadSampler(cur);
                }
                else if (!xmlStrcmp(cur->name, TAG_CHANNEL))
                {
                    xmlChar* src = xmlGetProp(cur, ATTR_SOURCE);
                    animation.mChannel.mSource = xmlChar2string(src);
                    xmlFree(src);

                    xmlChar* target = xmlGetProp(cur, ATTR_TARGET);
                    animation.mChannel.mTarget = xmlChar2string(target);
                    xmlFree(target);
                }
                else
                {
                    LOGW("Unsupported Collada tag: %s\n", cur->name);
                }
            }
            cur = cur->next;
        }

        return animation;
    }

    Sampler AbsColladaLoader::loadSampler(xmlNode* node)
    {
        Sampler sampler;

        xmlNode* cur = node->children;
        while (cur)
        {
            if (cur->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur->name, TAG_INPUT))
                {
                    Input input = loadInput(cur);
                    if (input.mSemantic.compare("INPUT") == 0)
                    {
                        sampler.mInput = input;
                    }
                    else if (input.mSemantic.compare("OUTPUT") == 0)
                    {
                        sampler.mOutput = input;
                    }
                    else if (input.mSemantic.compare("INTERPOLATION") == 0)
                    {
                        sampler.mInterpolation = input;
                    }
                    else if (input.mSemantic.compare("IN_TANGENT") == 0)
                    {
                        sampler.mInTangent = input;
                    }
                    else if (input.mSemantic.compare("OUT_TANGENT") == 0)
                    {
                        sampler.mOutTangent = input;
                    }
                }
            }

            cur = cur->next;
        }

        return sampler;
    }

#pragma mark Utils
    string AbsColladaLoader::xmlChar2string(xmlChar* a_xmlChar)
    {
        if (!a_xmlChar)
        {
            return "";
        }
        // TODO FIX ME UTF-8
        return (char*) a_xmlChar;
    }

    bool AbsColladaLoader::isLightNode(xmlNode* node)
    {
        xmlNode* child = node->children;
        while (child)
        {
            if (child->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(child->name, TAG_INSTANCE_LIGHT))
                {
                    xmlChar* url = xmlGetProp(child, ATTR_URL);
                    string urlStr = xmlChar2string(url);
                    xmlFree(url);
                    for (int i = 0; i < mCollada.mLights.size(); ++i)
                    {
                        Light* light = mCollada.mLights.at(i);
                        if (urlStr.substr(1).compare(light->mID) == 0)
                        {
                            return true;
                        }
                    }
                }
            }
            child = child->next;
        }

        return false;
    }

    bool AbsColladaLoader::isCameraNode(xmlNode* node)
    {
        xmlNode* child = node->children;
        while (child)
        {
            if (child->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(child->name, TAG_INSTANCE_CAMERA))
                {
                    xmlChar* url = xmlGetProp(child, ATTR_URL);
                    string urlStr = xmlChar2string(url);
                    xmlFree(url);
                    for (int i = 0; i < mCollada.mCameras.size(); ++i)
                    {
                        Camera cam = mCollada.mCameras.at(i);
                        if (urlStr.substr(1).compare(cam.mID) == 0)
                        {
                            return true;
                        }
                    }
                }
            }
            child = child->next;
        }

        return false;
    }

    bool AbsColladaLoader::isCameraTargetNode(xmlNode* node)
    {
        xmlChar* name = xmlGetProp(node, ATTR_NAME);
        string nameStr = xmlChar2string(name);
        xmlFree(name);
        if (nameStr.find(".target") != string::npos)
        {
            return true;
        }

        return false;
    }

    float* AbsColladaLoader::copyFloatArray(const FloatArray* fa)
    {
        float* res = new float[fa->mCount];
        for (int i = 0; i < fa->mCount; ++i)
        {
            res[i] = fa->mFloats[i];
        }

        return res;
    }

    SceneNode* AbsColladaLoader::findNodeByID(const string nodeID)
    {
        for (int i = 0; i < mCollada.mNodes.size(); ++i)
        {
            SceneNode* cur = mCollada.mNodes.at(i);
            SceneNode* res = findNodeByID(cur, nodeID);
            if (res)
            {
                return res;
            }
        }

        for (int i = 0; i < mCollada.mVisualScenes.size(); ++i)
        {
            VisualScene vs = mCollada.mVisualScenes.at(i);

            for (int j = 0; j < vs.mSceneNodePtrs.size(); ++j)
            {
                SceneNode* cur = vs.mSceneNodePtrs.at(j);
                SceneNode* res = findNodeByID(cur, nodeID);
                if (res)
                {
                    return res;
                }
            }
        }

        return NULL;
    }

    SceneNode* AbsColladaLoader::findNodeByID(SceneNode* rootPtr, const string nodeID)
    {
        if (!rootPtr || nodeID.length() <= 0)
        {
            return NULL;
        }

        SceneNode* cur = rootPtr;
        if (cur->mID.compare(nodeID) == 0 || cur->mSID.compare(nodeID) == 0)
        {
            return cur;
        }

        for (int i = 0; i < cur->mChildren.size(); ++i)
        {
            SceneNode* found = findNodeByID(cur->mChildren.at(i), nodeID);
            if (found)
            {
                return found;
            }
        }

        return NULL;
    }

    JointNode* AbsColladaLoader::findJointNodeByID(const string nodeID)
    {
        for (int i = 0; i < mCollada.mVisualScenes.size(); ++i)
        {
            VisualScene vs = mCollada.mVisualScenes.at(i);

            for (int j = 0; j < vs.mJointRootPtrs.size(); ++j)
            {
                JointNode* rootPtr = vs.mJointRootPtrs.at(j);
                JointNode* jnPtr = findJointNodeByID(rootPtr, nodeID);
                if (jnPtr)
                {
                    return jnPtr;
                }
            }
        }

        return NULL;
    }

    JointNode* AbsColladaLoader::findJointNodeByID(JointNode* rootPtr, const string nodeID)
    {
        if (!rootPtr || nodeID.length() <= 0)
        {
            return NULL;
        }

        JointNode* cur = rootPtr;
        if (cur->mID.compare(nodeID) == 0 || cur->mSID.compare(nodeID) == 0)
        {
            return cur;
        }

        for (int i = 0; i < cur->mChildren.size(); ++i)
        {
            JointNode* found = findJointNodeByID(cur->mChildren.at(i), nodeID);
            if (found)
            {
                return found;
            }
        }

        return NULL;
    }

    bool AbsColladaLoader::findTransformBySID(JointNode* nodePtr, const string sid, Transform& res)
    {
        string splitSID = sid;
        int pos = sid.find('.');
        if (pos != string::npos)
        {
            splitSID = sid.substr(0, pos);
        }
        for (int i = 0, n = nodePtr->mTransforms.size();i < n;++i)
        {
            Transform trans = nodePtr->mTransforms[i];
            if (trans.mSID.length() == 0)
            {
                LOGW("findTransformBySID trans sid is null. node = %s", nodePtr->getName().c_str());
                continue;
            }
            if (trans.mSID.compare(splitSID) == 0)
            {
                res = trans;
                return true;
            }
        }
        return false;
    }

    int AbsColladaLoader::findIndexInSkinTable(const SkinPtr& skinPtr, const JointNode* node)
    {
        for (int i = 0; i < skinPtr->mJNCount; ++i)
        {
            JointNode* jn = skinPtr->mJNPtrTable[i];
            if (jn->mID.compare(node->mID) == 0)
            {
                return i;
            }
        }
        return -1;
    }

    void AbsColladaLoader::prepareMaterial()
    {
        for (int i = 0; i < mCollada.mEffects.size(); ++i)
        {
            Effect effect = mCollada.mEffects.at(i);
            string texname = effect.mProfile.mTechnique.mPhong.mDiffuse.mTextureID;
            if (texname.length() > 0)
            {
                for (int j = 0; j < mCollada.mImages.size(); ++j)
                {
                    Image image = mCollada.mImages.at(j);
                    if (texname.compare(image.mID) == 0)
                    {
                        mCollada.mEffects.at(i).mProfile.mTechnique.mPhong.mDiffuse.mTextureID = image.mInitFrom;
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < mCollada.mMaterials.size(); ++i)
        {
            Material& material = mCollada.mMaterials.at(i);
            for (int j = 0; j < mCollada.mEffects.size(); ++j)
            {
                Effect effect = mCollada.mEffects.at(j);
                if (material.mEffectURL.substr(1).compare(effect.mID) == 0)
                {
                    mCollada.mMaterials.at(i).mEffect = effect;
                    break;
                }
            }
        }
        
        for (int g = 0; g < mCollada.mGeometryPtrs.size(); ++g)
        {
            Mesh* mesh = (Mesh*)(mCollada.mGeometryPtrs[g]);
            for (int p = 0;p < mesh->mPrimitives.size();++p)
            {
                Primitive* primitive = mesh->mPrimitives[p];
                string matName = primitive->mMaterialStr;
                string bindTargetID;
                map<string, string>::iterator it = mMaterialSymbolMap.find(matName);
                if (it != mMaterialSymbolMap.end())
                {
                    bindTargetID = it->second;
                    if (bindTargetID.length() > 0)
                    {
                        bindTargetID = bindTargetID.substr(1);
                    }
                }
                
                if (matName.length() > 0)
                {
                    bool found = false;
                    for (int i = 0;i < mCollada.mMaterials.size(); ++i)
                    {
                        Material material = mCollada.mMaterials.at(i);
                        if (material.mID.compare(matName) == 0
                                || material.mID.compare(bindTargetID) == 0
                                || material.mName.compare(matName) == 0)
                        {
                            primitive->mMaterial = material;
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        LOGW("Cannot find material for triangle, name = %s\n", matName.c_str());
                    }
                }
            }
        }
    }

    void AbsColladaLoader::prepareLights()
    {
        for (int v = 0; v < mCollada.mVisualScenes.size(); ++v)
        {
            VisualScene vs = mCollada.mVisualScenes.at(v);

            for (int l = 0; l < vs.mLights.size(); ++l)
            {
                SceneNode* light = vs.mLights.at(l);

                Matrix trans = light->getMatrix();
                SceneNode* parent = light->mParentPtr;
                while (parent)
                {
                    Matrix pm = parent->getMatrix();
                    trans = trans * pm;
                    parent = parent->mParentPtr;
                }
                light->setMatrix(trans);
            }
        }
    }

    void AbsColladaLoader::prepareNode()
    {
        // find instance node which might contains transform data
        for (int i = 0; i < mCollada.mInstanceNodePtrs.size(); ++i)
        {
            SceneNode* insNodePtr = mCollada.mInstanceNodePtrs.at(i);
            SceneNode* node = findNodeByID(insNodePtr->mURL);
            if (node)
            {
                insNodePtr->mChildren.push_back(node);
            }
            else
            {
                LOGW("Cannot find node for instance node. URL = %s\n", insNodePtr->mURL.c_str());
            }
        }
    }

    void AbsColladaLoader::prepareGeometry()
    {
        mCollada.mStats.mNodeHasGeoData = 0;
        for (int v = 0; v < mCollada.mVisualScenes.size(); ++v)
        {
            VisualScene vs = mCollada.mVisualScenes.at(v);

            for (int g = 0; g < vs.mSceneNodePtrs.size(); ++g)
            {
                SceneNode* gnPtr = vs.mSceneNodePtrs.at(g);
                findGeoData(gnPtr);

                // find controller data
                for (int i = 0; i < mCollada.mControllers.size(); ++i)
                {
                    Controller controller = mCollada.mControllers.at(i);
                    if (gnPtr->mController.mURL.length() > 0
                            && gnPtr->mController.mURL.substr(1).compare(controller.mID) == 0)
                    {
                        // copy controller data to this node
                        gnPtr->mController.mID = controller.mID;
                        for (int j = 0; j < controller.mSkinPtrs.size(); ++j)
                        {
                            SkinPtr skinPtr = controller.mSkinPtrs.at(j);
                            gnPtr->mController.mSkinPtrs.push_back(skinPtr);
                        }
                    }
                }
            }
        }
    }

    void AbsColladaLoader::findGeoData(SceneNode* gnPtr)
    {
        // find geometry data
        for (int g = 0; g < gnPtr->mGeoInstance.size(); ++g)
        {
            bool found = false;
            InstanceGeometry& instance = gnPtr->mGeoInstance[g];
            for (int i = 0; i < mCollada.mGeometryPtrs.size(); ++i)
            {
                Geometry* gPtr = mCollada.mGeometryPtrs.at(i);
                if ((instance.mGeometry.mURL.length() > 0 && instance.mGeometry.mURL.substr(1).compare(gPtr->mID) == 0)
                    || (gnPtr->mSID.length() > 0 && gPtr->mID.length() > 0 && gnPtr->mSID.compare(gPtr->mID) == 0))
                {
                    found = true;
                    instance.mGeoData = gPtr;
                    mCollada.mStats.mNodeHasGeoData++;
                    if (mLogLevel == EFull)
                    {
                        LOGD("findGeoData [SceneNode = %s, Geo data = %s]\n", gnPtr->getName().c_str(), gPtr->mID.c_str());
                    }
                    break;
                }
            }

            if (!found && instance.mGeometry.mURL.length() > 0)
            {
                LOGW("cannot find geo data for scene node: %s, Geo data url = %s]\n",
                        gnPtr->getName().c_str(), instance.mGeometry.mURL.c_str());
            }
        }

        for (int i = 0; i < gnPtr->mChildren.size(); ++i)
        {
            SceneNode* child = gnPtr->mChildren.at(i);
            findGeoData(child);
        }
    }

    void AbsColladaLoader::updateJointsAni()
    {
        for (int i = 0; i < mCollada.mAnimationGroups.size(); ++i)
        {
            AnimationGroup ag = mCollada.mAnimationGroups.at(i);
            for (int a = 0, n = ag.mAnimations.size(); a < n; ++a)
            {
                Animation& anim = ag.mAnimations[a];
                string targetID = anim.mChannel.getTargetID();

                JointNode* jnPtr;
                for (int j = 0; j < mCollada.mVisualScenes.size(); ++j)
                {
                    VisualScene scene = mCollada.mVisualScenes.at(j);
                    for (int k = 0; k < scene.mJointRootPtrs.size(); ++k)
                    {
                        jnPtr = findJointNodeByID(scene.mJointRootPtrs.at(k), targetID);

                        if (jnPtr)
                        {
                            updateJointsAni(anim, jnPtr);
                            jnPtr->mAnimSet = true;
                        }
                    }
                }
            }
        }
        
        for (int j = 0; j < mCollada.mVisualScenes.size(); ++j)
        {
            VisualScene scene = mCollada.mVisualScenes.at(j);
            for (int k = 0; k < scene.mJointRootPtrs.size(); ++k)
            {
                JointNode* jp = scene.mJointRootPtrs.at(k);
                if (!jp->mAnimSet)
                {
                    LOGW("Joint (name = %s) animation not set, NULL transform or bad collada file?\n", jp->getName().c_str());
                }
            }
        }
    }

    void AbsColladaLoader::updateJointsAni(Animation& animation, JointNode* nodePtr)
    {
        SourcePtr timeData = animation.mInputPtr;
        SourcePtr transformData = animation.mOutputPtr;
        SourcePtr interpolationData = animation.mInterpolationPtr;
        SourcePtr intangentData = animation.mInTangent;
        SourcePtr outtangentData = animation.mOutTangent;

        int kfCnt = timeData->mFloatArrayPtr->mCount;
        if (kfCnt == 0)
        {
            LOGI("dummy animation data, node = %s", nodePtr->getName().c_str());
            return;
        }

        int transformDataLength = transformData->mFloatArrayPtr->mCount;

        string sid = animation.mChannel.getTargetSID();
        int pos = sid.find('.');
        string subType; // X, Y or Z
        if (pos != string::npos)
        {
            string type = sid.substr(0, pos);
            subType = sid.substr(pos + 1, sid.length());
            sid = type;
        }
        
        // this checks if our animation is
        // the type of transform like transform(0)(0)
        // not support by now
        bool replaceTransType = false;
        int rowIdx = 0;
        int colIdx = 0;
        /*pos = sid.find('(');
        if (pos != string::npos)
        {
            replaceTransType = true;
            string idxStr = sid.substr(pos, sid.length() - pos);
            sid = sid.substr(0, pos);
            int rowEndPos = idxStr.find(')');
            string rowStr = idxStr.substr(1, rowEndPos-1);
            rowIdx = atoi(rowStr.c_str());
            
            int lastFront = idxStr.find_last_of('(');
            int lastEnd = idxStr.find_last_of(')');
            string colStr = idxStr.substr(lastFront+1, lastEnd-lastFront-1);
            colIdx = atoi(colStr.c_str());
        }*/
        
        Transform transform;
        bool found = findTransformBySID(nodePtr, sid, transform);
        if (!found)
        {
            // either for unsupported transform or NULL transform
            LOGI("updateJointsAni. cannot find transform. sid = %s\n", sid.c_str());
            return;
        }

        if (kfCnt > nodePtr->mKFCount)
        {
            nodePtr->mKFCount = kfCnt;
            // TODO if count is not the same for each transform
            // we should copy&delete previous key frame
            nodePtr->mKeyFrames = new KeyFrame[nodePtr->mKFCount];
            for (int i = 0; i < nodePtr->mKFCount; ++i)
            {
                nodePtr->mKeyFrames[i].mTime = timeData->mFloatArrayPtr->mFloats[i];

                if (interpolationData)
                {
                    string& inter = interpolationData->mNameArrayPtr->mNames[i];
                    if (inter.compare("LINEAR") == 0)
                    {
                        nodePtr->mKeyFrames[i].mInterpolation = Transform::ELinearInterpolation;
                    }
                    else if (inter.compare("BEZIER") == 0)
                    {
                        nodePtr->mKeyFrames[i].mInterpolation = Transform::EBezierInterpolation;
                    }
                }
            }
        }

        unsigned int stride = transformData->mTechniqueCommon.mAccessor.mStride;
        float* input = transformData->mFloatArrayPtr->mFloats;
        if (stride <= 0 || stride == 16 || transformDataLength / kfCnt == 16)
        {
            // baked matrix
            Transform bindPos(nodePtr->getMatrix().getData());
            for (int i = 0; i < kfCnt; ++i)
            {
                Transform trans;
                trans.mType = Transform::EMatrix;
                trans.getMatrix() = Matrix((input + i * 16));
                nodePtr->mKeyFrames[i].addTransform(trans);
            }
        }
        else
        {
            for (int i = 0; i < kfCnt; ++i)
            {
                // firstly we add bind pose transforms, later we will replace
                if (nodePtr->mKeyFrames[i].mTransforms.size() == 0)
                {
                    for (int k = 0, n = nodePtr->mTransforms.size();k < n;++k)
                    {
                        nodePtr->mKeyFrames[i].addTransform(nodePtr->mTransforms[k]);
                    }
                }
            }
            
            for (int i = 0, j = 0; i < kfCnt; ++i, j += stride)
            {
                // if animation data sid matches bind pos sid replace transform element
                for (int k = 0, n = nodePtr->mKeyFrames[i].mTransforms.size();k < n;++k)
                {
                    Transform& animTrans = nodePtr->mKeyFrames[i].mTransforms[k];
                    if (animTrans.mSID.compare(sid) == 0)
                    {
                        animTrans.mInterpolation = nodePtr->mKeyFrames[i].mInterpolation;
                        if (animTrans.mType == Transform::ETranslate
                                || animTrans.mType == Transform::EScale)
                        {
                            int len = subType.length();
                            if (len > 0)
                            {
                                if (subType.compare("X") == 0)
                                {
                                    animTrans.mElements[0] = input[j];
                                }
                                else if (subType.compare("Y") == 0)
                                {
                                    animTrans.mElements[1] = input[j];
                                }
                                else if (subType.compare("Z") == 0)
                                {
                                    animTrans.mElements[2] = input[j];
                                }
                            }
                            else
                            {
                                animTrans.mElements[0] = input[j];
                                animTrans.mElements[1] = input[j + 1];
                                animTrans.mElements[2] = input[j + 2];
                            }
                            animTrans.generateMatrix();
                        }
                        else if (animTrans.mType == Transform::ERotate)
                        {
                            animTrans.mElements[3] = input[j];
                            animTrans.generateMatrix();
                        }
                        else if (replaceTransType)
                        {
                            animTrans.getMatrix().getData()[colIdx*4+rowIdx] = input[j];
                        }
                        else
                        {
                            memcpy(animTrans.getMatrix().getData(), &input[j * 16], 16 * sizeof(float));
                        }
                    }
                }
            }
        }

        if (intangentData)
        {
            unsigned int stride = intangentData->mTechniqueCommon.mAccessor.mStride;
            float* input = intangentData->mFloatArrayPtr->mFloats;
            for (int i = 0, j = 0; i < kfCnt; ++i, j += stride)
            {
                for (int k = 0, n = nodePtr->mKeyFrames[i].mTransforms.size();k < n;++k)
                {
                    Transform& animTrans = nodePtr->mKeyFrames[i].mTransforms[k];
                    if (animTrans.mSID.compare(sid) == 0)
                    {
                        for (int s = 0;s < stride;++s)
                        {
                            float v = animTrans.mType == Transform::ERotate ? -Math::DegreesToRadians(input[j + s]) : input[j + s];
                            animTrans.mInTangent[s] = v;
                        }
                    }
                }
            }
        }

        if (outtangentData)
        {
            unsigned int stride = outtangentData->mTechniqueCommon.mAccessor.mStride;
            float* input = outtangentData->mFloatArrayPtr->mFloats;
            for (int i = 0, j = 0; i < kfCnt; ++i, j += stride)
            {
                for (int k = 0, n = nodePtr->mKeyFrames[i].mTransforms.size();k < n;++k)
                {
                    Transform& animTrans = nodePtr->mKeyFrames[i].mTransforms[k];
                    if (animTrans.mSID.compare(sid) == 0)
                    {
                        for (int s = 0;s < stride;++s)
                        {
                            float v = animTrans.mType == Transform::ERotate ? -Math::DegreesToRadians(input[j + s]) : input[j + s];
                            animTrans.mOutTangent[s] = v;
                        }
                    }
                }
            }
        }
    }

    void AbsColladaLoader::prepareSkins()
    {
        for (int i = 0; i < mCollada.mVisualScenes.size(); ++i)
        {
            VisualScene vs = mCollada.mVisualScenes.at(i);
            for (int k = 0; k < vs.mJointRootPtrs.size(); ++k)
            {
                JointNode* rootPtr = vs.mJointRootPtrs.at(k);
                for (int j = 0; j < mCollada.mControllers.size(); ++j)
                {
                    Controller ctl = mCollada.mControllers.at(j);
                    for (int k = 0; k < ctl.mSkinPtrs.size(); ++k)
                    {
                        SkinPtr sPtr = ctl.mSkinPtrs.at(k);
                        prepareSkin(sPtr, rootPtr);
                    }
                }
            }
        }
    }

    void AbsColladaLoader::prepareSkin(SkinPtr skinPtr, JointNode* jnrootPtr)
    {
        if (!skinPtr || !jnrootPtr)
        {
            return;
        }

        VertexWeightPtr vw = skinPtr->mVertexWeightPtr;
        Input weightInput = vw->mWeight;
        Input jointInput = vw->mJoint;
        SourcePtr jointSrc;
        SourcePtr ibmSrc;
        SourcePtr weightSrc;
        for (int j = 0; j < skinPtr->mSourcePtrs.size(); j++)
        {
            SourcePtr srcPtr = skinPtr->mSourcePtrs.at(j);
            if (jointInput.mSource.substr(1).compare(srcPtr->mID) == 0)
            {
                jointSrc = srcPtr;
            }

            if (skinPtr->mInvBindMatrix.mSource.substr(1).compare(srcPtr->mID) == 0)
            {
                ibmSrc = srcPtr;
            }

            if (weightInput.mSource.substr(1).compare(srcPtr->mID) == 0)
            {
                weightSrc = srcPtr;
            }
        }

        // find joint node in the visual scene joint node hierarchy
        int jnCnt = jointSrc->mNameArrayPtr->mCount;
        int jnIBMCnt = ibmSrc->mFloatArrayPtr->mCount;

        // each joint node should have one inverse bind matrix
        assert(jnCnt * 16 == jnIBMCnt);

        string* jointNames = jointSrc->mNameArrayPtr->mNames;
        skinPtr->mJNCount = jnCnt;
        skinPtr->mJNPtrTable = new JointNode*[jnCnt];
        for (int i = 0; i < jnCnt; ++i)
        {
            JointNode* origin = findJointNodeByID(jointNames[i]);
            if (origin)
            {
                JointNode* jn = origin;
                if (jn->mParentPtr)
                {
                    jn->mParentIDStr = jn->mParentPtr->mID;
                }
                else
                {
                    skinPtr->mRoots.push_back(i);
                }

                for (int j = 0; j < 16; ++j)
                {
                    jn->mInvBindMatrix[j] = ibmSrc->mFloatArrayPtr->mFloats[i * 16 + j];
                    origin->mInvBindMatrix[j] = jn->mInvBindMatrix[j];
                }
                skinPtr->mJNPtrTable[i] = jn;
            }
            else
            {
                LOGE("cannot find joint, mal file. name = %s\n", jointNames[i].c_str());
            }
        }

        float* weightFloats = weightSrc->mFloatArrayPtr->mFloats;
        int curidx = 0;

        // 4 components
        skinPtr->mJNIndexCount = vw->mVCountLength * 4;
        skinPtr->mJNIndex = new unsigned int[skinPtr->mJNIndexCount];
        skinPtr->mJNWeight = new float[skinPtr->mJNIndexCount];
        for (int j = 0; j < vw->mVCountLength; ++j)
        {
            int vcount = vw->mVCount[j];
            float weightMag = 0;
            for (int k = 0; k < 4; ++k)
            {
                if (k < vcount)
                {
                    int jointNameIdx = vw->mV[curidx + k * 2];
                    skinPtr->mJNIndex[j * 4 + k] = jointNameIdx;

                    int weightIdx = vw->mV[curidx + k * 2 + 1];
                    float weight = weightFloats[weightIdx];
                    weightMag += weight;
                    skinPtr->mJNWeight[j * 4 + k] = weight;
                }
                else
                {
                    // no need to set index to something like -1
                    // because weight is 0, nothing will happen
                    skinPtr->mJNIndex[j * 4 + k] = 0;
                    skinPtr->mJNWeight[j * 4 + k] = 0;
                }
            }

            if (weightMag < 0.99 || weightMag > 1.01)
            {
                // normalize weight
                for (int k = 0; k < vcount; k++)
                {
                    skinPtr->mJNWeight[j * 4 + k] /= weightMag;
                }
            }
            curidx = curidx + vcount * 2;
        }
    }

    void AbsColladaLoader::dumpScene()
    {
        LOGD("------ dump scene start ------\n");
        for (int i = 0; i < mCollada.mVisualScenes.size(); ++i)
        {
            VisualScene vs = mCollada.mVisualScenes.at(i);

            for (int j = 0; j < vs.mSceneNodePtrs.size(); ++j)
            {
                SceneNode* cur = vs.mSceneNodePtrs.at(j);
                dumpScene(cur, 0);
            }
        }
        LOGD("------ dump scene end -------\n");
    }

    void AbsColladaLoader::dumpScene(SceneNode* node, int level)
    {
        string tab = "";
        for (int i = 0; i < level; ++i)
        {
            tab += "    ";
        }
        LOGD("%s%s, %s, %s\n", tab.c_str(), node->getName().c_str(), node->mID.c_str(), node->mSID.c_str());

        for (int i = 0; i < node->mChildren.size(); ++i)
        {
            dumpScene(node->mChildren.at(i), level + 1);
        }
    }
    
    void AbsColladaLoader::printStats()
    {
        Stats& stats = mCollada.mStats;
        LOGD("number of lines primitive: %d\n", stats.mNbLine);
        LOGD("number of triangles primitive: %d\n", stats.mNbTriangle);
        LOGD("number of polygons primitive: %d\n", stats.mNbPolygon);
        LOGD("number of polylist primitive: %d\n", stats.mNbPolylist);
        LOGD("number of node that contains geo data: %d\n", stats.mNodeHasGeoData);
    }
    
    char* AbsColladaLoader::trim(char* str)
    {
        size_t len = 0;
        char *frontp = str - 1;
        char *endp = NULL;
        
        if( str == NULL )
            return NULL;
        
        if( str[0] == '\0' )
            return str;
        
        len = strlen(str);
        endp = str + len;
        
        /* Move the front and back pointers to address
         * the first non-whitespace characters from
         * each end.
         */
        while( isCharSpace(*(++frontp)) );
        while( isCharSpace(*(--endp)) && endp != frontp );
        
        if( str + len - 1 != endp )
            *(endp + 1) = '\0';
        else if( frontp != str &&  endp == frontp )
            *str = '\0';
        
        /* Shift the string so that it starts at str so
         * that if it's dynamically allocated, we can
         * still free it on the returned pointer.  Note
         * the reuse of endp to mean the front of the
         * string buffer now.
         */
        endp = str;
        if( frontp != str )
        {
            while( *frontp ) *endp++ = *frontp++;
            *endp = '\0';
        }
        
        
        return str;
    }

    bool AbsColladaLoader::isCharSpace(char c)
    {
    	return c == ' ' || c == '\t';
    }
    
    Scene* AbsColladaLoader::loadingCanceled(xmlDoc* doc)
    {
        if (doc)
        {
            xmlFreeDoc(doc);
        }
        HouyiDelete(mScene);
        mScene = 0;
        return 0;
    }
}
