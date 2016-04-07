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
            char headerTag[5];
            file->Read(headerTag, 1, sizeof(char) * 5);
            LOGE("headerTag is: %s \n", headerTag);
            if(strncmp(headerTag, "solid", 5) == 0) {
                //It mean ASCII STL file.
                int pos = 0;
                char currentChar;
                char mTempCharBuffer[15][50];
                int count = 0;
                int j = 0;
                char* header;
                
                for(int i = 0; i < 15; i++) {
                    for(int k = 0; k < 50; k++){
                        mTempCharBuffer[i][k] = '\0';
                    }
                }
                while(pos < size) {
                    currentChar = data[pos++];
                    if(currentChar == '\n' || currentChar == '\r') {
                        
                    } else if (currentChar == ' ' || currentChar == '	') {
                        if (j > 0) {
                            count++;
                        }
                        j = 0;
                        continue;
                    } else {
                        mTempCharBuffer[count][j++] = currentChar;
                        continue;
                    }
                    
                    //LOGE("data is: %s, %s, %s,  %s, %d", mTempCharBuffer[0], mTempCharBuffer[1], mTempCharBuffer[2], mTempCharBuffer[3], count);
                    if(strncmp(mTempCharBuffer[0], "solid", 5) == 0) {
                        header = mTempCharBuffer[1];
                    } else if(strncmp(mTempCharBuffer[0], "facet", 5) == 0 && strncmp(mTempCharBuffer[1], "normal", 6) == 0) {
                        vertextNormalArray.push_back(atof(mTempCharBuffer[2]));
                        vertextNormalArray.push_back(atof(mTempCharBuffer[3]));
                        vertextNormalArray.push_back(atof(mTempCharBuffer[4]));

                        vertextNormalArray.push_back(atof(mTempCharBuffer[2]));
                        vertextNormalArray.push_back(atof(mTempCharBuffer[3]));
                        vertextNormalArray.push_back(atof(mTempCharBuffer[4]));

                        vertextNormalArray.push_back(atof(mTempCharBuffer[2]));
                        vertextNormalArray.push_back(atof(mTempCharBuffer[3]));
                        vertextNormalArray.push_back(atof(mTempCharBuffer[4]));
                    } else if(strncmp(mTempCharBuffer[0], "vertex", 6) == 0) {
                        vertextArray.push_back(atof(mTempCharBuffer[1]));
                        vertextArray.push_back(atof(mTempCharBuffer[2]));
                        vertextArray.push_back(atof(mTempCharBuffer[3]));
                    }

                    count = 0;
                    j = 0;
                    for(int i = 0; i < 15; i++) {
                        for(int k = 0; k < 50; k++){
                            mTempCharBuffer[i][k] = '\0';
                        }
                    }
                }
                
                int vertexSize = vertextArray.size();
                int vertexNormalSize = vertextNormalArray.size();
                vertexPosition = new float[vertexSize];
                vertexNormal = new float[vertexNormalSize];
                
                j = 0;
                for (int i = 0; i < vertexSize; i++) {
                    vertexPosition[i] = vertextArray[i];
                }
                
               
                j = 0;
                for (int i = 0; i < vertexNormalSize; i++) {
                    vertexNormal[i] = vertextNormalArray[i];
                }
                
                Mesh* mesh = HouyiNew Mesh();
                mesh->setName(header);
                mScene->addMesh(mesh);
                SceneNode* node = HouyiNew SceneNode();
                node->setMesh(mesh);
                VertexBufferPtr vb = new VertexBuffer();
                vb->setPosition(vertexPosition, vertexSize);
                vb->setNormal(vertexNormal, vertexNormalSize);
                
                SurfacePtr s = new Surface(vb);
                mesh->addSurface(s);
                
                mScene->addSceneNode(node);
                mScene->addRenderable(node);
                HouyiDelete(file);
            } else {
                file->Rewind();
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
        }
        return mScene;
    }
}
