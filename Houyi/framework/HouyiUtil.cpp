//
//  HouyiUtils.cpp
//  Houyi
//
//  Created by Lance Mao on 18/12/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#include "HouyiUtil.h"
#include "HouyiAABB.h"
#include "HouyiMesh.h"

namespace Houyi
{
    int Util::nextPowerOf2(int n)
    {
        if (n <= 0 || n > (1 << 30))
        {
            return -1;
        }
        
        n -= 1;
        n |= n >> 16;
        n |= n >> 8;
        n |= n >> 4;
        n |= n >> 2;
        n |= n >> 1;
        return n + 1;
    }
    
    int Util::prePowerOf2(int n)
    {
        if (n <= 0 || n > (1 << 30))
        {
            return -1;
        }
        
        int result = 1;
        while ((result << 1) < n)
            result <<= 1;
        
        return result;
    }
    
    int Util::roundPowerOf2(int n)
    {
        int next = nextPowerOf2(n);
        int pre = prePowerOf2(n);
        return next - n < n - pre ? next : pre;
    }
    
    float Util::ClampToZeroOne(float value)
    {
        if (value < .0f)
            return .0f;
        else if (value > 1.0f)
            return 1.0f;
        else
            return value;
    }
    
    float Util::ApproximateCubicBezierParameter (float atX, float P0_X, float C0_X, float P1_X, float C1_X )
    {
        static const float APPROXIMATION_EPSILON = 1.0e-09;
        static const float VERYSMALL = 1.0e-20;
        static const int MAXIMUM_ITERATIONS = 512;
        
        if (atX - P0_X < VERYSMALL)
            return 0.0;
        
        if (P1_X - atX < VERYSMALL)
            return 1.0;
        
        long iterationStep = 0;
        
        float u = 0.0f; float v = 1.0f;
        
        //iteratively apply subdivision to approach value atX
        while (iterationStep < MAXIMUM_ITERATIONS) {
            
            // de Casteljau Subdivision.
            double a = (P0_X + C0_X)*0.5f;
            double b = (C0_X + C1_X)*0.5f;
            double c = (C1_X + P1_X)*0.5f;
            double d = (a + b)*0.5f;
            double e = (b + c)*0.5f;
            double f = (d + e)*0.5f; //this one is on the curve!
            
            //The curve point is close enough to our wanted atX
            if (fabs(f - atX) < APPROXIMATION_EPSILON)
                return ClampToZeroOne((u + v)*0.5f);
            
            //dichotomy
            if (f < atX) {
                P0_X = f;
                C0_X = e;
                C1_X = c;
                u = (u + v)*0.5f;
            } else {
                C0_X = a; C1_X = d; P1_X = f; v = (u + v)*0.5f;
            }
            
            iterationStep++;
        }
        
        return ClampToZeroOne((u + v)*0.5f);
    }
    
    float Util::collisionTest(const RenderEntity& entity, Camera* camera, float screenX, float screenY, float viewPortWidth, float viewPortHeight, bool onlyAABB)
    {
        SceneNode* node = entity.mNode;
        float ndcX = screenX * 2 / viewPortWidth - 1;
        float ndcY = 1 - screenY * 2 / viewPortHeight;
        Ray ray = camera->castRay(ndcX, ndcY);
        AABB* aabb = node->getAABB();
        if (aabb && aabb->getCollisionVB())
        {
            VertexBufferPtr vbAABB = aabb->getCollisionVB();
            float* pos = vbAABB->getPosition();
            int trianglecnt = vbAABB->getPositionLength() / 9;
            if (trianglecnt <= 0)
            {
                // dummy mesh
                return FLT_MAX;
            }

            // first test with AABB, if not even collides with AABB no need to test each triangle
            bool collidesWithAABB = false;
            Vector3 intersect;
            for (int k = 0;k < trianglecnt;k++)
            {
                Vector3 mv0 = Vector3(pos[k * 9], pos[k * 9 + 1], pos[k * 9 + 2]);
                Vector3 mv1 = Vector3(pos[k * 9 + 3], pos[k * 9 + 4], pos[k * 9 + 5]);
                Vector3 mv2 = Vector3(pos[k * 9 + 6], pos[k * 9 + 7], pos[k * 9 + 8]);
                Vector3 wv0 = node->getLocal2World() * mv0;
                Vector3 wv1 = node->getLocal2World() * mv1;
                Vector3 wv2 = node->getLocal2World() * mv2;
                Vector3 n = (wv1 - wv0).crossProduct(wv2 - wv0);
                pair<bool, Real> dis = Math::intersects(ray, wv0, wv1, wv2, n);
                if (dis.first)
                {
                    collidesWithAABB = true;
                    break;
                }
            }

            if (!collidesWithAABB)
            {
                return FLT_MAX;
            }

            if (onlyAABB)
            {
                return intersect.x;
            }
        }
        
        float nearest = FLT_MAX;
        SurfacePtr surface = entity.mSurface;
        if (!surface)
        {
            return FLT_MAX;
        }
        VertexBufferPtr vb = surface->getVertexBuffer();
        if (!vb)
        {
            return FLT_MAX;
        }
        IndexBufferPtr ib = surface->getIndexBuffer();
        float* pos = vb->getPosition();
        if (ib)
        {
            int trianglecnt = ib->getTriangleIndicesLength() / 3;
            uint* indices = ib->getIntTriangleIndices();
            if (indices)
            {
                for (int k = 0;k < trianglecnt;k++)
                {
                    int idx1 = indices[k * 3];
                    int idx2 = indices[k * 3 + 1];
                    int idx3 = indices[k * 3 + 2];
                    Vector3 mv0 = Vector3(pos[idx1 * 3], pos[idx1 * 3 + 1], pos[idx1 * 3 + 2]);
                    Vector3 mv1 = Vector3(pos[idx2 * 3], pos[idx2 * 3 + 1], pos[idx2 * 3 + 2]);
                    Vector3 mv2 = Vector3(pos[idx3 * 3], pos[idx3 * 3 + 1], pos[idx3 * 3 + 2]);
                    Vector3 wv0 = node->getLocal2World() * mv0;
                    Vector3 wv1 = node->getLocal2World() * mv1;
                    Vector3 wv2 = node->getLocal2World() * mv2;
                    Vector3 n = (wv1 - wv0).crossProduct(wv2 - wv0);
                    pair<bool, Real> dis = Math::intersects(ray, wv0, wv1, wv2, n);
                    if (dis.first)
                    {
                        //                    cout << "t = " << intersect.x << " u = " << intersect.y  << " v = " << intersect.z << endl;
                        if (dis.second < nearest)
                        {
                            nearest = dis.second;
                        }
                    }
                }
            }
        }
        else
        {
            int trianglecnt = vb->getPositionLength() / 9;
            for (int k = 0;k < trianglecnt;k++)
            {
                Vector3 mv0 = Vector3(pos[k * 9], pos[k * 9 + 1], pos[k * 9 + 2]);
                Vector3 mv1 = Vector3(pos[k * 9 + 3], pos[k * 9 + 4], pos[k * 9 + 5]);
                Vector3 mv2 = Vector3(pos[k * 9 + 6], pos[k * 9 + 7], pos[k * 9 + 8]);
                Vector3 wv0 = node->getLocal2World() * mv0;
                Vector3 wv1 = node->getLocal2World() * mv1;
                Vector3 wv2 = node->getLocal2World() * mv2;
                Vector3 intersect;
                if (Math::rayTriangleIntersect(intersect, ray.getOrigin(), ray.getDirection(), wv0, wv1, wv2))
                {
                    //                    cout << "t = " << intersect.x << " u = " << intersect.y  << " v = " << intersect.z << endl;
                    if (intersect.x < nearest)
                    {
                        nearest = intersect.x;
                    }
                }
            }
        }
        
        return nearest;
    }

    uint Util::hashVertex(const VertexData& data)
    {
        const Vector3& pos = data.position;
        const Vector3& n = data.normal;
        const Vector2& tex = data.texcoord;
        const Vector4& bone = data.boneIndex;
        const Vector3& tangent = data.tangent;
        uint* px = (uint*)&(pos.x);
        uint* py = (uint*)&(pos.y);
        uint* pz = (uint*)&(pos.z);
        uint* nx = (uint*)&(n.x);
        uint* ny = (uint*)&(n.y);
        uint* nz = (uint*)&(n.z);
        uint* t = (uint*)&(tex.x);
        uint* s = (uint*)&(tex.y);
        uint* tx = (uint*)&(tangent.x);
        uint* ty = (uint*)&(tangent.y);
        uint* tz = (uint*)&(tangent.z);
        return (*px)*1559 + (*py)*2207 + (*pz)*7121
                + (*nx)*75372143 + (*ny)*44444443 + (*nz)*91439939
                + (*t)*991 + (*s)*997
                + bone.x * 12791 + bone.y * 12799 + bone.z * 13619 + bone.w * 13967
                + (*tx)*75372143 + (*ty)*44444443 + (*tz)*91439939;
    }
    
    void Util::printMatrix(Matrix4 m)
    {
        for (int i = 0;i < 4;++i)
        {
            LOGD("%.4f, %.4f, %.4f, %.4f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
        }
    }
}
