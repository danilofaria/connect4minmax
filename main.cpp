/*
 Connect 4
 */
#define false 0
#define true 1

#define N_ROWS 4
#define N_COLUMNS 4

#define INFINITY 99999

#include <stdio.h>
int i,j;

/*Game table, in each position it holds the number of the player
 that has picked that position (1 or 2) ou 0 if it has not been taken.
 
 For example:
 
 0 1 2 3 4 5 6
 0 |0|0|0|0|0|0|0|
 1 |0|0|0|0|0|0|0|
 2 |0|0|0|0|0|0|0|
 3 |0|0|0|0|0|0|0|
 4 |0|1|1|1|0|0|0|
 5 |1|2|2|2|2|0|0|
 */
int table[N_ROWS][N_COLUMNS];

// Either 1 or 2
int current_player = 1;
int max_player = 1;
// Each player's score
int score1 = 0;
int score2 = 0;

// current column picked by current player
int current_move;
// current row picked by current player
int current_row;

/*
 Structures for maintaining the state of the recursion.
 */
typedef struct state
{
    int table[N_ROWS][N_COLUMNS]; //board state
    int current_move;
    int parent_index;
    int node_value;
    int child_count; // -1 means children havent been generated yet, non negative numbers mean how many children are left to be checked for minmax values
    int depth;
    
} state;


/*
 Prints table and score
 */
void print_table(int t[N_ROWS][N_COLUMNS]) {
    
     printf("~* CONNECT 4 *~ \n \n");
    
    // Print table
    for (i = 0; i < N_ROWS; i++) {
        printf("|");
        for (j = 0; j < N_COLUMNS; j++) {
            if (t[i][j] == 0)
                printf(" . ");
            if (t[i][j] == 1)
                printf(" X ");
            if (t[i][j] == 2)
                printf(" 0 ");
            printf("|");
        }
        printf("\n");
    }
    
    // Print numbers
     printf("\n+ ");
     for (j=0; j < N_COLUMNS; j++)
       printf("%d   ",j);
     printf("+ \n \n");
    
//     Score
     printf("SCORE: \n Player 1 (X) = %d \n Player 2 (0) = %d \n \n", score1, score2);
}


void print_state(state s){
    // printf("%s\n", );
    print_table(s.table);
    printf("current move %d\n", s.current_move);
    printf("parent index %d\n", s.parent_index);
    printf("node value %d\n", s.node_value);
    printf("child count %d\n", s.child_count);
    printf("depth %d\n", s.depth);
}

/*
 Checks if player won by making a sequence of 4 markers either
 horizontally, vertically or diagonally.
 */
int current_player_won(int table[N_ROWS][N_COLUMNS], int current_row, int current_move, int current_player){
    // Check for vertical sequence
    // Look at last marker placed and compare with the 3 markers below it
    if ((current_row < N_ROWS-3)
        && (table[current_row][current_move] == table[current_row + 1][current_move])
        && (table[current_row + 1][current_move] == table[current_row + 2][current_move])
        && (table[current_row+ 2][current_move] == table[current_row + 3][current_move]))
        return true;
    
    // Check for horizontal sequence
    int sequence_length = 1;
    j = 1;
    while ((current_move - j >= 0) && (table[current_row][current_move - j] == current_player)){
        j++; sequence_length++;
    }
    j = 1;
    while ((current_move + j < N_COLUMNS) && (table[current_row][current_move + j] == current_player)){
        j++; sequence_length++;
    }
    if (sequence_length >= 4)
        return true;
    
    //Check for diagonal sequence
    sequence_length = 1;
    j = 1;
    while((current_move - j >= 0) && (current_row - j >= 0) && (table[current_row - j][current_move - j] == current_player)){
        j++; sequence_length++;
    }
    j = 1;
    while ((current_move + j < N_COLUMNS) && (current_row + j <= 5) && (table[current_row + j][current_move + j] == current_player)){
        j++; sequence_length++;
    }
    if (sequence_length >= 4)
        return true;
    
    //Check for inverted diagonal sequence
    sequence_length = 1;
    j = 1;
    while((current_move - j >= 0) && (current_row + j < N_ROWS) && (table[current_row + j][current_move - j] == current_player)){
        j++; sequence_length++;
    }
    j = 1;
    while ((current_move + j < N_COLUMNS) && (current_row - j >= 0) && (table[current_row - j][current_move + j] == current_player)){
        j++; sequence_length++;
    }
    if (sequence_length >= 4)
        return true;
    
    return false;
}

int column_is_full (int table[N_ROWS][N_COLUMNS], int column_j) {
    return (table[0][column_j] != 0);
}

int table_is_full(int table[N_ROWS][N_COLUMNS]) {
    for (j = 0; j < N_COLUMNS; j++){
        //If some column is not full, then table is not full
        if (table[0][j] == 0)
            return false;
    }
    return true;
}

void copy_table(int new_table[N_ROWS][N_COLUMNS], int source_table[N_ROWS][N_COLUMNS])
{
    for (int i = 0; i < N_ROWS; i++)
        for (int j = 0; j < N_COLUMNS; j++)
            new_table[i][j] = source_table[i][j];
}

typedef struct stack
{
    int last_i;
    state data[400];
} stack;

void stack_push(stack &s, state some_state){
    s.last_i = s.last_i + 1;
    // printf("last i: %d\n",s.last_i);
    s.data[s.last_i] = some_state;
    // s.data[s.last_i].current_move = some_state.current_move;
    // s.data[s.last_i].parent_index = some_state.parent_index;
    // s.data[s.last_i].node_value = some_state.node_value;
    // s.data[s.last_i].child_count = some_state.child_count;
    // s.data[s.last_i].depth = some_state.depth;
    
    // print_state(some_state);
    // print_state(s.data[s.last_i]);
    
}

int stack_is_empty(stack &s){
    return (s.last_i == -1);
}

state stack_pop(stack &s){
    state st = s.data[s.last_i];
    s.last_i--;
    return st;
}

state stack_peek(stack &s){
    // printf("last i: %d\n",s.last_i);
    return s.data[s.last_i];
    // return st;
}

stack new_stack(){
    stack s;
    s.last_i = -1;
    return s;
}






state new_state(int t[N_ROWS][N_COLUMNS], int current_move, int parent_index, int node_value, int child_count, int depth){
    state s;
    copy_table(s.table, t);
    s.current_move = current_move;
    s.parent_index = parent_index;
    s.node_value = node_value;
    s.child_count = child_count;
    s.depth = depth;
    // print_state(s);
    
    return s;
}




int minmax(int current_table[N_ROWS][N_COLUMNS], int origin_is_max){
    
    int value = INFINITY;
    if (origin_is_max) value = -INFINITY;
    
    state origin;
    origin = new_state(current_table, -1, -1, value, -1, 0);
    
    stack s = new_stack();
    
    stack_push(s, origin);
    
    int best_move = -1;
    
    while(!stack_is_empty(s)){
        state current_state = stack_peek(s);
        int current_index = s.last_i;
        int current_depth = current_state.depth;
        int is_max = (current_depth % 2 == 0);
        if (!origin_is_max) is_max = !(is_max);
        int last_player = 1;
        if (is_max) last_player = 2;
        int current_move = current_state.current_move;
        int parent_index = current_state.parent_index;
        
        //  printf("current state:\n");
        //  print_state(current_state);
        
        int lose = 0, full_table = 0, all_children_accounted_for = 0;
        // If not the origin node, then test for win and full table situations
        if (current_move != -1){
            // retrieve row index of current state's move
            int row = 0;
            while (current_state.table[row][current_move] == 0)
                row++;
            
            //check if player wins in this move
            lose = current_player_won(current_state.table, row, current_move, last_player);
            
            //check if table gets full
            full_table = table_is_full(current_state.table);
        }

        all_children_accounted_for = current_state.child_count == 0;
            
        //if node is terminal (leaf) or all children have been computed
        if (lose || full_table || all_children_accounted_for ){//
            
            // If origin node, then end search
            if (current_move == -1){
                stack_pop(s);
                continue;
            }
            
            int value = 0;
            if (lose)
                value = -1;
            if (!is_max)
                value = -value;
            if (all_children_accounted_for)
                value = current_state.node_value;
            
            state parent_state = s.data[parent_index];
            int parent_value = parent_state.node_value;
            
            int parents_parent_index = s.data[parent_index].parent_index;
            
            //If current state is max, the parent state is min
            if(is_max){
                // if parent state has bigger value, give it the smaller value
                if (parent_value > value){
                    
                    s.data[parent_index].node_value = value;
                    if(current_depth == 1)
                        best_move = current_move;
                    
                    if (parents_parent_index != -1){
                        int alpha = s.data[parents_parent_index].node_value;
                        int beta = value;
                        if (alpha >= beta){
                            s.last_i = parent_index;
                            s.data[parent_index].child_count = 0;
                            continue;
                        }
                    }
                    
                }
            }//otherwise, parent state is max
            else{
                if (parent_value < value){
                
                    s.data[parent_index].node_value = value;
                    if(current_depth == 1)
                        best_move = current_move;
                
                    if (parents_parent_index != -1){
                        int beta = s.data[parents_parent_index].node_value;
                        int alpha = value;
                        if (alpha >= beta){
                            s.last_i = parent_index;
                            s.data[parent_index].child_count = 0;
                            continue;
                        }
                    }
                    
                }
            }
            
            s.data[parent_index].child_count--;
            stack_pop(s);
            continue;
        }
        
        
        // Generate children
        int child_count = 0;
        int child_value = -INFINITY;
        if (is_max) child_value = INFINITY;
        
        for(j = 0; j < N_COLUMNS; j++ ){
            if (column_is_full(current_state.table, j)) continue;
            child_count++;
            
            int child_table[N_ROWS][N_COLUMNS];
            copy_table(child_table, current_state.table);
            
            // Find row where we can play next
            int row = N_ROWS-1;
            while (child_table[row][j] != 0)
                row--;
            
            if (is_max)
                child_table[row][j] = 1;
            else
                child_table[row][j] = 2;
            
            state child;
            child = new_state(child_table, j, current_index, child_value, -1, current_depth+1);
            
            stack_push(s, child);
        }
        
        s.data[current_index].child_count = child_count;
    }
    
    printf("best move %d\n", best_move);
    return best_move;
}




void clear_table(){
    for (i = 0; i < N_ROWS; i++)
        for (j = 0; j < N_COLUMNS; j++)
            table[i][j] = 0;
}




/*
 Ask player which column to pick and change table accordingly
 */
void pick_column() {
    
    if (current_player == 1){
        current_move = -1;
        printf("Pick a column, then press enter, player %d:\n", current_player);
        scanf ("%d", &current_move);
    
        // while move is invalid, keep asking
        while(current_move < 0 || current_move > N_COLUMNS-1 || column_is_full(table, current_move)){
            printf("invalid move, pick another column:\n");
            scanf ("%d",&current_move);
        }
    }else{
        current_move = minmax(table,max_player==2);
    }
    
    // Find row where his move will be performed
    // It will be the first row with value equals 0
    int row = N_ROWS-1;
    while (table[row][current_move] != 0)
        row--;
    
    // Change table accordingly
    table[row][current_move] = current_player;
    
    // Store row where player just placed his marker
    // Used to check if current player won the game
    current_row = row;
}

/*
 Switch current player
 */
void switch_player(){
    if (current_player == 1)
        current_player = 2;
    else
        current_player = 1;     
}



/* 
 Increase current player's score
 */
void update_score(){
    if (current_player == 1)
        score1++;
    else
        score2++;
}

int main(){
    
//    printf("minmax: %d", minmax(table,1));
    
    // game loop
    while (true) { 
        clear_table();
        print_table(table);
        // The player who starts is the max player
        max_player = current_player;
        
        // match loop
        while (true) {
            pick_column();
            print_table(table); 
            
            if (current_player_won(table, current_row, current_move, current_player)){
                update_score();
                print_table(table);
                printf("Congratulations, Player %d! \n", current_player);
                // leave match
                break;
            } 
            // If nobody wins and table is full, we come to a draw
            else if (table_is_full(table)){
                printf("Draw! \n");
                // leave match
                break;
            } 
            
            switch_player();
        }
        
        printf("Do you wish to play again?(y/n) \n");
        char play_again;  
        scanf (" %c",&play_again);
        if (!(play_again == 'y' || play_again == 'Y'))         
            break;
        
    }
    
    return 0; 
    
}
