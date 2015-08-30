package com.dwtech.android.houyimodelloader;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.ActionBar;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ImageView;

import com.dwtech.android.blue3d.R;
import com.google.analytics.tracking.android.EasyTracker;

public class SingleImageActivity extends Activity {
    
    private ImageView mIVMain;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.setContentView(R.layout.single_image);
        mIVMain = (ImageView)this.findViewById(R.id.iv_single);
        
        final ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
        actionBar.setTitle("");
        
        Bitmap bitmap = ModelLoaderActivity.mScreenshot;
        if (bitmap != null) {
            mIVMain.setImageBitmap(bitmap);
        }
    }
    
    @Override
	public void onStart() {
		super.onStart();
		EasyTracker.getInstance(this).activityStart(this);
	}

	@Override
	public void onStop() {
		super.onStop();
		EasyTracker.getInstance(this).activityStop(this);
	}
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case android.R.id.home:
            onBackPressed();
            break;
            
        case R.id.action_share:
            startShare(ModelLoaderActivity.mScreenshot);
            break;
        }

        return super.onOptionsItemSelected(item);
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.action_single, menu);
        return super.onCreateOptionsMenu(menu);
    }

    private void startShare(final Bitmap bitmap) {
        this.runOnUiThread(new Runnable(){
            public void run() {
                Uri uri = saveImageToSDCard(bitmap);

                Intent intent = new Intent();
                intent.setAction(Intent.ACTION_SEND);
                intent.setType("*/*");
                intent.putExtra(Intent.EXTRA_STREAM, uri);
                startActivity(intent);
            }
        });
    }
    
    public Uri saveImageToSDCard(Bitmap bitmap) {
        File path = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES);
        File imageFile = new File(path, "sc.jpg");
        FileOutputStream fileOutPutStream;
        try {
            fileOutPutStream = new FileOutputStream(imageFile);
            bitmap.compress(Bitmap.CompressFormat.JPEG, 100, fileOutPutStream);

            fileOutPutStream.flush();
            fileOutPutStream.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        
        return Uri.parse("file://" + imageFile.getAbsolutePath());
    }
}
