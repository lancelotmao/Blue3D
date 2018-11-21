package com.dwtech.android.houyi;

public class HouyiOrthographicCamera extends HouyiCamera{

    protected HouyiOrthographicCamera(long id) {
        super(id);
    }
    
    public static HouyiOrthographicCamera createOrthographicCamera(float l, float r, float t, float b) {
        long handle = nativeCreateOrthographicCamera(l, r, t, b);
        return new HouyiOrthographicCamera(handle);
    }
    
    public native void setViewPort(float l, float r, float t, float b);
    
    private native static long nativeCreateOrthographicCamera(float l, float r, float t, float b);
}
