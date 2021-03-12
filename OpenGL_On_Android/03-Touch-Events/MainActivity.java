package com.astromedicomp.win_landscape;

import android.view.Window;													
import android.view.WindowManager;			
import android.app.Activity;				
import android.os.Bundle;					
import android.content.pm.ActivityInfo;		
import android.graphics.Color;				

public class MainActivity extends Activity {
	private MyView myview;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
       super.onCreate(savedInstanceState);
			this.requestWindowFeature(Window.FEATURE_NO_TITLE);
			this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,		
									WindowManager.LayoutParams.FLAG_FULLSCREEN);				
			MainActivity.this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);			
	
			myview=new MyView(this);
			this.getWindow().getDecorView().setBackgroundColor(Color.rgb(0,0,0));
			setContentView(myview);	
    }
}
