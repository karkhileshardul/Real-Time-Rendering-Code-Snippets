package com.astromedicomp.win_hello2;

import android.widget.TextView;				//for "TextView" class
import android.graphics.Color;				//for "Color" class
import android.view.Gravity;				//for "Gravity" class
import android.content.Context;				//for drawing context related

public class MyView extends TextView{
		MyView(Context context){
			super(context);
			setText("HELLO UNIVERSE!!!");
			setTextSize(60);
			setTextColor(Color.YELLOW);
			setGravity(Gravity.CENTER);	
		}
}