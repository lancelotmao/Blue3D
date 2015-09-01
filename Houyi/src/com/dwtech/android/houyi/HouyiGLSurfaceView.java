
package com.dwtech.android.houyi;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.SurfaceHolder;

import com.dwtech.android.houyi.shader.HIShaders;

public class HouyiGLSurfaceView extends GLSurfaceView implements HIShaders {
    
    protected static final String TAG = HouyiGLSurfaceView.class.getSimpleName();
    
    protected HouyiGLSurfaceRenderer mRenderer;
    
    public HouyiGLSurfaceView(Context context) {
        super(context);
        
        this.getHolder().addCallback(this);
        setEGLContextClientVersion(2);
        this.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        getHolder().setFormat(PixelFormat.RGBA_8888);
        mRenderer = new HouyiGLSurfaceRenderer(context);
        mRenderer.setGLView(this);
        setRenderer(mRenderer);
    }
    
    public HouyiGLSurfaceView(Context context, HouyiGLSurfaceRenderer renderer) {
        super(context);
        
        this.getHolder().addCallback(this);
        setEGLContextClientVersion(2);
        this.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        getHolder().setFormat(PixelFormat.RGBA_8888);
        mRenderer = renderer;
        mRenderer.setGLView(this);
        setRenderer(renderer);
    }
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        super.surfaceDestroyed(holder);
        mRenderer.onSurfaceDestroyed();
    }
    
    @Override
    public void onPause() {
        super.onPause();
        HEngine.onPause();
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        boolean ret = mRenderer.onTouchEvent(e);
        requestRender();
        return ret;
    }
    
    public HouyiGLSurfaceRenderer getRenderer() {
        return mRenderer;
    }
    
    public void lockRenderThread() {
        if (mRenderer != null) {
            mRenderer.lockRenderThread();
        }
    }
    
    public void unlockRenderThread() {
        if (mRenderer != null) {
            mRenderer.unlockRenderThread();
        }
    }
}
