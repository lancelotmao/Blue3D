package com.dwtech.android.filemanager;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.ArrayList;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.dwtech.android.blue3d.R;
import com.dwtech.android.data.DataManager;
import com.dwtech.android.houyi.HEngine;
import com.dwtech.android.houyi.utils.IOUtils;
import com.dwtech.data.Item;

public class GridAdapter extends BaseAdapter {

    public static final int MODE_SAMPLE = 0;
    public static final int MODE_LOCAL = 0;
    public static final int MODE_DROPBOX = 0;
    
    public static final String[] MODEL_NAME = { 
        "Crate_dae.dae",
        "female.houyi",
        "astroboy.dae", 
        "samba.houyi"};
    
    private String TUTORIAL[] = {
            "Crate",
            "Female",
            "Astro Boy",
            "Samba",
            };
    
    private int THUMBNAIL[] = {
            R.drawable.crate,
            R.drawable.female,
            R.drawable.astroboy,
            R.drawable.samba,
    };
    
	private Context mContext;
	private LayoutInflater mInflater;
	private Drawable mFolder;
    private Drawable mDefaultThumb;
	
	private ArrayList<Item> mFolders = new ArrayList<Item>();
	private ArrayList<Item> mItems = new ArrayList<Item>();
	private Object mLock = new Object();
	
	public GridAdapter(Context context) {
		mContext = context;
		mInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		mFolder = context.getResources().getDrawable(R.drawable.folder);
        mDefaultThumb = context.getResources().getDrawable(R.drawable.default_thumb);
        DataManager.mItems = mItems;
		setSampleItems();
	}
	
	@Override
	public int getCount() {
		return mFolders.size() + mItems.size();
	}

	@Override
	public Object getItem(int index) {
	    if (index < mFolders.size()) {
	        return mFolders.get(index);
	    }
		return mItems.get(index - mFolders.size());
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
	    ViewHolderItem holder;
	    if (convertView == null) {
	        convertView = mInflater.inflate(R.layout.grid_item, null);
	        holder = new ViewHolderItem();
	        holder.mIVThumb = (ImageView)convertView.findViewById(R.id.iv_thumb);
	        holder.mIVFrame = (ImageView)convertView.findViewById(R.id.iv_frame);
	        holder.mIVPlay = (ImageView)convertView.findViewById(R.id.iv_play);
	        holder.mTVName = (TextView)convertView.findViewById(R.id.tv_name);
	        convertView.setTag(holder);
	    } else {
	        holder = (ViewHolderItem) convertView.getTag();
	    }
	    
	    Item item = (Item)getItem(position);
	    Bitmap bmp = null;
	    if (item.isSample()){
	        holder.mIVThumb.setImageDrawable(item.getThumb());
	    } else if (item.isDir()){
	        holder.mIVThumb.setImageDrawable(mFolder);
	    } else {
	        String thumbPath = item.getThumbnailPath();
	        FileInputStream in = null;
	        try {
	        	in = mContext.openFileInput(thumbPath);
				bmp = BitmapFactory.decodeStream(in);
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			} finally {
				IOUtils.closeSilently(in);
			}
	        if (bmp == null) {
	            holder.mIVThumb.setImageDrawable(mDefaultThumb);
	        } else {
	            holder.mIVThumb.setImageBitmap(bmp);
	        }
	    }
	    
	    holder.mTVName.setText(item.getDisplayName());
	    
	    if (item.isSample() || bmp != null) {
	        holder.mIVFrame.setVisibility(View.VISIBLE);
	    } else {
	        holder.mIVFrame.setVisibility(View.INVISIBLE);
	    }
	    
	    holder.mIVPlay.setVisibility(item.hasAnim() ? View.VISIBLE : View.INVISIBLE);
	    
		return convertView;
	}
	
	static class ViewHolderItem {
	    ImageView mIVThumb;
	    ImageView mIVFrame;
	    ImageView mIVPlay;
	    TextView mTVName;
	}
	
	public void clear() {
	    synchronized (mLock) {
    	    mFolders.clear();
    	    mItems.clear();
	    }
	}
	
	public void setSampleItems() {
	    clear();
	    
	    String samples[];
	    try {
	    	samples = mContext.getAssets().list("samples");
	    	for (String sample : samples) {
	    		String files[] = mContext.getAssets().list("samples/" + sample);
	    		for (String file : files) {
	    			if (file.endsWith("dae") || file.endsWith("obj") || file.endsWith("3ds") || file.endsWith("stl")
	    					 || file.endsWith("houyi")) {
	    				Item item = new Item();
	    		        item.setIsSample(true);
	    		        item.setDisplayName(sample);
	    		        item.setLocalPath("samples/" + sample + "/" + file);
	    		        Bitmap bmp = HEngine.createBitmap(mContext, "samples/" + sample + "/thumbnail.png");
	    		        item.setThumb(new BitmapDrawable(mContext.getResources(), bmp));
	    		        mItems.add(item);
	    			}
	    		}
	    	}
	    } catch (Exception e) {
	    	
	    }
	}
	
	public void addItem(Item item) {
	    synchronized (mLock) {
    	    ArrayList<Item> target = null;
    	    if (item.isDir()) {
    	        target = mFolders;
    	    } else {
    	        target = mItems;
    	    }
    	    
    	    int i = 0;
            for (;i < target.size();++i) {
                if (item.getDisplayName().compareToIgnoreCase(target.get(i).getDisplayName()) < 0
                        || item.getDisplayName().equals("Blue3D")) {
                    target.add(i, item);
                    break;
                }
            }
            if (i == target.size()) {
                target.add(item);
            }
	    }
	}
	
	public int getFolderCount() {
	    return mFolders.size();
	}
}
