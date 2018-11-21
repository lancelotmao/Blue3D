package com.dwtech.android.houyi;

public class HouyiMesh extends HouyiObject {
    protected HouyiMesh(long id) {
        super(id);
    }
    
    public static HouyiMesh createMesh(long id) {
        return new HouyiMesh(id);
    }
    
    public static final int RenderModeTriangles = 1;
    public static final int RenderModeLines = 2;
    public static final int RenderModePoints = 4;
    
    public void enableLighting() {
        enableLighting(true);
    }
    
    public void disableLighting() {
        enableLighting(false);
    }
    
	public native static void setTexture(long mesh, long texture);
	public native static void setRenderMode(long mesh, int renderMode);
	public native void showAABB();
	public native void hideAABB();
	public native void enableLighting(boolean enable);
}
