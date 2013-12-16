package com.github.xxhvictor.VeriCodeIdenti;
import android.graphics.Bitmap;

interface IIdentiService
{
	String identify(in Bitmap bitmap);
	void preprocess();
	void training();
}
