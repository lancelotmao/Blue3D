package com.dwtech.android.houyi;

public class HouyiSceneNode extends HouyiObject {

    public static final int VISIBLE = 0;
    public static final int INVISIBLE = 1;
    public static final int GONE = 2;
    
    private HouyiMesh mMesh;
    
    public HouyiSceneNode(long id) {
        super(id);
    }
    
    public HouyiSceneNode(HouyiMesh mesh) {
        super(createNode());
        setMesh(mesh);
    }

    public void setMesh(HouyiMesh mesh) {
        mMesh = mesh;
        nativeSetMesh(mMesh.getId());
    }
    
    public HouyiMesh getMesh() {
        return mMesh;
    }
    
    private static native long createNode();
    private native void nativeSetMesh(long mesh);
    public native void setIdentity();
    public native void translate(float x, float y, float z);
    public native void rotate(float x, float y, float z, float r);
    public native void scale(float x, float y, float z);
    
    public native void setVisibility(int visibility);
    public void setColor(HouyiScene scene, int color) {
        setColor(scene, ((color >> 16) & 0xff) / 255f,
                ((color >> 8) & 0xff) / 255f,
                ((color) & 0xff) / 255f,
                ((color >> 24) & 0xff) / 255f);
    }
    public native void setColor(HouyiScene scene, float r, float g, float b, float a);
}
