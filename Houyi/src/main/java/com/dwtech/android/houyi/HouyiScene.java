package com.dwtech.android.houyi;

import android.graphics.Bitmap;

public class HouyiScene extends HouyiObject {

    private int mIndex;
    
    protected HouyiScene(long id) {
        super(id);
    }

    public int getIndex() {
        return mIndex;
    }
    
    public void setIndex(int index) {
        mIndex = index;
    }
    
    public HouyiSceneNode addGrid() {
        // c mesh is valid but java mesh not set
        return new HouyiSceneNode(nativeAddGrid());
    }
    
    public int getSkeletonCount() {
        return nativeGetSkeletonCount();
    }
    
    public boolean isSkeletonPaused() {
        return nativeIsSkeletonPaused();
    }
    
    public void pauseSkeleton() {
        nativePauseSkeleton();
    }
    
    public void resumeSkeleton() {
        nativeResumeSkeleton();
    }

    public void addCubeEnvMap(Bitmap[] skyBoxBitmaps) {
        nativeAddCubeEnvMap(skyBoxBitmaps[0], skyBoxBitmaps[1], skyBoxBitmaps[2], skyBoxBitmaps[3], skyBoxBitmaps[4], skyBoxBitmaps[5]);
    }
    
    private native long nativeAddGrid();
    private native int nativeGetSkeletonCount();
    private native boolean nativeIsSkeletonPaused();
    private native void nativePauseSkeleton();
    private native void nativeResumeSkeleton();
    private native void nativeAddCubeEnvMap(Bitmap b0, Bitmap b1, Bitmap b2, Bitmap b3, Bitmap b4, Bitmap b5);
}
