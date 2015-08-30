#include "HouyiGLPass.h"
#include "HIShaders.h"
#include "HouyiRoot.h"
#include "HouyiRenderer.h"
#include "HouyiGLUtils.h"
#include "HouyiGLEnvPass.h"
#include "HouyiGLDefaultPass.h"
#include "HouyiGLShadowPass.h"
#include "HouyiGLFirePass.h"
#include "HouyiSettings.h"
#include "glshaders.h"
#include "HouyiMaterialManager.h"

namespace Houyi
{
    GLPass::GLPass(int program) : Pass(program)
    {
        mName = "Base GL Pass";
        init();
    }

    GLPass::GLPass(const char* vertex, const char* fragment) : Pass(vertex, fragment)
    {
        mName = "Base GL Pass";
        init();
    }

    GLPass::~GLPass()
    {
        if (!Root::getInstance()->mAutoFinalize)
        {
            glDeleteProgram(mProgram);
        }
    }
    
    void GLPass::init()
    {
    }

    void GLPass::beginPass()
    {
        if (mPassState == EInvalid)
        {
            mProgram = GLUtils::loadShader(mVertex, mFragment);
            mPassState = ENormal;
            LOGD("reload shader after context lost. mName = %s\n", mName.c_str());
        }
        glUseProgram(mProgram);
        Pass::beginPass();
    }

    void GLPass::endPass()
    {
        Pass::endPass();
    }

    void GLPass::uploadMaterial(Material* material, MaterialManager* matMan)
    {
		int shadingMode = Settings::getInstance()->getShadingMode();
        if (material->isPassConfigurable())
        {
			if (shadingMode == Settings::ESM_Solid)
			{
				material = &matMan->getSolid();
			}
			else if (shadingMode == Settings::ESM_Wireframe)
			{
				material = &matMan->getWireFrame();
			}
			else if (shadingMode == Settings::ESM_Hologram)
			{
				material = &matMan->getWireFrame();
			}
        }
        
        if (!material || matMan->getLastBound() == material)
        {
            return;
        }

        RenderState& renderState = material->getRenderState();
        int locAttenuationEnabled = getUniformLocation("iAttenuationEnabled");
        if (locAttenuationEnabled >= 0)
        {
            glUniform1i(locAttenuationEnabled, renderState.isAttenuationEnabled() ? 1 : 0);
        }

        int locLightingEnabled = getUniformLocation("iLightingEnabled");
        if (locLightingEnabled >= 0)
        {
            bool enabled = renderState.isLightingEnabled() && Settings::getInstance()->isLightingEnabled();
            glUniform1i(locLightingEnabled, enabled ? 1 : 0);
        }
        
        int locMaterialAmbient = getUniformLocation("uMaterialAmbient");
        if (locMaterialAmbient >= 0)
        {
            Color color = material->getAmbientColor();
            glUniform4f(locMaterialAmbient, color.mR, color.mG, color.mB, color.mA);
        }

        int locMaterialDiffuse = getUniformLocation("uMaterialDiffuse");
        if (locMaterialDiffuse >= 0)
        {
            Color color = material->getDiffuseColor();
            glUniform4f(locMaterialDiffuse, color.mR, color.mG, color.mB, color.mA);
        }

        int locMaterialSpec = getUniformLocation("uMaterialSpec");
        if (locMaterialSpec >= 0)
        {
            if (renderState.isSpecularShadingEnabled() && material->getShininess() > 0)
            {
                Color color = material->getSpecularColor();
                glUniform4f(locMaterialSpec, color.mR, color.mG, color.mB, color.mA);
                float shininess = material->getShininess();
                int locShininess = getUniformLocation("uShininess");
                if (locShininess >= 0)
                {
                    glUniform1f(locShininess, shininess);
                }
            }
            else
            {
                glUniform4f(locMaterialSpec, 0.0f, 0.0f, 0.0f, 0.0f);
            }
        }

        int locMaterialDoubleSided = getUniformLocation("uDoubleSided");
        if (locMaterialDoubleSided >= 0)
        {
            glUniform1i(locMaterialDoubleSided, renderState.isDoubleSided() ? 1 : 0);
        }
        CHECK_GL_ERROR("mesh material setup done");
        
        int locTexEnv = getUniformLocation("uTextureMode");
        if (locTexEnv >= 0)
        {
            int mode = material->getTextureMode();
            glUniform1i(locTexEnv, mode == RenderState::EUnspecified ? renderState.getTexEnv() : mode);
        }

        int matTexCnt = material->getTextureCount();
        if (matTexCnt > 0 && (shadingMode == Settings::ESM_Default || shadingMode == Settings::ESM_PPL || !material->isPassConfigurable()))
        {
            for (int i = 0;i < matTexCnt;++i)
            {
                uploadTexture(material->getTexture(i), renderState, i);
            }
        }
        else if (material->getExternTexID() > 0)
        {
        	glBindTexture(GL_TEXTURE_2D, 0);
#ifdef ANDROID
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, material->getExternTexID());
#endif
        }
        else if (material->getCubeMapTexID() > 0)
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, material->getCubeMapTexID());
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, 0);
            int locTexEnv = getUniformLocation("uTextureMode");
            if (locTexEnv >= 0)
            {
                glUniform1i(locTexEnv, RenderState::ENoTex);
            }
        }
        CHECK_GL_ERROR("mesh texture setup done");

        matMan->setLastBound(material, this);
    }

    void GLPass::uploadTexture(const Texture* texture, const RenderState& renderState, int index)
    {
        if (!texture)
        {
            return;
        }
        
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(texture->getType(), texture->getTextureId());
        int locTex = getUniformLocation(texture->getName());
        glUniform1i(locTex, index);
//        if (mIsTrilinearFilterEnabled)
//        {
//            glGenerateMipmap (GL_TEXTURE_2D);
//        }
        int locUScale = getUniformLocation("uScale");
        int locVScale = getUniformLocation("vScale");
        if (locUScale >= 0 && locVScale >= 0)
        {
            glUniform1f(locUScale, texture->getUScaleFactor());
            glUniform1f(locVScale, texture->getVScaleFactor());
        }
        int locTexScroll = getUniformLocation("uTexScroll");
        if (locTexScroll >= 0)
        {
            glUniform2f(locTexScroll, texture->getUScroll(), texture->getVScroll());
        }

        int locTexFitCenterOffset = getUniformLocation("uFitCenterOffset");
        if (locTexFitCenterOffset >= 0)
        {
            glUniform2f(locTexFitCenterOffset, texture->getFitCenterOffsetU(),
                    texture->getFitCenterOffsetV());
        }
    }

    void GLPass::onRender(Scene* scene)
    {
        if (scene->getStats().mVertexCount > 4096)
        {
            glEnable(GL_CULL_FACE);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
    }

    Pass* GLPass::createPass(int shaderId)
    {
        Pass* res = NULL;
        if (shaderId == HSHADER_CUBEMAP)
        {
            res = HouyiNew GLEnvPass(vshaders[shaderId], pshaders[shaderId]);
        }
        else if (shaderId == HSHADER_SHADOW_CAST)
        {
            res = HouyiNew GLShadowPass(vshaders[shaderId], pshaders[shaderId]);
        }
        else if (shaderId == HSHADER_FIRE)
        {
            res = HouyiNew GLFirePass(vshaders[shaderId], pshaders[shaderId]);
        }
        else
        {
            res = HouyiNew GLDefaultPass(vshaders[shaderId], pshaders[shaderId]);
        }

        return res;
    }

    void GLPass::invalidate()
    {
        Pass::invalidate();
        mUniformMap.clear();
//        MaterialManager::getInstance()->setLastBound(0, 0);
    }

    int GLPass::getAtribLocation(string key)
    {
        if (mPassState != ENormal)
        {
            LOGE("GLPass::getAtribLocation. mPassState = %d\n", mPassState);
            return -1;
        }
        map<string, int>::iterator it = mAttribMap.find(key);
        int loc = -1;
        if (it != mAttribMap.end())
        {
            loc = it->second;
        }
        else
        {
            loc = glGetAttribLocation(mProgram, key.c_str());
            mAttribMap.insert(pair<string, int>(key, loc));
        }
        return loc;
    }
    
    int GLPass::getUniformLocation(string key)
    {
        map<string, int>::iterator it = mUniformMap.find(key);
        int loc = -1;
        if (it != mUniformMap.end())
        {
            loc = it->second;
        }
        else
        {
            loc = glGetUniformLocation(mProgram, key.c_str());
            mUniformMap.insert(pair<string, int>(key, loc));
        }
        return loc;
    }

    void GLPass::uploadMatrix4(const string& uniformName, const Matrix4& m, bool transpose)
    {
        int locMat = getUniformLocation(uniformName);
        if (locMat >= 0)
        {
            if (transpose)
            {
                Matrix4 t = m.transpose();
                glUniformMatrix4fv(locMat, 1, GL_FALSE, t.getData());
            }
            else
            {
                glUniformMatrix4fv(locMat, 1, GL_FALSE, m.getData());
            }
        }
    }
    
    void GLPass::uploadMatrix3(const string& uniformName, const Matrix3& m, bool transpose)
    {
        int locMat = getUniformLocation(uniformName);
        if (locMat >= 0)
        {
            if (transpose)
            {
                Matrix3 t = m.Transpose();
                glUniformMatrix3fv(locMat, 1, GL_FALSE, (float*)&t);
            }
            else
            {
                glUniformMatrix3fv(locMat, 1, GL_FALSE, (float*)&m);
            }
        }
    }

    void GLPass::uploadWorldMat(const Matrix4& m)
    {
        uploadMatrix4("matWorld", m, true);
    }
    
    void GLPass::uploadNormalMat(const Matrix3& m)
    {
        uploadMatrix3("matNormal", m, true);
    }

    void GLPass::uploadEyeMat(const Matrix4& m)
    {
        uploadMatrix4("matEye", m, true);
    }

    void GLPass::uploadProjectionMat(const Matrix4& m)
    {
        uploadMatrix4("matProjection", m, true);
    }
}
