package com.example.puzzle;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;

public class MainActivity extends Activity {

	LinearLayout linear;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		linear = (LinearLayout)findViewById(R.id.main);
		
		Button btn=(Button)findViewById(R.id.start);
		OnClickListener listener=new OnClickListener(){
			public void onClick(View v){
				Intent intent=new Intent(MainActivity.this, MainActivity2.class);
				startActivity(intent);
				finish();
			}
		};
		btn.setOnClickListener(listener);
	}

}
