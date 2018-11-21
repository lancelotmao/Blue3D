package com.dwtech.android.houyi.utils;

import java.io.Closeable;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.Reader;
import java.io.StringWriter;
import java.io.Writer;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.net.Uri;
import android.os.Environment;

public class IOUtils {

    private IOUtils() {}

    public static String getAssetFileContent(Context context, String fileName) {
        Reader in = null;
        StringWriter content = new StringWriter();
        try {
            in = new InputStreamReader(context.getAssets().open(fileName,
                    AssetManager.ACCESS_STREAMING));
            dump(in, content);
        } catch (Exception e) {
            return null;
        } finally {
            closeSilently(in);
        }
        
        return content.toString();
    }
    
    public static String getUriFileContent(Context context, Uri uri) {
        Reader in = null;
        StringWriter content = new StringWriter();
        try {
            in = new InputStreamReader(context.getContentResolver().openInputStream(uri));
            dump(in, content);
        } catch (Exception e) {     
            return null;
        } finally {
            closeSilently(in);
        }
        
        return content.toString();
    }

    public static void dump(InputStream is, OutputStream os) throws IOException {
        byte buffer[] = new byte[4096];
        int rc = is.read(buffer, 0, buffer.length);
        while (rc > 0) {
            os.write(buffer, 0, rc);
            rc = is.read(buffer, 0, buffer.length);
        }
    }

    public static void dump(Reader is, Writer os) throws IOException {
        char buffer[] = new char[4096];
        int rc = is.read(buffer, 0, buffer.length);
        while (rc > 0) {
            os.write(buffer, 0, rc);
            rc = is.read(buffer, 0, buffer.length);
        }
    }

    public static void closeSilently(Closeable c) {
        if (c != null) {
            try{c.close();}catch(Exception e){}
        }
    }
    
    public static void loadLibrary(String libName) {
    	System.loadLibrary(libName);
    }
    
    public static void loadLibrary(Context context, String libName) {
        String packageName = context.getPackageName();
        String libPath = "/data/data/" + packageName + "/lib/" + libName;
        if (new File(libPath).exists()) {
            System.load(libPath);
        } else {
            System.loadLibrary(libName);
        }
    }
    
    public static void dumpBitmap(Bitmap bitmap, String name) {
        FileOutputStream os = null;
        try {
            CompressFormat mode = CompressFormat.PNG;
            int idx = name.indexOf("."); 
            if (idx == -1) {
                name += ".png";
            } else if (name.endsWith(".jpg") || name.endsWith(".jpeg")) {
                mode = CompressFormat.JPEG;
            }
            os = new FileOutputStream(Environment.getExternalStorageDirectory().getPath() + "/" + name);
            bitmap.compress(mode, 100, os);
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            IOUtils.closeSilently(os);
        }
    }
}
