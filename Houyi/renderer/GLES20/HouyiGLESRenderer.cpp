#include "HouyiGLESRenderer.h"

#include <stdio.h>
#include <stdlib.h>

#include "HIShaders.h"
#include "HouyiGLUtils.h"
#include "HouyiGLESTexture.h"
#include "HouyiUtil.h"
#include "HouyiGLDefaultPass.h"
#include "HouyiCapability.h"
#include "HouyiAABB.h"
#include "HouyiSettings.h"

namespace Houyi
{
    GLESRenderer::GLESRenderer()
    {
        mShaderMan = HouyiNew ShaderManager(this);
    }

    GLESRenderer::~GLESRenderer()
    {
    }

    string GLESRenderer::getName()
    {
        return "GLES Renderer";
    }

    Pass* GLESRenderer::loadShader(int sid)
    {
        Pass* pass = NULL;
        if (sid < 0 || sid >= HSHADER_COUNT)
        {
            LOGE("Could not load shader. Shader id %d is out of bounds %d\n", sid, HSHADER_COUNT);
        }
        else
        {
            pass = GLPass::createPass(sid);
        }
        return pass;
    }

    Pass* GLESRenderer::loadShader(const char* pVertexSource, const char* pFragmentSource)
    {
        return HouyiNew GLDefaultPass(pVertexSource, pFragmentSource);
    }

    bool GLESRenderer::uploadShader(Pass* pass)
    {
        int program = GLUtils::loadShader(pass->getVertexShader(), pass->getFragmentShader());
        if (program > 0)
        {
            pass->setProgram(program);
            return true;
        }
        return false;
    }

    void GLESRenderer::onWindowCreated()
    {
        LOGI("GLESRenderer::onWindowCreated\n");
        Renderer::onWindowCreated();
    }

    void GLESRenderer::onWindowChanged(int width, int height)
    {
        checkGlError("onWindowChanged start");

        bool dimensionChanged = mWidth != width || mHeight != height;

        Renderer::onWindowChanged(width, height);

        mWidth = width;
        mHeight = height;

        LOGI("GLESRenderer::onWindowChanged(%f, %f)\n", mWidth, mHeight);
        glViewport(0, 0, (int)mWidth, (int)mHeight);

        if ((mMSAARequested && !mIsMSAAEnabled) || (mIsMSAAEnabled && dimensionChanged))
        {
            enableMSAA(mSampleRate, mIsTrilinearFilterEnabled);
        }

        checkGlError("onWindowChanged end");
    }

    void GLESRenderer::setViewport(float left, float top, float width, float height)
    {
        mWidth = width;
        mHeight = height;
        glViewport((int)left, (int)top, (int)width, (int)height);
    }

    void GLESRenderer::setVertexAttribute(VertexBufferPtr vertexBuffer)
    {
        if (mActiveVB == vertexBuffer)
        {
            return;
        }

        int vertexComponentSize = vertexBuffer->getVertexComponentSize();
        GLuint vPositionHandle = mCurrentPass->getAtribLocation("aPosition");
        CHECK_GL_ERROR("glGetAttribLocation");
        bool isBound = vertexBuffer->isBound();
        if (isBound)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getPositionHandle());
        }
        
        glVertexAttribPointer(vPositionHandle, vertexComponentSize, GL_FLOAT, GL_FALSE, 0,
                              isBound ? 0 : vertexBuffer->getPosition());
        CHECK_GL_ERROR("glVertexPositionPointer");
        glEnableVertexAttribArray(vPositionHandle);
        CHECK_GL_ERROR("glEnableVertexPositionArray");

        int attribute = vertexBuffer->getAttributeFlag();
        int vTexcoordHandle = mCurrentPass->getAtribLocation("aTexcoord");
        if ((attribute & VertexBuffer::Attribute_Texcoord) != 0 && vTexcoordHandle >= 0)
        {
            bool useVBO = isBound && vertexBuffer->getTextureHandle() > 0;
            if (useVBO)
            {
                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getTextureHandle());
                glVertexAttribPointer(vTexcoordHandle, 2, GL_FLOAT, GL_FALSE, 0, 0);
            }
            else
            {
                glVertexAttribPointer(vTexcoordHandle, 2, GL_FLOAT, GL_FALSE, 0, vertexBuffer->getTexcoord());
            }
            glEnableVertexAttribArray(vTexcoordHandle);
        }
        else
        {
            glDisableVertexAttribArray(vTexcoordHandle);
        }

        int vNormalHandle = mCurrentPass->getAtribLocation("aNormal");
        if ((attribute & VertexBuffer::Attribute_Normal) != 0 && vNormalHandle >= 0)
        {
            
            CHECK_GL_ERROR("glGetAttribLocation");
            if (isBound)
            {
                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getNormalHandle());
            }
            glVertexAttribPointer(vNormalHandle, 3, GL_FLOAT, GL_FALSE, 0, isBound ? 0 : vertexBuffer->getNormal());
            CHECK_GL_ERROR("glVertexNormalPointer");
            glEnableVertexAttribArray(vNormalHandle);
            CHECK_GL_ERROR("glEnableNormalArray");
        }
        else if(vNormalHandle >= 0)
        {
            glDisableVertexAttribArray(vNormalHandle);
        }

        int vBoneHandle = mCurrentPass->getAtribLocation("aIndex");
        int vWeightHandle = mCurrentPass->getAtribLocation("aWeight");
        if ((attribute & VertexBuffer::Attribute_Skeleton) != 0)
        {
            CHECK_GL_ERROR("glGetAttribLocation");
            if (vBoneHandle >= 0)
            {
                if (isBound)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getJointIndexHandle());
                }
                glVertexAttribPointer(vBoneHandle, 4, GL_FLOAT, GL_FALSE, 0, isBound ? 0 : vertexBuffer->getJointIndex());
                CHECK_GL_ERROR("glVertexBonePointer");
                glEnableVertexAttribArray(vBoneHandle);
                CHECK_GL_ERROR("glEnableIndexArray");
            }

            checkGlError("glGetAttribLocation");
            if (vWeightHandle >= 0)
            {
                if (isBound)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getJointWeightHandle());
                }
                glVertexAttribPointer(vWeightHandle, 4, GL_FLOAT, GL_FALSE, 0, isBound ? 0 : vertexBuffer->getJointWeight());
                CHECK_GL_ERROR("glVertexWeightPointer");
                glEnableVertexAttribArray(vWeightHandle);
                CHECK_GL_ERROR("glEnableWeightArray");
            }
        }
        else
        {
            if (vBoneHandle >= 0)
            {
                glDisableVertexAttribArray(vBoneHandle);
            }
            if (vWeightHandle >= 0)
            {
                glDisableVertexAttribArray(vWeightHandle);
            }
        }

        int vTangentHandle = mCurrentPass->getAtribLocation("aTangent");
        if ((attribute & VertexBuffer::Attribute_Tangent) != 0 && vTangentHandle >= 0)
        {
            CHECK_GL_ERROR("glGetAttribLocation");
            if (isBound)
            {
                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getTangentHandle());
            }
            glVertexAttribPointer(vTangentHandle, 3, GL_FLOAT, GL_FALSE, 0, isBound ? 0 : vertexBuffer->getTangent());
            CHECK_GL_ERROR("glVertexTangentPointer");
            glEnableVertexAttribArray(vTangentHandle);
            CHECK_GL_ERROR("glEnableTangentArray");
        }
        else if (vTangentHandle >= 0)
        {
            glDisableVertexAttribArray(vTangentHandle);
        }

        int vBinormalHandle = mCurrentPass->getAtribLocation("aBinormal");
        if ((attribute & VertexBuffer::Attribute_Binormal) != 0 && vBinormalHandle >= 0)
        {

            CHECK_GL_ERROR("glGetAttribLocation");
            if (isBound)
            {
                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getBinormalHandle());
            }
            glVertexAttribPointer(vBinormalHandle, 3, GL_FLOAT, GL_FALSE, 0, isBound ? 0 : vertexBuffer->getBinormal());
            CHECK_GL_ERROR("glVertexBiormalPointer");
            glEnableVertexAttribArray(vBinormalHandle);
            CHECK_GL_ERROR("glEnableBiormalArray");
        }
        else if (vBinormalHandle >= 0)
        {
            glDisableVertexAttribArray(vBinormalHandle);
        }

        int vColorHandle = mCurrentPass->getAtribLocation("aColor");
        if ((attribute & VertexBuffer::Attribute_Color) != 0 && vColorHandle >= 0)
        {

            CHECK_GL_ERROR("glGetAttribLocation");
            if (isBound)
            {
                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getColorHandle());
            }
            glVertexAttribPointer(vColorHandle, 4, GL_FLOAT, GL_FALSE, 0, isBound ? 0 : vertexBuffer->getColor());
            CHECK_GL_ERROR("glVertexColorPointer");
            glEnableVertexAttribArray(vColorHandle);
            CHECK_GL_ERROR("glEnableColorArray");
        }
        else if (vColorHandle >= 0)
        {
            glDisableVertexAttribArray(vColorHandle);
        }

        mActiveVB = vertexBuffer;
    }

    bool GLESRenderer::bindVertexBuffer(VertexBufferPtr vertexBuffer)
    {
        if (vertexBuffer && vertexBuffer->useVBO())
        {
            CHECK_GL_ERROR("bindVertexBuffer start");
            int count = vertexBuffer->getVBOCount();
            GLuint* name = new GLuint[count];
            glGenBuffers(count, name);
            vertexBuffer->setVBOs(count, name);
            
            int idx = 0;
            if (vertexBuffer->getPosition())
            {
                GLuint vboHandle = name[idx++];
                vertexBuffer->setPositionHandle(vboHandle);
                glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
                glBufferData(GL_ARRAY_BUFFER,
                             vertexBuffer->getPositionLength() * sizeof(float),
                             vertexBuffer->getPosition(), GL_STATIC_DRAW);
            }

            if (vertexBuffer->getTexcoord())
            {
                GLuint vboHandle = name[idx++];
                vertexBuffer->setTextureHandle(vboHandle);
                glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
                glBufferData(GL_ARRAY_BUFFER,
                             vertexBuffer->getTexcoordLength() * sizeof(float),
                             vertexBuffer->getTexcoord(), GL_STATIC_DRAW);
            }
            
            if (vertexBuffer->getNormal())
            {
                GLuint vboHandle = name[idx++];
                vertexBuffer->setNormalHandle(vboHandle);
                glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
                glBufferData(GL_ARRAY_BUFFER,
                             vertexBuffer->getNormalLength() * sizeof(float),
                             vertexBuffer->getNormal(), GL_STATIC_DRAW);
            }
            
            if (vertexBuffer->getJointIndex())
            {
                GLuint vboHandle = name[idx++];
                vertexBuffer->setJointIndexHandle(vboHandle);
                glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
                glBufferData(GL_ARRAY_BUFFER,
                             vertexBuffer->getJointIndexLength() * sizeof(float),
                             vertexBuffer->getJointIndex(), GL_STATIC_DRAW);
            }
            
            if (vertexBuffer->getJointWeight())
            {
                GLuint vboHandle = name[idx++];
                vertexBuffer->setJointWeightHandle(vboHandle);
                glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
                glBufferData(GL_ARRAY_BUFFER,
                             vertexBuffer->getJointWeightLength() * sizeof(float),
                             vertexBuffer->getJointWeight(), GL_STATIC_DRAW);
            }

            if (vertexBuffer->getTangent())
            {
                GLuint handle = name[idx++];
                vertexBuffer->setTangentHandle(handle);
                glBindBuffer(GL_ARRAY_BUFFER, handle);
                glBufferData(GL_ARRAY_BUFFER,
                             vertexBuffer->getTangentLength() * sizeof(float),
                             vertexBuffer->getTangent(), GL_STATIC_DRAW);
            }

            if (vertexBuffer->getBinormal())
            {
                GLuint handle = name[idx++];
                vertexBuffer->setBinormalHandle(handle);
                glBindBuffer(GL_ARRAY_BUFFER, handle);
                glBufferData(GL_ARRAY_BUFFER,
                             vertexBuffer->getBinormalLength() * sizeof(float),
                             vertexBuffer->getBinormal(), GL_STATIC_DRAW);
            }

            if (vertexBuffer->getColor())
            {
                GLuint handle = name[idx++];
                vertexBuffer->setColorHandle(handle);
                glBindBuffer(GL_ARRAY_BUFFER, handle);
                glBufferData(GL_ARRAY_BUFFER,
                             vertexBuffer->getColorLength() * sizeof(float),
                             vertexBuffer->getColor(), GL_STATIC_DRAW);
            }
        
            delete[] name;

            vertexBuffer->setBind(true);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            CHECK_GL_ERROR("bindVertexBuffer done");

            return true;
        }
        else
        {
            return false;
        }
    }

    void GLESRenderer::deleteHardwareBuffer(int count, uint* handle)
    {
        CHECK_GL_ERROR("delete hardware buffer start");
        glDeleteBuffers(count, handle);
        CHECK_GL_ERROR("delete hardware buffer end");
    }

    void GLESRenderer::bindRenderTarget(RenderTarget* rt)
    {
        if (rt)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, rt->getHandle());
            glViewport(0, 0, rt->getWidth(), rt->getHeight());
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, mDefaultFBO);
            int backingWidth = mWidth;
            if (currentPlatform() == PLATFORM_IOS)
            {
                backingWidth = getRenderBufferWidth();
            }
            int backingHeight = mHeight;
            if (currentPlatform() == PLATFORM_IOS)
            {
                backingHeight = getRenderBufferHeight();
            }
            glViewport(0, 0, backingWidth, backingHeight);
        }
    }

    bool GLESRenderer::beginRender(World* world)
    {
        CHECK_GL_ERROR("GLESRenderer::beginRender start");
        bool ret = Renderer::beginRender(world);
        CHECK_GL_ERROR("GLESRenderer::beginRender end");

        const Color& clearColor = Settings::getInstance()->getClearColor();
        glClearColor(clearColor.mR, clearColor.mG, clearColor.mB, clearColor.mA);
        glClear((GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
        CHECK_GL_ERROR("glClear");
        return ret;
    }

    bool GLESRenderer::render(World* world)
    {
        CHECK_GL_ERROR("GLESRenderer::render");
        return Renderer::render(world);
    }

    bool GLESRenderer::endRender(World* world)
    {
        CHECK_GL_ERROR("GLESRenderer::endRender");
        return Renderer::endRender(world);
    }

    bool GLESRenderer::renderSceneEntity(const RenderEntity& entity)
    {
        SceneNode* node = entity.mNode;
        SurfacePtr surface = entity.mSurface;
        Material* material = entity.mMaterial;
        Scene* scene = mCurrentRenderingScene;
        
        Mesh* mesh = node->getMesh();
        uploadBone(mesh);
        CHECK_GL_ERROR("bone setup done");

        GLPass* pass = (GLPass*)mCurrentPass;
        RenderState& renderState = material->getRenderState();
        
        if (renderState.isDepthTestEnabled())
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
        
        if (renderState.isAlphaBlendEnabled())
        {
            glEnable(GL_BLEND);
            glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            glDisable(GL_BLEND);
        }

        int locShadowFactor = pass->getUniformLocation("uShadowFactor");
        if (locShadowFactor >= 0)
        {
            glUniform1f(locShadowFactor, mesh->getShadowFactor());
        }

        int locIsOccluder = pass->getUniformLocation("uIsOccluder");
        if (locIsOccluder >= 0)
        {
            glUniform1i(locIsOccluder, mesh->isShadowCastEnabled());
        }
        CHECK_GL_ERROR("mesh shadow uniform setup done");
        
        if (renderState.isLightingEnabled())
        {
            // support all fixed or none fixed. No mix
            bool fixedPos = false;
            
            static const int MAX_DIFFUSE = 4;
            
            float lightPosMat[MAX_DIFFUSE * 4];
            float lightColorMat[MAX_DIFFUSE * 4];
            float ambient[4] = {0, 0, 0, 0};
            int diffIdx = 0;
            // 1 ambient
            int maxLightCount = min(scene->getLightCount(), MAX_DIFFUSE + 1);
            for (int i = 0;i < maxLightCount;++i)
            {
                Light* light = scene->getLight(i);
                
                fixedPos |= light->isPosFixed();
                
                float* color = light->getColor();
                if (light->isAmbient())
                {
                    memcpy(ambient, color, 4 * sizeof(float));
                }
                else
                {
                    float pos[4];
                    light->getWorldPosition(pos);
                    memcpy(lightPosMat + 4 * diffIdx, pos, 4 * sizeof(float));
                    memcpy(lightColorMat + 4 * diffIdx, color, 4 * sizeof(float));
                    diffIdx++;
                    if(diffIdx >= MAX_DIFFUSE)
                    {
                        break;
                    }
                }
            }
            int locIsPosFixed = pass->getUniformLocation("uIsLightPosFixed");
            if (locIsPosFixed >= 0)
            {
                glUniform1i(locIsPosFixed, fixedPos ? 1 : 0);
            }
            int locLightingCount = pass->getUniformLocation("uLightCount");
            if (locLightingCount >= 0)
            {
                glUniform1i(locLightingCount, diffIdx);
            }
            
            int locLightPos = pass->getUniformLocation("uLightPos");
            glUniform4fv(locLightPos, 4, lightPosMat);
            int locLightColor = pass->getUniformLocation("uLightColor");
            glUniform4fv(locLightColor, 4, lightColorMat);
            
            int locAmbientLight = pass->getUniformLocation("uAmbientLight");
            if (renderState.isAmbientShadingEnabled())
            {
                glUniform4f(locAmbientLight, ambient[0], ambient[1], ambient[2], ambient[3]);
            }
            else
            {
                glUniform4f(locAmbientLight, 0, 0, 0, 0);
            }
        }
        CHECK_GL_ERROR("light setup done");
        
        if (renderState.isShadowEnabled() && scene->getLightCount() > 0)
        {
            Light* light = scene->getLight(0); // TODO
            int locMat = pass->getUniformLocation("matLightView");
            glUniformMatrix4fv(locMat, 1, false, light->getShadowCastViewMatrix().getData());
            
            int locProMat = pass->getUniformLocation("matLightProjection");
            glUniformMatrix4fv(locProMat, 1, false, light->getShadowCastProjectionwMatrix().getData());
            
            int locAASampleRate = pass->getUniformLocation("uShadowMappingAASampleRate");
            glUniform1i(locAASampleRate, 4);
            
            if (pass->getLastPassResult().mTextureIds.size() > 0)
            {
                glActiveTexture(GL_TEXTURE7);
                glBindTexture(GL_TEXTURE_2D, pass->getLastPassResult().mTextureIds[0]);
                int loc = pass->getUniformLocation("texShadowMap");
                glUniform1i(loc, 7);
            }
            CHECK_GL_ERROR("shadow setup done");
        }

        pass->uploadMaterial(material, scene->getMaterialManager());

        // world view matrix
        pass->uploadWorldMat(node->getLocal2World());

        // normal matrix
        pass->uploadNormalMat(node->getLocal2WorldNormal());
        CHECK_GL_ERROR("mesh transform setup done");

        renderMeshSurface(surface, Settings::getInstance()->getShadingMode() == Settings::ESM_Wireframe
        		&& material->isPassConfigurable());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return true;
    }

    void GLESRenderer::uploadBone(Mesh* mesh)
    {
        if (!mCurrentPass || !mesh)
        {
            return;
        }
        
        GLPass* pass = (GLPass*)mCurrentPass;
        int locBoneEnabled = pass->getUniformLocation("iBoneEnabled");
        SkeletonRef& skeleton = mesh->getSkeletonRef();
        if (skeleton)
        {
            if (skeleton.isHardwareEnabled())
            {
                float* uploadMat = mesh->getUploadBoneMatrix();
                for (int i = 0, n = skeleton.getJointsCount();i < n;++i)
                {
                    JointNode* jn = skeleton.getJointNode(i);
                    Matrix4 m = jn->mWorldMatrix;
                    memcpy(uploadMat + i*16, m.transpose().getData(), sizeof(float) * 16);
                }

                glUniform1i(locBoneEnabled, 1);
                pass->uploadMatrix4("matBindShape", mesh->getBindShapeMatrix(), true);
                int locBone = pass->getUniformLocation("matBone");
                glUniformMatrix4fv(locBone, Skeleton::MAX_BONE_COUNT, false, uploadMat);
            }
            else
            {
                if (locBoneEnabled >= 0)
                {
                    glUniform1i(locBoneEnabled, 0);
                }
            }
        }
        else if (locBoneEnabled >= 0)
        {
            glUniform1i(locBoneEnabled, 0);
        }
    }

    void GLESRenderer::renderMeshSurface(SurfacePtr surface, bool wireframe)
    {
        if (!surface)
        {
            return;
        }
        
        VertexBufferPtr vertexBuffer = surface->getVertexBuffer();
        if (!vertexBuffer)
        {
            return;
        }

        if (!vertexBuffer->isBound())
        {
            bindVertexBuffer(vertexBuffer);
        }
        setVertexAttribute(vertexBuffer);

        if (wireframe)
        {
            renderWireFrame(surface);
            return;
        }

        int meshRenderMode = surface->getRenderMode();
        int vertexComponentSize = vertexBuffer->getVertexComponentSize();
        IndexBufferPtr indexBufferPtr = surface->getIndexBuffer();
        if (indexBufferPtr)
        {
            if ((meshRenderMode & Const::ERenderModePoint) != 0)
            {
                GLPass* pass = (GLPass*)mCurrentPass;
                int locPointSize = pass->getUniformLocation("uPointSize");
                if (locPointSize >= 0)
                {
                    glUniform1f(locPointSize, surface->getPointSize());
                }
                glDrawArrays(GL_POINTS, 0, vertexBuffer->getPositionLength() / vertexComponentSize);
                mPointCount += vertexBuffer->getPositionLength() / vertexComponentSize;
            }
            if (((meshRenderMode & Const::ERenderModeLine) != 0)
                && (indexBufferPtr->getIntLineIndices() || indexBufferPtr->getShortLineIndices()))
            {
                int size = indexBufferPtr->getLineIndexSize();
                void* data = size == 2 ? (void*)(indexBufferPtr->getShortLineIndices()) : (void*)(indexBufferPtr->getIntLineIndices());
                glLineWidth(surface->getLineWidth());
                glDrawElements(GL_LINES, indexBufferPtr->getLineIndicesLength(),
                               size == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, data);
            }
            if (((meshRenderMode & Const::ERenderModeTriangle) != 0)
                && (indexBufferPtr->getIntTriangleIndices() || indexBufferPtr->getShortTriangleIndices()))
            {
                int size = indexBufferPtr->getTriangleIndexSize();
            	int length = indexBufferPtr->getTriangleIndicesLength();
            	void* data = size == 2 ? (void*)(indexBufferPtr->getShortTriangleIndices()) : (void*)(indexBufferPtr->getIntTriangleIndices());
            	if (surface->getVertexBuffer()->useVBO())
            	{
            		unsigned int handle = surface->getBufferHandle();
            		if (handle == 0)
            		{
            			glGenBuffers(1, &handle);
            			surface->setBufferHandle(handle);
            		}

            		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
            		if (!surface->isBufferValid())
            		{
            			glBufferData(GL_ELEMENT_ARRAY_BUFFER, length * (size == 2 ? sizeof(ushort) : sizeof(uint)),
            			        data, GL_STATIC_DRAW);
            			surface->setBufferValid(true);
            		}
            		glDrawElements(GL_TRIANGLES, length, size == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            	}
            	else
            	{
					glDrawElements(GL_TRIANGLES, length, size == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, data);
            	}
                mTriangleCount += length / 3;
            }
            CHECK_GL_ERROR("glDrawElements. Probably INT index unsupported");
        }
        else
        {
            int vertexMode = vertexBuffer->getVertexMode();
            int elementCount = vertexBuffer->getPositionLength() / vertexComponentSize;
            if ((meshRenderMode & Const::ERenderModePoint) != 0)
            {
                GLPass* pass = (GLPass*)mCurrentPass;
                int locPointSize = pass->getUniformLocation("uPointSize");
                if (locPointSize >= 0)
                {
                    glUniform1f(locPointSize, surface->getPointSize());
                }
                glDrawArrays(GL_POINTS, 0, elementCount);
                mPointCount += elementCount;
            }
            else if (((meshRenderMode & Const::ERenderModeLine) != 0) && vertexMode == VertexBuffer::EExpandAsLines)
            {
                glLineWidth(surface->getLineWidth());
                glDrawArrays(GL_LINES, 0, elementCount);
            }
            else if ((meshRenderMode & Const::ERenderModeTriangle) != 0)
            {
                glDrawArrays(GL_TRIANGLES, 0, elementCount);
            }
        }

        if ((meshRenderMode & Const::ERenderModeWireFrame) != 0)
        {
            renderWireFrame(surface);
        }
    }
    
    void GLESRenderer::renderWireFrame(SurfacePtr surface, int lineWidth)
    {
        IndexBufferPtr wireFrame;
        if(surface->getRenderMode() == Const::ERenderModeLine)
        {
            wireFrame = surface->getIndexBuffer();
        }
        else
        {
            wireFrame = surface->getWireFrameIndexBuffer();
        }
        
        if (wireFrame)
        {
            int size = wireFrame->getLineIndexSize();
            void* data = size == 2 ? (void*)wireFrame->getShortLineIndices() : (void*)wireFrame->getIntLineIndices();
            glLineWidth(lineWidth);
            glDrawElements(GL_LINES, wireFrame->getLineIndicesLength(),
                    size == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, data);
        }
    }

    RenderTarget* GLESRenderer::createRenderTarget(int width, int height)
    {
        return GLUtils::createRenderTarget(width, height);
    }

    void GLESRenderer::destroyRenderTarget(RenderTarget* renderTarget)
    {
        GLuint handle[1];
        handle[0] = renderTarget->getHandle();
        glDeleteFramebuffers(1, handle);
        checkGlError("glDeleteFramebuffers");
    }

    void GLESRenderer::renderMSAAQuad()
    {
//        float color = 0.0f;
//        glClearColor(color, color, color, 1.0f);
//        checkGlError("glClearColor");
//        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//        checkGlError("glClear");
//
//        World* world = mCurrentRenderingWorld;
//
//        // back up camera and set MSAA camera which is fixed
//        Camera* backupCamera = world->getCurrentCamera();
//
//
//        // back up current shader program
//        GLuint backupProgram = mProgram;
//
//        mProgram = mMSAAProgram;
//        glUseProgram(mProgram);
//
//        Camera* camera = &mMSAACamera;
//        world->setCurrentCamera(camera);
//        int locProMat = glGetUniformLocation(mProgram, "matProjection");
//        glUniformMatrix4fv(locProMat, 1, false, camera->getProjectionMatrix());
//
//        int rtWidth = mMSAARenderTarget->getWidth();
//        int rtHeight = mMSAARenderTarget->getHeight();
//        int locFBOWidth = glGetUniformLocation(mMSAAProgram, "width");
//        if (locFBOWidth >= 0)
//        {
//            glUniform1f(locFBOWidth, rtWidth);
//        }
//
//        int locFBOHeight = glGetUniformLocation(mMSAAProgram, "height");
//        if (locFBOHeight >= 0)
//        {
//            glUniform1f(locFBOHeight, rtHeight);
//        }
//
//        int locSampleRate = glGetUniformLocation(mMSAAProgram, "sampleRate");
//        if (locSampleRate >= 0)
//        {
//            glUniform1f(locSampleRate, mSampleRate);
//        }
//
//        //TODO render quad
////        renderMesh(mMSAAQuad);
//
//        // restore
//        world->setCurrentCamera(backupCamera);
//        mProgram = backupProgram;
    }
}
