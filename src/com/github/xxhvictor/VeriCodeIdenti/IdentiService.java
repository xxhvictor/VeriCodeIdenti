package com.github.xxhvictor.VeriCodeIdenti;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;

import android.app.Service;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.IBinder;
import android.os.RemoteException;

import com.github.xxhvictor.VeriCodeIdenti.VeriCodeIdenti;

public class IdentiService extends Service
{
	VeriCodeIdenti mIdentifier = null;
	boolean mHasTransferedModel = false;
    private IIdentiService.Stub mStub = new IIdentiService.Stub() {
		
		@Override
		public String identify(Bitmap bitmap) throws RemoteException {
			return getIdentifier().identify(bitmap);
		}
		
		@Override
		public void preprocess() throws RemoteException {
			getIdentifier().preporcess();
		};
		
		@Override
		public void training() throws RemoteException {
			getIdentifier().training();
		};
	};
	
	VeriCodeIdenti getIdentifier(){
		if( mIdentifier == null)
			mIdentifier = new VeriCodeIdenti();
		return mIdentifier;
	}
	
	@Override
	public IBinder onBind(Intent intent) {
		return mStub;
	}
	
	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();
	}
}
