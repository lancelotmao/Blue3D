package com.dwtech.android.houyi;

public class HouyiMaterial extends HouyiObject {

	protected HouyiMaterial(long id) {
		super(id);
	}

	// umimplemented yet
	public native void enableDepthTest(boolean enable);
	public native void enableLighting(boolean enable);
	public native void enableAlphaBlend(boolean enable);
}
