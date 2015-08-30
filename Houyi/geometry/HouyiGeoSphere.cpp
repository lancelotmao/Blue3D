#include "HouyiGeoSphere.h"

namespace Houyi
{
    GeoSphere::GeoSphere(float r, float dtheta, float dphi) : mR(r), mDtheta(dtheta), mDphi(dphi)
	{
		generate();
	}

	bool GeoSphere::generate(int vertexMode, int vertexAttribute, int indexFlag)
	{
		float length = mR;
		float dtheta = Math::DegreesToRadians(mDtheta);
		float dphi = Math::DegreesToRadians(mDphi);

		int thetacnt = 180 / (int)mDtheta;
		int phicnt = 360 / (int)mDphi;
		float theta = -Math::HALF_PI;
		float stepU = dphi / Math::TWO_PI;
		float stepV = dtheta / Math::PI;
		vector<Vector3> vertice;
		vector<Vector2> texcoord;
		float v = 0;
		for (int i = 0;i <= thetacnt;i++, theta += dtheta, v += stepV)
		{
			float thetaPlusdtheta = theta + dtheta;
			float phi = Math::HALF_PI; // start with 0, 0, -1 so that texture can face in front
			float u = 0;
			for (int j = 0;j <= phicnt;j++, phi += dphi, u += stepU)
			{
			    // 2 -- 3
			    // 1 -- 4
				float phiPlusdphi = phi + dphi;
				Vector3 v1(cos(theta) * cos(phi), sin(theta), -cos(theta) * sin(phi));
				Vector3 v2(cos(thetaPlusdtheta) * cos(phi), sin(thetaPlusdtheta), -cos(thetaPlusdtheta) * sin(phi));
				Vector3 v3(cos(thetaPlusdtheta) * cos(phiPlusdphi), sin(thetaPlusdtheta), -cos(thetaPlusdtheta) * sin(phiPlusdphi));

				vertice.push_back(v1);
				vertice.push_back(v3);
				vertice.push_back(v2);

				Vector2 t1, t2, t3;
				if ((vertexAttribute & VertexBuffer::Attribute_Texcoord) != 0)
				{
                    t1 = Vector2(u, v);
                    t2 = Vector2(u, v + stepV);
                    t3 = Vector2(u + stepU, v + stepV);
                    texcoord.push_back(t1);
                    texcoord.push_back(t3);
                    texcoord.push_back(t2);
				}

				if (i > - 90 && i < 90)
				{
					Vector3 v4 = Vector3(cos(theta) * cos(phiPlusdphi), sin(theta), -cos(theta) * sin(phiPlusdphi));
					vertice.push_back(v1);
					vertice.push_back(v4);
					vertice.push_back(v3);

					if ((vertexAttribute & VertexBuffer::Attribute_Texcoord) != 0)
					{
                        Vector2 t4(u + stepU, v);
                        texcoord.push_back(t1);
                        texcoord.push_back(t4);
                        texcoord.push_back(t3);
					}
				}
			}
		}

		VertexBufferPtr vertexBuffer = new VertexBuffer();
		int vcnt = vertice.size();
		float* pos = new float[vcnt * 3];
		vertexBuffer->setPosition(pos, vcnt * 3);
		float* normal = 0;
		if ((vertexAttribute & VertexBuffer::Attribute_Normal) != 0)
        {
		    normal = new float[vcnt * 3];
		    vertexBuffer->setNormal(normal, vcnt * 3);
        }
		float* tex = 0;
		if ((vertexAttribute & VertexBuffer::Attribute_Texcoord) != 0)
		{
		    tex = new float[vcnt * 2];
		    vertexBuffer->setTexcoord(tex, vcnt * 2);
		}
		for (int i = 0;i < vcnt;i++)
		{
			Vector3 v = vertice.at(i);
			pos[i * 3] = v.x * length;
			pos[i * 3 + 1] = v.y * length;
			pos[i * 3 + 2] = v.z * length;

			if (normal)
			{
                normal[i * 3] = v.x;
                normal[i * 3 + 1] = v.y;
                normal[i * 3 + 2] = v.z;
			}
			if (tex)
			{
			    Vector2 t = texcoord[i];
			    tex[i * 2] = t.x;
			    tex[i * 2 + 1] = t.y;
			}
		}

		SurfacePtr surface = new Surface(vertexBuffer);
		addSurface(surface);

		setExpandedVertexBuffer(vertexBuffer);
		mMode = EExpanded;

		return true;
	}
}
