package com.dwtech.android.houyi;

import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.graphics.SurfaceTexture.OnFrameAvailableListener;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.util.Log;
import android.view.Surface;
import android.view.WindowManager;

import com.dwtech.android.houyi.shader.HIShaders;

public class HouyiCameraPreviewRenderer extends HouyiGLSurfaceRenderer implements HIShaders{

	public static final int OCULUS_RIFT_DISTORT = 1;
	public static final int BARREL_DISTORT = 2;
	
	protected int mCameraTextureId;
	protected SurfaceTexture mCameraTexture = null;
	protected Camera mCamera = null;
	protected boolean mHasNewCameraFrame = false;
	protected static Object mLock = new Object();
	
	protected HouyiOrthographicCamera mOrthographicCamera;
	protected int mScreenRotation;
	protected int mDistortionType;
	protected int mNewDistortionType;// add this because we can only set shader in GL thread
	
	private HouyiSceneNode mPlaneLeft;
	private HouyiSceneNode mPlaneRight;
	private int[] mTids;
	private GL10 mGL;
	private boolean mMirrorEnabled;
	private boolean mCameraEnabled = true;
    
	public HouyiCameraPreviewRenderer(Context context) {
		super(context);
	}

	@Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	super.onSurfaceCreated(gl, config);
    	
    	// watch out, gl might be the same for example lock/unlock screen
    	Log.w(TAG, "" + gl);
    	
    	if (mWorld == null) {
    		mWorld = HEngine.getWorld();
        }
    	
    	setShader();
    	
        if (mTids == null || mGL != gl) {
            mTids = new int[1];
            GLES20.glGenTextures(1, mTids, 0);
            mCameraTextureId = mTids[0];
            GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, mCameraTextureId);
            GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST);
            GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_NEAREST);
            // Clamp to edge is only option.
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_WRAP_S, GL10.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_WRAP_T, GL10.GL_CLAMP_TO_EDGE);
        }
        
        mHasNewCameraFrame = false;
        if (mCameraTexture == null || mGL != gl) {
            mCameraTexture = new SurfaceTexture(mCameraTextureId);
            mCameraTexture.setOnFrameAvailableListener(new OnFrameAvailableListener() {
                @Override
                public void onFrameAvailable(SurfaceTexture surface) {
                    mHasNewCameraFrame = true;
                }
            });
        }
        
        mGL = gl;
    }
    
    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        super.onSurfaceChanged(gl, width, height);
        if (mOrthographicCamera == null || mOrthographicCamera.getId() == 0)
        	mOrthographicCamera = HouyiOrthographicCamera.createOrthographicCamera(0, width, 0, height);
        else
        	mOrthographicCamera.setViewPort(0, width, 0, height);
        mWorld.setCurrentCamera(mOrthographicCamera);
        
        if (mPlaneLeft == null) {
	        HouyiMesh planeLeft = HouyiGeometry.createPlane(1, 1, 1, 1);
	        HouyiMesh.setRenderMode(planeLeft.getId(), HouyiMesh.RenderModeTriangles);
	        mWorld = HEngine.getWorld();
	        mPlaneLeft = new HouyiSceneNode(planeLeft);
	        mWorld.addSceneNode(mPlaneLeft);
        }
        
        if (mPlaneRight == null) {
	        HouyiMesh planeRight = HouyiGeometry.createPlane(1, 1, 1, 1);
	        HouyiMesh.setRenderMode(planeRight.getId(), HouyiMesh.RenderModeTriangles);
	        mWorld = HEngine.getWorld();
	        mPlaneRight = new HouyiSceneNode(planeRight);
	        mWorld.addSceneNode(mPlaneRight);
        }
        
        mPlaneRight.setVisibility(mMirrorEnabled ? HouyiSceneNode.VISIBLE : HouyiSceneNode.GONE);

        if (mCamera != null)
        	closeCamera();
        
        // only open camera in landscape mode if mirror is enabled
        if (!mMirrorEnabled || width > height)
            openCamera();
    }
    
    @Override
    public void onSurfaceDestroyed() {
        closeCamera();
        if (mCameraTexture != null) {
            mCameraTexture.release();
            mCameraTexture.setOnFrameAvailableListener(null);
            mCameraTexture = null;
        }
        HEngine.deletePtr(mOrthographicCamera.getId());
        super.onSurfaceDestroyed();

        // Java clean
        mOrthographicCamera = null;
        mPlaneLeft = null;
        mPlaneRight = null;
        mTids = null;
    }

    protected void onDrawFrameLocked(GL10 gl) {
        if (mNewDistortionType != mDistortionType)
            setShader();
        
    	int loc = GLES20.glGetUniformLocation(mPass.getProgram(), "uIsMirrored");
    	GLES20.glUniform1i(loc, mMirrorEnabled ? 1 : 0);
    	
        // use GL_TEXTURE1 to avoid conflict with GL_TEXTURE_2D
        gl.glActiveTexture(GL10.GL_TEXTURE1);
        
        if (mCamera != null) {
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
            
            mPlaneLeft.setIdentity();
            
            if (mMirrorEnabled) {
            	if (mScreenRotation == Surface.ROTATION_270) {
                	mPlaneLeft.scale(-mWidth/2, mHeight, 1);
                } else if (mScreenRotation == Surface.ROTATION_90) {
                	mPlaneLeft.scale(mWidth/2, -mHeight, 1);
                }
                mPlaneLeft.translate(mWidth/4, mHeight/2, 0);
                
	            mPlaneRight.setIdentity();
	            if (mScreenRotation == Surface.ROTATION_270) {
	                mPlaneRight.scale(-mWidth/2, mHeight, 1);
	            } else if (mScreenRotation == Surface.ROTATION_90) {
	            	mPlaneRight.scale(mWidth/2, -mHeight, 1);
	            }
	            mPlaneRight.translate(mWidth*3/4, mHeight / 2, 0);
            } else {
            	// in normal case, left plane will occupy full screen
                if (mScreenRotation == Surface.ROTATION_270) {
                    mPlaneLeft.scale(mWidth, mHeight, 1);
                } else {
                    mPlaneLeft.scale(mWidth, -mHeight, 1);
                }
                mPlaneLeft.translate(mWidth/2, mHeight/2, 0);
                
            }
            
            super.onDrawFrameLocked(gl);
        } else {
            GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, 0);
            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        }
    }
    
    public void openCamera() {
    	if (mCamera != null || !mCameraEnabled) return;
    	
    	synchronized (mLock) {
    		try {
    	        
	            mCamera = Camera.open(Camera.CameraInfo.CAMERA_FACING_BACK);
    	        
	            Camera.Parameters params = mCamera.getParameters();
	            int previewWidth = params.getPreviewSize().width;
	            int previewHeight = params.getPreviewSize().height;
	            Log.i(TAG, "camera preview default w = " + previewWidth + " h = " + previewHeight);
	            if (useMinPreviewSize()) {
		            List<Size> list = mCamera.getParameters().getSupportedPictureSizes();
		            int minWidth=Integer.MAX_VALUE;
		            int minHeight=Integer.MAX_VALUE;
		            for(int i = 0; i<list.size(); i++){
		                if(list.get(i).height<minHeight){ // align vertical
		                    minWidth = list.get(i).width;
		                    minHeight = list.get(i).height;
		                }
		            }
		            previewWidth = minWidth;
		            previewHeight = minHeight;
		            
	            } else {
	                Size size = getBestPreviewSize(mWidth, mHeight, params);
	                previewWidth = size.width;
	                previewHeight = size.height;
	            }
	            Log.i(TAG, "set camera preview w = " + previewWidth + " h = " + previewHeight);
	            params.setPreviewSize(previewWidth, previewHeight);
	            mCamera.setParameters(params);
	            
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
//	                mCamera.stopPreview();
	                mCamera.release();
	                mCamera = null;
	                Log.i(TAG, "Camera closed");
            	}
            }
        } catch (Exception e) {
        	e.printStackTrace();
        }
    }
    
    public boolean useMinPreviewSize() {
    	return false;
    }
    
    public void setMirrorEnabled(boolean enable) {
        mMirrorEnabled = enable;
        if (mPlaneRight != null)
            mPlaneRight.setVisibility(mMirrorEnabled ? HouyiSceneNode.VISIBLE : HouyiSceneNode.GONE);
    }
    
    public void setCameraEnabled(boolean enable) {
    	mCameraEnabled = enable;
    }
    
    public void setDistortionType(int type) {
        mNewDistortionType = type;
    }
    
    private void setShader() {
        mDistortionType = mNewDistortionType;
        
        int shader = HSHADER_CAMERA;
        if (mDistortionType == OCULUS_RIFT_DISTORT) {
            shader = HSHADER_CAMERA_OCULUS_RIFT;
        } else if (mDistortionType == BARREL_DISTORT) {
            
        }
        mPass = new HouyiPass(HEngine.loadProgram(shader));
        GLES20.glUseProgram(mPass.getProgram());
        HEngine.setPass(mPass);
    }
}
