
package com.dwtech.android.houyi;

import android.app.Activity;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.util.Log;

import com.dwtech.android.houyi.HouyiGLSurfaceRenderer.OnScreenCaptureListener;

public class HouyiActivity extends Activity implements OnScreenCaptureListener {
    
    protected HouyiGLSurfaceView mView;
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i("Houyi", "HouyiActivity onCreate " + this);
        super.onCreate(savedInstanceState);
        HEngine.ignite(this.getApplicationContext());
        HEngine.incRef();
    }
    
    @Override
    protected void onPause() {
        Log.i("Houyi", "HouyiActivity onPause " + this);
        if (HEngine.isOn()) {
            HEngine.onPause();
        }
        if (mView != null) {
            mView.onPause();
        }
        super.onPause();
    }
    
    @Override
    protected void onResume() {
        Log.i("Houyi", "HouyiActivity onResume " + this);
        super.onResume();
        if (!HEngine.isOn()) {
            HEngine.ignite(this.getApplicationContext());
        }
        HEngine.onResume();
        if (mView != null) {
            mView.onResume();
        }
    }
    
    @Override
    protected void onStop() {
        Log.i("Houyi", "HouyiActivity onStop " + this);
        super.onStop();
    }
    
    @Override
    protected void onDestroy() {
        Log.i("Houyi", "HouyiActivity onDestroy " + this);
        if (HEngine.isOn()) {
        	HEngine.decRef();
        }
        super.onDestroy();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
    }

    @Override
    public void onBackPressed() {
        if (!HEngine.isOn()) {
            super.onBackPressed();
            return;
        }
        
        mView.getRenderer().lockRenderThread();
        try{
            boolean handled = HEngine.onBackPressed();
            if (!handled)
                super.onBackPressed();
        } finally {
            mView.getRenderer().unlockRenderThread();
        }
    }

    @Override
    public void onScreenCaptured(Bitmap bitmap) {
    }
    
    public void captureScreen() {
        captureScreen(this);
    }
    
    public void captureScreen(final OnScreenCaptureListener listener) {
        mView.mRenderer.captureScreen(listener);
    }
}
