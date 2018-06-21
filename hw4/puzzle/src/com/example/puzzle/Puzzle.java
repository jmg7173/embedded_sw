package com.example.puzzle;

import android.util.Log;

public class Puzzle {
	static public boolean isMatched(int puzzle[][]){
		int row, col, cur;
		boolean flag = true;
		row = puzzle.length;
		col = puzzle[0].length;
		cur = 0;
		for(int i = 0; i < row; i++){
			for(int j = 0; j < col; j++){
				if(puzzle[i][j] != cur + 1){
					if(i != row - 1 || j != col - 1 || puzzle[i][j] != 0){
						return false;
					}
					Log.d(null, String.format("i : %d, j: %d, val: %d", i, j, puzzle[i][j]));				
				}
				cur = puzzle[i][j];
			}
		}
		return true;
	}
	
	static public int[] canMove(int puzzle[][], int cur_row, int cur_col){
		int row, col;
		row = puzzle.length;
		col = puzzle[0].length;
		
		if(cur_row > 0)
			if(puzzle[cur_row-1][cur_col] == 0)
				return new int[]{cur_row-1, cur_col, 1};
		if(cur_col > 0)
			if(puzzle[cur_row][cur_col-1] == 0)
				return new int[]{cur_row, cur_col-1, 1};
		if(cur_col < col-1)
			if(puzzle[cur_row][cur_col+1] == 0)
				return new int[]{cur_row, cur_col+1, 1};
		if(cur_row < row-1)
			if(puzzle[cur_row+1][cur_col] == 0)
				return new int[]{cur_row+1, cur_col, 1};
		return new int[]{0, 0, 0};
	}
	
	static public int[] findIndex(int puzzle[][], int val){
		int row, col;
		row = puzzle.length;
		col = puzzle[0].length;
		for(int i = 0; i<row; i++){
			for(int j = 0; j<col; j++){
				if(puzzle[i][j] == val)
					return new int[]{i, j};
			}
		}
		
		return new int[]{-1, -1};
	}
}
