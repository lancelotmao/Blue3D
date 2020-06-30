package com.dwtech.data;

import java.io.FileInputStream;
import java.io.FileNotFoundException;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Environment;

//import com.dropbox.client2.DropboxAPI.Entry;
import com.dwtech.android.houyi.utils.IOUtils;

public class Item {
    protected String mLocalPath;
    protected String mDisplayName;
    protected Drawable mThumb;
    protected boolean mIsDir;
    protected boolean mIsSample;
    protected boolean mIsMap;
    protected boolean mHasAnim;
//    protected Entry mDBEntry;
    
    public void setLocalPath(String path) {
        mLocalPath = path;
    }
    
    public String getLocalPath() {
        return mLocalPath;
    }
    
    public String getDBPath() {
//        if (mDBEntry != null) {
//            return mDBEntry.path;
//        }
        return null;
    }
    
    public String getDisplayName() {
        return mDisplayName;
    }
    
    public void setDisplayName(String name) {
        mDisplayName = name;
    }
    
    public void setIsDir(boolean isDir) {
        mIsDir = isDir;
    }
    
    public boolean isDir() {
        return mIsDir;
    }
    
    public void setThumb(Drawable drawable) {
        mThumb = drawable;
    }
    
    public Drawable getThumb() {
        return mThumb;
    }
    
//    public void setDBEntry(Entry ent) {
//        mDBEntry = ent;
//    }
//
//    public Entry getDBEntry() {
//        return mDBEntry;
//    }
    
    public Bitmap getFilmstripThumbnail(Context contex) {
        if (mIsSample){
            return drawableToBitmap(mThumb);
        } else {
        	String thumbPath = getThumbnailPath();
        	Bitmap bmp = null;
        	FileInputStream in = null;
	        try {
	        	in = contex.openFileInput(thumbPath);
				bmp = BitmapFactory.decodeStream(in);
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			} finally {
				IOUtils.closeSilently(in);
			}
	        return bmp;
        }
    }
    
    public static Bitmap drawableToBitmap(Drawable drawable) {
        if (drawable instanceof BitmapDrawable) {
            return ((BitmapDrawable) drawable).getBitmap();
        }

        Bitmap bitmap = Bitmap.createBitmap(drawable.getIntrinsicWidth(),
                drawable.getIntrinsicHeight(), Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);
        drawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
        drawable.draw(canvas);

        return bitmap;
    }
    
    public boolean isSample() {
        return mIsSample;
    }
    
    public void setIsSample(boolean isSample) {
        mIsSample = isSample;
    }

    public boolean isMap() {
        return mIsMap;
    }

    public void setIsMap(boolean isMap) {
    	mIsMap = isMap;
    }

    public boolean hasAnim() {
        return mHasAnim;
    }
    
    public void setHasAnim(boolean hasAnim) {
        mHasAnim = hasAnim;
    }
    
    public String getThumbnailPath() {
    	String res = mLocalPath.replace("/", "%_%");
    	return res + ".png";
    }
    
    public static String getHouyiPath(String filePath) {
        return Environment.getExternalStorageDirectory().toString() + "/Blue3D/"
                + filePath.replace("/", "%_%") + ".houyi";
    }
}
