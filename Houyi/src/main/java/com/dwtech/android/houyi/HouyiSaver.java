package com.dwtech.android.houyi;

public class HouyiSaver extends HouyiObject {

    protected HouyiSaver(long id) {
        super(id);
    }

    public static boolean save(String path, HouyiScene scene) {
        return nativeSave(path, scene);
    }
    
    private static native boolean nativeSave(String path, HouyiScene scene);
}
