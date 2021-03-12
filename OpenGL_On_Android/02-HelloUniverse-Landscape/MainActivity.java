package com.astromedicomp.win_hello2;		//Convention so reverse order

import android.view.Window;					//for "Window" class									
import android.view.WindowManager;			//for "WindowManager" class
import android.app.Activity;				//for "Window" class									
import android.os.Bundle;					//for "WindowManager" class									
import android.content.pm.ActivityInfo;		//for "ActivityInfo" class	
import android.graphics.Color;				//for "Color" class

public class MainActivity extends Activity{
	private MyView myview;
	@Override
	protected void onCreate(Bundle savedInstanceState){				// like WM_CREATE:
			super.onCreate(savedInstanceState);
			this.requestWindowFeature(Window.FEATURE_NO_TITLE);
			this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,		
									WindowManager.LayoutParams.FLAG_FULLSCREEN);				
			MainActivity.this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);			
	
			myview=new MyView(this);
			this.getWindow().getDecorView().setBackgroundColor(Color.rgb(0,0,0));
			setContentView(myview);	
	}
	@Override
	protected void onPause(){
		super.onPause();
	}
	@Override
	protected void onResume(){
		super.onResume();
	}
}
