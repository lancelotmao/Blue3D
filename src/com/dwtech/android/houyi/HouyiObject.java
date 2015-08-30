package com.dwtech.android.houyi;

public abstract class HouyiObject {
   
    //used by native side.
    private final long mHouyiId;
    
    protected HouyiObject(long id) {
        mHouyiId = id;
    }
    
    public long getId() {
        return mHouyiId;
    }
    
    public String getName() {
        return nativeGetName();
    }
    
    private native String nativeGetName();
}
