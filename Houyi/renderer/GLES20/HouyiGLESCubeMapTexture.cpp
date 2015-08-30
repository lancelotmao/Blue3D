#include "HouyiGLESCubeMapTexture.h"
#include "HouyiGLUtils.h"

namespace Houyi
{
    GLESCubeMapTexture::GLESCubeMapTexture(ImagePtr images[6], bool upload)
    {
        for (int i = 0;i < 6;++i)
        {
            mImages[i] = images[i];
        }

        if (upload)
        {
            uploadTexture();
        }
    }

    GLESCubeMapTexture::~GLESCubeMapTexture()
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void GLESCubeMapTexture::uploadTexture()
    {
        glGenerate();
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);
        for (int f = 0; f < 6; ++f) {
            GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + f;
            ImagePtr image = mImages[f];
            if (image)
            {
                image->lock();
                glTexImage2D(face, 0, GL_RGBA, image->getWidth(), image->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image->getData());
                image->unlock();
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        checkGlError("GLESCubeMapTexture::uploadTexture");
    }

    void GLESCubeMapTexture::uploadTexSubImage2D()
    {
        uploadTexture();
    }
    
    void GLESCubeMapTexture::uploadTexSubImage2D(ImagePtr bitmap, int x, int y, int width, int height)
    {
        uploadTexture();
    }
    
    void GLESCubeMapTexture::uploadTexSubImage2D(void* data, int x, int y, int width, int height)
    {
        uploadTexture();
    }
}
