package com.dwtech.android.houyi.utils;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;

public class BitmapUtils {
    public static Bitmap cropCenter(Bitmap bitmap, int targetWidth, int targetHeight) {
        int srcX = 0;
        int srcY = 0;
        int width = targetWidth;
        int height = targetHeight;
        int w = bitmap.getWidth();
        int h = bitmap.getHeight();
        if ((float)width / w > (float)height / h)
        {
            srcY = (h - height / width * w) / 2;
        }
        else
        {
            srcX = (w - width / height * h) / 2;
        }

        Bitmap target = Bitmap.createBitmap(targetWidth, targetHeight, getConfig(bitmap));
        Canvas canvas = new Canvas(target);
        Paint paint = new Paint(Paint.FILTER_BITMAP_FLAG | Paint.DITHER_FLAG);
        canvas.drawBitmap(bitmap, new Rect(srcX, srcY, w-srcX, h-srcY), new Rect(0, 0, width, height), paint);
        return target;
    }
    
    private static Bitmap.Config getConfig(Bitmap bitmap) {
        Bitmap.Config config = bitmap.getConfig();
        if (config == null) {
            config = Bitmap.Config.ARGB_8888;
        }
        return config;
    }
}
