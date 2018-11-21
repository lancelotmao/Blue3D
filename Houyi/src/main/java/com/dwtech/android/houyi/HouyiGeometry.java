package com.dwtech.android.houyi;

public class HouyiGeometry extends HouyiMesh {
	protected HouyiGeometry(long id) {
        super(id);
    }
	
	public static HouyiGeometry createPlane(int horizontalCount, int verticalCount, float horizontalUnit, float verticalUnit) {
	    HouyiGeometry res = new HouyiGeometry(nativeCreatePlane(horizontalCount, verticalCount, horizontalUnit, verticalUnit));
	    return res;
	}
	
    public static native long nativeCreatePlane(int horizontalCount, int verticalCount, float horizontalUnit, float verticalUnit);
	public static native long createPrime(int count);
}
