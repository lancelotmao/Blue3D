package com.android.vending.billing;

import java.util.ArrayList;

import org.json.JSONException;
import org.json.JSONObject;

import com.dwtech.android.blue3d.Blue3DActivity;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.PendingIntent;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

public class IABService {
	public static final String TAG = IABService.class.getSimpleName();
	
    private Object mService;
    private Activity mActivity;
    
    private String mProPrice;

    public IABService(Activity context) {
        mActivity = context;
    }

    private ServiceConnection mServiceConn = new ServiceConnection() {
        @Override
        public void onServiceDisconnected(ComponentName name) {
            mService = null;
        }

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
//            mService = IInAppBillingService.Stub.asInterface(service);
            synchronized (this) {
            	notify();
            }
        }
    };

    public void bind() {
        mActivity.bindService(new Intent(
                "com.android.vending.billing.InAppBillingService.BIND"),
                mServiceConn, Context.BIND_AUTO_CREATE);
    }

    public void unbind() {
        if (mService != null) {
            mActivity.unbindService(mServiceConn);
        }
    }

    public void listProduct() {
    	mProPrice = null;
        ListProductTask task = new ListProductTask();
        task.execute();
    }
    
    public void purchasePro() {
        PurchaseProductTask task = new PurchaseProductTask();
        task.execute();
    }

    private class ListProductTask extends AsyncTask<Void, Void, Void> {
        @Override
        protected void onPreExecute() {
            super.onPreExecute();
        }

        @Override
        protected Void doInBackground(Void... params) {
        	while (mService == null) {
        		try {
        			Thread.yield();
				} catch (Exception e) {
					e.printStackTrace();
				}
        	}
        	
        	Log.d(TAG, "start listing products");
            ArrayList<String> skuList = new ArrayList<String>();
            skuList.add("pro");
            Bundle querySkus = new Bundle();
            querySkus.putStringArrayList("ITEM_ID_LIST", skuList);

//            try {
//                Bundle skuDetails = mService.getSkuDetails(3, mActivity.getPackageName(), "inapp", querySkus);
//
//                int response = skuDetails.getInt("RESPONSE_CODE");
//                if (response == 0) {
//                   ArrayList<String> responseList = skuDetails.getStringArrayList("DETAILS_LIST");
//                   Log.d(TAG, "start listing products size = " + responseList.size());
//
//                   for (String thisResponse : responseList) {
//                      JSONObject object = new JSONObject(thisResponse);
//                      String sku = object.getString("productId");
//                      String price = object.getString("price");
//                      Log.d(TAG, "sku = " + sku + ", price = " + price);
//                      if (sku.equals("pro")) {
//                          mProPrice = price;
//                      }
//                   }
//                }
//            } catch (RemoteException e) {
//                e.printStackTrace();
//            } catch (JSONException e) {
//                e.printStackTrace();
//            }
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            super.onPostExecute(result);
            if (mProPrice != null) {
            	popProductDialog();
            }
        }
    }
    
    private class PurchaseProductTask extends AsyncTask<Void, Void, Boolean> {
        @Override
        protected void onPreExecute() {
            super.onPreExecute();
        }

        @Override
        protected Boolean doInBackground(Void... params) {
//            try {
//            	Log.d(TAG, "start purchasing products");
//                Bundle buyIntentBundle = mService.getBuyIntent(3, mActivity.getPackageName(),
//                        "pro", "inapp", "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAwA3367PndB8BTIAHlCrCi2h11Hhhg019iZx+R+RSPq9ka6Z6E31Bu1NE9WQhfrhf8UB5KB7OCudZQ/VnWxu0XA7BCQ+A164hHLfU2eyQwuNghyI6VZs0/R2fHTgn1tqieEoH0Sr60dLm++8QUDPR957aT+mwGnwEW7EcjivpU8Je/N61bWK/mCGylM8MfnPPO4BK231J3ZRMC7SLD81OyzaUQ4T+BF0yqGKhYCepA/XOM+9pjUzF/QpddUM/btBMim4RLgTIJDdnlth66638caXaYUVI+moBh3R96kvbzHKT5gWffyl9hH1sLk4LLl0ihcI4doo39dByqVpSgnB49QIDAQAB");
//                PendingIntent pendingIntent = buyIntentBundle.getParcelable("BUY_INTENT");
//                if (pendingIntent == null) {
//                	return false;
//                }
//                mActivity.startIntentSenderForResult(pendingIntent.getIntentSender(),
//                        1001, new Intent(), Integer.valueOf(0), Integer.valueOf(0),
//                        Integer.valueOf(0));
//                Log.d(TAG, "purchase intent sent");
//            } catch (Exception e) {
//                e.printStackTrace();
//            }
            return true;
        }

        @Override
        protected void onPostExecute(Boolean result) {
            super.onPostExecute(result);
            if (!result) {
            	((Blue3DActivity)mActivity).purchased();
            }
        }
    }
    
    public void popProductDialog() {
		new AlertDialog.Builder(mActivity)
				.setTitle("Upgrade to Pro: " + mProPrice)
				.setMessage("Pro version will remove ads")
				.setPositiveButton(android.R.string.yes,
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog,
									int which) {
								purchasePro();
							}
						})
				.setNegativeButton(android.R.string.no,
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog,
									int which) {
								// do nothing
							}
						}).show();
    }
}
