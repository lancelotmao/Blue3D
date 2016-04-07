package com.dwtech.android.houyi;

public class HouyiLoader extends HouyiObject {
	protected HouyiLoader(long id) {
		super(id);
	}

    public static HouyiLoader createLoader(String filePath) {
        long nativeLoaderID = nativeCreateLoader(filePath == null ? "" : filePath);
        return nativeLoaderID == 0 ? null : new HouyiLoader(nativeLoaderID);
    }
	
    public HouyiScene load(byte[] data, int size, String filePath, int options) {
        long scene = nativeLoad(data, size, filePath == null ? "" : filePath, options);
        return scene == 0 ? null : new HouyiScene(scene);
    }
    
    public void cancel() {
    	nativeCancel();
    }
    
    public static String[] getTexturePaths(byte[] data, int size, String path) {
    	return nativeGetTexturePaths(data, size, path);
    }
    
    public float getProgress() {
        return nativeGetProgress();
    }
    
    private static native long nativeCreateLoader(String filePath);
    private native long nativeLoad(byte[] data, int size, String filePath, int options);
	private native void nativeCancel();
	private static native String[] nativeGetTexturePaths(byte[] data, int size, String path);
	private native float nativeGetProgress();
}
