package com.dwtech.android.houyi;

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
    
    private native long nativeAddGrid();
    private native int nativeGetSkeletonCount();
    private native boolean nativeIsSkeletonPaused();
    private native void nativePauseSkeleton();
    private native void nativeResumeSkeleton();
}
