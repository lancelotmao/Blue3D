package com.dwtech.android.blue3d;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import org.json.JSONException;
import org.json.JSONObject;

import android.Manifest;
import android.app.ActionBar;
import android.app.ActionBar.OnNavigationListener;
import android.app.ActionBar.Tab;
import android.app.AlertDialog;
import android.app.FragmentTransaction;
import android.app.ProgressDialog;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.graphics.drawable.ColorDrawable;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.text.Html;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.FrameLayout;
import android.widget.GridView;
import android.widget.ProgressBar;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.FragmentActivity;

import com.android.vending.billing.IABService;
import com.dropbox.client2.DropboxAPI;
import com.dropbox.client2.DropboxAPI.Entry;
import com.dropbox.client2.ProgressListener;
import com.dropbox.client2.android.AndroidAuthSession;
import com.dropbox.client2.exception.DropboxException;
import com.dropbox.client2.session.AppKeyPair;
import com.dropbox.client2.session.Session.AccessType;
import com.dwtech.android.data.DataManager;
import com.dwtech.android.filemanager.GridAdapter;
import com.dwtech.android.houyi.HouyiAssetManager;
import com.dwtech.android.houyi.HouyiLoader;
import com.dwtech.android.houyi.utils.IOUtils;
import com.dwtech.android.houyimodelloader.ModelLoaderActivity;
import com.dwtech.android.houyimodelloader.PreferenceManager;
import com.dwtech.data.Item;
import com.google.android.material.bottomnavigation.BottomNavigationView;
//import com.google.analytics.tracking.android.EasyTracker;
//import com.google.android.gms.ads.AdRequest;
//import com.google.android.gms.ads.AdSize;
//import com.google.android.gms.ads.AdView;
//import com.google.android.gms.common.ConnectionResult;
//import com.google.android.gms.common.GooglePlayServicesUtil;

public class Blue3DActivity extends FragmentActivity implements ActionBar.TabListener,
OnNavigationListener {

    private static final int REQUEST_PERMISSION = 10000;

	static {
		IOUtils.loadLibrary("houyiiconv");
		IOUtils.loadLibrary("houyixml");
		IOUtils.loadLibrary("houyi");
		IOUtils.loadLibrary("blue3d");
	}
	
    private static final int LIST_FOLDER_LOCAL = 0;
    private static final int LIST_FOLDER_DB = 1;
    
    // action bar
    private static final int TAB_SAMPLE = 0;
    private static final int TAB_LOCAL = 1;
    private static final int TAB_DROPBOX = 2;
    private static final int NAV_MODE_TAB = 1;
    private static final int NAV_MODE_LIST = 2;
    private int mNavMode = NAV_MODE_LIST;
    private int mCurrentTab;
    
    // local
    private PreferenceManager mPreMan;
    private File mCD = Environment.getExternalStorageDirectory();
    
    // drop box
    final static private String APP_KEY = "dbjmsvqza5beaig";
    final static private String APP_SECRET = "529dztjxul8w91h";
    final static private AccessType ACCESS_TYPE = AccessType.AUTO;
    final static private String BLUE_DIR = "/Blue3D";
    final static private String DB_CACHE_DIR = "/Blue3D/DropboxCache";
    private DropboxAPI<AndroidAuthSession> mDBApi;
    private String mDBCD = "/";
    private ProgressDialog mLoadingDialog;
    private int mDownloadCount;
    private Item mDownloadingItem;
    
    // UI
    private FrameLayout mMainLayout;
//    private AdView mAdView;
    private ProgressBar mProgressBar;
    private GridView mGridView;
    private GridAdapter mAdapter;
    private boolean mListing;
    BottomNavigationView tabBar;
    
    // in app billing
    private IABService mIABService;
    
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// load preference first which might contains drop box token already
		mPreMan = PreferenceManager.getInstance(this);
		mPreMan.load();
		String lastCD = mPreMan.getLastCD();
		if (lastCD != null) {
		    mCD = new File(mPreMan.getLastCD());
		}
		
		setContentView(R.layout.main);
		mMainLayout = (FrameLayout)findViewById(R.id.fl_main);
		mProgressBar = (ProgressBar)findViewById(R.id.pb_listing);
		mGridView = (GridView) findViewById(R.id.gridView1);
		mAdapter = new GridAdapter(this);
		
		initAdmobView();

		ActionBar ab = getActionBar();
		if (ab != null) {
		    ab.setBackgroundDrawable(new ColorDrawable(0xFFEDEDED));

            getActionBar().setTitle(Html.fromHtml("<font color=\"black\">" + getString(R.string.app_name) + "</font>"));

        }

//		int cc = 3;
//		if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) {
//		    cc = getResources().getInteger(R.integer.column_count_port);
//		} else {
//		    cc = getResources().getInteger(R.integer.column_count_land);
//		}
//		mGridView.setNumColumns(cc);
		mGridView.setAdapter(mAdapter);
		mGridView.setOnItemClickListener(new OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
					long row) {

			    if (tabBar.getSelectedItemId() == R.id.menu_sample) {
			        startSample(row);
			    } else if (tabBar.getSelectedItemId() == R.id.menu_file) {
			        localClicked((int)row);
                }
//			    else if (getActionBar().getSelectedNavigationIndex() == TAB_DROPBOX) {
//                    if (!isProgressBarShowing()) {
//                        dropboxClicked((int)row);
//                    }
//                }
			}
		});

		tabBar = findViewById(R.id.bottom_navigation);
        tabBar.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                showContent(item);
                return true;
            }
        });

	    // Drop box
//	    AppKeyPair appKeys = new AppKeyPair(APP_KEY, APP_SECRET);
//	    AndroidAuthSession session = new AndroidAuthSession(appKeys, ACCESS_TYPE);
//	    mDBApi = new DropboxAPI<AndroidAuthSession>(session);

        if (PackageManager.PERMISSION_GRANTED != ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.CAMERA}, REQUEST_PERMISSION);
        } else {
            createFolder(Environment.getExternalStorageDirectory() + BLUE_DIR);
        }
	}
	
	@Override
	public void onDestroy() {
	    super.onDestroy();
	    if (mIABService != null) {
	        mIABService.unbind();
	    }
	}
	
	@Override
	public void onStart() {
		super.onStart();
//		EasyTracker.getInstance(this).activityStart(this);
	}

	@Override
	public void onStop() {
		super.onStop();
//		EasyTracker.getInstance(this).activityStop(this);
	}

	@Override
	protected void onResume() {
	    super.onResume();

	    initAdmobView();
	    
//	    if (mDBApi.getSession().authenticationSuccessful()) {
//	        try {
//	            // Required to complete auth, sets the access token on the session
//	            mDBApi.getSession().finishAuthentication();
//
//	            String accessToken = mDBApi.getSession().getOAuth2AccessToken();
//	            PreferenceManager.getInstance(this).saveDBAccessToken(accessToken);
//
//	            if (getActionBar().getSelectedNavigationIndex() == TAB_DROPBOX) {
//		            Log.i("Houyi", "DropBox logged in. accessToken = " + accessToken);
//		            if (getActionBar().getSelectedNavigationIndex() == TAB_DROPBOX) {
//		                startListFolder(LIST_FOLDER_DB);
//		            }
//	            }
//	        } catch (IllegalStateException e) {
//	            Log.i("Houyi", "Error authenticating drop box", e);
//	        }
//	    }
	    
	    mAdapter.notifyDataSetChanged();
	}
	
	@Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        initAdmobView();
        int cc = 3;
        if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) {
            cc = getResources().getInteger(R.integer.column_count_port);
        } else {
            cc = getResources().getInteger(R.integer.column_count_land);
        }
        mGridView.setNumColumns(cc);
    }

    @Override
    public void onBackPressed() {
        if (tabBar.getSelectedItemId() == R.id.menu_file) {
        	Log.d("Houyi", "mCD = " + mCD);
            if (!mCD.equals(Environment.getExternalStorageDirectory())) {
                mCD = mCD.getParentFile();
                startListFolder(LIST_FOLDER_LOCAL);
                return;
            }
        } else if (getActionBar().getSelectedNavigationIndex() == TAB_DROPBOX) {
        	Log.d("Houyi", "mDBCD = " + mDBCD);
            if (!mDBCD.equals("/")) {
                File f = new File(mDBCD);
                mDBCD = f.getParent();
                startListFolder(LIST_FOLDER_DB);
                return;
            }
        }
        super.onBackPressed();
    }

//	@Override
//	public boolean onCreateOptionsMenu(Menu menu) {
//	    // Inflate the menu items for use in the action bar
//	    MenuInflater inflater = getMenuInflater();
//	    inflater.inflate(R.menu.action_menu, menu);
//	    for (int i = 0;i < menu.size();++i) {
//	        MenuItem item = menu.getItem(i);
//	        if (item.getItemId() == R.id.action_upgrade && mPreMan.isPro()) {
//	            item.setVisible(false);
//	        }
//	        if (item.getItemId() == R.id.action_refresh
//	        && getActionBar().getSelectedNavigationIndex() == TAB_SAMPLE) {
//	        	item.setVisible(false);
//	        }
//	    }
//	    return super.onCreateOptionsMenu(menu);
//	}
	
	@Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case R.id.action_about:
            showAbout();
            break;
        case R.id.action_upgrade :
            mIABService = new IABService(this);
            mIABService.bind();
            mIABService.listProduct();
            break;
        case R.id.action_refresh:
        	startListFolder();
        	break;
        }
        return super.onOptionsItemSelected(item);
	}

    @Override
    public void onTabReselected(Tab arg0, FragmentTransaction arg1) {
    }

    @Override
    public void onTabSelected(Tab tab, FragmentTransaction ft) {
    }

    @Override
    public void onTabUnselected(Tab tab, FragmentTransaction ft) {
    }

    @Override
    public boolean onNavigationItemSelected(int itemPosition, long itemId) {
    	if (mCurrentTab == itemPosition) {
    		return false;
    	}
        if (itemPosition == TAB_SAMPLE) {
            mAdapter.setSampleItems();
            mAdapter.notifyDataSetChanged();
        } else if (itemPosition == TAB_LOCAL) {
            startListFolder(LIST_FOLDER_LOCAL);
        } else if (itemPosition == TAB_DROPBOX) {
            createFolder(Environment.getExternalStorageDirectory() + DB_CACHE_DIR);
            mAdapter.clear();
            mAdapter.notifyDataSetChanged();
            String accessToken = PreferenceManager.getInstance(this).getDBAccessToken();
            if (accessToken != null) {
                mDBApi.getSession().setOAuth2AccessToken(accessToken);
                startListFolder(LIST_FOLDER_DB);
            } else {
                mDBApi.getSession().startOAuth2Authentication(this);
            }
        }
        mCurrentTab = itemPosition;
        invalidateOptionsMenu();
        return true;
    }

    private void showContent(MenuItem item) {
        if (item.getItemId() == R.id.menu_sample) {
            mAdapter.setSampleItems();
            mAdapter.notifyDataSetChanged();
        } else if (item.getItemId() == R.id.menu_file) {
            if (PackageManager.PERMISSION_GRANTED == ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
                startListFolder(LIST_FOLDER_LOCAL);
            } else {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.CAMERA}, REQUEST_PERMISSION);
            }
        }
    }
    
    private void startListFolder() {
    	if (getActionBar().getSelectedNavigationIndex() == TAB_LOCAL) {
    		startListFolder(LIST_FOLDER_LOCAL);
    	} else if (getActionBar().getSelectedNavigationIndex() == TAB_DROPBOX) {
    		startListFolder(LIST_FOLDER_DB);
    	}
    }
    
    private void startListFolder(int type) {
    	if (mListing) {
    		// listing in progress
    		return;
    	}
    	
    	mListing = true;
        mAdapter.clear();
        if (type == 1) {
            showProgressBar();
        }
        ListFolderTask task = new ListFolderTask();
        task.execute(type);
    }
    
    private class ListFolderTask extends AsyncTask<Integer, Integer, Void>{  
        @Override  
        protected void onPreExecute() {  
            super.onPreExecute();  
        }  
          
        @Override  
        protected Void doInBackground(Integer... params) {
            if (params[0] == 0) {
                listLocalFolder();
            } else if (params[0] == 1) {
                listDBFolder();
            }
            return null;
        }  
  
        @Override  
        protected void onPostExecute(Void result) {  
            super.onPostExecute(result);
            mPreMan.setLastCD(mCD.toString());
            mAdapter.notifyDataSetChanged();
            mListing = false;
            hideProgressBar();
        }  
    }
    
    private class DownloadTask extends AsyncTask<String, String, Void>{
    	private String mDBPath;
    	private String mLocalPath;
    	
        @Override  
        protected void onPreExecute() {  
            super.onPreExecute();  
        }  
          
        @Override  
        protected Void doInBackground(String... params) {
        	mDBPath = params[0];
            mLocalPath = getDBCacheFolder() + mDBPath;
            File f = new File(mLocalPath);
            // download file to local then start local
        	try {
        		if (!f.exists()) {
        			createFolder(f.getParent());
        			f.createNewFile();
        		}
            	FileOutputStream fos = new FileOutputStream(f);
            	mDBApi.getFile(mDBPath, null, fos, new ProgressListener() {

                    @Override
                    public void onProgress(long bytes, long total) {
                    	updateDownloadProgress(bytes, total);
                    }
                });
        	} catch(Exception e) {
        		e.printStackTrace();
        	}
            return null;
        }  
  
        @Override  
        protected void onPostExecute(Void result) {
            super.onPostExecute(result);
            mDownloadCount--;
            
            if (mDownloadCount < 0) {
	            InputStream in = null;
				try {
				    in = new FileInputStream(new File(mLocalPath));
					int length = in.available();
					byte[] data = new byte[length];
					in.read(data);
					
					String[] texPaths = HouyiLoader.getTexturePaths(data, data.length, mDBPath);
					if (texPaths.length > 0) {
					    mLoadingDialog.setMessage("Downloading Texture");
					    mLoadingDialog.setProgress(0);
						mDownloadCount = 0;
						for (String p : texPaths) {
							Log.d("Houyi", p);
							DownloadTask task = new DownloadTask();
							task.execute(p);
							mDownloadCount++;
						}
					} else {
					    mLoadingDialog.dismiss();
						startLocal(mDownloadingItem.getLocalPath(), true);
					}
				} catch (IOException e) {
					e.printStackTrace();
				} finally {
					IOUtils.closeSilently(in);
				}
            } else if (mDownloadCount == 0) {
                mLoadingDialog.dismiss();
            	startLocal(mDownloadingItem.getLocalPath(), true);
            } else {
                mLoadingDialog.setProgress(0);
            }
        }  
    }
    
    private void localClicked(int index) {
        Item item = (Item)mAdapter.getItem(index);
        String path = item.getLocalPath();
        File file = new File(path);
        if (file.isDirectory()) {
            mCD = file;
            startListFolder(LIST_FOLDER_LOCAL);
        } else if (path.toLowerCase().endsWith(".hae")
                || path.toLowerCase().endsWith(".dae")
                || path.toLowerCase().endsWith(".tmx")
                || path.toLowerCase().endsWith(".obj")
                || path.toLowerCase().endsWith(".stl")
                || path.toLowerCase().endsWith(".3ds")) {
            startLocal(path, false);
            DataManager.mFocus = index - mAdapter.getFolderCount();
        }
    }
    
    private void dropboxClicked(int index) {
//        Item item = (Item)mAdapter.getItem(index);
//        Entry ent = item.getDBEntry();
//        if (ent == null) {
//            return;
//        }
//        if (ent.isDir) {
//            mDBCD = ent.path;
//            startListFolder(LIST_FOLDER_DB);
//        } else if (ent.fileName().endsWith(".dae")) {
//            mDownloadCount = 0;
//            mDownloadingItem = item;
//            String localPath = getDBCacheFolder() + ent.path;
//            mDownloadingItem.setLocalPath(localPath);
//            File f = new File(localPath);
//            // local cache with latest revision exists starts local
//            // TODO save revision and get
//            if (f.exists()) {
//                startLocal(f.toString(), true);
//            } else {
//            	mLoadingDialog = new ProgressDialog(this);
//				mLoadingDialog.setTitle("Loading");
//				String fileName = ent.fileName();
//				mLoadingDialog.setMessage("Downloading " + fileName);
//				mLoadingDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
//				mLoadingDialog.setProgress(0);
//				mLoadingDialog.show();
//                // download file to local then start local
//            	DownloadTask task = new DownloadTask();
//                task.execute(ent.path);
//            }
//            DataManager.mFocus = index - mAdapter.getFolderCount();
//        }
    }
    
    private void listLocalFolder() {
//        Log.d("Houyi", "Listing Local Folder: " + mCD);
        String[] files = mCD.list();
        if (files == null) {
        	Log.e("Houyi", "listLocalFolder returns NULL");
        	return;
        }
        for (String file : files) {
            Item item = new Item();
            String path = mCD + "/" + file;
            item.setLocalPath(path);
            item.setDisplayName(file);
            File f = new File(path);
            if (f.isDirectory()) {
                item.setIsDir(true);
            } else if (!file.toLowerCase().endsWith(".dae") && file.toLowerCase().endsWith(".hae") && !file.toLowerCase().endsWith(".tmx")) {
                continue;
            }
            mAdapter.addItem(item);
//            Log.d("Houyi", "Local file name = " + file);
        }
    }
    
    private void listDBFolder() {
//        Log.d("Houyi", "Listing Dropbox Folder: " + mDBCD);
        Entry dirent;
        try {
            dirent = mDBApi.metadata(mDBCD, 1000, null, true, null);
            for (Entry ent: dirent.contents) {
                Item item = new Item();
                String fn = ent.fileName();
                item.setDisplayName(fn);
                if (ent.isDir) {
                    item.setIsDir(true);
                } else if (!fn.toLowerCase().endsWith(".dae")) {
                    continue;
                }
                item.setLocalPath(getDBCacheFolder() + ent.path);
//                item.setDBEntry(ent);
                mAdapter.addItem(item);
//                Log.d("Houyi", "DropBox file name = " + ent.fileName());
            }
        } catch (DropboxException e) {
            e.printStackTrace();
        }
    }
    
    private void startSample(long row) {
        if (HouyiAssetManager.getInstance(this, DataManager.getInstance()).isLoading()) {
            Log.w("Houyi", "cannot start sample since world is loading");
            return;
        }
        Intent i = new Intent();
        i.putExtra("Index", (int)row);
        i.setClass(this, ModelLoaderActivity.class);
        startActivity(i);
        DataManager.mFocus = (int)row;
    }
    
    private void startLocal(String path, boolean isDB) {
        if (HouyiAssetManager.getInstance(this, DataManager.getInstance()).isLoading()) {
            Log.w("Houyi", "cannot start local since world is loading");
            return;
        }
        Intent i = new Intent();
        i.putExtra("isDB", isDB);
        i.setClass(this, ModelLoaderActivity.class);
        startActivity(i);
    }

    private void createFolder(String filePath) {
    	createFolder(new File(filePath));
    }
    
    private void createFolder(File file) {
        File targetFile = file;
        
        File f = targetFile;
        while (!f.exists()) {
            while (!f.getParentFile().exists()) {
                f = f.getParentFile();
            }
            boolean ret = f.mkdir();
            if (ret) {
                Log.d("Houyi", "folder created: " + f);
            } else {
                Log.w("Houyi", "folder cannot create: " + f);
            }
            f = targetFile;
        }
    }
    
    private String getDBCacheFolder() {
        return Environment.getExternalStorageDirectory() + DB_CACHE_DIR;
    }
    
    private void updateDownloadProgress(long bytes, long total) {
    	final int p = (int)(bytes * 100 / total);
    	mGridView.post(new Runnable(){
			@Override
			public void run() {
				mLoadingDialog.setProgress(p);
			}
    	});
    }
    
    private void showProgressBar() {
        mProgressBar.setVisibility(View.VISIBLE);
    }
    
    private void hideProgressBar() {
        mProgressBar.setVisibility(View.GONE);
    }
    
    private boolean isProgressBarShowing() {
        return mProgressBar.getVisibility() == View.VISIBLE;
    }
    
    private void initAdmobView() {
        if (mPreMan.isPro()) {
            return;
        }
        
//        int ret = GooglePlayServicesUtil.isGooglePlayServicesAvailable(getApplicationContext());
//        if (ret != ConnectionResult.SUCCESS) {
//            GooglePlayServicesUtil.getErrorDialog(ret, this, 1);
//        }
//
//        if (mAdView != null) {
//            mMainLayout.removeView(mAdView);
//        }
//        mAdView = new AdView(this);
//        mAdView.setAdSize(AdSize.SMART_BANNER);
//        mAdView.setAdUnitId("a1531d674343139");
//        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(
//                LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
//        params.gravity=Gravity.BOTTOM;
//        mAdView.setLayoutParams(params);
//        mMainLayout.addView(mAdView);
//        AdRequest adRequest = new AdRequest.Builder()
//        .addTestDevice(AdRequest.DEVICE_ID_EMULATOR)
//        .addTestDevice("INSERT_YOUR_HASHED_DEVICE_ID_HERE")
//        .build();
//
//        // Start loading the ad in the background.
//        mAdView.loadAd(adRequest);
//
//        int h = mAdView.getAdSize().getHeightInPixels(this);
//        FrameLayout.LayoutParams p = (FrameLayout.LayoutParams)mGridView.getLayoutParams();
//        p.bottomMargin = h;
//        mGridView.setLayoutParams(p);
    }
    
    private void showAbout() {
        new AlertDialog.Builder(this)
        .setTitle(R.string.about_title)
        .setMessage(R.string.about_content)
        .setPositiveButton(android.R.string.ok,null).show();
    }
    
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == 1001) {
			String purchaseData = data.getStringExtra("INAPP_PURCHASE_DATA");
			String dataSignature = data.getStringExtra("INAPP_DATA_SIGNATURE");

			if (resultCode == RESULT_OK) {
				try {
					JSONObject jo = new JSONObject(purchaseData);
					String sku = jo.getString("productId");
					Log.i(IABService.TAG, "You have bought the " + sku);
					Log.i(IABService.TAG, dataSignature);
					purchased();
				} catch (JSONException e) {
					Log.i(IABService.TAG, "Failed to parse purchase data.");
					e.printStackTrace();
				}
			}
		}
	}
	
	public void purchased() {
		mPreMan.savePro();
//		if (mAdView != null) {
//			mMainLayout.removeView(mAdView);
//		}
	}
}
