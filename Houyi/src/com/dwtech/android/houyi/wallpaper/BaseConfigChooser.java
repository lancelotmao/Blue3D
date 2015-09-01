package com.dwtech.android.houyi.wallpaper;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;

import android.opengl.GLSurfaceView;

public abstract class BaseConfigChooser implements GLSurfaceView.EGLConfigChooser {
    public BaseConfigChooser(int[] configSpec) {
        mConfigSpec = configSpec;
    }

    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
        int[] num_config = new int[1];
        egl.eglChooseConfig(display, mConfigSpec, null, 0, num_config);

        int numConfigs = num_config[0];

        if (numConfigs <= 0) {
            throw new IllegalArgumentException("No configs match configSpec");
        }

        EGLConfig[] configs = new EGLConfig[numConfigs];
        egl.eglChooseConfig(display, mConfigSpec, configs, numConfigs, num_config);
        EGLConfig config = chooseConfig(egl, display, configs);
        if (config == null) {
            throw new IllegalArgumentException("No config chosen");
        }
        return config;
    }

    abstract EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs);

    protected int[] mConfigSpec;
}
