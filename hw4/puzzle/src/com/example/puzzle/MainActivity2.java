package com.example.puzzle;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Point;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.GridLayout;
import android.widget.LinearLayout;

public class MainActivity2 extends Activity{
	LinearLayout linear;
	EditText text;
	GridLayout grid;
	int puzzle[][];
	int fd;
	Button buttons[][];
	OnClickListener btn_change;
	ReadThread mThread;
	
	public native int openDriver();
	public native void writeDriver(int fd, int val);
	public native int readDriver(int fd);
	public native void closeDriver(int fd);
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main2);
		
		Log.d(null, "Activity 2 loaded");
		
		System.loadLibrary("driver-func");
		Log.d(null, "library loaded");
		
		fd = openDriver();
		Log.d(null, "driver opened");
		
		mThread = new ReadThread();
		mThread.setDaemon(true);
		mThread.start();
		Log.d(null, "thread generated");
		
		linear = (LinearLayout)findViewById(R.id.linLayoutBtn);

		text = (EditText)findViewById(R.id.editText1);
		Button make = (Button)findViewById(R.id.make);
		btn_change = new OnClickListener(){
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				Button b = (Button)v;
				String s = b.getText().toString();
				if(s.equals(""))
					return;
				int num = Integer.parseInt(s);
				int idx[] = Puzzle.findIndex(puzzle, num);
				int result[] = Puzzle.canMove(puzzle, idx[0], idx[1]);
				
				if(result[2] == 1){
					int tmp = puzzle[idx[0]][idx[1]];
					puzzle[idx[0]][idx[1]] = puzzle[result[0]][result[1]];
					puzzle[result[0]][result[1]] = tmp;
					writeDriver(fd, 0);
				}
				else
					return;
				
				boolean matched = Puzzle.isMatched(puzzle);
				linear.removeAllViews();
				genBtn(puzzle.length, puzzle[0].length);

				// TODO: when matched
				if(matched){
					Log.d(null, "tile matched!");
					writeDriver(fd, 1);
				}
				else{
					Log.d(null, "unmatched..");
				}
			}
		};
		
		OnClickListener listener = new OnClickListener(){
			public void onClick(View v){
				String[] s = text.getText().toString().split(" ");
				Log.d(null, "[DEBUG]strings:" + s.toString());
				if(s.length != 2){
					AlertDialog alert = new AlertDialog.Builder(MainActivity2.this).create();
					alert.setMessage("Invalid numbers");
					alert.setButton("OK", new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
							// TODO Auto-generated method stub
							
						}
					});
					return;
				}
				int row = Integer.parseInt(s[0]);
				int col = Integer.parseInt(s[1]);
				puzzle = new int[row][col];
				Log.d(null, "[DEBUG]row: " + String.valueOf(row) + "col: " + String.valueOf(col));
				for(int i = 0; i < row; ++i){
					for(int j = 0; j < col; ++j){
						puzzle[i][j] = i * col + j+1;
					}
				}
				puzzle[row-1][col-1] = 0;
				MainActivity2.this.shuffle();
				Log.d(null, "shuffle done!");
				linear.removeAllViews();
				genBtn(row, col);
				text.setText("");
			}
		};
		
		make.setOnClickListener(listener);
	}
	
	class ReadThread extends Thread{
		public void run(){
			
			while(true){
				int ret = readDriver(fd);
				if(ret == 1)
					break;
			}
			
			Log.d(null, "Timer ended!");
			closeDriver(fd);
			Intent intent=new Intent(MainActivity2.this, MainActivity.class);
			startActivity(intent);
			finish();
		}
	}
	
	public void genBtn(int row, int col){
		Point size = new Point();
		getWindowManager().getDefaultDisplay().getSize(size);
		int screenWidth = size.x;
		int screenHeight = size.y;
		Log.d(null, "Screensize: " + String.valueOf(screenWidth) + " " + String.valueOf(screenHeight));
		grid = new GridLayout(MainActivity2.this);
		grid.setColumnCount(col);
		grid.setRowCount(row);
		LinearLayout.LayoutParams llp = new LinearLayout.LayoutParams(screenHeight/col, 80);
		LinearLayout.LayoutParams glp = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
		grid.setLayoutParams(glp);
		buttons = new Button[row][col];
		for(int i = 0; i < row; ++i){
			//puzzle_view[i] = new LinearLayout(MainActivity2.this);
			//puzzle_view[i].setLayoutParams(new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT));
			for(int j = 0; j < col; ++j){
				buttons[i][j] = new Button(MainActivity2.this);
				if(puzzle[i][j] != 0)
					buttons[i][j].setText(String.valueOf(puzzle[i][j]));
				else{
					buttons[i][j].setText("");
					buttons[i][j].setBackgroundColor(Color.BLACK);
					
				}
				buttons[i][j].setOnClickListener(btn_change);
				buttons[i][j].setLayoutParams(llp);
				grid.addView(buttons[i][j]);
			}
		}
		linear.addView(grid);
	}
	
	public void shuffle(){
		int direction[][] = {{-1,0},{1,0},{0,-1},{0,1}};
		int row, col;
		
		row = puzzle.length;
		col = puzzle[0].length;
		int cur_row = row-1;
		int cur_col = col-1;
		int next_row, next_col, tmp;
		int cnt = 0;

		Log.d(null, "original state:");
		for(int i = 0; i<row; i++){
			String s = "";
			for(int j = 0; j<col; j++){
				s += String.valueOf(puzzle[i][j]) + " ";
			}
			Log.d(null, s);
		}
		
		while(Puzzle.isMatched(puzzle) || cnt <= 50){
			List<int[]> list = new ArrayList<int[]>();
			if(cur_row > 0)
				list.add(direction[0]);
			if(cur_col > 0)
				list.add(direction[2]);
			if(cur_col < col-1)
				list.add(direction[3]);
			if(cur_row < row-1)
				list.add(direction[1]);
			int[] next = list.get(new Random().nextInt(list.size()));
			next_row = next[0] + cur_row;
			next_col = next[1] + cur_col;
			
			tmp = puzzle[cur_row][cur_col];
			puzzle[cur_row][cur_col] = puzzle[next_row][next_col];
			puzzle[next_row][next_col] = tmp;
			
			cur_row = next_row;
			cur_col = next_col;
			cnt++;
		}
		Log.d(null, "shuffle: " + String.valueOf(cnt));
	}
}

