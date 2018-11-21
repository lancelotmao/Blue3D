package com.dwtech.android.houyi;

public class HouyiView extends HouyiSceneNode {

    public HouyiView(long id) {
        super(id);
    }
    
    public void fadeOut(double duration, double delay, int interType) {
        nativeFadeOut(duration, delay, interType);
    }
    
    private native void nativeFadeOut(double duration, double delay, int interType);
}
