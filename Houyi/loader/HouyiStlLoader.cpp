//
//  HouyiStlLoader.cpp
//  Houyi
//
//  Created by Ross on 14-6-6.
//  Copyright (c) 2014年 Lance Mao. All rights reserved.
//

#include "HouyiStlLoader.h"
#include "HouyiFile.h"

namespace Houyi {

    Scene* StlLoader::loadFromMemory(const char* data, int size)
    {
        File* file = HouyiNew File(mFilePath.c_str(), "r");
        if(!file) {
            LOGE("new File failed means do not have memory now!");
            return mScene;
        }
        bool success = file->Open();
        if(success) {
            char header[80];
            file->Read(header, 1, 80);
            LOGE("header is: %s \n", header);

            int triangleNumber = 0;
            file->Read(&triangleNumber, 1, sizeof(int));
            LOGE("triangle number is: %d \n", triangleNumber);

            if(!triangleNumber) {
                return mScene;
            }

            float* vertexPosition = new float[triangleNumber * 9];
            float* vertexNormal = new float[triangleNumber * 9];

            int normalIndex = 0;
            int positionIndex = 0;
            int trianglesCount = 0;
            float tempVertexNormal[3];
            while(trianglesCount <= triangleNumber) {
                file->Read(tempVertexNormal, 1, sizeof(float) * 3);

                for(int i = 0; i < 3; i++) {
                    vertexNormal[normalIndex++] = tempVertexNormal[0];
                    vertexNormal[normalIndex++] = tempVertexNormal[1];
                    vertexNormal[normalIndex++] = tempVertexNormal[2];

                    int index = positionIndex;
                    file->Read(&vertexPosition[index], 1, sizeof(float) * 3);
                    positionIndex += 3;
                }

                trianglesCount++;

                short byteCount = 0;
                file->Read(&byteCount, 1, 2);
                int valid = (byteCount >> 15) & 0x0001;
                if(valid) {
                    int red = (byteCount & 0x7c00) >> 10;
                    int green = (byteCount & 0x03E0) >> 5;
                    int blue = byteCount & 0x001F;
                    LOGE("r,g,b is: %d, %d, %d\n", red, green, blue);
                }
            }
            
            Mesh* mesh = HouyiNew Mesh();
            mesh->setName(header);
            mScene->addMesh(mesh);
            SceneNode* node = HouyiNew SceneNode();
            node->setMesh(mesh);
            VertexBufferPtr vb = new VertexBuffer();
            vb->setPosition(vertexPosition, triangleNumber * 9);
            vb->setNormal(vertexNormal, triangleNumber * 9);
            
            SurfacePtr s = new Surface(vb);
            mesh->addSurface(s);

            mScene->addSceneNode(node);
            mScene->addRenderable(node);
            HouyiDelete(file);
        }
        return mScene;
    }
}
