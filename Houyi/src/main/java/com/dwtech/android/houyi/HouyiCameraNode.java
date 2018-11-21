package com.dwtech.android.houyi;

public class HouyiCameraNode extends HouyiSceneNode {
    public HouyiCameraNode(long id) {
        super(id);
    }

    public void setPreviewSize(float w, float h) {
        nativeSetPreviewSize(w, h);
    }
    
    public native void nativeSetPreviewSize(float w, float h);
}
