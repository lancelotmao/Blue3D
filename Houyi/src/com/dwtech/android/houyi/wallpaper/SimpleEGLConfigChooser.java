package com.dwtech.android.houyi.wallpaper;

public class SimpleEGLConfigChooser extends ComponentSizeChooser {
    public SimpleEGLConfigChooser(boolean withDepthBuffer) {
        super(4, 4, 4, 0, withDepthBuffer ? 16 : 0, 0);
        // Adjust target values. This way we'll accept a 4444 or
        // 555 buffer if there's no 565 buffer available.
        mRedSize = 5;
        mGreenSize = 6;
        mBlueSize = 5;
    }
}
