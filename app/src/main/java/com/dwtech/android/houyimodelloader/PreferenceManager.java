package com.dwtech.android.houyimodelloader;

import android.content.Context;
import android.content.SharedPreferences;

public class PreferenceManager {
    private static final String SP_NAME = "Prefs";
    private static final String SP_KEY_IS_PRO = "isPro";
    private static final String SP_KEY_DB_ACCESS_TAKEN = "dbAccessToken";
    private static final String SP_KEY_SHOW_STATS = "showStats";
    private static final String SP_KEY_SHOW_GRID = "showGrid";
    private static final String SP_KEY_SHOW_AXIS = "showAxis";
    private static final String SP_KEY_SHOW_AABB = "showAABB";
    private static final String SP_KEY_SHOW_SKY_BOX = "showSkyBox";
    
    private static final String SP_KEY_ROTATE_X = "rotateX";
    private static final String SP_KEY_ROTATE_Y = "rotateY";
    
    private static final String SP_KEY_LAST_CD = "lastCD";
    
    private static PreferenceManager mIns;
    private Context mContext;
    
    private String mDBAccessToken;
    private boolean mShowStats;
    private boolean mShowGrid;
    private boolean mShowAxis;
    private boolean mShowAABB;
    private boolean mShowSkyBox;
    
    private boolean mRotateX;
    private boolean mRotateY;
    
    private int mShadingMode;
    private String mLastCD;
    
    private PreferenceManager() {
    }

    public static PreferenceManager getInstance(Context context) {
        if (mIns == null) {
            mIns = new PreferenceManager();
        }
        mIns.mContext = context;
        return mIns;
    }
    
    public void load() {
        SharedPreferences settings = mContext.getSharedPreferences(SP_NAME, 0);
        mDBAccessToken = settings.getString(SP_KEY_DB_ACCESS_TAKEN, null);
        mShowStats = settings.getBoolean(SP_KEY_SHOW_STATS, false);
        mShowGrid = settings.getBoolean(SP_KEY_SHOW_GRID, false);
        mShowAxis = settings.getBoolean(SP_KEY_SHOW_AXIS, false);
        mShowAABB = settings.getBoolean(SP_KEY_SHOW_AABB, false);
        mShowSkyBox = settings.getBoolean(SP_KEY_SHOW_SKY_BOX, false);
        mRotateX = settings.getBoolean(SP_KEY_ROTATE_X, true);
        mRotateY = settings.getBoolean(SP_KEY_ROTATE_Y, true);
//        mShadingMode = settings.getInt(SP_KEY_SHADING_MODE, 0);
        mLastCD = settings.getString(SP_KEY_LAST_CD, null);
    }
    
    public boolean isPro() {
        SharedPreferences settings = mContext.getSharedPreferences(SP_NAME, 0);
        return settings.getBoolean(SP_KEY_IS_PRO, false);
    }
    
    public void savePro() {
        SharedPreferences settings = mContext.getSharedPreferences(SP_NAME, 0);
        settings.edit().putBoolean(SP_KEY_IS_PRO, true).commit();
    }
    
    public String getDBAccessToken() {
        return mDBAccessToken;
    }
    
    public void saveDBAccessToken(String at) {
        mDBAccessToken = at;
        SharedPreferences settings = mContext.getSharedPreferences(SP_NAME, 0);
        settings.edit().putString(SP_KEY_DB_ACCESS_TAKEN, at).commit();
    }
    
    public boolean getShowStats() {
        return mShowStats;
    }
    
    public void setShowStats(boolean show) {
        mShowStats = show;
        SharedPreferences settings = mContext.getSharedPreferences(SP_NAME, 0);
        settings.edit().putBoolean(SP_KEY_SHOW_STATS, show).commit();
    }
    
    public boolean getShowGrid() {
        return mShowGrid;
    }
    
    public void setShowGrid(boolean show) {
        mShowGrid = show;
        SharedPreferences settings = mContext.getSharedPreferences(SP_NAME, 0);
        settings.edit().putBoolean(SP_KEY_SHOW_GRID, show).commit();
    }
    
    public boolean getShowAxis() {
        return mShowAxis;
    }
    
    public void setShowAxis(boolean show) {
        mShowAxis = show;
        SharedPreferences settings = mContext.getSharedPreferences(SP_NAME, 0);
        settings.edit().putBoolean(SP_KEY_SHOW_AXIS, show).commit();
    }
    
    public boolean getShowAABB() {
        return mShowAABB;
    }
    
    public void setShowAABB(boolean show) {
        mShowAABB = show;
        SharedPreferences settings = mContext.getSharedPreferences(SP_NAME, 0);
        settings.edit().putBoolean(SP_KEY_SHOW_AABB, show).commit();
    }

    public boolean getShowSkyBox() {
        return mShowSkyBox;
    }

    public void setShowSkyBox(boolean show) {
        mShowSkyBox = show;
        SharedPreferences settings = mContext.getSharedPreferences(SP_NAME, 0);
        settings.edit().putBoolean(SP_KEY_SHOW_SKY_BOX, show).commit();
    }
    
    public boolean getRotateX() {
        return mRotateX;
    }
    
    public void setRotateX(boolean x) {
        mRotateX = x;
//        SharedPreferences settings = mContext.getSharedPreferences(SP_NAME, 0);
//        settings.edit().putBoolean(SP_KEY_ROTATE_X, x).commit();
    }
    
    public boolean getRotateY() {
        return mRotateY;
    }
    
    public void setRotateY(boolean y) {
        mRotateY = y;
//        SharedPreferences settings = mContext.getSharedPreferences(SP_NAME, 0);
//        settings.edit().putBoolean(SP_KEY_ROTATE_Y, y).commit();
    }
    
    public int getShadingMode() {
        return mShadingMode;
    }
    
    public void setShadingMode(int mode) {
        mShadingMode = mode;
//        SharedPreferences settings = mContext.getSharedPreferences(SP_NAME, 0);
//        settings.edit().putInt(SP_KEY_SHADING_MODE, mode).commit();
    }
    
    public String getLastCD() {
        return mLastCD;
    }
    
    public void setLastCD(String lastCD) {
        mLastCD = lastCD;
        SharedPreferences settings = mContext.getSharedPreferences(SP_NAME, 0);
        settings.edit().putString(SP_KEY_LAST_CD, lastCD).commit();
    }
}
