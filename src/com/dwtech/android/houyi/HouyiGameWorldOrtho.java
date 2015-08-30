package com.dwtech.android.houyi;

public class HouyiGameWorldOrtho extends HouyiWorld {

    public HouyiGameWorldOrtho(long id) {
        super(id);
    }

    public static HouyiGameWorldOrtho createWorld() {
        return new HouyiGameWorldOrtho(nativeCreateWorld());
    }
    
    private static native long nativeCreateWorld();
}
