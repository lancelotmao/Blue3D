package com.dwtech.android.houyi;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.locks.ReentrantLock;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Rect;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.os.Handler;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;

import com.dwtech.android.houyi.utils.SensorFusion;

public class HouyiGLSurfaceRenderer implements Renderer {
    public interface GLJob {
        boolean doJob();
    }
    
    protected static final String TAG = "Houyi";
    protected static final int EVENT_QUEUE_LENGTH = 32;
    
    protected Context mContext;
    protected GL10 mGL;
    protected GLSurfaceView mGLView;
    protected Handler mHandler;
    protected int mTick;
    
    protected float mHeight;
    protected float mWidth;
    protected HouyiGestureListener mGestureListener;
    protected float mPointerX;
    protected float mPointerY;
    protected float[] mLastX = new float[EVENT_QUEUE_LENGTH];
    protected float[] mLastY = new float[EVENT_QUEUE_LENGTH];
    protected long[] mEventTime = new long[EVENT_QUEUE_LENGTH];
    protected int mEventIndex;
    protected HouyiWorld mWorld;
    protected HouyiPass mPass;
    protected ReentrantLock mLock = new ReentrantLock();
    protected List<GLJob> mMessages;
    protected List<GLJob> mPostMessages;
    
    protected SensorFusion mSensorFusion;
    
    protected GestureDetector mGestureDetector;
    
    public HouyiGLSurfaceRenderer(Context context) {
        mContext = context;
        mMessages = Collections.synchronizedList(new LinkedList<GLJob>());
        mPostMessages = Collections.synchronizedList(new LinkedList<GLJob>());
        mHandler = new Handler();
        mGestureListener = new HouyiGestureListener();
        mGestureDetector = new GestureDetector(context, mGestureListener, null, true);
    }
    
    public Context getContext() {
        return mContext;
    }
    
    // NOTE: this function will be called when user lock/unlock screen
    // although at this point surfaceDestroy is not called
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	if (!HEngine.isOn()) {
    		HEngine.ignite(mContext.getApplicationContext());
    	}
    	if (mGL != null && mGL != gl) {
    	    Log.w(TAG, "not same GL, clearing context");
    	    onClearContext();
    	}
        HEngine.onWindowCreated();
        mGL = gl;
        Log.d(TAG, "onSurfaceCreated");
    }
    
    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mWidth = width;
        mHeight = height;
        
        HEngine.onWindowChanged(width, height);
    }
    
    public void onSurfaceDestroyed() {
    	Log.i(TAG, "Java onSurfaceDestroyed");
    }
    
    @Override
    public void onDrawFrame(GL10 gl) {
        mLock.lock();
        try {
            onDrawFrameLocked(gl);
        } finally {
            mLock.unlock();
        }
        mTick++;
    }
    
    protected void onDrawFrameLocked(GL10 gl) {
        if (!HEngine.isOn()) {
            return;
        }
        onMessage();
        HEngine.renderAll();
        if (HEngine.isRenderRequested()) {
            mGLView.requestRender();
        }
        onPostMessage();
        
        HEngine.requestRender(false);
    }
    
    protected void onMessage() {
        if (mMessages.size() > 0) {
            GLJob msg = mMessages.get(0);
            if (msg != null && msg.doJob()) {
                mMessages.remove(0);
            }
        }
    }
    
    protected void onPostMessage() {
        if (mPostMessages.size() > 0) {
            GLJob msg = mPostMessages.get(0);
            if (msg != null && msg.doJob()) {
                mPostMessages.remove(0);
            }
        }
    }
    
    public HouyiWorld getWorld() {
        return mWorld;
    }
    
    public void lockRenderThread() {
        mLock.lock();
    }
    
    public void unlockRenderThread() {
        mLock.unlock();
    }
    
    public boolean onTouchEvent(MotionEvent e) {
        lockRenderThread();
        
        mGestureDetector.onTouchEvent(e);
        
        try {
            HouyiWorld world = mWorld;
            if (world == null) {
                return false;
            }
            
            mPointerX = e.getX();
            mPointerY = e.getY();
            return HEngine.onTouch(e);
        } finally {
            unlockRenderThread();
        }
    }
    
    public void sendGLMessage(GLJob msg) {
        mMessages.add(msg);
        if (mGLView != null) {
            mGLView.requestRender();
        }
    }
    
    public void sendGLPostMessage(GLJob msg) {
        mPostMessages.add(msg);
        if (mGLView != null) {
            mGLView.requestRender();
        }
    }
    
    protected class HouyiGestureListener extends GestureDetector.SimpleOnGestureListener {
        @Override
        public boolean onDoubleTap(MotionEvent e) {
            lockRenderThread();
            try {
                return HEngine.onDoubleTap(e.getX(), e.getY());
            } finally {
                unlockRenderThread();
            }
        }
    }
    
    public void setGLView(GLSurfaceView glview) {
        mGLView = glview;
    }
    
    // util function
    public byte[] loadTextureData(String assetName) {
        Bitmap bitmap = null;
        try {
            bitmap = BitmapFactory.decodeStream(mContext.getAssets().open(assetName));
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
        
        int bytes = bitmap.getWidth() * bitmap.getHeight() * 4;
        ByteBuffer byteBuffer = ByteBuffer.allocate(bytes);
        bitmap.copyPixelsToBuffer(byteBuffer);
        return byteBuffer.array();
    }
    
    public void captureScreen(final OnScreenCaptureListener listener) {
        sendGLPostMessage(new GLJob() {
            @Override
            public boolean doJob() {
                if (HouyiTextureManager.hasPendingTexture() || mWorld == null) {
                    return false;
                }
                
                mWorld.getRootView().setVisibility(HouyiSceneNode.INVISIBLE);
                HEngine.renderAll();
                
                Rect rect = new Rect(0, 0, (int)mWidth, (int)mHeight);
                int width = rect.width();
                int height = rect.height();
                int data[] = new int[width * height];
                ByteBuffer vbb = ByteBuffer.allocateDirect(data.length * 4);
                vbb.order(ByteOrder.nativeOrder());
                IntBuffer ib = vbb.asIntBuffer();
                ib.position(0);
                
                GLES20.glReadPixels(rect.left, rect.top, width, height, GL10.GL_RGBA, GL10.GL_UNSIGNED_BYTE, ib);
                ib.get(data);
                
                for (int y = 0, n = (height+1) / 2; y < n; y++) {
                    for (int x = 0; x < width; x++) {
                        int tidx = x + (height - y - 1) * width;
                        int idx = x + y * width;
                        int pix = data[idx];
                        int a = ((pix >> 24) & 0xFF);
                        int r = ((pix >> 16) & 0xFF);
                        int g = ((pix >> 8) & 0xFF);
                        int b = ((pix) & 0xFF);
                        pix = a << 24 | b << 16 | g << 8 | r;
                        
                        int tpix = data[tidx];
                        a = ((tpix >> 24) & 0xFF);
                        r = ((tpix >> 16) & 0xFF);
                        g = ((tpix >> 8) & 0xFF);
                        b = ((tpix) & 0xFF);
                        tpix = a << 24 | b << 16 | g << 8 | r;
                        
                        data[tidx] = pix;
                        data[idx] = tpix;
                    }
                }
                
                // do this before notify because listener might set world to null
                if (mWorld != null && mWorld.getRootView() != null) {
                    mWorld.getRootView().setVisibility(HouyiSceneNode.VISIBLE);
                }
                HEngine.renderAll();
                
                Bitmap res = Bitmap.createBitmap(data, width, height, Config.ARGB_8888);
                if (listener != null) {
                    listener.onScreenCaptured(res);
                }
                return true;
            }
        });
    }
    
    public interface OnScreenCaptureListener {
        void onScreenCaptured(Bitmap bitmap);
    }
    
    protected void onClearContext() {
        HEngine.clearContext();
    }
}
