
package com.dwtech.android.blue3d;

import java.lang.ref.WeakReference;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.LayoutInflater;
import android.view.WindowManager;
import android.widget.TextView;

import com.dwtech.android.houyi.HEngine;
import com.dwtech.android.houyi.HouyiActivity;
import com.dwtech.android.houyi.math.HMath;
import com.dwtech.android.blue3d.R;

public class HouyiTestActivity extends HouyiActivity {
    protected View mDebugView;
    protected TextView mTVFPS;
    protected TextView mTVVCount;
    protected TextView mTVElementCount;
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
    }
    
    protected void addDebugView() {
        LayoutInflater mInflater = (LayoutInflater)getApplicationContext().getSystemService(
                Context.LAYOUT_INFLATER_SERVICE);
        mDebugView = mInflater.inflate(R.layout.debug, null);
        mTVFPS = (TextView)mDebugView.findViewById(R.id.tv_fps);
        mTVVCount = (TextView)mDebugView.findViewById(R.id.tv_vcount);
        mTVElementCount = (TextView)mDebugView.findViewById(R.id.tv_element_count);
        
        mHandler = new HouyiHandler(this);
        mHandler.sendMessage(Message.obtain(mHandler));
        addContentView(mDebugView, new LayoutParams(LayoutParams.WRAP_CONTENT,
                LayoutParams.WRAP_CONTENT));
    }
    
    protected Handler mHandler;
    
    protected static class HouyiHandler extends Handler {
        final WeakReference<HouyiTestActivity> mActivity;
        
        HouyiHandler(HouyiTestActivity activity) {
            mActivity = new WeakReference<HouyiTestActivity>(activity);
        }
        
        @Override
        public void handleMessage(Message msg) {
            HouyiTestActivity activity = mActivity.get();
            if (activity != null) {
                sendMessageDelayed(Message.obtain(), 1000);
                activity.mView.getRenderer().lockRenderThread();
                try {
                    activity.mTVFPS.setText("FPS: " + (int)(HMath.clamp(HEngine.getFPS(), 0, 60)));
                    activity.mTVVCount.setText("Vertex Count: " + HEngine.getVertexCount());
                    int pc = HEngine.getPointCount();
                    int lc = HEngine.getLineCount();
                    int vc = HEngine.getTriangleCount();
                    StringBuffer sb = new StringBuffer();
                    if (pc > 0) {
                        sb.append("Point Count: " + pc);
                    }
                    if (lc > 0) {
                        if (sb.length() > 0)
                            sb.append(", ");
                        sb.append("Line Count: " + lc);
                    }
                    if (vc > 0) {
                        if (sb.length() > 0)
                            sb.append(", ");
                        sb.append("Triangle Count: " + vc);
                    }
                    activity.mTVElementCount.setText(sb.toString());
                } finally {
                    activity.mView.getRenderer().unlockRenderThread();
                }
            }
        }
    }
}
