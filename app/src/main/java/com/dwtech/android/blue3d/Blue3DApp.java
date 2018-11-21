package com.dwtech.android.blue3d;

import android.app.Application;

import com.dwtech.android.houyi.utils.IOUtils;

public class Blue3DApp extends Application {
    static {
        IOUtils.loadLibrary("blue3d");
    }
}
