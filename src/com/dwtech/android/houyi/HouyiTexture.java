package com.dwtech.android.houyi;

import java.io.IOException;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

public class HouyiTexture extends HouyiObject {
    public static final int REPEAT = 0;
    public static final int CLAMP = 1;
    
    public HouyiTexture(long id) {
        super(id);
    }

    public static HouyiTexture createAssetTexture(Context context, String assetName){
        Bitmap bitmap = null;
        try {
            bitmap = BitmapFactory.decodeStream(context.getAssets().open(assetName));
            return createTextureFromBitmap(bitmap);
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        } finally {
            if (bitmap != null) bitmap.recycle();
        }
    }    
    
    public static HouyiTexture createTextureFromBitmap(Bitmap bmp) {
        return new HouyiTexture(nativeCreateTextureFromBitmap(bmp, true));
    }
    
    public native static long createTextureFromAsset(String name, boolean upload);
	public native static long nativeCreateTextureFromBitmap(Bitmap bmp, boolean upload);
	public native static long createTextureFromFileDescriptor(int fileDescriptor, boolean upload);
	public native static long createTextureFromMemory(byte[] textureData, int length, boolean upload);
	public native static long createTextureFromTexture(long texture);
	public native static long createTextureFromId(int Id);
	
	// returns GLESCubeMapTexture instance, not texture id
	public native static long createCubeMapFromMemory(byte[] textureData0, int length0, 
	        byte[] textureData1, int length1, 
	        byte[] textureData2, int length2, 
	        byte[] textureData3, int length3, 
	        byte[] textureData4, int length4, 
	        byte[] textureData5, int length5);
	
	public static long createCubeMapFromAsset(String[] fileNames) {
	    return createCubeMapFromAsset(fileNames[0], fileNames[1], fileNames[2], 
	            fileNames[3], fileNames[4], fileNames[5]);
	}
	public native static long createCubeMapFromAsset(String fn0, String fn1, String fn2,
	        String fn3, String fn4, String fn5);
	
	public native void setWrapMode(int s, int t);
}
