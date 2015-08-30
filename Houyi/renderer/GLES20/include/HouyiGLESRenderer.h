#ifndef Houyi_HouyiGLESRenderer_h
#define Houyi_HouyiGLESRenderer_h

#include "Platform.h"

#include "HouyiRenderer.h"
#include "HouyiPlane.h"

namespace Houyi{
    
	class GLESRenderer : public Renderer
	{
	public:
		GLESRenderer();
        virtual ~GLESRenderer();
        
        virtual string getName();
        virtual Pass* loadShader(int index);
        virtual Pass* loadShader(const char* pVertexSource, const char* pFragmentSource);
        virtual bool uploadShader(Pass* pass);
        virtual void onWindowCreated();
        virtual void onWindowChanged(int width, int height);
        virtual void setViewport(float left, float top, float width, float height);
        virtual bool bindVertexBuffer(VertexBufferPtr vertexBuffer);
        virtual void deleteHardwareBuffer(int count, uint* handle);
        virtual void bindRenderTarget(RenderTarget* rt);

        virtual bool beginRender(World* world);
		virtual bool render(World* world);
		virtual bool endRender(World* world);

		virtual bool renderSceneEntity(const RenderEntity& entity);
		virtual void renderMeshSurface(SurfacePtr surface, bool wireframe);
        virtual void renderWireFrame(SurfacePtr surface, int lineWidth = 1);

		virtual void uploadBone(Mesh* mesh);

	protected:
		virtual RenderTarget* createRenderTarget(int width, int height);
		void destroyRenderTarget(RenderTarget* renderTarget);
		virtual void renderMSAAQuad();

	private:
        void setVertexAttribute(VertexBufferPtr vertexBuffer);
	};
}

#endif
