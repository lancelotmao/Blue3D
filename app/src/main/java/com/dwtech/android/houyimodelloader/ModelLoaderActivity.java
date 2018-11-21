package com.dwtech.android.houyimodelloader;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;

import android.app.ActionBar;
import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.drawable.ColorDrawable;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.GestureDetector;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.RadioGroup;
import android.widget.RadioGroup.OnCheckedChangeListener;

import com.dwtech.android.blue3d.Blue3DWorld;
import com.dwtech.android.blue3d.HouyiTestActivity;
import com.dwtech.android.blue3d.R;
import com.dwtech.android.data.DataManager;
import com.dwtech.android.houyi.HEngine;
import com.dwtech.android.houyi.HouyiGLSurfaceView;
import com.dwtech.android.houyi.HouyiScene;
import com.dwtech.android.houyi.HouyiSceneNode;
import com.dwtech.android.houyi.HouyiSetting;
import com.dwtech.android.houyi.HouyiWorld;
import com.dwtech.android.houyi.utils.BitmapUtils;
import com.dwtech.android.houyi.utils.IOUtils;
import com.dwtech.android.houyimodelloader.ModelLoaderRenderer.OnLoadingListener;
import com.dwtech.android.houyimodelloader.ModelLoaderRenderer.PlayIconStatusListener;
import com.dwtech.android.houyimodelloader.ModelLoaderRenderer.ScreenCaptureListener;
import com.dwtech.data.Item;
//import com.google.analytics.tracking.android.EasyTracker;

public class ModelLoaderActivity extends HouyiTestActivity implements OnLoadingListener, ScreenCaptureListener, PlayIconStatusListener {
    private ModelLoaderRenderer mRenderer;
    private PreferenceManager mPreMan;
    private ImageView mIVPlay;
    private FrameLayout mMainLayout;
    private boolean mEngineStalled;
    private boolean mIsPresenting;
    private int mScreenShotType;
    private int mScreenShotRequestedIndex;// focus index when screen shot is requested
    private GestureDetector mGestureDetector;
    public static Bitmap mScreenshot;
    
    @Override
	public void onCreate(Bundle savedInstanceState) {
        Log.d("Houyi", "ModelLoaderActivity onCreate");
        
        super.onCreate(savedInstanceState);
        
        mGestureDetector = new GestureDetector(this, mGestureListener);
        mPreMan = PreferenceManager.getInstance(this);
        mPreMan.load();
        
        final ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
        
        Intent i = getIntent();
        Uri uri = i.getData();
        boolean isDB = i.getBooleanExtra("isDB", false);
        boolean isSingle = false;
        Item item = null;
        if (uri != null) { // from MyFile, Email attachment, etc
            DataManager.mFocus = 0;
            item = new Item();
            item.setLocalPath(uri.getPath());
            isSingle = true;
        } else if (isDB) {
        	item = DataManager.getFocusItem();
        	isSingle = true;
        } else {
            item = DataManager.getFocusItem();
        }
        mRenderer = new ModelLoaderRenderer(item, this);
        if (isSingle) {
            mRenderer.setViewMode(ModelLoaderRenderer.ViewModeSingle);
        } else {
            mRenderer.setViewMode(ModelLoaderRenderer.ViewModeNoraml);
        }
        mRenderer.setOnLoadingListener(this);
        mRenderer.mPlayIconListener = this;
        mView = new HouyiGLSurfaceView(this, mRenderer);
        setContentView(R.layout.viewer);
        mMainLayout = (FrameLayout)findViewById(R.id.main_frame);
        mMainLayout.addView(mView, 0);
        addDebugView();
        
        // view for play/pause when there is animation
        mIVPlay = (ImageView)findViewById(R.id.iv_play);
        mIVPlay.setOnClickListener(new OnClickListener(){

			@Override
			public void onClick(View v) {
			    mRenderer.lockRenderThread();
			    try {
    			    HouyiWorld world = mRenderer.getWorld();
    			    if (world != null) {
        			    if (world.getFocusScene().isSkeletonPaused()) {
        			        world.getFocusScene().resumeSkeleton();
                        } else {
                            world.getFocusScene().pauseSkeleton();
                        }
        				updatePlayIcon();
    			    }
			    } finally {
			        mRenderer.unlockRenderThread();
			    }
			}
        });
        
        applySetting();
        mRenderer.requireScreenCapture(this);
        hideActionBarDelay();
    }

    @Override
    protected void onResume() {
        super.onResume();
        updatePlayIcon();
    }

    @Override
	public void onStart() {
		super.onStart();
//		EasyTracker.getInstance(this).activityStart(this);
	}

	@Override
	public void onStop() {
		super.onStop();
//		EasyTracker.getInstance(this).activityStop(this);
	}
	
    @Override
    protected void onPause() {
        mRenderer.pause();
        super.onPause();
    }
    
    @Override
    protected void onDestroy() {
        Log.d("Houyi", "ModelLoaderActivity onDestroy");
        mRenderer.lockRenderThread();
        mRenderer.destroy();
        if (!mEngineStalled && HEngine.isOn()){
        	HEngine.stall();
        }
        super.onDestroy();
        mRenderer.unlockRenderThread();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu items for use in the action bar
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.action_view, menu);
        return super.onCreateOptionsMenu(menu);
    }
    
    @Override
	public boolean onPrepareOptionsMenu(Menu menu) {
        mHandler.removeCallbacks(mHideActionBarRunnable);
		MenuItem mi = menu.findItem(R.id.action_presentation);
		this.mRenderer.lockRenderThread();
		try {
			if (mIsPresenting) {
				mi.setTitle(R.string.stop_presentation);
			} else {
				mi.setTitle(R.string.presentation);
			}
			return super.onPrepareOptionsMenu(menu);
		} finally {
			this.mRenderer.unlockRenderThread();
		}
	}

	@Override
    public boolean onOptionsItemSelected(MenuItem item) {
	    if (mRenderer.isLoading() && item.getItemId() != android.R.id.home) {
            Log.w("Houyi", "cannot use menu since world is loading");
            return false;
        }
	    
	    mRenderer.lockRenderThread();
	    try {
            switch (item.getItemId()) {
            case android.R.id.home: 
                onBackPressed();
                break;
            case R.id.action_camera:
                mRenderer.toggleCamera();
                break;
            case R.id.action_show_hide:
                showDialogShowHide();
                break;
            case R.id.action_control:
                showDialogControl();
                break;
            case R.id.action_shading:
                showDialogShading();
                break;
            case R.id.action_presentation:
            	presentMenuPressed();
            	break;
            case R.id.action_screenshot:
                takeScreenShot();
                break;
            }
	    } finally {
	        mRenderer.unlockRenderThread();
	    }
        return false;
    }
	
    private void showDialogShowHide() {
        final Dialog dialog = new Dialog(this);
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        dialog.getWindow().setBackgroundDrawable(new ColorDrawable(android.graphics.Color.TRANSPARENT));
        dialog.setContentView(R.layout.dialog_show_hide);
        
        CheckBox cb = (CheckBox) dialog.findViewById(R.id.cb_stats);
        cb.setChecked(mPreMan.getShowStats());
        cb.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    mDebugView.setVisibility(View.VISIBLE);
                } else {
                    mDebugView.setVisibility(View.INVISIBLE);
                }
                mPreMan.setShowStats(isChecked);
            }
        });
        
        cb = (CheckBox) dialog.findViewById(R.id.cb_grid);
        cb.setChecked(mPreMan.getShowGrid());
        cb.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                mPreMan.setShowGrid(isChecked);
                mRenderer.showGrid(isChecked);
            }
        });
        
        cb = (CheckBox) dialog.findViewById(R.id.cb_axis);
        cb.setChecked(mPreMan.getShowAxis());
        cb.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                mPreMan.setShowAxis(isChecked);
                mRenderer.showAxis(isChecked);
            }
        });
        
        cb = (CheckBox) dialog.findViewById(R.id.cb_aabb);
        cb.setChecked(mPreMan.getShowAABB());
        cb.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                mPreMan.setShowAABB(isChecked);
                mRenderer.showAABB(isChecked);
            }
        });
        
        dialog.show();
    }
    
    private void showDialogControl() {
        final Dialog dialog = new Dialog(this);
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        dialog.getWindow().setBackgroundDrawable(new ColorDrawable(android.graphics.Color.TRANSPARENT));
        dialog.setContentView(R.layout.dialog_control);
        
        final CheckBox cbx = (CheckBox) dialog.findViewById(R.id.cb_x);
        final CheckBox cby = (CheckBox) dialog.findViewById(R.id.cb_y);
        cbx.setChecked(mPreMan.getRotateX());
        cby.setChecked(mPreMan.getRotateY());
        cbx.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                boolean x = isChecked;
                boolean y = cby.isChecked();
                mPreMan.setRotateX(isChecked);
                mRenderer.setCameraConstraints(x, y);
            }
        });
        cby.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                boolean x = cbx.isChecked();
                boolean y = isChecked;
                mPreMan.setRotateY(isChecked);
                mRenderer.setCameraConstraints(x, y);
            }
        });
        dialog.show();
    }
    
    private void showDialogShading() {
        final Dialog dialog = new Dialog(this);
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        dialog.getWindow().setBackgroundDrawable(new ColorDrawable(android.graphics.Color.TRANSPARENT));
        dialog.setContentView(R.layout.dialog_shading);
        
        int curMode = mPreMan.getShadingMode();
        RadioGroup rg = (RadioGroup)dialog.findViewById(R.id.rg_shading_mode);
        if (curMode == 0)
            rg.check(R.id.rb_sm_default);
        else if (curMode == 1)
            rg.check(R.id.rb_sm_ppl);
        else if (curMode == 2)
            rg.check(R.id.rb_sm_solid);
        else if (curMode == 3)
            rg.check(R.id.rb_sm_wireframe);
        else if (curMode == 4)
            rg.check(R.id.rb_sm_hologram);
        
        rg.setOnCheckedChangeListener(new OnCheckedChangeListener(){

            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                int mode = 0;
                switch (group.getCheckedRadioButtonId()) {
                case R.id.rb_sm_default:
                    mode = 0;
                    break;
                case R.id.rb_sm_ppl:
                    mode = 1;
                    break;
                case R.id.rb_sm_solid:
                    mode = 2;
                    break;
                case R.id.rb_sm_wireframe:
                    mode = 3;
                    break;
                case R.id.rb_sm_hologram:
                    mode = 4;
                    break;
                }
                
                mRenderer.lockRenderThread();
                HouyiSetting.setShadingMode(mode);
                mRenderer.setShadingMode(mode);
                mPreMan.setShadingMode(mode);
                mRenderer.unlockRenderThread();
            }
        });
        dialog.show();
    }
    
    private void presentMenuPressed() {
    	if (mIsPresenting) {
    		mRenderer.stopPresent();
    	} else {
    		mRenderer.startPresent(1);
    	}
    	mIsPresenting = !mIsPresenting;
    }
    
    private void applySetting() {
        mDebugView.setVisibility(mPreMan.getShowStats() ? View.VISIBLE : View.INVISIBLE);
        mRenderer.showGrid(mPreMan.getShowGrid());
    }

	@Override
	public void onLoadingFinished(HouyiScene scene) {
	    final int cnt = scene.getSkeletonCount();
        mHandler.post(new Runnable() {
            public void run() {
                mIVPlay.setVisibility(cnt == 0 ? View.GONE : View.VISIBLE);
            }
        });
	}
	
	@Override
    public void onBackPressed() {
		Log.d("Houyi", "onBackPressed");
		mHandler.removeCallbacks(mHideActionBarRunnable);
		
		mRenderer.lockRenderThread();
		try {
		    mRenderer.destroy();
			if (!mRenderer.isShowingLoadingDialog()) {
			    mRenderer.closeCamera();
				HEngine.stall();
				mEngineStalled = true;
				Log.d("Houyi", "onBackPressed. HEngine.stall()");
			}
		} finally {
			mRenderer.unlockRenderThread();
		}
		super.onBackPressed();
	}
	
	@Override
    public void onScreenCaptured(Bitmap bitmap) {
	    if (mScreenShotType == 0) {
	        if (mScreenShotRequestedIndex != DataManager.mFocus) {
	            // focus has changed. discard and request again
	            doScreenCapture();
	            return;
	        }
	        
    	    FileOutputStream fos = null;
            try {
                Bitmap scaled = BitmapUtils.cropCenter(bitmap, 256, 256);
                String fn = DataManager.getFocusItem().getThumbnailPath();
                fos = openFileOutput(fn, Context.MODE_PRIVATE);
                scaled.compress(CompressFormat.PNG, 100, fos);
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } finally {
                IOUtils.closeSilently(fos);
                mRenderer.showGrid(mPreMan.getShowGrid());
                mRenderer.showAxis(mPreMan.getShowAxis());
                mRenderer.showAABB(mPreMan.getShowAABB());
                ((Blue3DWorld)mRenderer.getWorld()).invalidateFilmstrip();
            }
	    } else {
	        mScreenshot = bitmap;
	        Intent i = new Intent();
	        i.setClass(this, SingleImageActivity.class);
	        startActivity(i);
	    }
    }

    @Override
    public void doScreenCapture() {
        mScreenShotRequestedIndex = DataManager.mFocus;
        mScreenShotType = 0;
        mRenderer.showGrid(false);
        mRenderer.showAxis(false);
        mRenderer.showAABB(false);
        captureScreen(this);
    }
    
    private void takeScreenShot() {
        mScreenShotType = 1;
        captureScreen(this);
    }
    
    @Override
    public void updatePlayIcon() {
        mHandler.post(new Runnable(){
            @Override
            public void run() {
                if (HEngine.getWorld().getFocusScene().isSkeletonPaused()) {
                    mIVPlay.setImageResource(R.drawable.play);
                } else {
                    mIVPlay.setImageResource(R.drawable.pause);
                }
            }
        });
    }
    
    private GestureDetector.OnGestureListener mGestureListener = new GestureDetector.SimpleOnGestureListener(){
        @Override
        public boolean onSingleTapUp(MotionEvent e) {
            getActionBar().show();
            mRenderer.lockRenderThread();
            try {
                HouyiWorld world = mRenderer.getWorld();
                Log.d("Houyi", "onSingleTapUp: " + world);
                if (world != null && HEngine.isOn() && !mRenderer.isCanceled()) {
                    world.getRootView().setVisibility(HouyiSceneNode.VISIBLE);
                    if (world.getFocusScene().getSkeletonCount() > 0) {
                        mIVPlay.setVisibility(View.VISIBLE);
                    }
                }
                Log.d("Houyi", "onSingleTapUp end");
            } finally {
                mRenderer.unlockRenderThread();
            }
            return false;
        }
    };

    @Override
    public boolean dispatchTouchEvent(MotionEvent event) {
        mGestureDetector.onTouchEvent(event);
        hideActionBarDelay();
        return super.dispatchTouchEvent(event);
    }
    
    private void hideActionBarDelay() {
        mHandler.removeCallbacks(mHideActionBarRunnable);
        mHandler.postDelayed(mHideActionBarRunnable, 3000);
    }
    
    private Runnable mHideActionBarRunnable = new Runnable() {
        @Override
        public void run() {
            mRenderer.lockRenderThread();
            try {
                if (HEngine.isOn()) {
                    getActionBar().hide();
                    HouyiWorld world = mRenderer.getWorld();
                    if (world != null && !mRenderer.isCanceled() && HEngine.isOn()) {
                        world.getRootView().setVisibility(HouyiSceneNode.GONE);
                    }
                    mIVPlay.setVisibility(View.GONE);
                }
            } finally {
                mRenderer.unlockRenderThread();
            }
        }
    };
}
