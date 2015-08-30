#include "Houyi3dsLoader.h"
#include "HouyiScene.h"
#include "HouyiStringUtils.h"
#include "HouyiUtil.h"

namespace Houyi
{
    const bool LOG_DEBUG = false;
    
    static const int SMOOTH_THRESHHOLD = 8192;
    
    static const int M3DMAGIC = 0x4d4d;
//    static const int CMAGIC = 0xc23d;
    static const int M3D_VERSION = 0x0002;
    static const int COLOR_24 = 0x0011;
//    static const int LIN_COLOR_24 = 0x0012;
    static const int COLOR_F = 0x0010;
    static const int INT_PERCENTAGE = 0x0030;
    static const int FLOAT_PERCENTAGE = 0x0031;
    static const int MDATA = 0x3d3d;
    static const int MESH_VERSION = 0x3d3e;

    static const int MAT_ENTRY = 0xafff;
    static const int MASTER_SCALE = 0x0100;
    static const int NAMED_OBJECT = 0x4000;
    static const int MAT_NAME = 0xa000;
    static const int MAT_AMBIENT = 0xa010;
    static const int MAT_DIFFUSE = 0xa020;
    static const int MAT_SPECULAR = 0xa030;
    static const int MAT_GLOSSINESS = 0xa040;
    static const int MAT_SPECULAR_LEVEL = 0xa041;
    static const int MAT_TRANSPARENCY = 0xa050;
//    static const int MAT_XPFALL = 0xa052;
//    static const int MAT_REFBLUR = 0xa053;
//    static const int MAT_SELF_ILPCT = 0xa084;
//    static const int MAT_WIRESIZE = 0xa087;
//    static const int MAT_XPFALLIN = 0xa08a;
//    static const int MAT_SHADING = 0xa100;
    static const int MAT_MAPPING = 0xa200;
    static const int MAT_TEX_NAME = 0xa300;

    static const int N_TRI_OBJECT = 0x4100;
    static const int POINT_ARRAY = 0x4110;
    static const int FACE_ARRAY = 0x4120;
    static const int MESH_MAT_GROUP = 0x4130;
    static const int TEX_VERTS = 0x4140;
    static const int SMOOTH_GROUP = 0x4150;
    static const int MESH_MATRIX = 0x4160;

    static const int N_DIRECT_LIGHT = 0x4600;
    static const int DL_INNER_RANGE = 0x4659;
    static const int DL_OUTER_RANGE = 0x465a;
    static const int DL_MULTIPLIER = 0x465b;
    static const int N_CAMERA = 0x4700;
    static const int CAM_RANGES = 0x4720;

    static const int KFDATA = 0xb000;
    static const int KFHDR = 0xb00a;
    static const int KFSEG = 0xb008;
    static const int OBJECT_NODE_TAG = 0xb002;
    static const int CAMERA_NODE_TAG = 0xb003;
    static const int TARGET_NODE_TAG = 0xb004;
    static const int LIGHT_NODE_TAG = 0xb005;
    static const int KFCURTIME = 0xb009;
    static const int NODE_HDR = 0xb010;
    static const int INSTANCE_NAME = 0xb011;
//    static const int PRESCALE = 0xb012;
    static const int PIVOT = 0xb013;
    static const int POS_TRACK_TAG = 0xb020;
    static const int ROT_TRACK_TAG = 0xb021;
    static const int SCL_TRACK_TAG = 0xb022;
    static const int FOV_TRACK_TAG = 0xb023;
    static const int ROLL_TRACK_TAG = 0xb024;
    static const int NODE_ID = 0xb030;
    
    Houyi3dsLoader::~Houyi3dsLoader()
    {
        for(int i = 0;i < mNodes.size();++i)
        {
            HouyiDelete(mNodes[i]);
        }
    }

    Scene* Houyi3dsLoader::loadFromMemory(const char* data, int size)
    {
        DataInputStream in(data, size);

        int length = in.available();

        while (in.getPos() < length)
        {
            int chunkID = in.readShort();
            int chunkLength = in.readInt() - 6;
            switch (chunkID)
            {
                case M3DMAGIC:
                    break;

                case M3D_VERSION:
                {
                    int m3dv = in.readInt(); // M3DVersion
                    if (LOG_DEBUG)
                        LOGD("M3D_VERSION = %d\n", m3dv);
                }
                    break;

                case MDATA:
                    load3DSFile(&in, chunkLength);
                    break;

                case KFDATA:
                    loadKeyFrame(&in, chunkLength);
                    break;

                default:
                    if (LOG_DEBUG)
                        LOGD("load Skipping chunk... 0x%.8x\n", chunkID);
                    in.skip(chunkLength);
                    break;
            }
        }
        
        // first try with OBJECT_NODE_TAG
        // if not present add every node
        if (mScene->getRenderableCount() == 0)
        {
            for (int i = 0;i < mNodes.size();++i)
            {
                mScene->addSceneNode(mNodes[i]);
                mScene->addRenderable(mNodes[i]);
            }
            mNodes.clear();
        }
        
        if (LOG_DEBUG)
        {
            LOGD("--------dumping hierarchy------\n");
            mScene->dumpHierarchy();
            LOGD("--------dumping hierarchy------\n");
        }
        
        return mScene;
    }

    void Houyi3dsLoader::load3DSFile(DataInputStream* in, int length)
    {
        int end = in->getPos() + length;
        while (in->getPos() < end)
        {
            int chunkID = in->readShort();
            int chunkLength = in->readInt() - 6;
            switch (chunkID)
            {
                case M3D_VERSION:
                {
                    int m3dversion = in->readInt(); // M3DVersion
                    if (LOG_DEBUG) LOGD("M3DVersion = %d\n", m3dversion);
                }
                    break;

                    // Mesh version
                case MESH_VERSION: // 0x3d3e
                {
                    int meshVersion = in->readInt(); // Mesh Version
                    if (LOG_DEBUG) cout << "Mesh Version = " << meshVersion << endl;
                }
                    break;

                case MASTER_SCALE:
                {
                    float ms = in->readFloat();
//                    mWorld->setMasterScale(ms); // Master Scale
                    if (LOG_DEBUG) cout << "Master Scale = " << ms << endl;
                }
                    break;

                case MAT_ENTRY:
                {
                    Material* mat = loadMaterial(in, chunkLength);
                    mScene->getMaterialManager()->addMaterial(mat);
                }
                    break;

                    // named object
                case NAMED_OBJECT: // 0x4000
                    loadNamedObject(in, chunkLength);
                    break;

                default:
                    if (LOG_DEBUG) cout << "load3DSFile Skipping chunk... " << hex << chunkID << endl;
                    in->skip(chunkLength);
                    break;
            }
        }
    }

    Material* Houyi3dsLoader::loadMaterial(DataInputStream* in, int length) {
        Material* mat = HouyiNew Material();
        int end = in->getPos() + length;
        while (in->getPos() < end)
        {
            int chunkID = in->readShort();
            int chunkLength = in->readInt() - 6;
            switch (chunkID)
            {
                case MAT_NAME:
                {
                    string name = load3DSString(in);
                    mat->setName(name);
                    if (LOG_DEBUG) LOGD("Loading Material: %s\n", name.c_str());
                }
                    break;

                case MAT_AMBIENT:
                {
                    int color = loadColor(in, chunkLength);
                    mat->setAmbientColor(color);
                    if (LOG_DEBUG) LOGD("Material Ambient: 0x%.8x\n", color);
                }
                    break;

                case MAT_DIFFUSE:
                {
                    int color = loadColor(in, chunkLength);
                    mat->setDiffuseColor(color);
                    if (LOG_DEBUG) LOGD("Material Diffuse: 0x%.8x\n", color);
                }
                    break;

                case MAT_SPECULAR:
                {
                    int color = loadColor(in, chunkLength);
                    mat->setSpecularColor(color);
                    if (LOG_DEBUG) LOGD("Material Specular: 0x%.8x\n", color);
                }
                    break;

                case MAT_SPECULAR_LEVEL:
                {
                    // do not set. some exporter mistakenly exports level to 0
                    // which causes no specular effect
                    float specularLevel = loadPercentage(in, chunkLength);
                    if (LOG_DEBUG) cout << "Material Specular Level: " << specularLevel << endl;
                }
                    break;

                case MAT_GLOSSINESS:
                {
                    float glossiness = loadPercentage(in, chunkLength);
                    if (glossiness >= 0 && glossiness < 1)
                    {
                        // Blinn-Torrance-Sparrow Mode
                        glossiness *= 256;
                    }
                    glossiness = Math::Clamp<float>(glossiness, 0, 256);
                    mat->setShininess(glossiness);
                    if (LOG_DEBUG) LOGD("Material Glossiness: %f\n", glossiness);
                }
                    break;

                case MAT_TRANSPARENCY:
                {
                    float trans = loadPercentage(in, chunkLength);
                    mat->setTransparency(trans);
                    if (LOG_DEBUG) cout << "Material Transparency: " << trans << endl;
                }
                    break;

                case MAT_MAPPING:
                    loadMaterialMap(mat, in, chunkLength);
                    break;

                default:
                    if (LOG_DEBUG) cout << "loadMaterial Skipping chunk... " << hex << chunkID << endl;
                    in->skip(chunkLength);
                    break;
            }
        }

        if (LOG_DEBUG) cout << "Finished Loading Material: " << mat->getName() << endl;
        return mat;
    }

    void Houyi3dsLoader::loadMaterialMap(Material* mat, DataInputStream* in, int length)
    {
        int end = in->getPos() + length;
        while (in->getPos() < end)
        {
            int chunkID = in->readShort();
            int chunkLength = in->readInt() - 6;
            switch (chunkID) {
                case MAT_TEX_NAME:
                {
                    string texName = load3DSString(in);
                    string paths[3];
                    paths[0] = StringUtils::getLastPathComponent(texName);
                    paths[1] = StringUtils::getPath(mFilePath) + StringUtils::getLastPathComponent(texName);
                    paths[2] = StringUtils::getTexturePath(mFilePath, texName);
                    mat->createTextureByPath(mScene, paths);
                    if (LOG_DEBUG) cout << "Material Texture Name: " << texName << endl;
                }
                    break;
                default:
                    if (LOG_DEBUG) cout << "loadMaterialMap Skipping chunk... " << hex << chunkID << endl;
                    in->skip(chunkLength);
                    break;
            }
        }
    }

    void Houyi3dsLoader::loadNamedObject(DataInputStream* in, int length)
    {
        int end = in->getPos() + length;
        string name = load3DSString(in);

        while (in->getPos() < end)
        {
            int chunkID = in->readShort();
            int chunkLength = in->readInt() - 6;
            switch (chunkID)
            {
                    // Named Triangle Object
                case N_TRI_OBJECT: // 0x4100
                {
                    if (LOG_DEBUG) LOGD("Loading Named Triangle Object: %s\n", name.c_str());
                    Mesh* mesh = loadNamedTriangleObject(in, chunkLength, name);
                    mScene->addMesh(mesh);
                    if (LOG_DEBUG) LOGD("Finished Named Triangle Object: %s\n", name.c_str());
                }
                    break;

                case N_DIRECT_LIGHT: // 0x4600
                {
                    if (LOG_DEBUG) cout << "Loading Light: " << name << endl;
                    Light* light = loadLight(in, chunkLength);
                    light->setName(name);
                    mScene->addLight(light);
                    if (LOG_DEBUG) cout << "Finished Loading Light: " << name << endl;
                }
                    break;

                case N_CAMERA: // 0x4700
                {
                    Camera* cam = loadCamera(in, chunkLength);
                    cam->setName(name);
                    cam->lookAt();
                    if (cam->getNearRange() == 0){
                        cam->setNearRange(1);
                    }
                    mScene->addCamera(cam);
                }
                    break;

                default:
                    if (LOG_DEBUG) cout << "loadNamedObject Skipping chunk... " << hex << chunkID << endl;
                    in->skip(chunkLength);
                    break;
            }
        }
    }

    Mesh* Houyi3dsLoader::loadNamedTriangleObject(DataInputStream* in, int length, string name)
    {
        Mesh* mesh = HouyiNew Mesh();
        mesh->setName(name);
        SceneNode* node = HouyiNew SceneNode();
        node->setMesh(mesh);
        node->setName(name);
        VertexBufferPtr vb = new VertexBuffer();
        uint* faceArray = 0;
        IndexBufferPtr faceIndexBuffer;
        int end = in->getPos() + length;
        while (in->getPos() < end)
        {
            int chunkID = in->readShort();
            int chunkLength = in->readInt() - 6;
            switch (chunkID)
            {
                    // vertex
                case POINT_ARRAY: // 0x4110
                {
                    int cnt = in->readShort();
                    if (LOG_DEBUG) LOGD("Number of raw vertices: %d\n", cnt);
                    float* vertices = new float[cnt * 3];
                    for (int i = 0; i < cnt; i++)
                    {
                        float a = in->readFloat();
                        float b = in->readFloat();
                        float c = in->readFloat();
                        vertices[i * 3] = a;
                        vertices[i * 3 + 1] = b;
                        vertices[i * 3 + 2] = c;
                        //cout << "Vertex " << i << " x=" << a << ", y=" << b << ", z=" << c << endl;
    //                    cout << a << ", " << b << ", " << c << ", " << endl;
                    }

                    vb->setPosition(vertices, cnt * 3);
                }
                    break;

                    // texture coordinates
                case TEX_VERTS: // 0x4140
                {
                    int cnt = in->readShort();
                    if (LOG_DEBUG) LOGD("Number of raw texcoord: %d\n", cnt);
                    float* texcoord = new float[cnt * 2];
                    for (int i = 0; i < cnt; i++)
                    {
                        float a = in->readFloat();
                        float b = in->readFloat();
                        texcoord[i * 2] = a;
                        texcoord[i * 2 + 1] = b;
                        // Log.d(TAG, "tex v" + i + " s=" + a + ", t=" + b);
                    }

                    vb->setTexcoord(texcoord, cnt * 2);
                }
                    break;

                    // faces
                case FACE_ARRAY: // 0x4120
                {
                    int vertexCount = vb->getVertexCount();
                    float* position = vb->getPosition();
                    float* normals = new float[vertexCount * 3];
                    vector<Vector3> *normalarray = new vector<Vector3>[vertexCount];
                    
                    faceIndexBuffer = new IndexBuffer();
                    int facecnt = in->readShort();
                    if (LOG_DEBUG) LOGD("Number of faces: %d\n", facecnt);
                    faceArray = new uint[facecnt * 3];
                    for (int i = 0; i < facecnt; i++)
                    {
                        int a = in->readShort();
                        int b = in->readShort();
                        int c = in->readShort();
                        faceArray[i * 3] = a;
                        faceArray[i * 3 + 1] = b;
                        faceArray[i * 3 + 2] = c;
//                        LOGE("indices: %d,%d,%d\n", a, b, c);
                        // skipping flag, not sure how to use it
                        in->readShort();
                        
                        // calculate face normal
                        float t1x = position[a * 3];
                        float t1y = position[a * 3 + 1];
                        float t1z = position[a * 3 + 2];
                        float t2x = position[b * 3];
                        float t2y = position[b * 3 + 1];
                        float t2z = position[b * 3 + 2];
                        float t3x = position[c * 3];
                        float t3y = position[c * 3 + 1];
                        float t3z = position[c * 3 + 2];
                        
                        // calculate the face normal
                        Vector3 normal;
                        normal.x = (((t1y - t3y) * (t2z - t3z)) - ((t1z - t3z) * (t2y - t3y)));
                        normal.y = (((t1z - t3z) * (t2x - t3x)) - ((t1x - t3x) * (t2z - t3z)));
                        normal.z = (((t1x - t3x) * (t2y - t3y)) - ((t1y - t3y) * (t2x - t3x)));
                        normal.normalize();
                        
                        // add the face normal to each vertex
                        // later we will use average method to
                        // calculate the final normal for each vertex
                        normalarray[a].push_back(normal);
                        normalarray[b].push_back(normal);
                        normalarray[c].push_back(normal);
                    }
                    faceIndexBuffer->setTriangleIndices(faceArray, facecnt * 3);
                    
                    // iterate each vertex and calculate the average normal
                    for (int i = 0; i < vertexCount; i++)
                    {
                        Vector3 finalVector = Vector3::ZERO;
                        // Log.d(TAG, "Raw Normal Count: " + normalarray[i].size());
                        for (int j = 0; j < normalarray[i].size(); j++)
                        {
                            finalVector = finalVector + normalarray[i].at(j);
                        }
                        
                        finalVector.normalize();
                        normals[i * 3] = finalVector.x;
                        normals[i * 3 + 1] = finalVector.y;
                        normals[i * 3 + 2] = finalVector.z;
                    }
                    
                    delete[] normalarray;
                    
                    vb->setNormal(normals, vertexCount * 3);
                }
                    break;

                    // material group
                case MESH_MAT_GROUP: // 0x4130
                {
                    string matName = load3DSString(in);
                    Material* mat = mScene->getMaterialManager()->getMaterial(matName);
                    if (mat)
                    {
                        node->addMaterial(mat);
                    }
                    else
                    {
                        LOGE("cannot find material: %s\n", matName.c_str());
                    }
                    int matFaceCnt = in->readShort();
                    if (LOG_DEBUG) LOGD("Loading MAT GROUP number of sub faces: %d\n", matFaceCnt);
                    uint* indices = new uint[matFaceCnt * 3];
                    for (int i = 0; i < matFaceCnt; i++)
                    {
                        // Note that this is Face index, not vertex index
                        int idx = in->readShort();
                        indices[i * 3] = faceArray[idx * 3];
                        indices[i * 3 + 1] = faceArray[idx * 3 + 1];
                        indices[i * 3 + 2] = faceArray[idx * 3 + 2];
                    }
                    
                    IndexBufferPtr ib = new IndexBuffer();
                    ib->setTriangleIndices(indices, matFaceCnt * 3);
                    SurfacePtr s = new Surface(vb, ib);
                    mesh->addSurface(s);
                }
                    break;
                    
                    case SMOOTH_GROUP: // 0x4150
                {
                    int faceCnt = faceIndexBuffer->getTriangleIndicesLength() / 3;
                    if (faceCnt > SMOOTH_THRESHHOLD){
                        cout << "Skipping Smooth group... mUseFaceNormal = "
                        << mUseFaceNormal << "; Vertex Count = " << faceCnt << endl;
                        in->skip(chunkLength);
                        break;
                    }
                    
                    float* data = vb->getPosition();
                    float* normal = vb->getNormal();
                    // for (int i = 0;i < normal.length / 3;i++){
                    // Log.d(TAG, "Before smooth: " + normal[i * 3] + normal[i * 3 +
                    // 1] + normal[i * 3 + 2]);
                    // }
                    float* targetNormal = new float[vb->getNormalLength()];
                    memset(targetNormal, 0, vb->getNormalLength() * sizeof(float));
                    
                    vector<int>* smoothGroups = new vector<int>[32];
                    
                    for (int i = 0, n = chunkLength / 4; i < n; i++)
                    {
                        int groupID = in->readInt();
//                        LOGE("Smooth Groop No. %d, id = %d\n", i, groupID);
                        
                        if (groupID > 0 && groupID <= 32)
                        {
                            smoothGroups[groupID - 1].push_back(i);
                        }
                        else
                        {
                            for (int j = 0; j < 3; j++)
                            {
                                int idx = faceArray[i * 3];
                                targetNormal[(idx + j) * 3] = normal[(idx + j) * 3];
                                targetNormal[(idx + j) * 3 + 1] = normal[(idx + j) * 3 + 1];
                                targetNormal[(idx + j) * 3 + 2] = normal[(idx + j) * 3 + 2];
                            }
                        }
                    }
                    
                    static float thresh = 0.1;

                    // Reset vertex normal according to smooth group
                    for (int k = 0; k < 32; k++) {
                        for (int i = 0; i < smoothGroups[k].size(); i++) {
                            int faceIndex = smoothGroups[k].at(i);
                            uint indexV1 = faceArray[faceIndex * 3];
                            uint indexV2 = faceArray[faceIndex * 3 + 1];
                            uint indexV3 = faceArray[faceIndex * 3 + 2];
                            
                            for (int j = i + 1; j < smoothGroups[k].size(); j++) {
                                // if the two face are in the same smooth group
                                // and they have vertex that has same xyz
                                // average the normal of those vertices
                                int trgIndex = smoothGroups[k].at(j);
                                
                                for (int m = 0; m < 3; m++) {
                                    for (int n = 0; n < 3; n++) {
                                        uint v1 = faceArray[faceIndex * 3 + n];
                                        uint v2 = faceArray[trgIndex * 3 + m];
                                        float x1 = data[v1 * 3];
                                        float y1 = data[v1 * 3 + 1];
                                        float z1 = data[v1 * 3 + 2];
                                        float x2 = data[v2 * 3];
                                        float y2 = data[v2 * 3 + 1];
                                        float z2 = data[v2 * 3 + 2];
                                        if (Math::Abs(x1 - x2) < thresh && Math::Abs(y1 - y2) < thresh && Math::Abs(z1 - z2) < thresh) {
                                            targetNormal[v1 * 3] += normal[v2 * 3];
                                            targetNormal[v1 * 3 + 1] += normal[v2 * 3 + 1];
                                            targetNormal[v1 * 3 + 2] += normal[v2 * 3 + 2];
                                            targetNormal[v2 * 3] += normal[v1 * 3];
                                            targetNormal[v2 * 3 + 1] += normal[v1 * 3 + 1];
                                            targetNormal[v2 * 3 + 2] += normal[v1 * 3 + 2];
                                        }
                                    }
                                }
                            }
                            
                            targetNormal[indexV1 * 3] += normal[indexV1 * 3];
                            targetNormal[indexV1 * 3 + 1] += normal[indexV1 * 3 + 1];
                            targetNormal[indexV1 * 3 + 2] += normal[indexV1 * 3 + 2];
                            targetNormal[indexV2 * 3] += normal[indexV2 * 3];
                            targetNormal[indexV2 * 3 + 1] += normal[indexV2 * 3 + 1];
                            targetNormal[indexV2 * 3 + 2] += normal[indexV2 * 3 + 2];
                            targetNormal[indexV3 * 3] += normal[indexV3 * 3];
                            targetNormal[indexV3 * 3 + 1] += normal[indexV3 * 3 + 1];
                            targetNormal[indexV3 * 3 + 2] += normal[indexV3 * 3 + 2];
                        }
                    }
                    
                    // normalize target
                    int vercnt = vb->getVertexCount();
                    for (int i = 0; i < vercnt; i++) {
                        Vector3 vn(targetNormal[i * 3],
                                   targetNormal[i * 3 + 1],
                                   targetNormal[i * 3 + 2]);
                        if (vn == Vector3::ZERO)
                        {
                            targetNormal[i * 3] = normal[i * 3];
                            targetNormal[i * 3 + 1] = normal[i * 3 + 1];
                            targetNormal[i * 3 + 2] = normal[i * 3 + 2];
                        }
                        else
                        {
                            vn.normalize();
                            targetNormal[i * 3] = vn.x;
                            targetNormal[i * 3 + 1] = vn.y;
                            targetNormal[i * 3 + 2] = vn.z;
                        }
                    }
                    
                    //				for (int i = 0;i < vb->getNormalCount() / 3;i++){
                    //                    cout << "After smooth: " << i << " " << targetNormal[i * 3] << ", " << targetNormal[i * 3 + 1] << ", " << targetNormal[i * 3 + 2] << endl;
                    //				}
                    
                    vb->setNormal(targetNormal, vb->getNormalLength());
                    
                    delete [] smoothGroups;
                }
                    break;


                case MESH_MATRIX: // 0x4160
                {
                    Matrix4 matrix;
                    matrix[0][0] = in->readFloat();
                    matrix[1][0] = in->readFloat();
                    matrix[2][0] = in->readFloat();
                    matrix[3][0] = 0;
                    matrix[0][1] = in->readFloat();
                    matrix[1][1] = in->readFloat();
                    matrix[2][1] = in->readFloat();
                    matrix[3][1] = 0;
                    matrix[0][2] = in->readFloat();
                    matrix[1][2] = in->readFloat();
                    matrix[2][2] = in->readFloat();
                    matrix[3][2] = 0;
                    matrix[0][3] = in->readFloat();
                    matrix[1][3] = in->readFloat();
                    matrix[2][3] = in->readFloat();
                    matrix[3][3] = 1;
                    float det = matrix.determinant();
                    if (det < 0)
                    {
                        matrix[0][0] *= -1;
                        if (LOG_DEBUG) cout << "Negative determinant for object " + mesh->getName()
                              << " Det = " << det << endl;
                    }
                    Matrix4 inv = matrix.inverse();
                    node->setLocal(inv);
                    if (LOG_DEBUG) Util::printMatrix(inv);
                }
                    break;

                default:
                    if (LOG_DEBUG) cout << "loadNamedTriangleObject Skipping chunk... " << hex << chunkID << endl;
                    in->skip(chunkLength);
                    break;
            }
        }

        if (mesh->getSurfaceCount() == 0 && faceIndexBuffer)
        {
            // in case no sub mat face we use the whole face
            SurfacePtr s = new Surface(vb, faceIndexBuffer);
            mesh->addSurface(s);
        }

        mNodes.push_back(node);
        return mesh;
    }

    Camera* Houyi3dsLoader::loadCamera(DataInputStream* in, int length){
        if (LOG_DEBUG) cout << "loading Camera" << endl;
        int end = in->getPos() + length;

        Camera* camera = HouyiNew Camera();

        float camerax = in->readFloat();
        float cameray = in->readFloat();
        float cameraz = in->readFloat();
        camera->setPosition(camerax, cameray, cameraz);
        if (LOG_DEBUG) cout << "Camera Position X = " << camerax << "; Y = " << cameray << "; Z = " << cameraz << endl;

        float targetx = in->readFloat();
        float targety = in->readFloat();
        float targetz = in->readFloat();
        camera->setLookAt(targetx, targety, targetz);
        if (LOG_DEBUG) cout << "Camera target X = " << targetx << "; Y = " << targety << "; Z = " << targetz << endl;

        float bankAngle = in->readFloat();
        camera->setBankAngle(bankAngle);

        float focus = in->readFloat();
        camera->setFocus(focus);

        if (LOG_DEBUG) cout << "Camera Bank Angle = " << bankAngle << "; Focus = " << focus << endl;

        while (in->getPos() < end) {
            int chunkID = in->readShort();
            int chunkLength = in->readInt() - 6;
            switch (chunkID) {
                case CAM_RANGES: {
                    float nearrange = in->readFloat();
                    camera->setNearRange(nearrange);
                    float farrange = in->readFloat();
                    camera->setFarRange(farrange);

                    if (LOG_DEBUG) cout << "Camera Near Range = " << nearrange << "; Far Range = " << farrange << endl;
                }
                    break;

                default:
                    if (LOG_DEBUG) cout << "loadCamera Skipping chunk... " << hex << chunkID << endl;
                    in->skip(chunkLength);
                    break;
            }
        }

        return camera;
    }

    Light* Houyi3dsLoader::loadLight(DataInputStream* in, int length){
        int end = in->getPos() + length;

        Light* light = HouyiNew Light();

        float x = in->readFloat();
        float y = in->readFloat();
        float z = in->readFloat();
        light->setPosition(x, y, z);
        if (LOG_DEBUG) cout << "Light Pos X = " << x << "; Y = " << y << "; Z = " << z << endl;

        while (in->getPos() < end) {
            int chunkID = in->readShort();
            int chunkLength = in->readInt() - 6;
            switch (chunkID) {
                case COLOR_F: {
                    float r = in->readFloat();
                    float g = in->readFloat();
                    float b = in->readFloat();

                    light->setColor(r, g, b);
                    if (LOG_DEBUG) cout << "Light Color (float) R = " << r << "; G = " << g << "; B = " << b << endl;
                }
                    break;

                case COLOR_24: {
                    int r = in->readUnsignedByte();
                    int g = in->readUnsignedByte();
                    int b = in->readUnsignedByte();
                    light->setColor(r, g, b);
                    if (LOG_DEBUG) cout << "Light Color (int24) R = " << r << "; G = " << g << "; B = " << b << endl;
                }
                    break;

                case DL_INNER_RANGE:
                {
                    float innerRange = in->readFloat();
                    if (LOG_DEBUG) cout << "Light Inner Range = " << innerRange << endl;
                }
                    break;

                case DL_OUTER_RANGE:
                {
                    float outterRange = in->readFloat();
                    if (LOG_DEBUG) cout << "Light Outter Range = " << outterRange << endl;
                }
                    break;

                case DL_MULTIPLIER:
                {
                    float multiplier = in->readFloat();
                    light->setMultiplier(multiplier);
                    if (LOG_DEBUG) cout << "Light multiplier = " << multiplier << endl;
                }
                    break;

                default:
                    if (LOG_DEBUG) cout << "loadLight Skipping chunk... " << hex << chunkID << endl;
                    in->skip(chunkLength);
                    break;
            }
        }

        return light;
    }

    void Houyi3dsLoader::loadKeyFrame(DataInputStream* in, int length)
    {
        int end = in->getPos() + length;
        while (in->getPos() < end) {
            int chunkID = in->readShort();
            int chunkLength = in->readInt() - 6;
            switch (chunkID) {
                case KFHDR:
                {
                    int revision = in->readShort();
                    string name = load3DSString(in);
                    int anilen = in->readInt();
                    if (LOG_DEBUG) cout << "Revision = " << revision << "; Name = " << name
                          << "; Animation Length = " << anilen << endl;
                }
                    break;

                case KFSEG:
                {
                    int sgstart = in->readInt();
                    int sgend = in->readInt();
                    if (LOG_DEBUG) cout << "Seg Start = " << sgstart << "; Seg End = " << sgend << endl;
                }
                    break;

                case OBJECT_NODE_TAG: // 0xb002
                {
                    if (LOG_DEBUG) cout << "Loading KFrame Object Node" << endl;
                    loadObjectNodeTag(in, chunkLength);
                    if (LOG_DEBUG) cout << "Finish Loading KFrame Object Node" << endl;
                }
                    break;

                case CAMERA_NODE_TAG: // 0xb003
                    loadCameraNodeTag(in, chunkLength);
                    break;

                case TARGET_NODE_TAG: // 0xb004
                    loadTargetNodeTag(in, chunkLength);
                    break;

                case LIGHT_NODE_TAG: // 0xb005
                    loadLightNodeTag(in, chunkLength);
                    break;

                case KFCURTIME: // oxb009
                {
                    int curKF = in->readInt();
                    if (LOG_DEBUG) cout << "KFCURTIME = " << curKF << endl;
//                    mWorld->setCurFrame(curKF);
                }
                    break;

                default:
                    if (LOG_DEBUG) cout << "loadKeyFrame Skipping chunk... " << hex << chunkID << endl;
                    in->skip(chunkLength);
                    break;
            }
        }
    }

    void Houyi3dsLoader::loadObjectNodeTag(DataInputStream* in, int length)
    {
        SceneNode* obj = NULL;
        int id = 0;
        Vector3 pivot;
        int end = in->getPos() + length;
        while (in->getPos() < end)
        {
            int chunkID = in->readShort();
            int chunkLength = in->readInt() - 6;
            switch (chunkID)
            {
                case NODE_HDR: // 0xb010
                {
                    string name = load3DSString(in);
                    obj = findNodeByName(name);
                    if (obj == NULL)
                    {
                        obj = HouyiNew SceneNode();
                        obj->setName(name);
                    }
                    else
                    {
                        obj = obj->clone();
                    }

                    obj->setId(id);
                    int flag1 = in->readShort();
                    int flag2 = in->readShort();
                    short hierarchy = in->readShort();
                    Node* p = mScene->getRootSceneNode()->findNodeById(hierarchy);
                    if (LOG_DEBUG) cout << "Object Name = " << obj->getName() << "; Flag1 = " << flag1
                       << "; Flag2 = " << flag2 << " Hierarchy ID = "
                       << dec << hierarchy << endl;
                    if (p && hierarchy >= 0)
                    {
                        p->addChild(obj);
                        if (LOG_DEBUG) cout << "Node " << obj->getName() << "` s parent is Node "
                              << p->getName() << " (ID = " << p->getId() << ")" << endl;
                    }
                    else
                    {
                        mScene->addSceneNode(obj);
                    }
                    if (obj->getMesh())
                    {
                        mScene->addRenderable(obj);
                    }
                }
                    break;

                case INSTANCE_NAME:
                {
                    string name = load3DSString(in);
                    obj->setName(name);
                    if (LOG_DEBUG) cout << "Instance name: " << name << endl;
                }
                    break;

                    //          case PRESCALE:
                    //          {
                    //              int a = 0;
                    //              int b = a;
                    //          }
                    //              break;

                case PIVOT:
                {
                    float px = in->readFloat();
                    float py = in->readFloat();
                    float pz = in->readFloat();
                    pivot = Vector3(px, py, pz);
                    if (LOG_DEBUG) LOGD("Pivot = %f,%f,%f\n", px, py, pz);
                }
                    break;

                case POS_TRACK_TAG: // 0xb020
                {
                    int flag = in->readShort();
                    int unknown1 = in->readShort();
                    int unknown2 = in->readShort();
                    int unknown3 = in->readShort();
                    int unknown4 = in->readShort();
                    int keys = in->readShort();
                    int unknown5 = in->readShort();
                    if (LOG_DEBUG) cout << "POS TAG. Flag = " << flag
                          << "; Number of key frames = " << keys
                          << "; Unknown1 = " << unknown1
                          << "; Unknown2 = " << unknown2
                          << "; Unknown3 = " << unknown3
                          << "; Unknown4 = " << unknown4
                          << "; Unknown5 = " << unknown5 << endl;
                    KeyFrame* kfs = 0;
                    if (obj->getKFCount() == 0)
                    {
                        kfs = new KeyFrame[keys];
                        obj->setKFs(kfs, keys);
                    }
                    else
                    {
                        kfs = obj->getKFs();
                    }
                    for (int i = 0; i < keys; i++)
                    {
                        int framenum = in->readShort();
                        in->readInt();
                        float posx = in->readFloat();
                        float posy = in->readFloat();
                        float posz = in->readFloat();
                        if (LOG_DEBUG) LOGD("Frame num = %d; pos = %f,%f,%f\n", framenum, posx, posy, posz);

                        KeyFrame& kf = kfs[framenum];
                        Transform trans(Transform::ETranslate, posx, posy, posz, 0);
                        kf.mTransforms.insert(kf.mTransforms.begin(), trans);
                    }
                }
                    break;

                case ROT_TRACK_TAG: // 0xb021
                {
                    int flag = in->readShort();
                    int unknown1 = in->readShort();
                    int unknown2 = in->readShort();
                    int unknown3 = in->readShort();
                    int unknown4 = in->readShort();
                    int keys = in->readShort();
                    int unknown5 = in->readShort();
                    if (LOG_DEBUG) cout << "ROT TAG. Flag = " << flag
                          << "; Number of key frames = " << keys
                          << "; Unknown1 = " << unknown1
                          << "; Unknown2 = " << unknown2
                          << "; Unknown3 = " << unknown3
                          << "; Unknown4 = " << unknown4
                          << "; Unknown5 = " << unknown5 << endl;
                    KeyFrame* kfs = 0;
                    if (obj->getKFCount() == 0)
                    {
                        kfs = new KeyFrame[keys];
                        obj->setKFs(kfs, keys);
                    }
                    else
                    {
                        kfs = obj->getKFs();
                    }
                    for (int i = 0; i < keys; i++)
                    {
                        int framenum = in->readShort();
                        in->readInt();
                        float rad = in->readFloat();
                        float axisx = in->readFloat();
                        float axisy = in->readFloat();
                        float axisz = in->readFloat();
                        if (LOG_DEBUG) LOGD("Frame num = %d; rad = %f; axis = %f,%f,%f\n", framenum, rad, axisx, axisy, axisz);
                        KeyFrame& kf = kfs[framenum];
                        Transform trans(Transform::ERotate, rad, axisx, axisy, axisz);
                        kf.mTransforms.insert(kf.mTransforms.begin(), trans);
                    }
                }
                    break;

                case SCL_TRACK_TAG: // 0xb022
                {
                    int flag = in->readShort();
                    int unknown1 = in->readShort();
                    int unknown2 = in->readShort();
                    int unknown3 = in->readShort();
                    int unknown4 = in->readShort();
                    int keys = in->readShort();
                    int unknown5 = in->readShort();
                    if (LOG_DEBUG) cout << "SCL TAG. Flag = " << flag
                          << "; Number of key frames = " << keys
                          << "; Unknown1 = " << unknown1
                          << "; Unknown2 = " << unknown2
                          << "; Unknown3 = " << unknown3
                          << "; Unknown4 = " << unknown4
                          << "; Unknown5 = " << unknown5 << endl;
                    KeyFrame* kfs = 0;
                    if (obj->getKFCount() == 0)
                    {
                        kfs = new KeyFrame[keys];
                        obj->setKFs(kfs, keys);
                    }
                    else
                    {
                        kfs = obj->getKFs();
                    }
                    for (int i = 0; i < keys; i++) {
                        int framenum = in->readShort();
                        in->readInt();
                        float scalex = in->readFloat();
                        float scaley = in->readFloat();
                        float scalez = in->readFloat();
                        if (LOG_DEBUG) LOGD("Frame num = %d; scale = %f,%f,%f\n", framenum, scalex, scaley, scalez);

                        KeyFrame& kf = kfs[framenum];
                        Transform trans(Transform::EScale, scalex, scaley, scalez, 0);
                        kf.mTransforms.insert(kf.mTransforms.begin(), trans);
                    }
                }
                    break;

                case NODE_ID: // 0xb030
                {
                    id = in->readShort();
                    if (LOG_DEBUG) cout << "ID = " << id << endl;
                }
                    break;

                default:
                    if (LOG_DEBUG) cout << "loadObjectNodeTag Skipping chunk... " << hex << chunkID << endl;
                    in->skip(chunkLength);
                    break;
            }
        }

        int kfcnt = obj->getKFCount();
        for (int i = 0;i < kfcnt;++i)
        {
            KeyFrame& kf = obj->getKFs()[i];
            
            // if pivot is not all zero add it
            if (pivot != Vector3::ZERO)
            {
                Transform trans(Transform::ETranslate, -pivot.x, -pivot.y, -pivot.z, 0);
                kf.mTransforms.insert(kf.mTransforms.begin(), trans);
            }

            kf.generateMatrix();
        }
    }

    void Houyi3dsLoader::loadCameraNodeTag(DataInputStream* in, int length) {

        if (LOG_DEBUG) cout << "Load Camera Node Tag" << endl;

        Camera* camera = NULL;
        int end = in->getPos() + length;
        while (in->getPos() < end) {
            int chunkID = in->readShort();
            int chunkLength = in->readInt() - 6;
            switch (chunkID) {
                case NODE_ID: // 0xb030
                {
                    int id = in->readShort();
                    cout << "ID = " << id << endl;
                }
                    break;

                case NODE_HDR: // 0xb010
                {
                    string name = load3DSString(in);
                    camera = mScene->getCameraByName(name);
                    int flag1 = in->readShort();
                    int flag2 = in->readShort();
                    short hierarchy = (short) in->readShort();
                    if (LOG_DEBUG) cout << "Object Name = " << name
                          << "; Flag1 = " << flag1
                          << "; Flag2 = " << flag2
                          << " Hierarchy ID = " << hierarchy << endl;
                }
                    break;

                case POS_TRACK_TAG: // 0xb020
                {
                    int flag = in->readShort();
                    int unknown1 = in->readShort();
                    int unknown2 = in->readShort();
                    int last_frame_idx = in->readShort();
                    int unknown4 = in->readShort();
                    int keys = in->readShort();
                    int max_frame_idx = in->readShort();
                    if (LOG_DEBUG) cout << "POS TAG. Flag = " << flag
                          << "; Number of key frames = " << keys
                          << "; Unknown1 = " << unknown1
                          << "; Unknown2 = " << unknown2
                          << "; Unknown3 = " << last_frame_idx
                          << "; Unknown4 = " << unknown4
                          << "; Max Frame Count = " << max_frame_idx << endl;
//                    camera->mKFPos = new float*[last_frame_idx + 1];
//                    camera->mKFPosLength = last_frame_idx + 1;
//                    camera->setMaxKFCount(last_frame_idx + 1);
//                    for (int i = 0;i < camera->mKFPosLength;i++){
//                        camera->mKFPos[i] = new float[3];
//                        camera->mKFPos[i][0] = 0;
//                        camera->mKFPos[i][1] = 0;
//                        camera->mKFPos[i][2] = 0;
//                    }
                    for (int i = 0; i < keys; i++) {
                        int framenum = in->readShort();
                        int unknown6 = in->readInt();
                        float posx = in->readFloat();
                        float posy = in->readFloat();
                        float posz = in->readFloat();
                        if (LOG_DEBUG) cout << "Frame num = " << framenum
                              << "; unknown6 = " << unknown6
                              << "; PosX = " << posx
                              << "; PosY = " << posy
                              << "; PosZ = " << posz << endl;

                        if (camera != NULL) {
//                            camera->setKFPos(framenum, posx, posy, posz);
                        }
                    }
                }
                    break;

                case FOV_TRACK_TAG: // 0xb023
                {
                    int flag = in->readShort();
                    int unknown1 = in->readShort();
                    int unknown2 = in->readShort();
                    int unknown3 = in->readShort();
                    int unknown4 = in->readShort();
                    int keys = in->readShort();
                    int unknown5 = in->readShort();
                    if (LOG_DEBUG) cout << "FOV TAG. Flag = " << flag
                          << "; Number of key frames = " << keys
                          << "; Unknown1 = " << unknown1
                          << "; Unknown2 = " << unknown2
                          << "; Unknown3 = " << unknown3
                          << "; Unknown4 = " << unknown4
                          << "; Unknown5 = " << unknown5 << endl;
                    for (int i = 0; i < keys; i++) {
                        int framenum = in->readShort();
                        int unknown6 = in->readInt();
                        float fov = in->readFloat();
                        camera->setFOV(fov);

                        if (LOG_DEBUG) cout << "Frame num = " << framenum
                              << "; unknown6 = " << unknown6
                              << "; FOV = " << fov << endl;
                    }
                }
                    break;

                case ROLL_TRACK_TAG: // 0xb024
                {
                    int flag = in->readShort();
                    int unknown1 = in->readShort();
                    int unknown2 = in->readShort();
                    int unknown3 = in->readShort();
                    int unknown4 = in->readShort();
                    int keys = in->readShort();
                    int unknown5 = in->readShort();
                    if (LOG_DEBUG) cout << "Roll TAG. Flag = " << flag
                          << "; Number of key frames = " << keys
                          << "; Unknown1 = " << unknown1
                          << "; Unknown2 = " << unknown2
                          << "; Unknown3 = " << unknown3
                          << "; Unknown4 = " << unknown4
                          << "; Unknown5 = " << unknown5 << endl;
                    for (int i = 0; i < keys; i++) {
                        int framenum = in->readShort();
                        int unknown6 = in->readInt();
                        float roll = in->readFloat();
                        camera->setRoll(roll);

                        if (LOG_DEBUG) cout << "Frame num = " << framenum
                              << "; unknown6 = " << unknown6
                              << "; Roll = " << roll << endl;
                    }
                }
                    break;

                default:
                    cout <<"loadCameraNodeTag Skipping chunk... " << hex << chunkID << endl;
                    in->skip(chunkLength);
                    break;
            }
        }

        if (camera != NULL) {
            camera->lookAt();
        }
    }

    void Houyi3dsLoader::loadTargetNodeTag(DataInputStream* in, int length)
    {
        if (LOG_DEBUG) cout << "Load Target Node Tag" << endl;

        Camera* camera = NULL;
        int end = in->getPos() + length;
        while (in->getPos() < end) {
            int chunkID = in->readShort();
            int chunkLength = in->readInt() - 6;
            switch (chunkID) {
                case NODE_ID: // 0xb030
                {
                    int id = in->readShort();
                    cout << "ID = " << id << endl;
                }
                    break;

                case NODE_HDR: // 0xb010
                {
                    string name = load3DSString(in);
                    camera = mScene->getCameraByName(name);
                    int flag1 = in->readShort();
                    int flag2 = in->readShort();
                    short hierarchy = (short) in->readShort();
                    if (LOG_DEBUG) cout << "Object Name = " << name
                          << "; Flag1 = " << flag1
                          << "; Flag2 = " << flag2
                          << " Hierarchy ID = " << hierarchy << endl;
                }
                    break;

                case POS_TRACK_TAG: // 0xb020
                {
                    int flag = in->readShort();
                    int unknown1 = in->readShort();
                    int unknown2 = in->readShort();
                    int last_frame_idx = in->readShort();
                    int unknown4 = in->readShort();
                    int keys = in->readShort();
                    int max_frame_idx = in->readShort();
                    if (LOG_DEBUG) cout << "POS TAG. Flag = " << flag
                          << "; Number of key frames = " << keys
                          << "; Unknown1 = " << unknown1
                          << "; Unknown2 = " << unknown2
                          << "; Unknown3 = " << last_frame_idx
                          << "; Unknown4 = " << unknown4
                          << "; Max Frame Count = " << max_frame_idx << endl;
                    camera->mKFTargetPos = new float*[last_frame_idx + 1];
                    camera->mKFTargetPosLength = last_frame_idx + 1;
//                    camera->setMaxKFCount(last_frame_idx + 1);
                    for (int i = 0;i < camera->mKFTargetPosLength;i++){
                        camera->mKFTargetPos[i] = new float[3];
                        camera->mKFTargetPos[i][0] = 0;
                        camera->mKFTargetPos[i][1] = 0;
                        camera->mKFTargetPos[i][2] = 0;
                    }
                    for (int i = 0; i < keys; i++) {
                        int framenum = in->readShort();
                        int unknown6 = in->readInt();
                        float posx = in->readFloat();
                        float posy = in->readFloat();
                        float posz = in->readFloat();
                        if (LOG_DEBUG) cout << "Frame num = " << framenum
                              << "; unknown6 = " << unknown6
                              << "; PosX = " << posx
                              << "; PosY = " << posy
                              << "; PosZ = " << posz << endl;

                        if (camera != NULL) {
                            camera->setKFTargetPos(framenum, posx, posy, posz);
                        }
                    }
                }
                    break;

                default:
                    if (LOG_DEBUG) cout << "loadTargetNodeTag Skipping chunk... " << hex << chunkID << endl;
                    in->skip(chunkLength);
                    break;
            }
        }

        if (camera != NULL) {
            camera->lookAt();
        }
    }

    void Houyi3dsLoader::loadLightNodeTag(DataInputStream* in, int length)
    {
        if (LOG_DEBUG) cout << "Load Light Node Tag" << endl;

        Light* light = NULL;
        int end = in->getPos() + length;
        while (in->getPos() < end) {
            int chunkID = in->readShort();
            int chunkLength = in->readInt() - 6;
            switch (chunkID) {
                case NODE_ID: // 0xb030
                {
                    int id = in->readShort();
                    if (LOG_DEBUG) cout << "ID = " << id << endl;
                }
                    break;

                case NODE_HDR: // 0xb010
                {
                    string name = load3DSString(in);
                    light = mScene->getLightByName(name);
                    int flag1 = in->readShort();
                    int flag2 = in->readShort();
                    short hierarchy = (short) in->readShort();
                    if (LOG_DEBUG) cout << "Object Name = " << name
                    << "; Flag1 = " << flag1
                    << "; Flag2 = " << flag2
                    << " Hierarchy ID = " << hierarchy << endl;
                }
                    break;

                case POS_TRACK_TAG: // 0xb020
                {
                    int flag = in->readShort();
                    int unknown1 = in->readShort();
                    int unknown2 = in->readShort();
                    int last_frame_idx = in->readShort();
                    int unknown4 = in->readShort();
                    int keys = in->readShort();
                    int max_frame_idx = in->readShort();
                    if (LOG_DEBUG) cout << "POS TAG. Flag = " << flag
                    << "; Number of key frames = " << keys
                    << "; Unknown1 = " << unknown1
                    << "; Unknown2 = " << unknown2
                    << "; Unknown3 = " << last_frame_idx
                    << "; Unknown4 = " << unknown4
                    << "; Max Frame Count = " << max_frame_idx << endl;
//                    light->mKFPos = new float*[last_frame_idx + 1];
//                    light->mKFPosLength = last_frame_idx + 1;
//                    light->setMaxKFCount(last_frame_idx + 1);
//                    for (int i = 0;i < light->mKFPosLength;i++){
//                        light->mKFPos[i] = new float[3];
//                        light->mKFPos[i][0] = 0;
//                        light->mKFPos[i][1] = 0;
//                        light->mKFPos[i][2] = 0;
//                    }
                    for (int i = 0; i < keys; i++) {
                        int framenum = in->readShort();
                        int unknown6 = in->readInt();
                        float posx = in->readFloat();
                        float posy = in->readFloat();
                        float posz = in->readFloat();
                        if (LOG_DEBUG) cout << "Frame num = " << framenum
                        << "; unknown6 = " << unknown6
                        << "; PosX = " << posx
                        << "; PosY = " << posy
                        << "; PosZ = " << posz << endl;

                        if (light != NULL) {
//                            light->setKFPos(framenum, posx, posy, posz);
                        }
                    }
                }
                    break;

                default:
                    if (LOG_DEBUG) cout << "loadTargetNodeTag Skipping chunk... " << hex << chunkID << endl;
                    in->skip(chunkLength);
                    break;
            }
        }
    }

    string Houyi3dsLoader::load3DSString(DataInputStream* in)
    {
        string res;
        char c = in->readASCIIChar();
        while (c != 0) {
            res += c;
            c = in->readASCIIChar();
        }

        return res;
    }

    int Houyi3dsLoader::loadColor(DataInputStream* in, int length)
    {
        int res = 0;
        int end = in->getPos() + length;
        while (in->getPos() < end) {
            int chunkID = in->readShort();
            int chunkLength = in->readInt() - 6;
            switch (chunkID) {
                case COLOR_24: {
                    int r = in->readUnsignedByte() & 0xff;
                    int g = in->readUnsignedByte() & 0xff;
                    int b = in->readUnsignedByte() & 0xff;
                    res = b | (g << 8) | (r << 16);
                }
                    break;

                case COLOR_F: {
                    float r = in->readFloat();
                    float g = in->readFloat();
                    float b = in->readFloat();

                    res = ((int) (b * 255)) | ((int) (g * 255) << 8)
                    | ((int) (r * 255) << 16);
                }
                    break;

                default:
                    in->skip(chunkLength);
                    break;
            }
        }

        return res | 0xff000000;
    }

    float Houyi3dsLoader::loadPercentage(DataInputStream* in, int length)
    {
        float res = 0;
        int end = in->getPos() + length;
        while (in->getPos() < end) {
            int chunkID = in->readShort();
            int chunkLength = in->readInt() - 6;
            switch (chunkID) {
                case INT_PERCENTAGE:
                {
                    int per = in->readShort();
                    res = per / (float) 100;
                }
                    break;

                case FLOAT_PERCENTAGE:
                    res = in->readFloat();
                    break;

                default:
                    in->skip(chunkLength);
                    break;
            }
        }

        return res;
    }

    SceneNode* Houyi3dsLoader::findNodeByName(const string& name)
    {
        for (int i = 0;i < mNodes.size();++i)
        {
            if (name.compare(mNodes[i]->getName()) == 0)
            {
                return mNodes[i];
            }
        }
        return 0;
    }
}
