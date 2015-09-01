package com.dwtech.android.houyi.utils;

import android.content.Context;
import android.content.res.Configuration;
import android.view.Surface;
import android.view.WindowManager;

public class Utils {
    public static boolean isInitialPortrait(Context context) {
        int r = ((WindowManager)context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getRotation();
        int o = context.getResources().getConfiguration().orientation;
        return (o == Configuration.ORIENTATION_LANDSCAPE && (r == Surface.ROTATION_90 || r == Surface.ROTATION_270))
                || (o == Configuration.ORIENTATION_PORTRAIT && (r == Surface.ROTATION_0 || r == Surface.ROTATION_180));
    }
}
