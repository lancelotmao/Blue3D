package com.dwtech.android.data;

import java.util.ArrayList;
import java.util.HashMap;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;

import com.dwtech.android.houyi.HEngine;
import com.dwtech.android.houyi.HouyiAssetManager;
import com.dwtech.android.houyi.HouyiLoader;
import com.dwtech.android.houyi.HouyiRange;
import com.dwtech.android.houyi.HouyiScene;
import com.dwtech.android.houyi.HouyiSceneManager;
import com.dwtech.android.houyi.HouyiWorld;
import com.dwtech.android.houyimodelloader.ModelLoaderRenderer;
import com.dwtech.data.Item;
import com.dwtech.data.LoadModelJob;
import com.dwtech.data.LoadModelJob.LoadModelDoneListener;

public class DataManager implements LoadModelDoneListener {
    public interface FocusListener {
        void onFocusChangeBegin();
        void onFocusChanged(HouyiScene scene, int newFocus);
    }
    
    private static DataManager mIns = new DataManager();
    public static FocusListener mFocusListener;
    public static ArrayList<Item> mItems = new ArrayList<Item>();
    public static int mFocus;
    public static ModelLoaderRenderer mRenderer;
    private static int mCacheSize = 5;
    private static HashMap<String, LoadModelJob> mMap = new HashMap<String, LoadModelJob>();
    
    public static DataManager getInstance() {
        return mIns;
    }
    
    public static int getSize() {
        return mItems.size();
    }
    
    public static Bitmap getFilmstripThumbnail(int index, Context context) {
        if (index < 0 || index >= mItems.size()) {
            return null;
        }
        return mItems.get(index).getFilmstripThumbnail(context);
    }
    
    public static int getFocus() {
        return mFocus;
    }
    
    public static Item getFocusItem() {
        return mItems.get(mFocus);
    }
    
    public static void setFocus(Context context, int focus) {
        if (focus != mFocus) {
            HouyiWorld world = HEngine.getWorld();
            world.clearScene();
            deleteOutOfRangeScene(focus);
            HouyiAssetManager.getInstance(mIns).cancelInactive(getRange(mFocus, mItems.size()));
            Item item = mItems.get(focus);
            String filePath = item.getLocalPath();
            HouyiScene scene = HouyiSceneManager.findSceneByName(filePath);
            if (scene == null) {
                if (mFocusListener != null) {
                    mFocusListener.onFocusChangeBegin();
                }
                loadAsset(item, focus);
            } else {
                if (mFocusListener != null) {
                    scene.setIndex(focus);
                    mFocusListener.onFocusChanged(scene, focus);
                }
            }
            mFocus = focus;
        }
    }

    @Override
    public void onDone(LoadModelJob loadModelJob) {
        HouyiScene scene = loadModelJob.getScene();
        if (scene == null) {
            return;
        }
        
        int index = scene.getIndex();
        mMap.remove(loadModelJob.getFilePath());
        HouyiRange range = getRange(mFocus, mItems.size());
        if (index < range.location || index > range.location + range.length) {
            Log.d("Houyi", "too far from focus, discard: %s" + loadModelJob.getFilePath());
            HEngine.deletePtr(scene.getId());
            return;
        }
        
        mRenderer.lockRenderThread();
        try {
	        if (mRenderer.isCanceled()) {
	        	HEngine.deletePtr(scene.getId());
	        	return;
	        }
	        
	        // within content range add to global scene array
	        HouyiSceneManager.addScene(scene);
	        
	        if (mFocusListener != null && loadModelJob.getIndex() == mFocus) {
	            mFocusListener.onFocusChanged(scene, mFocus);
	        }
	        
	        if (mRenderer.getViewMode() == ModelLoaderRenderer.ViewModeNoraml) {
	            addPrefetchTask();
	        }
        } finally {
        	mRenderer.unlockRenderThread();
        }
    }
    
    private static void deleteOutOfRangeScene(int newFocus) {
        int start = newFocus - mCacheSize / 2;
        if (start < 0)
            start = 0;
        for (int i = HouyiSceneManager.getSceneCount() - 1; i >= 0; --i) {
            HouyiScene scene = HouyiSceneManager.getScene(i);
            for (int j = 0; j < start; ++j) {
                Item item = mItems.get(j);
                if (scene.getName().equals(item.getLocalPath())) {
                    HouyiSceneManager.deleteSceneDefer(i);
                }
            }

            for (int j = start + mCacheSize; j < mItems.size(); ++j) {
                Item item = mItems.get(j);
                if (scene.getName().equals(item.getLocalPath())) {
                    HouyiSceneManager.deleteSceneDefer(i);
                }
            }
        }
    }

    static void addPrefetchTask() {
        HouyiRange range = getRange(mFocus, mItems.size());
        for (int i = 0; i < range.length; ++i) {
            int index = i + range.location;
            Item item = mItems.get(index);
            if (item.getLocalPath() == null) {
                continue;
            }
            String filePath = item.getLocalPath();
            HouyiScene scene = HouyiSceneManager.findSceneByName(filePath);
            if (scene == null) {
                loadAsset(item, index);
            }
        }
    }

    static HouyiRange getRange(int middle, int count) {
        HouyiRange ret = new HouyiRange();
        ret.length = Math.min(mCacheSize, count);
        int length = mCacheSize / 2;
        if (middle >= length && middle < count - length) {
            ret.location = middle - length;
        } else if (middle < length) {
            ret.location = 0;
        } else {
            ret.location = count - ret.length;
        }

        return ret;
    }
    
    public static void loadAsset(Item item, int index) {
        String key = item.getLocalPath();
        if (!mMap.containsKey(key)) {
            LoadModelJob job = HouyiAssetManager.getInstance(mIns).loadAsset(item, index, 0);
            mMap.put(key, job);
        }
    }
    
    public static float getFocusLoadingProgress() {
        LoadModelJob job = mMap.get(getFocusItem().getLocalPath());
        if (job != null) {
            HouyiLoader loader = job.getLoader();
            if (loader != null) {
                return loader.getProgress();
            }
        }
        return 0;
    }
    
    public static void clear() {
        mMap.clear();
    }
    
    public static void resume() {
        addPrefetchTask();
    }
}
