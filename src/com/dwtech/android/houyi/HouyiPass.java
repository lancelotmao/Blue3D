package com.dwtech.android.houyi;

public class HouyiPass extends HouyiObject {

    public HouyiPass(long id) {
        super(id);
    }
    
    public static HouyiPass createPass(int shader) {
    	HouyiPass pass = new HouyiPass(loadProgram(shader));
    	return pass;
    }
    
    public static native int loadProgram(int shader);
    public native int getProgram();
    
    public void makeIdle() {
        setState(2);
    }
    
    public void makeActive() {
        setState(0);
    }
    
    private native void setState(int state);
}
