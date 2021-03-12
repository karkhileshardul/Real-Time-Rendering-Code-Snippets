package com.astromedicomp.win_hello1;		//Convention so reverse order

import android.app.Activity;				//for "Window" class									
import android.os.Bundle;					//for "WindowManager" class									
import android.view.Window;					//for "Window" class									
import android.view.WindowManager;			//for "WindowManager" class
import android.widget.TextView;				//for "TextView" class
import android.content.pm.ActivityInfo;		//for "ActivityInfo" class	
import android.view.Gravity;				//for "Gravity" class
import android.graphics.Color;				//for "Color" class

public class MainActivity extends Activity{
	@Override
	protected void onCreate(Bundle savedInstanceState){				// like WM_CREATE:
			super.onCreate(savedInstanceState);
			TextView myTextView=new TextView(this);

			this.requestWindowFeature(Window.FEATURE_NO_TITLE);
			this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,		
									WindowManager.LayoutParams.FLAG_FULLSCREEN);				
			MainActivity.this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);			

			myTextView.setText("HELLO WORLD!!!");
			myTextView.setTextSize(60);
			myTextView.setTextColor(Color.GREEN);
			myTextView.setGravity(Gravity.CENTER);	
		
			myTextView.setBackgroundColor(Color.BLACK);

			setContentView(myTextView);			
	}
}
