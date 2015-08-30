package com.dwtech.android.houyimodelloader;

import java.io.FileInputStream;
import java.io.FileNotFoundException;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.res.Configuration;
import android.graphics.SurfaceTexture;
import android.graphics.SurfaceTexture.OnFrameAvailableListener;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.util.Log;
import android.view.Surface;
import android.view.WindowManager;

import com.dwtech.android.blue3d.Blue3DWorld;
import com.dwtech.android.data.DataManager;
import com.dwtech.android.data.DataManager.FocusListener;
import com.dwtech.android.houyi.HEngine;
import com.dwtech.android.houyi.HouyiAssetManager;
import com.dwtech.android.houyi.HouyiCameraNode;
import com.dwtech.android.houyi.HouyiGLSurfaceRenderer;
import com.dwtech.android.houyi.HouyiGameWorldOrtho;
import com.dwtech.android.houyi.HouyiScene;
import com.dwtech.android.houyi.HouyiSceneManager;
import com.dwtech.android.houyi.HouyiSceneNode;
import com.dwtech.android.houyi.HouyiSetting;
import com.dwtech.android.houyi.HouyiWorld;
import com.dwtech.android.houyi.shader.HIShaders;
import com.dwtech.android.houyi.utils.IOUtils;
import com.dwtech.android.houyi.utils.Utils;
import com.dwtech.data.Item;

public class ModelLoaderRenderer extends HouyiGLSurfaceRenderer implements
		HIShaders, FocusListener {

	public interface OnLoadingListener {
		void onLoadingFinished(HouyiScene scene);
	}
	
	public interface ScreenCaptureListener {
	    void doScreenCapture();
	}
	
	public interface PlayIconStatusListener {
	    void updatePlayIcon();
	}
	
	public static final int ViewModeNoraml = 0;
	public static final int ViewModeSingle = 1;
	private int mViewMode;
	
	private OnLoadingListener mListener;
	private Item mItem;
	private ProgressDialog mLoadingDialog;
	private HouyiSceneNode mGrid;
	private HouyiSceneNode mAxis;
	private HouyiCameraNode mCameraPlane;
	
	private boolean mIsCanceled;
	private ScreenCaptureListener mScreenCaptureListener;
	
	// camera
	private boolean mCameraEnabled;
	private int mCameraTextureId;
	private SurfaceTexture mCameraTexture = null;
	private Camera mCamera = null;
	private boolean mHasNewCameraFrame = false;
	private static Object mLock = new Object();
    
    protected int mScreenRotation;
    
    private int[] mTids;
    PlayIconStatusListener mPlayIconListener;
	
	public ModelLoaderRenderer(Item item, Context context) {
	    super(context);
	    DataManager.mFocusListener = this;
	    DataManager.mRenderer = this;
	    mItem = item;
	}

	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		super.onSurfaceCreated(gl, config);

		mIsCanceled = false;
		DataManager.clear();
		
		if (mWorld == null) {
		    Log.d("Houyi", "Creating World");
			if (mItem.getLocalPath() != null && mItem.getLocalPath().endsWith(".tmx")) {
			    mWorld = HouyiGameWorldOrtho.createWorld();
			} else {
			    Blue3DWorld world = Blue3DWorld.createWorld();
			    world.setViewMode(mViewMode);
			    mWorld = world;
			}
			
		    HEngine.setWorld(mWorld);
		    if (mAxis == null) {
                mAxis = mWorld.addAxis();
                showAxis(PreferenceManager.getInstance(mContext).getShowAxis());
            }
            
            if (mCameraPlane == null) {
                mCameraPlane = mWorld.addCameraPlane(mCameraTextureId);
                mCameraPlane.setVisibility(HouyiSceneNode.INVISIBLE);
            }
		} else {
		    Log.d("Houyi", "World Already Created");
		}
		
		HouyiScene scene = HouyiSceneManager.findSceneByName(mItem.getLocalPath());
        if (scene == null) {
    		DataManager.loadAsset(mItem, DataManager.mFocus);
            startLoadingDialog();
        } else {
            mWorld.addScene(scene);
            mWorld.setFocusScene(scene);
            HEngine.clearContext();
        }
		
        if (mTids == null) {
            createCameraTexture();
        }
        
        if (mCameraEnabled) {
            openCamera();
            if (mCameraPlane != null) {
                mCameraPlane.setVisibility(HouyiSceneNode.VISIBLE);
            }
        }
	}
	
	@Override
	protected void onClearContext() {
        super.onClearContext();
        createCameraTexture();
    }
    
    @Override
    protected void onDrawFrameLocked(GL10 gl) {
        if (!HEngine.isOn() || mWorld == null) {
            return;
        }
        
        if (mCamera != null) {
            gl.glActiveTexture(GL10.GL_TEXTURE0);
            GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, mCameraTextureId);
            if (mHasNewCameraFrame) {
                mHasNewCameraFrame = false;
                try {
                    mCameraTexture.updateTexImage();
                } catch (Exception e) {
                    Log.e(TAG, "Cannot update camera texture");
                    e.printStackTrace();
                }
            }
            
            int o = ((WindowManager)mContext.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getRotation();
            if (mScreenRotation != o) {
                mScreenRotation = o;
            }
            
            mCameraPlane.setIdentity();
            
            if (Utils.isInitialPortrait(mContext)) {
                if (mContext.getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE) {
                    if (mScreenRotation == Surface.ROTATION_270) {
                        mCameraPlane.rotate(0, 0, 1, (float) Math.PI);
                    }
                } else {
                    if (mScreenRotation == Surface.ROTATION_0) {
                        mCameraPlane.rotate(0, 0, 1, (float) Math.PI / 2);
                    } else {
                        mCameraPlane.rotate(0, 0, 1, -(float) Math.PI / 2);
                    }
                }
            } else {
                if (mScreenRotation == Surface.ROTATION_180) {
                    mCameraPlane.rotate(0, 0, 1, (float) Math.PI);
                } else if (mScreenRotation == Surface.ROTATION_90) {
                    mCameraPlane.rotate(0, 0, 1, -(float) Math.PI / 2);
                } else if (mScreenRotation == Surface.ROTATION_270) {
                    mCameraPlane.rotate(0, 0, 1, (float) Math.PI / 2);
                }
            }
        }
        super.onDrawFrameLocked(gl);
    }
    
    public void pause() {
        closeCamera();
        if (mWorld != null) {
            mWorld.clearScene();
        }
        mWorld = null;
        mCameraPlane = null;
        mAxis = null;
    }
	
	Runnable mUpdateProgressRunnable = new Runnable(){

		@Override
		public void run() {
			if (isLoading() && mLoadingDialog != null && mWorld != null) {
				float p = DataManager.getFocusLoadingProgress();
				mLoadingDialog.setProgress((int)(p * 100));
				mHandler.postDelayed(mUpdateProgressRunnable, 500);
			}
		}
	};
	
	private void createCameraTexture() {
	    mTids = new int[1];
        GLES20.glGenTextures(1, mTids, 0);
        mCameraTextureId = mTids[0];
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, mCameraTextureId);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_NEAREST);
        // Clamp to edge is only option.
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_WRAP_S, GL10.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_WRAP_T, GL10.GL_CLAMP_TO_EDGE);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, 0);
        
        mHasNewCameraFrame = false;
        mCameraTexture = new SurfaceTexture(mCameraTextureId);
        mCameraTexture.setOnFrameAvailableListener(new OnFrameAvailableListener() {
            @Override
            public void onFrameAvailable(SurfaceTexture surface) {
                mHasNewCameraFrame = true;
            }
        });
	}

	public void showGrid(boolean show) {
	    lockRenderThread();
	    if (mWorld != null && mGrid != null && !mIsCanceled) {
	        mGrid.setVisibility(show ? HouyiSceneNode.VISIBLE : HouyiSceneNode.INVISIBLE);
	    }
	    unlockRenderThread();
	}
	
	public void showAxis(boolean show) {
	    lockRenderThread();
        if (mWorld != null && mAxis != null && !mIsCanceled) {
            mAxis.setVisibility(show ? HouyiSceneNode.VISIBLE : HouyiSceneNode.INVISIBLE);
        }
        unlockRenderThread();
    }
	
	public void showAABB(boolean show) {
	    lockRenderThread();
        if (mWorld != null && !mIsCanceled) {
            mWorld.showAABB(show);
        }
        unlockRenderThread();
    }
	
	public void setCameraConstraints(boolean x, boolean y) {
	    int res = 0;
	    if (!x) res |= 2;
	    if (!y) res |= 1;
	    if (mWorld != null && mWorld.getCurrentCameraJ() != null) {
	        mWorld.getCurrentCameraJ().setPeekConstraint(res);
	    }
	}
	
	public void startPresent(float speed) {
		if (mWorld != null && mWorld.getCurrentCameraJ() != null) {
	        mWorld.getCurrentCameraJ().startPresentation(speed);
	    }
	}
	
	public void stopPresent() {
		if (mWorld != null && mWorld.getCurrentCameraJ() != null) {
	        mWorld.getCurrentCameraJ().stopPresentation();
	    }
	}
	
	public boolean isPresenting() {
		if (mWorld != null && mWorld.getCurrentCameraJ() != null) {
	        return mWorld.getCurrentCameraJ().isPresenting();
	    }
		return false;
	}
	
	public void setOnLoadingListener(OnLoadingListener listener) {
		mListener = listener;
	}
	
	public void setShadingMode(int mode) {
//	    if (mode == 3) {
//	        mScifiPass.makeActive();
//	        mPass.makeIdle();
//	    } else {
//	        mPass.makeActive();
//	        mScifiPass.makeIdle();
//	    }
	}
	
	public boolean isLoading() {
		return HouyiAssetManager.getInstance(mContext, DataManager.getInstance()).isLoading();
	}
	
	public void destroy() {
		mIsCanceled = true;
	    HouyiAssetManager.getInstance(mContext, DataManager.getInstance()).cancelAll();
	    DataManager.clear();
	    mWorld = null;
	    Log.w("Houyi", "ModelLoaderRenderer destroyed");
	}
	
	public boolean isCanceled() {
		return mIsCanceled;
	}
	
	public void requireScreenCapture(ScreenCaptureListener listener) {
	    mScreenCaptureListener = listener;
	}
	
	public boolean isShowingLoadingDialog() {
		return mLoadingDialog == null ? false : mLoadingDialog.isShowing();
	}
	
	public boolean isCameraEnabled() {
	    return mCameraEnabled;
	}
	
	public void toggleCamera() {
	    if (mWorld != null && HEngine.isOn() && !mIsCanceled) {
            mCameraEnabled = !mCameraEnabled;
            if (mCameraEnabled) {
                openCamera();
                mCameraPlane.setVisibility(HouyiSceneNode.VISIBLE);
            } else {
                closeCamera();
                mCameraPlane.setVisibility(HouyiSceneNode.INVISIBLE);
            }
	    }
    }
	
	public void openCamera() {
        if (mWorld == null || mCamera != null || !mCameraEnabled) return;
        
        synchronized (mLock) {
            try {
                
                mCamera = Camera.open(Camera.CameraInfo.CAMERA_FACING_BACK);
                
                Camera.Parameters params = mCamera.getParameters();
                int previewWidth = params.getPreviewSize().width;
                int previewHeight = params.getPreviewSize().height;
                Log.i(TAG, "camera preview default w = " + previewWidth + " h = " + previewHeight);
                Size size = getBestPreviewSize(mWidth, mHeight, params);
                previewWidth = size.width;
                previewHeight = size.height;
                Log.i(TAG, "set camera preview w = " + previewWidth + " h = " + previewHeight);
                params.setPreviewSize(previewWidth, previewHeight);
                mCamera.setParameters(params);
                mCameraPlane.setPreviewSize(previewWidth, previewHeight);
                
                mCameraTexture.setDefaultBufferSize(previewWidth, previewHeight);
                mCamera.setPreviewTexture(mCameraTexture);
                mCamera.startPreview();
                mCamera.autoFocus(new Camera.AutoFocusCallback(){

                    @Override
                    public void onAutoFocus(boolean success, Camera camera) {
                        if (success) {
//                            camera.autoFocus(null);
                        } else {
                            camera.autoFocus(this);
                        }
                    }
                    
                });
                Log.i(TAG, "camera opened");
            } catch (Exception e) {
                Log.e(TAG, "failed to open camera. ", e);
                closeCamera();
            }
        }
    }
    
    private Camera.Size getBestPreviewSize(float width, float height,
            Camera.Parameters parameters) {
        Camera.Size result = null;

        for (Camera.Size size : parameters.getSupportedPreviewSizes()) {
            if (size.width <= width && size.height <= height) {
                if (result == null) {
                    result = size;
                } else {
                    int resultArea = result.width * result.height;
                    int newArea = size.width * size.height;

                    if (newArea > resultArea) {
                        result = size;
                    }
                }
            }
        }

        return (result);
    }
    
    public void closeCamera() {
        mHasNewCameraFrame = false;
        try {
            if (mCamera != null) {
                synchronized (mLock) {
                    mCamera.release();
                    mCamera = null;
                    Log.i(TAG, "Camera closed");
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public void setViewMode(int viewMode) {
        mViewMode = viewMode;
    }
    
    public int getViewMode() {
        return mViewMode;
    }
    
    private void setScene(HouyiScene scene) {
        HouyiWorld world = mWorld;
        if (world != null) {
            world.addScene(scene);
            world.setFocusScene(scene);
        }
    }

    @Override
    public void onFocusChanged(final HouyiScene scene, int focus) {
        if (scene == null) {
            return;
        }
        
        lockRenderThread();
        
        try {
            if (mIsCanceled) {
                return;
            }
            
            Log.d("Houyi", "onFocusChanged index = " + scene.getIndex() + " filePath = " + scene.getName());
            
            int index = scene.getIndex();
            if (index == focus) {
                mItem = DataManager.mItems.get(focus);
                setScene(scene);
                
                mGrid = scene.addGrid();
                showGrid(PreferenceManager.getInstance(mContext).getShowGrid());
                showAxis(PreferenceManager.getInstance(mContext).getShowAxis());
                mWorld.showAABB(PreferenceManager.getInstance(mContext).getShowAABB());
                HouyiSetting.setShadingMode(PreferenceManager.getInstance(mContext).getShadingMode());
                if (scene.getSkeletonCount() > 0) {
                    scene.resumeSkeleton();
                    if (mPlayIconListener != null){
                        mPlayIconListener.updatePlayIcon();
                    }
                }
                
                if (DataManager.mItems.size() > 0) {
                    Item item = DataManager.mItems.get(index);
                    if (mScreenCaptureListener != null && !item.isSample() && !isThumbnailValid(item.getThumbnailPath())) {
                        mScreenCaptureListener.doScreenCapture();
                    }
                }
                if (mListener != null) {
                    mListener.onLoadingFinished(scene);
                }
            }
        } finally {
            unlockRenderThread();
        }
        
        // dismiss loading dialog
        mHandler.removeCallbacks(mShowLoading);
        mHandler.post(new Runnable() {
            public void run() {
                if (mLoadingDialog != null) {
                    mLoadingDialog.dismiss();
                }
            }
        });
    }
    
    @Override
    public void onFocusChangeBegin() {
        startLoadingDialog();
    }
    
    private boolean isThumbnailValid(String name) {
    	Activity a = (Activity)mContext;
    	FileInputStream in = null;
		try {
			in = a.openFileInput(name);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} finally {
			IOUtils.closeSilently(in);
		}
    	return in != null;
    }
    
    private Runnable mShowLoading = new Runnable() {
        public void run() {
            if (isLoading() && !mIsCanceled) {
                mLoadingDialog = new ProgressDialog(getContext());
                mLoadingDialog.setTitle("Loading");
                String fileName = "";
                mLoadingDialog.setMessage("Importing " + fileName);
                mLoadingDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
                mLoadingDialog.setProgress(0);
                mLoadingDialog.show();
                mHandler.postDelayed(mUpdateProgressRunnable, 500);
            }
        }
    };
    
    private void startLoadingDialog() {
        mHandler.postDelayed(mShowLoading, 1000);
    }
}
