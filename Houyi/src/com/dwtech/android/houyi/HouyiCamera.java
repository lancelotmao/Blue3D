
package com.dwtech.android.houyi;

public class HouyiCamera extends HouyiObject {
    protected HouyiCamera(long id) {
        super(id);
    }
    
    public void setPeekConstraint(int constraint) {
        setPeekConstraint(this.getId(), constraint);
    }
    
    public void startPresentation(float speed) {
    	nativeStartPresentation(speed);
    }
    
    public void stopPresentation() {
    	nativeStopPresentation();
    }
    
    public boolean isPresenting() {
    	return nativeIsPresenting();
    }
    
    public static native void startPeek(long camera);
    public static native void peek(long camera, float x1, float y1, float x2, float y2);
    public static native void endPeek(long camera);
    public static native void setFOV(long camera, float fov);
    public static native void setLookAt(long camera, float x, float y, float z);
    public static native void setPosition(long camera, float x, float y, float z);
    public static native void setUp(long camera, float x, float y, float z);
    public static native void setNearFar(long camera, float near, float far);
    public static native void startPeekAnimation(long camera, float x1, float y1, float x2, float y2, long lastTime);
    public static native void stopPeekAnimation(long camera);
    public static native void setPeekConstraint(long camera, int constraint);
    public native void nativeStartPresentation(float speed);
    public native void nativeStopPresentation();
    public native boolean nativeIsPresenting();
}
