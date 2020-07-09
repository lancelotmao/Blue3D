package com.dwtech.android.houyi;

public class HouyiWorld extends HouyiObject {
        
    public HouyiWorld(long worldId) {
        super(worldId);
    }
    
    public static HouyiWorld createWorld() {
    	return new HouyiWorld(nativeCreateWorld());
    }
    
    public HouyiCamera getCurrentCameraJ() {
        return new HouyiCamera(getCurrentCamera());
    }
    
    public void setCurrentCamera(HouyiCamera camera) {
        nativeSetCurrentCamera(camera.getId());
    }
    
    public void addSceneNode(HouyiSceneNode node) {
        addSceneNode(node.getId());
    }
    
    public HouyiSceneNode addAxis() {
        // c mesh is valid but java mesh not set
        return new HouyiSceneNode(nativeAddAxis());
    }
    
    public HouyiCameraNode addCameraPlane(int tid) {
        // c mesh is valid but java mesh not set
        return new HouyiCameraNode(nativeAddCameraPlane(tid));
    }
    
    public void prepare() {
        nativePrepare();
    }
    
    public void showAABB(boolean show) {
        nativeShowAABB(show);
    }
    
    public void addScene(HouyiScene scene) {
        nativeAddScene(scene);
    }
    
    public HouyiScene getFocusScene() {
        return new HouyiScene(nativeGetFocusScene());
    }
    
    public void setFocusScene(HouyiScene scene) {
        nativeSetFocusScene(scene);
    }
    
    public void clearScene() {
        nativeClearScene();
    }
    
    public void requestLayout() {
        nativeRequestLayout();
    }
    
    public HouyiView getRootView() {
        return new HouyiView(nativeGetRootView());
    }
    
    private static native long nativeCreateWorld();
    public native long getCurrentCamera();
    private native void nativeSetCurrentCamera(long camera);
    private native void addSceneNode(long node);
    public native void update();
    public native long selectMesh(float x, float y);
    public native void onTouch(float x, float y, int t);
    private native long nativeAddAxis();
    private native long nativeAddCameraPlane(int tid);
    private native void nativePrepare();
    private native void nativeShowAABB(boolean show);
    private native void nativeAddScene(HouyiScene scene);
    private native long nativeGetFocusScene();
    private native void nativeSetFocusScene(HouyiScene scene);
    private native void nativeClearScene();
    private native void nativeRequestLayout();
    private native long nativeGetRootView();
}
