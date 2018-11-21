package com.dwtech.android.houyi;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.WindowManager;

import com.dwtech.android.houyi.utils.IOUtils;

public class HEngine {
    
    private static HouyiWorld sWorld;
	private static boolean mIsOn;
	// deal with multi-activity issue
	private static int mRefCount;
	
	public static synchronized HouyiWorld getWorld() {
	    long worldId = getWorldId();
	    if (sWorld == null || sWorld.getId() != worldId) {
	        sWorld = new HouyiWorld(worldId);
	    }
	    return sWorld;
	}

	public static long ignite(Context context) {
		IOUtils.loadLibrary(context, "houyiiconv");
		IOUtils.loadLibrary(context, "houyixml");
		IOUtils.loadLibrary(context, "houyi");
		mIsOn = true;
		WindowManager windowManger = (WindowManager)context.getSystemService(Context.WINDOW_SERVICE);
		DisplayMetrics outMetrics = new DisplayMetrics();
		windowManger.getDefaultDisplay().getMetrics(outMetrics);
		long root = ignite(context.getAssets(), outMetrics);
		setContext(context);
		return root;
	}
	private static native long ignite(AssetManager assetManager, DisplayMetrics displayMetrics);
	public static void stall() {
	    mIsOn = false;
	    mRefCount = 0;
	    stallNative();
	}
	private static native void stallNative();
	public static boolean isOn() {
	    return mIsOn;
	}
	
	public static void incRef() {
		mRefCount++;
	}
	
	public static void decRef() {
		--mRefCount;
		if (mRefCount <= 0) {
			stall();
		}
	}
	
	public static native void clearScene();
	
	public static native void setContext(Context context);
	public static native void clearContext();
	public static native void onResume();
	public static native void onPause();
	public static boolean onTouch(MotionEvent e) {
	    float x1 = e.getX();
	    float y1 = e.getY();
	    
	    int cType = -1;
	    int jType = e.getActionMasked();
	    if (jType == MotionEvent.ACTION_DOWN) {
	        cType = 0;
	    } else if (jType == MotionEvent.ACTION_MOVE) {
	        cType = 1;
	    } else if (jType == MotionEvent.ACTION_UP) {
            cType = 2;
        } else if (jType == MotionEvent.ACTION_POINTER_DOWN) {
            cType = 3;
        } else if (jType == MotionEvent.ACTION_POINTER_UP) {
            cType = 4;
        }  else {
            cType = jType;
        }
	    int pointerCount = e.getPointerCount();
	    if (pointerCount == 2) {
	        return onTouch2(x1, y1, e.getX(1), e.getY(1), cType);
	    } else if (pointerCount == 1) {
	        return onTouch(x1, y1, cType);
	    }
	    return false;
	}
	public static native boolean onTouch(float x, float y, int type);
	public static native boolean onTouch2(float x1, float y1, float x2, float y2, int type);
	public static native boolean isRenderRequested();
	public static native void requestRender(boolean flag);
	
	public static native int getRootVersion(long pRoot);
	
	public static native long loadProgram(int id);
    public static native long loadProgram2(String vertexSource, String fragmentSource);
    
    public static void addPass(HouyiPass pass) {
        addPass(pass.getId());
    }
    public static native void addPass(long pPass);
    
    public static void setPass(HouyiPass pass) {
        setPass(pass.getId());
    }
    public static native void setPass(long pPass);
    
    public static native void deletePtr(long ptr);
    public static native void enableMSAA(float sampleRate);
    public static native void onWindowCreated();
	public static native void onWindowChanged(float width, float height);
	public static native boolean renderAll();
	public static native boolean beginRender();
	public static native boolean render();
	public static native boolean endRender();
	public static native boolean renderMeshes();
	public static native int getMeshCount();
	public static native long getMesh(int index);
	public static void setWorld(HouyiWorld world) {
	    setWorld(world.getId());
	}
	public static native void setWorld(long world);
	
	public static native int getFPS();
	public static native int getVertexCount();
	public static native int getPointCount();
	public static native int getLineCount();
	public static native int getTriangleCount();
	public static native void printFPS(boolean print);
	
	public static native boolean onDoubleTap(float x, float y);
	public static native boolean onBackPressed();
	
    private static native long getWorldId();
    
    public static Bitmap createBitmap(Context context, String name) {
        Bitmap bitmap = null;
        InputStream in = null;
        try {
        	// first try opening file
            try {
                in = new FileInputStream(new File(name));
                bitmap = BitmapFactory.decodeStream(in);
                if (bitmap != null) {
                	return bitmap;
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
            
            // only for those delivered with APK
            try {
                in = context.getAssets().open(name);
                bitmap = BitmapFactory.decodeStream(in);
                return bitmap;
            } catch (IOException e) {
                e.printStackTrace();
            }
        } finally {
            IOUtils.closeSilently(in);
        }
        return bitmap;
    }
    
    public static Bitmap createBitmap(int width, int height) {
        return Bitmap.createBitmap(width, height, Config.ARGB_8888);
    }
}
