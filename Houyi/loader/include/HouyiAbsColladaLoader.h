//
//  AbsColladaLoader.h
//  houyi
//
//  Created by Lance Mao on 13/11/11.
//  Copyright (c) 2011 Dong wen. All rights reserved.
//

#ifndef houyi_AbsColladaLoader_h
#define houyi_AbsColladaLoader_h

#include "libxml/tree.h"
#include <iostream>
#include <vector>

#include "HouyiLoader.h"
#include "Collada.h"

using namespace std;
using namespace Houyi;

namespace collada
{
    // Abstract Collada Loader that loads xml data into intermediate data structure
    // implementation should derive this class to convert the intermediate data
    // into its customed data which should be memory saving and cache friendly
    class AbsColladaLoader: public Loader
    {
    public:
        enum LogLevel
        {
            ENormal,
            EStats,
            EFull
        };
        
        // attributes
        static const xmlChar* ATTR_ID;
        static const xmlChar* ATTR_SID;
        static const xmlChar* ATTR_NAME;
        static const xmlChar* ATTR_COUNT;
        static const xmlChar* ATTR_SEMANTIC;
        static const xmlChar* ATTR_OFFSET;
        static const xmlChar* ATTR_SET;
        static const xmlChar* ATTR_SOURCE;
        static const xmlChar* ATTR_TYPE;
        static const xmlChar* ATTR_TARGET;
        static const xmlChar* ATTR_URL;
        static const xmlChar* ATTR_TEXTURE;
        static const xmlChar* ATTR_SYMBOL;
        static const xmlChar* ATTR_MATERIAL;
        static const xmlChar* ATTR_STRIDE;
        static const xmlChar* ATTR_OPAQUE;
        
        // misc
        static const xmlChar* MISC_JOINT;
        
        // core tags
        static const xmlChar* TAG_SOURCE;
        static const xmlChar* TAG_FLOAT_ARRAY;
        static const xmlChar* TAG_NAME_ARRAY;
        static const xmlChar* TAG_INPUT;
        static const xmlChar* TAG_MATRIX;
        static const xmlChar* TAG_TRANSLATE;
        static const xmlChar* TAG_ROTATE;
        static const xmlChar* TAG_SCALE;
        static const xmlChar* TAG_FLOAT;
        static const xmlChar* TAG_ACCESSOR;
        static const xmlChar* TAG_PARAM;
        
        // asset tags
        static const xmlChar* TAG_ASSET;
        static const xmlChar* TAG_CONTRIBUTOR;
        static const xmlChar* TAG_AUTHORING_TOOL;
        static const xmlChar* TAG_UP_AXIS;
        
        // image tags
        static const xmlChar* TAG_LIBRARY_IMAGES;
        static const xmlChar* TAG_IMAGE;
        static const xmlChar* TAG_INIT_FROM;
        
        // material tags
        static const xmlChar* TAG_LIBRARY_MATERIALS;
        static const xmlChar* TAG_MATERIAL;
        static const xmlChar* TAG_INSTANCE_EFFECT;
        
        // effect tags
        static const xmlChar* TAG_LIBRARY_EFFECTS;
        static const xmlChar* TAG_EFFECT;
        static const xmlChar* TAG_PROFILE_COMMON;
        static const xmlChar* TAG_NEW_PARAM;
        static const xmlChar* TAG_EXTRA;
        static const xmlChar* TAG_SURFACE;
        static const xmlChar* TAG_SAMPLER2D;
        static const xmlChar* TAG_TECHNIQUE;
        static const xmlChar* TAG_PHONG;
        static const xmlChar* TAG_LAMBERT;
        static const xmlChar* TAG_BLINN;
        static const xmlChar* TAG_DOUBLE_SIDED;
        static const xmlChar* TAG_COLOR;
        static const xmlChar* TAG_EMISSION;
        static const xmlChar* TAG_AMBIENT;
        static const xmlChar* TAG_DIFFUSE;
        static const xmlChar* TAG_TEXTURE;
        static const xmlChar* TAG_MINFILTER;
        static const xmlChar* TAG_MAGFILTER;
        static const xmlChar* TAG_SPECULAR;
        static const xmlChar* TAG_SHININESS;
        static const xmlChar* TAG_TRANSPARENT;
        static const xmlChar* TAG_TRANSPARENCY;
        static const xmlChar* TAG_REFLECTIVE;
        
        // geometry tags
        static const xmlChar* TAG_LIBRARY_GEOMETRIES;
        static const xmlChar* TAG_GEOMETRY;
        static const xmlChar* TAG_INSTANCE_GEOMETRY;
        static const xmlChar* TAG_MESH;
        static const xmlChar* TAG_VERTICES;
        static const xmlChar* TAG_LINES;
        static const xmlChar* TAG_TRIAGNLES;
        static const xmlChar* TAG_POLYGONS;
        static const xmlChar* TAG_POLYLIST;
        static const xmlChar* TAG_P;
        
        // lighting tags
        static const xmlChar* TAG_LIBRARY_LIGHTS;
        static const xmlChar* TAG_LIGHT;
        static const xmlChar* TAG_INSTANCE_LIGHT;
        static const xmlChar* TAG_POINT;
        static const xmlChar* TAG_DIRECTIONAL;
        
        // camera tags
        static const xmlChar* TAG_LIBRARY_CAMERAS;
        static const xmlChar* TAG_CAMERA;
        static const xmlChar* TAG_INSTANCE_CAMERA;
        static const xmlChar* TAG_OPTICS;
        static const xmlChar* TAG_PERSPECTIVE;
        static const xmlChar* TAG_XFOV;
        static const xmlChar* TAG_YFOV;
        static const xmlChar* TAG_ASPECT_RATIO;
        static const xmlChar* TAG_NEAR;
        static const xmlChar* TAG_FAR;
        
        // visual scene tags
        static const xmlChar* TAG_LIBRARY_VISUAL_SCENES;
        static const xmlChar* TAG_LIBRARY_NODES;
        static const xmlChar* TAG_VISUAL_SCENE;
        static const xmlChar* TAG_NODE;
        static const xmlChar* TAG_INSTANCE_CONTROLLER;
        static const xmlChar* TAG_BIND_MATERIAL;
        static const xmlChar* TAG_INSTANCE_MATERIAL;
        static const xmlChar* TAG_TECHNIQUE_COMMON;
        static const xmlChar* TAG_INSTANCE_NODE;
        
        // controller tags
        static const xmlChar* TAG_LIBRARY_CONTROLLERS;
        static const xmlChar* TAG_CONTROLLER;
        static const xmlChar* TAG_SKIN;
        static const xmlChar* TAG_BIND_SHAPE_MATRIX;
        static const xmlChar* TAG_JOINTS;
        static const xmlChar* TAG_VERTEX_WEIGHTS;
        static const xmlChar* TAG_VCOUNT;
        static const xmlChar* TAG_V;
        
        // animation tags
        static const xmlChar* TAG_LIBRARY_ANIMATIONS;
        static const xmlChar* TAG_ANIMATION;
        static const xmlChar* TAG_SAMPLER;
        static const xmlChar* TAG_CHANNEL;
        
    public:
        AbsColladaLoader()
        {
            
        }
        
        AbsColladaLoader(const string fp) : Loader(fp)
        {

        }

        virtual ~AbsColladaLoader()
        {
        }

        // inherit from super class which is the general interface for loaders
        virtual Scene* loadFromMemory(const char* data, int size);

        // pure virtual function to make this class abstract. customize here
        virtual Scene* convert(Collada& collada) = 0;

        Collada mCollada;
        
        // log
        LogLevel mLogLevel;

    protected:
        // util functions that might be used by sub classes
        float* copyFloatArray(const FloatArray* fa);
        SceneNode* findNodeByID(const string nodeID);
        SceneNode* findNodeByID(SceneNode* rootPtr, const string nodeID);
        JointNode* findJointNodeByID(string nodeID);
        JointNode* findJointNodeByID(JointNode* rootPtr, const string nodeID);
        bool findTransformBySID(JointNode* nodePtr, const string sid, Transform& res);
        int findIndexInSkinTable(const SkinPtr& skin, const JointNode* node);

        // connect material to effects
        void prepareMaterial();

        // prepare light
        void prepareLights();

        // find instance node's target node
        void prepareNode();

        // find geometry data for each node in visual scene
        void prepareGeometry();
        void findGeoData(SceneNode* gnPtr);

        // add animatin data to joints node
        // TODO should we make copy of the nodes?
        void updateJointsAni();
        void updateJointsAni(Animation& animation, JointNode* nodePtr);

        // generate skin data
        void prepareSkins();
        void prepareSkin(SkinPtr skinPtr, JointNode* jnrootPtr);

        // function to load collada cores
        FloatArrayPtr loadFloatArray(xmlNode* node);
        FloatArrayPtr loadFloatArray(xmlNode* node, int count);
        NameArrayPtr loadNameArray(xmlNode* node);
        SourcePtr loadSource(xmlNode* node);
        Input loadInput(xmlNode* node);
        Matrix loadMatrix(xmlNode* node);
        float loadFloat(xmlNode* node);
        float loadInt(xmlNode* node);
        Transform loadTransform(xmlNode* node);
        Accessor loadAccessor(xmlNode* node);
        Param loadParam(xmlNode* node);

        void loadElement(xmlNode * a_node);

        // asset
        void loadAsset(xmlNode* node);
        Contributor loadContributor(xmlNode* node);
        
        // image
        void loadLibraryImages(xmlNode* node);
        Image loadImage(xmlNode* node);

        // material
        void loadLibraryMaterials(xmlNode* node);
        Material loadMaterial(xmlNode* node);

        // effect
        void loadLibraryEffects(xmlNode* node);
        Effect loadEffect(xmlNode* node);
        ProfileCommon loadProfileCommon(xmlNode* node);
        Extra loadExtra(xmlNode* node);
        NewParam loadNewParam(xmlNode* node);
        Surface loadSurface(xmlNode* node);
        Sampler2D loadSampler2D(xmlNode* node);
        Technique loadTechnique(xmlNode* node);
        Phong loadPhong(xmlNode* node);
        Color loadColor(xmlNode* node);
        Diffuse loadDiffuse(xmlNode* node);

        // geometry
        bool loadLibraryGeometries(xmlNode* node);
        bool loadGeometry(xmlNode* node);
        Mesh* loadMesh(xmlNode* node);
        Vertices loadVertices(xmlNode* node);
        Lines* loadLines(xmlNode* node);
        Triangles* loadTriangles(xmlNode* node);
        Polygons* loadPolygons(xmlNode* node);
        Polylist* loadPolylist(xmlNode* node);

        // light
        void loadLibraryLights(xmlNode* node);
        void loadLight(xmlNode* node);
        Point loadPoint(xmlNode* node);

        // camera
        void loadLibraryCameras(xmlNode* node);
        void loadCamera(xmlNode* node);
        Optics loadOptics(xmlNode* node);
        Perspective loadPerspective(xmlNode* node);

        // scene
        void loadLibraryVisualScenes(xmlNode* node);
        void loadLibraryNodes(xmlNode* node);
        void loadVisualScene(xmlNode* node);
        JointNode* loadJointNode(xmlNode* node);
        SceneNode* loadSceneNode(VisualScene& vs, xmlNode* node);
        InstanceGeometry loadInstanceGeo(xmlNode* node);
        SceneNode* loadLightNode(xmlNode* node);
        SceneNode* loadCameraNode(xmlNode* node);
        Controller loadInstanceController(xmlNode* node);
        TechniqueCommon loadTechniqueCommon(xmlNode* node);

        // controller
        void loadLibraryControllers(xmlNode* node);
        void loadController(xmlNode* node);
        SkinPtr loadSkin(xmlNode* node);
        VertexWeightPtr loadVertexWeight(xmlNode* node);

        // animation
        void loadLibraryAnimations(xmlNode* node);
        void loadAnimationGroup(xmlNode* node);
        Animation loadAnimation(xmlNode* node);
        Sampler loadSampler(xmlNode* node);
        
        // util functions
        string xmlChar2string(xmlChar* a_xmlChar); // UTF-8 encoding ???
        bool isLightNode(xmlNode* node);
        bool isCameraNode(xmlNode* node);
        bool isCameraTargetNode(xmlNode* node);
        void dumpScene();
        void dumpScene(SceneNode* node, int level);
        void printStats();
        char* trim(char* str);
        bool isCharSpace(char c);
        Scene* loadingCanceled(xmlDoc* doc);
        
    protected:
        map<string, string> mMaterialSymbolMap;
    };
} // namespace

#endif
