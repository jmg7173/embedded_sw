#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "binary_game.h"
#include "push_helper.h"

char mod_game(char* buf, char* job, char is_time, char chg){
    static int answer = 0;
    static int timer = 0;
    static int score = -1;
    static int input = 0;
    // 0: not start, 1: start, 2: game over
    static char game_start = 0;
    static char text[33] = {0};
    static char new_question = 0;

    int btn_a = 0, btn_b = 0;
    char is_multi = 0;

    // init
    if(score == -1 || chg){
        if(score == -1)
            srand(time(NULL));
        score = 0;
        timer = 0;
        answer = 0;
        new_question = 0;
        game_start = 0;
        sprintf(text, "Start game? ");
        sprintf(job, "2 init 0 lcd %s", text);
        return 1;
    }
    if(game_start == 0){ // game ready
        if(is_time && new_question){ // If next question
            timer = 9;
            input = -1;
            answer = rand() % 16;
            sprintf(text, "Answer: ");
            sprintf(job, "4 fnd %04d led %d char %d lcd %s", 
                    score, answer << 4, timer, text);
            new_question = 0;
            game_start = 1;
            return 1;
        }
        else if(is_time)
            return 0;
        
        get_push_id(buf, &btn_a, &btn_b, &is_multi);
        if(!is_multi)
            return 0;
        else{
            // if start button triggered
            if(btn_a == 2 && btn_b == 3){
                game_start = 1;
                score = 0;
                timer = 9;
                input = -1;
                answer = rand() % 16;
                sprintf(text, "Answer: ");
                sprintf(job, "3 led %d char %d lcd %s", 
                        answer << 4, timer, text);
                return 1;
            }
            else
                return 0;
        }
    }
    else if(game_start == 1){ // game is ongoing
        if(is_time){
            // count down
            timer--;
            if(timer == 0){
                game_start = 2;
                sprintf(text,"Game over! Press any one key.");
                sprintf(job,"2 init 0 lcd %s", text);
                return 1;
            }
        }
        else{
            // get user input - answer
            get_push_id(buf, &btn_a, &btn_b, &is_multi);
            if(is_multi){
                // submit pressed
                if(btn_a == 2 && btn_b == 3){
                    if(answer == input){
                        score++;
                        game_start = 0;
                        new_question = 1;
                        sprintf(text, "Correct!");
                        sprintf(job, 
                                "3 init 0 fnd %04d lcd %s",
                                score, text);
                        return 1;
                    }
                }
                // zero pressed
                else if(btn_a == 5 && btn_b == 6){
                    if(input == -1)
                        input = 0;
                    input *= 10;
                }
                // delete button pressed
                else if(btn_a == 8 && btn_b == 9){
                    if(input == -1)
                        input = 0;
                    input /= 10;
                }
                else
                    return 0;
            }
            else{
                if(input == -1)
                    input = 0; 
                input = input * 10 + btn_a;
            }
        }
    }
    else if(game_start == 2){ // game over
        if(is_time)
            return 0;
        get_push_id(buf, &btn_a, &btn_b, &is_multi);
        if(!is_multi && btn_a){ // Pressed any button
            game_start = 0;
            new_question = 0;
            answer = 0;
            sprintf(text, "Start game?");
            sprintf(job, "2 init 0 lcd %s", text);
            return 1;
        }
        else
            return 0;
    }

    else{
        return 0;
    }

    sprintf(text, "Answer: ");
    if(input != -1){
        sprintf(text+strlen(text), "%d", input);
    }
    sprintf(job, "4 led %d fnd %04d char %d lcd %s",
            answer << 4, score, timer, text); 
    return 1;
}
