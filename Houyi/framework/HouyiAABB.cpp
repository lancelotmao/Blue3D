#include "HouyiAABB.h"
#include "HouyiSceneNode.h"
#include "HouyiMaterialManager.h"
#include "HouyiScene.h"

namespace Houyi
{
    AABB::AABB(Scene* scene) : mIsValid(false), mIsAddedToWorld(false)
    {
        setName("AABB");
        mScene = scene;
    }
    
    AABB::~AABB()
    {
    }
    
    void AABB::init()
    {
        if (mMesh)
        {
            // already has data
            return;
        }
        
        mVisibility = EInvisible;
        setSelectable(false);
        addMaterial(&mScene->getMaterialManager()->getWireFrame());
        
        mMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
        mMax = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        
        mMesh = mScene->createMesh();
        mMesh->setName("AABB Mesh");
        SurfacePtr surface = new Surface();
        surface->setRenderMode(Const::ERenderModeLine);
        surface->setMesh(mMesh);
        VertexBufferPtr vb = new VertexBuffer(VertexBuffer::EExpandAsLines, 0);
        float* expand = new float[144];
        vb->setPosition(expand,144);
        vb->setUseVBO(false);
        surface->setRawVertexBuffer(vb);
        mMesh->addSurface(surface);
        IndexBufferPtr ib = new IndexBuffer();
        uint* indices = new uint[48];
        for (int i = 0;i < 48;++i)
        {
            indices[i] = i;
        }
        ib->setLineIndices(indices, 48);
        surface->setIndexBuffer(ib);
        
        setMesh(mMesh);
    }
    
    void AABB::reset()
    {
        mMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
        mMax = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    }

    void AABB::merge(SceneNode* node)
    {
        if (!node || !node->getAABB())
        {
            return;
        }

        AABB* another = node->getAABB();
        float mySpace[8 * 3];
        Matrix4 local = node->getLocal();
        const float* vertices = another->getAABBVertices();
        for (int i = 0;i < 8;++i)
        {
            Vector3 v(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
            v = local * v;
            mySpace[i * 3] = v.x;
            mySpace[i * 3 + 1] = v.y;
            mySpace[i * 3 + 2] = v.z;
        }
        Vector3 min;
        Vector3 max;
        calculateMinMax(min, max, mySpace, 24);
        mergeMinMax(mMin, mMax, min, max, mMin, mMax);
        fillDataUsingMinMax(mMin, mMax);
        updateRenderVB();
        updateCollisionVB();
    }

    void AABB::mergeAABB(AABB* aabb)
    {
    	if (!aabb)
		{
			return;
		}

		const float* vertices = aabb->getAABBVertices();
		Vector3 min;
		Vector3 max;
		calculateMinMax(min, max, vertices, 24);
		mergeMinMax(mMin, mMax, min, max, mMin, mMax);
		fillDataUsingMinMax(mMin, mMax);
		updateRenderVB();
		updateCollisionVB();
    }

    void AABB::updateRenderVB()
    {
        // expand 8 AABB vertices into some nice shape
        float* expand = mMesh->getSurface(0)->getRawVertexBuffer()->getPosition();
        float lenx = Math::Abs((mAABBVertices[21] - mAABBVertices[0]) / 4);
        float leny = Math::Abs((mAABBVertices[19] - mAABBVertices[1]) / 4);
        float lenz = Math::Abs((mAABBVertices[20] - mAABBVertices[2]) / 4);
        expand[0] = mAABBVertices[0];
        expand[1] = mAABBVertices[1];
        expand[2] = mAABBVertices[2];
        expand[3] = mAABBVertices[0] + lenx;
        expand[4] = mAABBVertices[1];
        expand[5] = mAABBVertices[2];
        expand[6] = mAABBVertices[0];
        expand[7] = mAABBVertices[1];
        expand[8] = mAABBVertices[2];
        expand[9] = mAABBVertices[0];
        expand[10] = mAABBVertices[1] + leny;
        expand[11] = mAABBVertices[2];
        expand[12] = mAABBVertices[0];
        expand[13] = mAABBVertices[1];
        expand[14] = mAABBVertices[2];
        expand[15] = mAABBVertices[0];
        expand[16] = mAABBVertices[1];
        expand[17] = mAABBVertices[2] + lenz;

        expand[18] = mAABBVertices[3];
        expand[19] = mAABBVertices[4];
        expand[20] = mAABBVertices[5];
        expand[21] = mAABBVertices[3] + lenx;
        expand[22] = mAABBVertices[4];
        expand[23] = mAABBVertices[5];
        expand[24] = mAABBVertices[3];
        expand[25] = mAABBVertices[4];
        expand[26] = mAABBVertices[5];
        expand[27] = mAABBVertices[3];
        expand[28] = mAABBVertices[4] - leny;
        expand[29] = mAABBVertices[5];
        expand[30] = mAABBVertices[3];
        expand[31] = mAABBVertices[4];
        expand[32] = mAABBVertices[5];
        expand[33] = mAABBVertices[3];
        expand[34] = mAABBVertices[4];
        expand[35] = mAABBVertices[5] + lenz;

        expand[36] = mAABBVertices[6];
        expand[37] = mAABBVertices[7];
        expand[38] = mAABBVertices[8];
        expand[39] = mAABBVertices[6] - lenx;
        expand[40] = mAABBVertices[7];
        expand[41] = mAABBVertices[8];
        expand[42] = mAABBVertices[6];
        expand[43] = mAABBVertices[7];
        expand[44] = mAABBVertices[8];
        expand[45] = mAABBVertices[6];
        expand[46] = mAABBVertices[7] - leny;
        expand[47] = mAABBVertices[8];
        expand[48] = mAABBVertices[6];
        expand[49] = mAABBVertices[7];
        expand[50] = mAABBVertices[8];
        expand[51] = mAABBVertices[6];
        expand[52] = mAABBVertices[7];
        expand[53] = mAABBVertices[8] + lenz;

        expand[54] = mAABBVertices[9];
        expand[55] = mAABBVertices[10];
        expand[56] = mAABBVertices[11];
        expand[57] = mAABBVertices[9] - lenx;
        expand[58] = mAABBVertices[10];
        expand[59] = mAABBVertices[11];
        expand[60] = mAABBVertices[9];
        expand[61] = mAABBVertices[10];
        expand[62] = mAABBVertices[11];
        expand[63] = mAABBVertices[9];
        expand[64] = mAABBVertices[10] + leny;
        expand[65] = mAABBVertices[11];
        expand[66] = mAABBVertices[9];
        expand[67] = mAABBVertices[10];
        expand[68] = mAABBVertices[11];
        expand[69] = mAABBVertices[9];
        expand[70] = mAABBVertices[10];
        expand[71] = mAABBVertices[11] + lenz;

        expand[72] = mAABBVertices[12];
        expand[73] = mAABBVertices[13];
        expand[74] = mAABBVertices[14];
        expand[75] = mAABBVertices[12] + lenx;
        expand[76] = mAABBVertices[13];
        expand[77] = mAABBVertices[14];
        expand[78] = mAABBVertices[12];
        expand[79] = mAABBVertices[13];
        expand[80] = mAABBVertices[14];
        expand[81] = mAABBVertices[12];
        expand[82] = mAABBVertices[13] + leny;
        expand[83] = mAABBVertices[14];
        expand[84] = mAABBVertices[12];
        expand[85] = mAABBVertices[13];
        expand[86] = mAABBVertices[14];
        expand[87] = mAABBVertices[12];
        expand[88] = mAABBVertices[13];
        expand[89] = mAABBVertices[14] - lenz;

        expand[90] = mAABBVertices[15];
        expand[91] = mAABBVertices[16];
        expand[92] = mAABBVertices[17];
        expand[93] = mAABBVertices[15] + lenx;
        expand[94] = mAABBVertices[16];
        expand[95] = mAABBVertices[17];
        expand[96] = mAABBVertices[15];
        expand[97] = mAABBVertices[16];
        expand[98] = mAABBVertices[17];
        expand[99] = mAABBVertices[15];
        expand[100] = mAABBVertices[16] - leny;
        expand[101] = mAABBVertices[17];
        expand[102] = mAABBVertices[15];
        expand[103] = mAABBVertices[16];
        expand[104] = mAABBVertices[17];
        expand[105] = mAABBVertices[15];
        expand[106] = mAABBVertices[16];
        expand[107] = mAABBVertices[17] - lenz;

        expand[108] = mAABBVertices[18];
        expand[109] = mAABBVertices[19];
        expand[110] = mAABBVertices[20];
        expand[111] = mAABBVertices[18] - lenx;
        expand[112] = mAABBVertices[19];
        expand[113] = mAABBVertices[20];
        expand[114] = mAABBVertices[18];
        expand[115] = mAABBVertices[19];
        expand[116] = mAABBVertices[20];
        expand[117] = mAABBVertices[18];
        expand[118] = mAABBVertices[19] - leny;
        expand[119] = mAABBVertices[20];
        expand[120] = mAABBVertices[18];
        expand[121] = mAABBVertices[19];
        expand[122] = mAABBVertices[20];
        expand[123] = mAABBVertices[18];
        expand[124] = mAABBVertices[19];
        expand[125] = mAABBVertices[20] - lenz;

        expand[126] = mAABBVertices[21];
        expand[127] = mAABBVertices[22];
        expand[128] = mAABBVertices[23];
        expand[129] = mAABBVertices[21] - lenx;
        expand[130] = mAABBVertices[22];
        expand[131] = mAABBVertices[23];
        expand[132] = mAABBVertices[21];
        expand[133] = mAABBVertices[22];
        expand[134] = mAABBVertices[23];
        expand[135] = mAABBVertices[21];
        expand[136] = mAABBVertices[22] + leny;
        expand[137] = mAABBVertices[23];
        expand[138] = mAABBVertices[21];
        expand[139] = mAABBVertices[22];
        expand[140] = mAABBVertices[23];
        expand[141] = mAABBVertices[21];
        expand[142] = mAABBVertices[22];
        expand[143] = mAABBVertices[23] - lenz;

        // either through generate or merge
        mIsValid = true;
    }

    void AABB::updateCollisionVB()
    {
        if (!mCollisionVB)
        {
            int vcnt = 36; // 2 triangles each face. 6 faces. so 2 * 6 * 3
            float* collision = new float[vcnt * 3];
            mCollisionVB = new VertexBuffer(VertexBuffer::EExpandAsTriangles, 0);
            mCollisionVB->setPosition(collision, vcnt * 3);
        }
        
        float* expand = mCollisionVB->getPosition();

        // back
        expand[0] = mAABBVertices[0];
        expand[1] = mAABBVertices[1];
        expand[2] = mAABBVertices[2];
        expand[3] = mAABBVertices[3];
        expand[4] = mAABBVertices[4];
        expand[5] = mAABBVertices[5];
        expand[6] = mAABBVertices[6];
        expand[7] = mAABBVertices[7];
        expand[8] = mAABBVertices[8];
        expand[9] = mAABBVertices[0];
        expand[10] = mAABBVertices[1];
        expand[11] = mAABBVertices[2];
        expand[12] = mAABBVertices[6];
        expand[13] = mAABBVertices[7];
        expand[14] = mAABBVertices[8];
        expand[15] = mAABBVertices[9];
        expand[16] = mAABBVertices[10];
        expand[17] = mAABBVertices[11];

        // top
        expand[18] = mAABBVertices[3];
        expand[19] = mAABBVertices[4];
        expand[20] = mAABBVertices[5];
        expand[21] = mAABBVertices[15];
        expand[22] = mAABBVertices[16];
        expand[23] = mAABBVertices[17];
        expand[24] = mAABBVertices[18];
        expand[25] = mAABBVertices[19];
        expand[26] = mAABBVertices[20];
        expand[27] = mAABBVertices[18];
        expand[28] = mAABBVertices[19];
        expand[29] = mAABBVertices[20];
        expand[30] = mAABBVertices[6];
        expand[31] = mAABBVertices[7];
        expand[32] = mAABBVertices[8];
        expand[33] = mAABBVertices[3];
        expand[34] = mAABBVertices[4];
        expand[35] = mAABBVertices[5];

        // front
        expand[36] = mAABBVertices[18];
        expand[37] = mAABBVertices[19];
        expand[38] = mAABBVertices[20];
        expand[39] = mAABBVertices[15];
        expand[40] = mAABBVertices[16];
        expand[41] = mAABBVertices[17];
        expand[42] = mAABBVertices[12];
        expand[43] = mAABBVertices[13];
        expand[44] = mAABBVertices[14];
        expand[45] = mAABBVertices[12];
        expand[46] = mAABBVertices[13];
        expand[47] = mAABBVertices[14];
        expand[48] = mAABBVertices[21];
        expand[49] = mAABBVertices[22];
        expand[50] = mAABBVertices[23];
        expand[51] = mAABBVertices[18];
        expand[52] = mAABBVertices[19];
        expand[53] = mAABBVertices[20];

        // bottom
        expand[54] = mAABBVertices[12];
        expand[55] = mAABBVertices[13];
        expand[56] = mAABBVertices[14];
        expand[57] = mAABBVertices[0];
        expand[58] = mAABBVertices[1];
        expand[59] = mAABBVertices[2];
        expand[60] = mAABBVertices[9];
        expand[61] = mAABBVertices[10];
        expand[62] = mAABBVertices[11];
        expand[63] = mAABBVertices[9];
        expand[64] = mAABBVertices[10];
        expand[65] = mAABBVertices[11];
        expand[66] = mAABBVertices[21];
        expand[67] = mAABBVertices[22];
        expand[68] = mAABBVertices[23];
        expand[69] = mAABBVertices[12];
        expand[70] = mAABBVertices[13];
        expand[71] = mAABBVertices[14];

        // left
        expand[72] = mAABBVertices[0];
        expand[73] = mAABBVertices[1];
        expand[74] = mAABBVertices[2];
        expand[75] = mAABBVertices[3];
        expand[76] = mAABBVertices[4];
        expand[77] = mAABBVertices[5];
        expand[78] = mAABBVertices[15];
        expand[79] = mAABBVertices[16];
        expand[80] = mAABBVertices[17];
        expand[81] = mAABBVertices[15];
        expand[82] = mAABBVertices[16];
        expand[83] = mAABBVertices[17];
        expand[84] = mAABBVertices[12];
        expand[85] = mAABBVertices[13];
        expand[86] = mAABBVertices[14];
        expand[87] = mAABBVertices[0];
        expand[88] = mAABBVertices[1];
        expand[89] = mAABBVertices[2];

        // right
        expand[90] = mAABBVertices[18];
        expand[91] = mAABBVertices[19];
        expand[92] = mAABBVertices[20];
        expand[93] = mAABBVertices[21];
        expand[94] = mAABBVertices[22];
        expand[95] = mAABBVertices[23];
        expand[96] = mAABBVertices[9];
        expand[97] = mAABBVertices[10];
        expand[98] = mAABBVertices[11];
        expand[99] = mAABBVertices[9];
        expand[100] = mAABBVertices[10];
        expand[101] = mAABBVertices[11];
        expand[102] = mAABBVertices[6];
        expand[103] = mAABBVertices[7];
        expand[104] = mAABBVertices[8];
        expand[105] = mAABBVertices[18];
        expand[106] = mAABBVertices[19];
        expand[107] = mAABBVertices[20];
    }

    void AABB::generate(Mesh* mesh)
    {
        init();
        
        if (!mesh)
        {
            return;
        }

        int surfaceCnt = mesh->getSurfaceCount();
        if (surfaceCnt == 0)
        {
            LOGE("MeshOptimizer::generateAABB. Mesh has no surface\n");
            return;
        }

        set<VertexBufferPtr> updatedSet;
        set<VertexBufferPtr>::iterator it;
        for (int i = 0;i < surfaceCnt;++i)
        {
            SurfacePtr surface = mesh->getSurface(i);
            VertexBufferPtr vb = surface->getVertexBuffer();
            if (!vb)
            {
                continue;
            }

            it = updatedSet.find(vb);
            if (it != updatedSet.end())
            {
                continue;
            }

            updatedSet.insert(vb);

            Vector3 min, max;
            calculateMinMax(min, max, vb->getPosition(), vb->getPositionLength());
            mergeMinMax(mMin, mMax, min, max, mMin, mMax);
        }
        
        fillDataUsingMinMax(mMin, mMax);

        updateRenderVB();
        updateCollisionVB();
    }

    void AABB::generate(SurfacePtr surface)
    {
		VertexBufferPtr vb = surface->getVertexBuffer();
		if (!vb)
		{
			return;
		}

		Vector3 min, max;
		calculateMinMax(min, max, vb->getPosition(), vb->getPositionLength());
		mergeMinMax(mMin, mMax, min, max, mMin, mMax);

		fillDataUsingMinMax(mMin, mMax);

		updateRenderVB();
		updateCollisionVB();
    }

    void AABB::calculateMinMax(Vector3& min, Vector3& max, const float* data, int length)
    {
        min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
        max = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        int vcnt = length / 3;
        for (int j = 0;j < vcnt;j++)
        {
            Vector3 vdst(data[j * 3], data[j * 3 + 1], data[j * 3 + 2]);
            if (vdst.x < min.x){
                min.x = vdst.x;
            }
            if (vdst.y < min.y){
                min.y = vdst.y;
            }
            if (vdst.z < min.z){
                min.z = vdst.z;
            }
            if (vdst.x > max.x){
                max.x = vdst.x;
            }
            if (vdst.y > max.y){
                max.y = vdst.y;
            }
            if (vdst.z > max.z){
                max.z = vdst.z;
            }
        }
    }

    void AABB::fillDataUsingMinMax(const Vector3& min, const Vector3& max)
    {
        float minX = min.x;
        float minY = min.y;
        float minZ = min.z;
        float maxX = max.x;
        float maxY = max.y;
        float maxZ = max.z;

        // offset a little bit to avoid z fighting
        float deltaX = maxX - minX;
        float deltaY = maxY - minY;
        float deltaZ = maxZ - minZ;
        minX -= 0.001f * deltaX;
        minY -= 0.001f * deltaY;
        minZ -= 0.001f * deltaZ;
        maxX += 0.001f * deltaX;
        maxY += 0.001f * deltaY;
        maxZ += 0.001f * deltaZ;

        // back
        mAABBVertices[0] = minX;
        mAABBVertices[1] = minY;
        mAABBVertices[2] = minZ;
        mAABBVertices[3] = minX;
        mAABBVertices[4] = maxY;
        mAABBVertices[5] = minZ;
        mAABBVertices[6] = maxX;
        mAABBVertices[7] = maxY;
        mAABBVertices[8] = minZ;
        mAABBVertices[9] = maxX;
        mAABBVertices[10] = minY;
        mAABBVertices[11] = minZ;

        //front
        mAABBVertices[12] = minX;
        mAABBVertices[13] = minY;
        mAABBVertices[14] = maxZ;
        mAABBVertices[15] = minX;
        mAABBVertices[16] = maxY;
        mAABBVertices[17] = maxZ;
        mAABBVertices[18] = maxX;
        mAABBVertices[19] = maxY;
        mAABBVertices[20] = maxZ;
        mAABBVertices[21] = maxX;
        mAABBVertices[22] = minY;
        mAABBVertices[23] = maxZ;
    }

    void AABB::mergeMinMax(Vector3& min, Vector3& max, const Vector3& min1, const Vector3& max1, const Vector3& min2, const Vector3& max2)
    {
        min.x = min1.x < min2.x ? min1.x : min2.x;
        min.y = min1.y < min2.y ? min1.y : min2.y;
        min.z = min1.z < min2.z ? min1.z : min2.z;

        max.x = max1.x > max2.x ? max1.x : max2.x;
        max.y = max1.y > max2.y ? max1.y : max2.y;
        max.z = max1.z > max2.z ? max1.z : max2.z;
    }
    
    SceneNode* AABB::clone()
    {
        AABB* aabb = HouyiNew AABB(mScene);
        (*aabb) = *this;
        aabb->mIsAddedToWorld = false;

        for (int i = 0;i < mEntities.size();++i)
        {
            RenderEntity e = mEntities[i];
            e.mNode = aabb;
            aabb->addEntity(e);
        }
        return aabb;
    }
}
