package com.dwtech.android.houyi;

import com.dwtech.data.Item;

public class HouyiSceneManager {
	public static void addScene(HouyiScene scene) {
	    nativeAddScene(scene.getId());
	}
	
	public static HouyiScene findSceneByName(String name) {
	    long sid = nativeFindSceneByName(name);
	    if (sid == 0) {
	        sid = nativeFindSceneByName(Item.getHouyiPath(name));
	    }
	    return sid == 0 ? null : new HouyiScene(sid);
	}
	
	public static int getSceneCount() {
	    return nativeGetSceneCount();
	}
	
	public static HouyiScene getScene(int index) {
	    return new HouyiScene(nativeGetScene(index));
	}
	
	public static void deleteSceneDefer(int index) {
	    nativeDeleteSceneDefer(index);
	}
	
	private static native void nativeAddScene(long scene);
	private static native long nativeFindSceneByName(String name);
	private static native int nativeGetSceneCount();
	private static native long nativeGetScene(int index);
	private static native void nativeDeleteSceneDefer(int index);
}
