package com.example.puzzle;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.GridLayout;
import android.widget.LinearLayout;

public class MainActivity2 extends Activity{
	LinearLayout linear;
	EditText text;
	GridLayout grid;
	//LinearLayout puzzle_view[];
	int puzzle[][];
	Button buttons[][];
	OnClickListener btn_change;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main2);
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
				}
				else
					return;
				
				boolean matched = Puzzle.isMatched(puzzle);
				linear.removeAllViews();
				genBtn(puzzle.length, puzzle[0].length);

				// TODO: when matched
				if(matched){
					Log.d(null, "tile matched!");
					linear.removeAllViews();
					
					return;
				}
				else{
					Log.d(null, "unmatched..");
					return;
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
						puzzle[i][j] = i * row + j+1;
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
	
	public void genBtn(int row, int col){
		grid = new GridLayout(MainActivity2.this);
		grid.setColumnCount(col);
		grid.setRowCount(row);
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
		
		for(int cnt = 0; cnt < 20; cnt++){
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
		}
	}
}

