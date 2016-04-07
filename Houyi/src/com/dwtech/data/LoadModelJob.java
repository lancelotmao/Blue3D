package com.dwtech.data;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.util.Log;

import com.dwtech.android.houyi.HEngine;
import com.dwtech.android.houyi.HouyiLoader;
import com.dwtech.android.houyi.HouyiSaver;
import com.dwtech.android.houyi.HouyiScene;
import com.dwtech.android.houyi.utils.IOUtils;

public class LoadModelJob implements Runnable {

    public static interface LoadModelDoneListener {
        void onDone(LoadModelJob loadModelJob);
    }
    
    private static final String TAG = LoadModelJob.class.getSimpleName();
    
    private Context mContext;
    private String mFilePath;
    private HouyiScene mScene;
    private int mIndex;
    private boolean mIsSample;
    private LoadModelDoneListener mListener;
    private HouyiLoader mLoader;
    private boolean mIsCanceled;
    private int mOptions;
    
    public LoadModelJob(Context context, int index, String filePath, boolean isSample, int options) {
        mContext = context;
        mIndex = index;
        mFilePath = filePath;
        mIsSample = isSample;
        mOptions = options;
    }
    
    @Override
    public void run() {
        InputStream in = null;
        HouyiScene scene = null;
        try {
            String path = mFilePath;
            String cachePath = Item.getHouyiPath(path);
            File cacheFile = new File(cachePath);
            if (cacheFile.exists() && false) {
                in = new FileInputStream(cacheFile);
                path = cachePath;
            } else if (mIsSample) {
                in = mContext.getAssets().open(mFilePath);
            } else {
                in = new FileInputStream(new File(mFilePath));
            }
            int length = in.available();
            byte[] data = new byte[length];
            in.read(data);
            
            mLoader = HouyiLoader.createLoader(path);
            if (mLoader != null) {
                scene = mLoader.load(data, data.length, path, mOptions);
                HEngine.deletePtr(mLoader.getId());
                if (mIsCanceled) {
                    Log.w(TAG, "LoadModelJob canceled. mIndex = " + mIndex + " mFilePath = " + path);
                    scene = null;
                } else if (scene == null) {
                    Log.e(TAG, "Loading failed. mIndex = " + mIndex + " mFilePath = " + path);
                } else {
                    scene.setIndex(mIndex);
                    if (!mFilePath.endsWith(".houyi") && !mIsSample) {
                        String savePath = Item.getHouyiPath(mFilePath);
                        File file = new File(savePath);
                        if(!file.exists()) {
                            //HouyiSaver.save(savePath, scene);
                        }
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            IOUtils.closeSilently(in);
        }
        mScene = scene;
        
        if (mListener != null) {
            mListener.onDone(this);
        }
    }
    
    public HouyiScene getScene() {
        return mScene;
    }
    
    public int getIndex() {
        return mIndex;
    }
    
    public String getFilePath() {
        return mFilePath;
    }
    
    public void setLoadModelListener(LoadModelDoneListener listener) {
        mListener = listener;
    }
    
    public void cancel() {
        mIsCanceled = true;
        if (mLoader != null) {
            mLoader.cancel();
        }
    }
    
    public HouyiLoader getLoader() {
        return mLoader;
    }
}
