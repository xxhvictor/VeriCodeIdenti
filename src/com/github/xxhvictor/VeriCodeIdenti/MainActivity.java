package com.github.xxhvictor.VeriCodeIdenti;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;

import com.github.xxhvictor.VeriCodeIdenti.R;
import com.github.xxhvictor.VeriCodeIdenti.VeriCodeIdenti;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Environment;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.BatteryStats.Timer;
import android.util.Log;
import android.view.View;

public class MainActivity extends Activity implements View.OnClickListener
{
	IIdentiService mIIdentiService = null;
	
	private void createDirs(){
		//main dir
		File sddir = Environment.getExternalStorageDirectory();
		String maindirString = sddir.getPath() + "/" + VeriCodeIdenti.MAIN_DIR;
		File maindir = new File(maindirString);
		if( !maindir.exists() )
			maindir.mkdirs();
		
		// sample dir
		String sampledirString = sddir.getPath() + "/" + VeriCodeIdenti.MAIN_DIR + "/" + VeriCodeIdenti.SAMPLE_DIR;
		File sampledir = new File(sampledirString);
		if( !sampledir.exists()){
			sampledir.mkdirs();
		}
		// raw dir
		String rawdirString = sddir.getPath() + "/" + VeriCodeIdenti.MAIN_DIR + "/" + VeriCodeIdenti.RAW_DIR;
		File rawdir = new File(rawdirString);
		if( !rawdir.exists()){
			rawdir.mkdirs();
		}	
		// train dir
		String traindirString = sddir.getPath() + "/" + VeriCodeIdenti.MAIN_DIR + "/" + VeriCodeIdenti.TRAIN_DIR;
		File traindir = new File(traindirString);
		if( !traindir.exists()){
			traindir.mkdirs();
		}
		// test dir
		String testdirString = sddir.getPath() + "/" + VeriCodeIdenti.MAIN_DIR + "/" + VeriCodeIdenti.TEST_DIR;
		File testdir = new File(testdirString);
		if( !testdir.exists()){
			testdir.mkdirs();
		}
	}
	
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        // bind the service
    	if( mIIdentiService == null ){
            bindService( new Intent(MainActivity.this, IdentiService.class), mSerConnection, Context.BIND_AUTO_CREATE);
            startService(new Intent(MainActivity.this, IdentiService.class) );
    	}
    	
    	//createDirs
    	createDirs();
    	
    	// register click listener
    	findViewById(R.id.preprocess).setOnClickListener(this);
    	findViewById(R.id.training).setOnClickListener(this);
    	findViewById(R.id.test).setOnClickListener(this);
    }
    
    private ServiceConnection mSerConnection = new ServiceConnection() {
		
		@Override
		public void onServiceDisconnected(ComponentName name) {
			mIIdentiService = null;
		}
		
		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			mIIdentiService = IIdentiService.Stub.asInterface(service);
		}
	};

    private void doPreprocess() {
    	if( mIIdentiService != null){
    		try {
    			mIIdentiService.preprocess();
    		} catch (RemoteException e){
    			e.printStackTrace();
    		}
    		showMessage("done preprocess");
    	}  else {
    		showMessage("service not found");
    	}
    }
    
    private void doTraining() {
    	if( mIIdentiService != null){
    		try {
    			mIIdentiService.training();
    		} catch (RemoteException e){
    			e.printStackTrace();
    		}
    		showMessage("done training");
    	}  else {
    		showMessage("service not found");
    	}
    }
    
    private void doTest() {
    	if( mIIdentiService == null){
    		showMessage(" service not exist!!");
    		return;
    	}
    	// read config file
    	File sddir = Environment.getExternalStorageDirectory();
    	String testdirString = sddir.getPath() + "/" + VeriCodeIdenti.MAIN_DIR + "/" + VeriCodeIdenti.TEST_DIR;
    	File testconfig = new File(testdirString+ "/"+ "test.ini");
    	if( !testconfig.exists()){
    		showMessage("test.ini not exist!");
    		return;
    	}
    	
    	StringBuffer identibufBuffer = new StringBuffer();
    	int total = 0;
    	int right = 0;
    	long accTime = 0;
		try {
			FileInputStream fis = new FileInputStream(testconfig);
			InputStreamReader isr = new InputStreamReader(fis, "UTF-8");//文件编码Unicode,UTF-8,ASCII,GB2312,Big5
			BufferedReader reader = new BufferedReader(isr);
			String line = null;
			while( (line=reader.readLine()) != null ){
				int pos = line.indexOf(" ");
				if( pos < 0)
					continue;
				String name = line.substring(0, pos);
				String code = line.substring(pos+1, line.length()).trim();
				if( name == null || name.length() ==0
						|| code == null || code.length() !=4 )
					continue;
				String fullname = testdirString + "/"+ name;
				Bitmap bitmap = BitmapFactory.decodeFile(fullname);
				if( bitmap == null )
					continue;
				
				String identicode = new String();
	    		try {
	    			long start = System.currentTimeMillis();
	    			identicode = mIIdentiService.identify(bitmap);
	    			long end = System.currentTimeMillis();
	    			accTime += end-start;
	    			identibufBuffer.append(name + " : " + code 
	    					+" ---> "+ identicode
	    					+ " : " + ((code == identicode)?"OK":"NO") );
	    			identibufBuffer.append("\n");
	    			++total;
	    			if(code == identicode)
	    				++right;
	    		} catch (RemoteException e){
	    			showMessage("remote error");
	    			e.printStackTrace();
	    		}
			}
			reader.close();
			isr.close();
			fis.close();
		} catch (IOException e){
			showMessage("io error");
			e.printStackTrace();
			return;
		}
		
		identibufBuffer.append("\n");
		identibufBuffer.append("\n");
		identibufBuffer.append("total : " + total + "\n");
		identibufBuffer.append("right : " + right + "\n");
		float ratio = (float)right/( total>0?total:1);
		identibufBuffer.append("ratio : " + ratio + "\n");
		identibufBuffer.append("totaltime: "+accTime +", avtime: "+ (float)accTime/total);
		String resultfilename = new String();
		resultfilename = testdirString + "/" + "result.txt";
		try{
			FileOutputStream fos = new FileOutputStream(resultfilename);
			OutputStreamWriter oWriter = new OutputStreamWriter(fos);
			oWriter.write(identibufBuffer.toString());
			oWriter.close();
			fos.close();
		} catch (IOException e){
			showMessage("save result error");
			e.printStackTrace();
		}
		showMessage("测试总数: " + total + ", 识别正确:"+right +"\n"
				+"正确率: " + ratio*100 + "%\n" 
				+ "总耗时: "+accTime +"ms,  平均耗时: "+ (float)accTime/total +"ms");
		return;

//    	// identification
//    	String code = null;
//    	Bitmap bitmap = BitmapFactory.decodeStream(is);
//    	if( mIIdentiService != null){
//    		code = new String();
//    		try {
//    			code = mIIdentiService.identify(bitmap);
//    		} catch (RemoteException e){
//    			e.printStackTrace();
//    		}
//    	} else {
//    		code = new String("service not found");
//    	}
//    	
//    	//show this
//    	showMessage(code);
    }
    
    private void showMessage(String msg) {
    	//show this
    	AlertDialog.Builder builder = new AlertDialog.Builder(this);
    	builder.setMessage(msg);
    	builder.setTitle("message");
    	builder.setPositiveButton("confirm", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
			}
    	} );
    	builder.create().show();
    }
    
    @Override
    public void onClick(View v) {
    	switch (v.getId()) {
		case R.id.preprocess:
			doPreprocess();
			break;
		case R.id.training:
			doTraining();
			break;
		case R.id.test:
			doTest();
			break;
		default:
			break;
		}
    }
    
    @Override
    protected void onDestroy() {
    	// unbind the service
    	if( mIIdentiService != null)
    		unbindService(mSerConnection);
    }
    
}
