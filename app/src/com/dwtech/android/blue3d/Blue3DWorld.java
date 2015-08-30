package com.dwtech.android.blue3d;

import com.dwtech.android.houyi.HouyiView;
import com.dwtech.android.houyi.HouyiWorld;

public class Blue3DWorld extends HouyiWorld {

	public Blue3DWorld(long worldId) {
		super(worldId);
	}

	public static Blue3DWorld createWorld() {
		return new Blue3DWorld(nativeCreateWorld());
	}
	
	public HouyiView getFilmstrip() {
	    return new HouyiView(nativeGetFilmstrip(getId()));
	}
	
	public void setViewMode(int viewMode) {
	    nativeSetViewMode(getId(), viewMode);
	}
	
	public void invalidateFilmstrip() {
	    nativeInvalidateFilmstrip(getId());
    }
	
	private static native long nativeCreateWorld();
	private native long nativeGetFilmstrip(long id);
	private native void nativeSetViewMode(long id, int mode);
	private native void nativeInvalidateFilmstrip(long id);
}
