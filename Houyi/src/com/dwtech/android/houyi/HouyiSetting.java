package com.dwtech.android.houyi;

public class HouyiSetting {
	public static final int ESM_Default = 0;
	public static final int ESM_PPL = 1;
	public static final int ESM_Solid = 2;
	public static final int ESM_Wireframe = 3;
	public static final int ESM_Hologram = 4;
    
    public static native void setShadingMode(int mode);
    public static native void setLightingEnabled(boolean enable);
}
