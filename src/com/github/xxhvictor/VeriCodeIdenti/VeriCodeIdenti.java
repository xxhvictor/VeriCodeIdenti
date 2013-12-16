package com.github.xxhvictor.VeriCodeIdenti;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.FileInputStream;
import java.io.Reader;

import android.graphics.Bitmap;
import android.os.Environment;
import android.util.Log;

import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

class VeriCodeIdenti {
	
	//load lib
	static {
		OpenCVLoader.initDebug();
		System.loadLibrary("VeriCodeIdenti");
	}
	
	public final static String MAIN_DIR = "VeriCodeIdenti";
	public final static String RAW_DIR = "raw";
	public final static String SAMPLE_DIR ="sample";
	public final static String TRAIN_DIR ="train";
	public final static String TEST_DIR ="test";
	
	
	public String identify(Bitmap bitmap){
		if( bitmap == null )
			return new String("bitmap null");
		
    	Mat mat = new Mat();
    	Utils.bitmapToMat(bitmap, mat);
    	int channel = mat.channels();
    	int depth = mat.depth();
    	
    	// make it gray
    	Mat grey = new Mat(mat.rows(), mat.cols(), CvType.CV_8U);
    	Imgproc.cvtColor(mat, grey, Imgproc.COLOR_RGB2GRAY);
    	
    	// make dir and save image
    	File mainPath = Environment.getExternalStorageDirectory();
    	File subPath = new File(mainPath.getPath() + "/VeriCodeIdenti");
    	if( !subPath.exists() ) {
    		subPath.mkdirs();
    		Log.v("xiexh3", "have make dirs");
    	}
    	File file = new File(subPath.getPath(), "t.png");
    	//Highgui.imwrite(file.toString(), grey);
    	
    	return identifyImp(grey.nativeObj);
	}
	
	public void preporcess(){
		// create dir
		File mainPath = Environment.getExternalStorageDirectory();
		String rawPathString = mainPath.getPath() + "/" + MAIN_DIR + "/" + RAW_DIR;
		File rawPath = new File(rawPathString);
		if( !rawPath.exists()){
			rawPath.mkdirs();
		}
		
		// read config file
		String configFilePathString = rawPathString +"/" + "config.ini";
		StringBuffer buffer = new StringBuffer();
		try {
			FileInputStream fis = new FileInputStream(configFilePathString);
			InputStreamReader isr = new InputStreamReader(fis, "UTF-8");//文件编码Unicode,UTF-8,ASCII,GB2312,Big5
			BufferedReader reader = new BufferedReader(isr);
			String line = null;
			while( (line=reader.readLine()) != null ){
				buffer.append(line);
				buffer.append(";");
			}
			reader.close();
			isr.close();
			fis.close();
		} catch (IOException e){
			e.printStackTrace();
			return;
		}
		
		//one by one
		String rawSamples = buffer.toString();
		int prevIndex = 0;
		int nextIndex = 0;
		do {
			nextIndex = rawSamples.indexOf(";", prevIndex);
			String fileName = null;
			if( nextIndex > 0){
				fileName = rawSamples.substring(prevIndex, nextIndex);
				prevIndex = nextIndex+1;
			} else {
				fileName = rawSamples.substring(prevIndex, rawSamples.length());
			}

			if( fileName !=null && fileName.length() > 0 ){
				// read and convert to grey mat
				String fullFileNameString = rawPathString + "/" + fileName +".png";
				Log.v("xiexh3", "read fullFileNameString:"+fullFileNameString);
				Mat mat = Highgui.imread(fullFileNameString, Highgui.CV_LOAD_IMAGE_GRAYSCALE);
		    	int channel = mat.channels();
		    	int depth = mat.depth();
				String savePath = mainPath +"/" + MAIN_DIR + "/" + SAMPLE_DIR;
				String namePrefix = fileName;
				preprocessImp(mat.nativeObj, savePath, namePrefix);
			}
		} while (nextIndex > 0 || nextIndex >= rawSamples.length()-1 );
	}
	
	public void training() {
		// create sample dir
		File mainPath = Environment.getExternalStorageDirectory();
		String samplePathString = mainPath.getPath() + "/" + MAIN_DIR + "/" + SAMPLE_DIR;
		File samplePath = new File(samplePathString);
		if( !samplePath.exists()){
			samplePath.mkdirs();
		}
		
		//read charlist file
		File charlistFile = new File(samplePathString + "charlist.ini");
		if( !charlistFile.exists()){
			
		}
		
		// read charlist file
		String configFilePathString = samplePathString +"/" + "charlist.ini";
		String charlist = null;
		try {
			FileInputStream fis = new FileInputStream(configFilePathString);
			InputStreamReader isr = new InputStreamReader(fis, "UTF-8");//文件编码Unicode,UTF-8,ASCII,GB2312,Big5
			BufferedReader reader = new BufferedReader(isr);
			charlist = reader.readLine();
			reader.close();
			isr.close();
			isr.close();
			fis.close();
		} catch (IOException e){
			Log.v("xiexh3", "charlist.ini not exist");
			e.printStackTrace();
			return;
		}
		if( charlist == null || charlist.length() == 0){
			Log.v("xiexh3", "charlist empty");
			return;
		}
		
		// read char's config file
		for( int i=0; i < charlist.length(); ++i){
			String charString = charlist.substring(i, i+1);
			// read config file
			String charConfigFilePathString = samplePathString +"/" + charString +".ini";
			try {
				FileInputStream fis = new FileInputStream(charConfigFilePathString);
				InputStreamReader isr = new InputStreamReader(fis, "UTF-8");//文件编码Unicode,UTF-8,ASCII,GB2312,Big5
				BufferedReader reader = new BufferedReader(isr);
				String filename = null;
				while( (filename=reader.readLine()) != null ){
					String imageString = samplePathString + "/" + filename + ".png";
					addSample(imageString, charString, false);
				}
				reader.close();
				isr.close();
				fis.close();
			} catch (IOException e){
				Log.v("xiexh3", "read char config file error");
				e.printStackTrace();
				return;
			}
		}
		
		//do actual training
		trainingImp();
	}
	
	private native String identifyImp(long mat);
	private native void preprocessImp(long mat, String savePath, String namePrefix);
	private native void addSample(String fullPath, String label, boolean clearPrev);
	private native void trainingImp();
}