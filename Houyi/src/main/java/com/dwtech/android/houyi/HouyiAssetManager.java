package com.dwtech.android.houyi;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import android.content.Context;

import com.dwtech.android.houyi.utils.PriorityThreadFactory;
import com.dwtech.data.Item;
import com.dwtech.data.LoadModelJob;
import com.dwtech.data.LoadModelJob.LoadModelDoneListener;

public class HouyiAssetManager implements LoadModelDoneListener {
	public static final int LO_MERGE_MESH = 1; // by default we should optimize
	public static final int LO_USE_FP = 1 << 1;
	
    public interface HouyiAssetManagerListener {
        void onLoadingFinished();
    }
    
    private static HouyiAssetManager mIns;
    private Context mContext;
    private LoadModelDoneListener mListener;
    private final ThreadPoolExecutor mExecutor;
    private BlockingQueue<LoadModelJob> mActive = new LinkedBlockingQueue<LoadModelJob>();
    
    public static HouyiAssetManager getInstance(Context context, LoadModelDoneListener listener) {
        if (mIns == null) {
            mIns = new HouyiAssetManager();
        }
        mIns.mContext = context;
        mIns.mListener = listener;
        return mIns;
    }
    
    public static HouyiAssetManager getInstance(LoadModelDoneListener listener) {
        if (mIns == null) {
            mIns = new HouyiAssetManager();
        }
        mIns.mListener = listener;
        return mIns;
    }
    
    private HouyiAssetManager(){
        mExecutor = new ThreadPoolExecutor(
                4, 6, 10,
                TimeUnit.SECONDS, new LinkedBlockingQueue<Runnable>(),
                new PriorityThreadFactory("thread-pool",
                android.os.Process.THREAD_PRIORITY_BACKGROUND));
    }
    
    public Context getContext() {
        return mContext;
    }
    
    public LoadModelJob loadAsset(Item item, int focus, int options) {
        LoadModelJob job = new LoadModelJob(mContext, focus, item.getLocalPath(), item.isSample(), options);
        job.setLoadModelListener(this);
        synchronized (mActive) {
            mActive.add(job);
        }
        mExecutor.execute(job);
        return job;
    }
    
    public void setListener(LoadModelDoneListener listener) {
        mListener = listener;
    }
    
    public void cancelAll() {
        synchronized (mActive) {
            for (LoadModelJob j : mActive) {
                j.cancel();
            }
            mActive.clear();
        }
    }
    
    public void cancelInactive(HouyiRange range) {
        int start = range.location;
        int length = range.length;
        synchronized (mActive) {
            for (LoadModelJob lj : mActive) {
                if (lj.getIndex() < start || lj.getIndex() >= start + length) {
                    lj.cancel();
                }
            }
        }
    }
    
    public boolean isLoading() {
    	return mActive.size() > 0;
    }

	@Override
	public void onDone(LoadModelJob loadModelJob) {
	    synchronized (mActive) {
	        mActive.remove(loadModelJob);
	    }
		if (mListener != null) {
			mListener.onDone(loadModelJob);
		}
	}
}
