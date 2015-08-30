#include "HouyiObjLoader.h"
#include "HouyiMath.h"
#include "HouyiVertexBuffer.h"
#include "HouyiMaterial.h"
#include "HouyiFile.h"

namespace Houyi
{	
	ObjLoader::ObjLoader() {
		for(int i = 0; i < 15; i++) {
			for(int j = 0; j < 50; j++){
				mTempCharBuffer[i][j] = '\0';
			}
		}

		for(int i = 0; i < 15; i++) {
			mTempIntBuffer[i][0] = 0;
		}
	}

    Scene* ObjLoader::loadFromMemory(const char* data, int size)
    {
        string fn = StringUtils::removeSuffix(mFilePath) + ".mtl";
    	loadMTLFile(fn.c_str());
		Mesh* mesh = NULL;
		SceneNode* node = 0;
		int pos = 0;
		char currentChar;
		char lastChar;
		int count = 0;
		int j = 0;
		bool first = true;
		bool fSpecial = false;
		bool twoLines = false;
		bool firstUsemtl = true;
		char lastMaterialName[20];

		iVertexNumber = 0;
		currentiVertexNumber = 0;
		iVertexTextureNumber = 0;
		currentiVertexTextureNumber = 0;
		iVertexNormalNumber = 0;
		currentiVertexNormalNumber = 0;
    	LOGE("start");
    	while(pos < size) {
    		lastChar = currentChar;
    		currentChar = data[pos++];
    		if(first && currentChar == 'f') {
    			mTempCharBuffer[0][0] = 'f';
				mTempIntBuffer[0][0] = 1;
				count++;
				fSpecial = true;
    		}

    		first = false;
    		if(currentChar == '\n' || currentChar == '\r') {
    			if(twoLines)
    			{
					if(lastChar == '\n' || lastChar == '\r')
					    twoLines= false;
    	    		first = false;
    	    		j = 0;
    				continue;
    			}

				mTempIntBuffer[count][0] = j;
        		if(count == 0 || (fSpecial && count == 1)){
            		first = true;
            		fSpecial = false;
            		j = 0;
            		count = 0;
            		clearBuffer();
            		continue;
        		}
				if (j > 0)
					count++;
    		} else if (currentChar == ' ' || currentChar == '	' || (fSpecial && currentChar == '/')) {
				if (j > 0) {
					mTempIntBuffer[count][0] = j;
					count++;
				}
				j = 0;
				continue;
			} else if(fSpecial && currentChar == '\\') {
				twoLines = true;
				continue;
			} else {
				if(!fSpecial || ((currentChar >= '0' && currentChar <= '9') || currentChar == '-' || currentChar == 'e' || currentChar == 'E'))
					mTempCharBuffer[count][j++] = currentChar;
				continue;
			}

    		//LOGE("data is: %s, %s, %s,  %s, %d", mTempCharBuffer[0], mTempCharBuffer[1], mTempCharBuffer[2], mTempCharBuffer[3], count);
    		//int count = fastSplit(line, ' ');
    		char tag = mTempCharBuffer[0][0];
    		if((tag == 'g' || tag == 'o') && count >= 2) {
    			mesh = HouyiNew Mesh();
    			mesh->setName(mTempCharBuffer[1]);
    			mScene->addMesh(mesh);
    			node = HouyiNew SceneNode();
    			node->setMesh(mesh);
    			LOGE("mesh name is: %s", mesh->getName().c_str());
    		} else if(tag == 'v' &&  mTempCharBuffer[0][1] != 'n' &&  mTempCharBuffer[0][1] != 't') {
    			if(vertextIndexArray.size() > 0) {
					if (lastMaterialName) {
						addSurface(mesh, node, lastMaterialName);
						mScene->addSceneNode(node);
						mScene->addRenderable(node);
					}

					iVertexNumber += currentiVertexNumber;
					iVertexNormalNumber += currentiVertexNormalNumber;
					iVertexTextureNumber += currentiVertexTextureNumber;
					currentiVertexNumber = 0;
					currentiVertexNormalNumber = 0;
					currentiVertexTextureNumber = 0;
					vertextArray.clear();
					vertextNormalArray.clear();
					vertextTextureArray.clear();
					firstUsemtl = true;
					LOGE("mesh over here: %s", mesh->getName().c_str());
    			}
    			vertextArray.push_back(atof(mTempCharBuffer[1]));
    			vertextArray.push_back(atof(mTempCharBuffer[2]));
    			vertextArray.push_back(atof(mTempCharBuffer[3]));
    			currentiVertexNumber++;
    		} else if(tag == 'v' &&  mTempCharBuffer[0][1] == 'n') {
    			vertextNormalArray.push_back(atof(mTempCharBuffer[1]));
    			vertextNormalArray.push_back(atof(mTempCharBuffer[2]));
    			vertextNormalArray.push_back(atof(mTempCharBuffer[3]));
    			currentiVertexNormalNumber++;
    		} else if(tag == 'v' &&  mTempCharBuffer[0][1] == 't') {
    			vertextTextureArray.push_back(atof(mTempCharBuffer[1]));
    			vertextTextureArray.push_back(atof(mTempCharBuffer[2]));
    			currentiVertexTextureNumber++;
    		} else if(tag == 'f') {
    			if(count == 7) {
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[1]));
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[3]));
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[5]));

                    if(vertextNormalArray.size() > 0) {
                        vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[2]));
                        vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[4]));
                        vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[6]));
                    } else if(vertextTextureArray.size() > 0) {
                        vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[2]));
                        vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[4]));
                        vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[6]));
                    }
    			} else if(count == 9) {
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[1]));
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[3]));
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[5]));
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[1]));
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[5]));
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[7]));
                    if(vertextNormalArray.size() > 0) {
                        vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[2]));
                        vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[4]));
                        vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[6]));
                        vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[2]));
                        vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[6]));
                        vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[8]));
                    } else if (vertextTextureArray.size() > 0) {
                        vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[2]));
                        vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[4]));
                        vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[6]));
                        vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[2]));
                        vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[6]));
                        vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[8]));
                    }

    			} else if(count == 10) {
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[1]));
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[4]));
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[7]));

    				vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[2]));
    				vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[5]));
    				vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[8]));

    				vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[3]));
    				vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[6]));
    				vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[9]));
    			} else if(count == 13) {
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[1]));
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[4]));
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[7]));

    				vertextIndexArray.push_back(atoi(mTempCharBuffer[1]));
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[7]));
    				vertextIndexArray.push_back(atoi(mTempCharBuffer[10]));

    				vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[2]));
    				vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[5]));
    				vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[8]));

    				vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[2]));
    				vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[8]));
    				vertextTextureIndexArray.push_back(atoi(mTempCharBuffer[11]));

    				vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[3]));
    				vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[6]));
    				vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[9]));

    				vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[3]));
    				vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[9]));
    				vertextNormalIndexArray.push_back(atoi(mTempCharBuffer[12]));
    			}
    		} else if(strncmp(mTempCharBuffer[0], "usemtl", 6) == 0) {
    			if(!firstUsemtl && lastMaterialName) {
    				addSurface(mesh, node, lastMaterialName);
    			}

    			firstUsemtl = false;
    			strncpy(lastMaterialName, mTempCharBuffer[1], mTempIntBuffer[1][0] + 1);
    		}

    		first = true;
    		fSpecial = false;
    		j = 0;
    		count = 0;
			twoLines = false;
    		clearBuffer();

    	}

		LOGE("last name is %s", lastMaterialName);
    	//Add the last surface in case of some special condition.
    	if(lastMaterialName) {
    		addSurface(mesh, node, lastMaterialName);
    		mScene->addSceneNode(node);
    		mScene->addRenderable(node);
    	}

		LOGE("last mesh over here: %s", mesh->getName().c_str());
    	LOGE("end");
        return mScene;
    }

    void ObjLoader::loadMTLFile(const char* filePath) {
        LOGE("loadMTLFile START");
    	char* buffer = 0;
    	long size = 0;

        File* file = HouyiNew File(filePath, "r");
        if(!file) {
            LOGE("new File failed means do not have memory now!");
            return;
        }
        bool success = file->Open();
        if(success) {
            file->Seek(0, SEEK_END);
            size = file->Tell();
            file->Rewind();
            buffer = (char*) malloc (sizeof(char) * size);
            file->Read(buffer, 1, size);
        }
        HouyiDelete(file);

		int pos = 0;
		int count = 0;
		int j = 0;
		Material *material = NULL;
		while (pos < size) {
			char currentChar = buffer[pos++];
			if (currentChar == '\n' || currentChar == '\r') {
				mTempIntBuffer[count][0] = j;
				if (count == 0) {
					j = 0;
					count = 0;
					clearBuffer();
					continue;
				}

				count++;
			} else if (currentChar == ' ' || currentChar == '	') {
				if (j > 0) {
					mTempIntBuffer[count][0] = j;
					count++;
				}
				j = 0;
				continue;
			} else {
				mTempCharBuffer[count][j++] = currentChar;
				continue;
			}

			char tag1 = mTempCharBuffer[0][0];
			char tag2 = mTempCharBuffer[0][1];
//			LOGE("data is: mTempCharBuffer is %c, %c", tag1, tag2);
			if (count >= 2 && (strncmp(mTempCharBuffer[0], "newmtl", 6) == 0)) {
				material = mScene->getMaterialManager()->createMaterial(
						mTempCharBuffer[1]);
			} else if (tag1 == 'K' && tag2 == 'a') {
				material->setAmbientColor(atof(mTempCharBuffer[1]),
						atof(mTempCharBuffer[2]),
						atof(mTempCharBuffer[3]));
			} else if (tag1 == 'K' && tag2 == 'd') {
				material->setDiffuseColor(atof(mTempCharBuffer[1]),
						atof(mTempCharBuffer[2]),
						atof(mTempCharBuffer[3]));
			} else if (tag1 == 'K' && tag2 == 's') {
				material->setSpecularColor(atof(mTempCharBuffer[1]),
						atof(mTempCharBuffer[2]),
						atof(mTempCharBuffer[3]));
			} else if (tag1 == 'N' && tag2 == 's') {
				material->setShininess(atof(mTempCharBuffer[1]));
			} else if(!strncmp(mTempCharBuffer[0], "map_Kd", 6)) {
				string texname = mTempCharBuffer[1];
	            string paths[3];
	            paths[0] = StringUtils::getLastPathComponent(texname);
	            paths[1] = StringUtils::getPath(mFilePath) + StringUtils::getLastPathComponent(texname);
	            paths[2] = StringUtils::getTexturePath(mFilePath, texname);
	            material->createTextureByPath(mScene, paths);
			}

			clearBuffer();
			j = 0;
			count = 0;
		}

		if(buffer)
			free(buffer);
        LOGE("loadMTLFile END");
    }

    void ObjLoader::addSurface(Mesh* mesh, SceneNode* node, const char* materialName) {
		converVertexIndex(vertextIndexArray, currentiVertexNumber,
				iVertexNumber);
		converVertexIndex(vertextNormalIndexArray,
				currentiVertexNormalNumber, iVertexNormalNumber);
		converVertexIndex(vertextTextureIndexArray,
				currentiVertexTextureNumber, iVertexTextureNumber);
        VertexBufferPtr vb = new VertexBuffer();
    	long size = vertextIndexArray.size();
        if(size > 0) {
            float* positionArray = new float[size * 3];
            
            int j = 0;
            for (int i = 0; i < size; i++, j += 3) {
                positionArray[j] = vertextArray[vertextIndexArray[i] * 3];
                positionArray[j + 1] = vertextArray[vertextIndexArray[i] * 3 + 1];
                positionArray[j + 2] = vertextArray[vertextIndexArray[i] * 3 + 2];
            }

            vb->setPosition(positionArray, size * 3);
        }

    	size = vertextNormalIndexArray.size();
        if(size > 0) {
            float* normalArray = new float[size * 3];
            int j = 0;
            for (int i = 0; i < size; i++, j += 3) {
                normalArray[j] = vertextNormalArray[vertextNormalIndexArray[i] * 3];
                normalArray[j + 1] = vertextNormalArray[vertextNormalIndexArray[i] * 3 + 1];
                normalArray[j + 2] = vertextNormalArray[vertextNormalIndexArray[i] * 3 + 2];
            }
            vb->setNormal(normalArray, size * 3);
        }
        
        size = vertextTextureIndexArray.size();
		if (size > 0) {
	    	float *textureArray = new float[size * 2];
			int j = 0;
			for (int i = 0; i < size; i++, j += 2) {
				textureArray[j] = vertextTextureArray[vertextTextureIndexArray[i] * 2];
				textureArray[j + 1] = vertextTextureArray[vertextTextureIndexArray[i] * 2 + 1];
			}
			vb->setTexcoord(textureArray, size * 2);
		}
		SurfacePtr s = new Surface(vb);
		mesh->addSurface(s);

		Material *material = mScene->getMaterialManager()->getMaterial(materialName);
		if(material && vertextNormalIndexArray.size() == 0 && material->getTextureCount() > 0)
			material->setTextureMode(RenderState::EReplace);
		node->addMaterial(material);
		vertextIndexArray.clear();
		vertextNormalIndexArray.clear();
		vertextTextureIndexArray.clear();
    }

    void ObjLoader::clearBuffer() {
    	int index = 0;
    	int value = 0;
    	while((value = mTempIntBuffer[index][0]) > 0) {
			for (int i = 0; i < value; i++) {
				mTempCharBuffer[index][i] = '\0';
			}
			mTempIntBuffer[index][0] = 0;
    		index++;
			if(index >= 15)
				break;
    	}
    }

    void ObjLoader::converVertexIndex(vector<int> &vertexIndex, int number, int preNumber) {
		for (int i = 0; i < vertexIndex.size(); i++) {
			if (vertexIndex[i] < 0) {
				vertexIndex[i] = vertexIndex[i] + number;
			} else {
				vertexIndex[i] = vertexIndex[i] - preNumber - 1;
			}
		}
    }
}
