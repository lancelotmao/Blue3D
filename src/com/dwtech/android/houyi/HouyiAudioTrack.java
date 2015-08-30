package com.dwtech.android.houyi;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.MediaPlayer;

public class HouyiAudioTrack {
    private String mFileName;
	private MediaPlayer mSoundPlayer;
	
	public HouyiAudioTrack(String fileName) {
	    mFileName = fileName;
	}
	
	public void play(Context context) {
    	if (mSoundPlayer == null) {
    		mSoundPlayer = new MediaPlayer(); 
    	}
        try {
            if (mSoundPlayer.isPlaying()) {
            	mSoundPlayer.stop();
            	mSoundPlayer.release();
            	mSoundPlayer = new MediaPlayer();
            }

            AssetFileDescriptor descriptor = context.getAssets().openFd(mFileName);
            mSoundPlayer.setDataSource(descriptor.getFileDescriptor(), descriptor.getStartOffset(), descriptor.getLength());
            descriptor.close();

            mSoundPlayer.prepare();
            mSoundPlayer.setVolume(1f, 1f);
            mSoundPlayer.setLooping(true);
            mSoundPlayer.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
	
	public void stop() {
		if (mSoundPlayer != null) {
			mSoundPlayer.stop();
        	mSoundPlayer.release();
        	mSoundPlayer = null;
		}
	}
	
	public int getCurrentPosition() {
	    return mSoundPlayer == null ? 0 : mSoundPlayer.getCurrentPosition();
	}
	
	public float getDuration() {
		return mSoundPlayer == null ? 0 : mSoundPlayer.getDuration();
	}
}
