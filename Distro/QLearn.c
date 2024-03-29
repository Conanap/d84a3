/*
	CSC D84 - Unit 3 - Reinforcement Learning
	
	This file contains stubs for implementing the Q-Learning method
	for reinforcement learning as discussed in lecture. You have to
	complete two versions of Q-Learning.
	
	* Standard Q-Learning, based on a full-state representation and
	  a large Q-Table
	* Feature based Q-Learning to handle problems too big to allow
	  for a full-state representation
	    
	Read the assignment handout carefully, then implement the
	required functions below. Sections where you have to add code
	are marked

	**************
	*** TO DO:
	**************

	If you add any helper functions, make sure you document them
	properly and indicate in the report.txt file what you added.
	
	Have fun!

	DO NOT FORGET TO 'valgrind' YOUR CODE - We will check for pointer
	management being done properly, and for memory leaks.

	Starter code: F.J.E. Jan. 16
*/

#include "QLearn.h"
#define inf 99999999
#define dinf 99999999.99

int dists[max_graph_size][max_graph_size];
int distSet = 0;

int deadend[max_graph_size] = {0};

void QLearn_update(int s, int a, double r, int s_new, double *QTable)
{
 /*
   This function implementes the Q-Learning update as stated in Lecture. It 
   receives as input a <s,a,r,s'> tuple, and updates the Q-table accordingly.
   
   Your work here is to calculate the required update for the Q-table entry
   for state s, and apply it to the Q-table
     
   The update involves two constants, alpha and lambda, which are defined in QLearn.h - you should not 
   have to change their values. Use them as they are.
     
   Details on how states are used for indexing into the QTable are shown
   below, in the comments for QLearn_action. Be sure to read those as well!
 */
 
  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/

  double max = -inf;
  for(int i = 0; i < 4; i++) {
    if (getQTable(QTable, s_new, i) > max) {
      max = getQTable(QTable, s_new, i);
    }
  }

  // Q(s,a)   += alpha   (r + gamma    max Q(s’,a’)   - Q(s,a))
  // assign so i don't want to risk function call
  // (*(QTable + (4 * s) + a)) += alpha * (r + lambda * max );
  (*(QTable + (4 * s) + a)) += alpha * (r + lambda * max - getQTable(QTable, s, a) );
  
}

int QLearn_action(double gr[max_graph_size][4], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], double pct, double *QTable, int size_X, int graph_size)
{
  /*
     This function decides the action the mouse will take. It receives as inputs
     - The graph - so you can check for walls! The mouse must never move through walls
     - The mouse position
     - The cat position
     - The chees position
     - A 'pct' value in [0,1] indicating the amount of time the mouse uses the QTable to decide its action,
       for example, if pct=.25, then 25% of the time the mouse uses the QTable to choose its action,
       the remaining 75% of the time it chooses randomly among the available actions.
       
     Remember that the training process involves random exploration initially, but as training
     proceeds we use the QTable more and more, in order to improve our QTable values around promising
     actions.
     
     The value of pct is controlled by QLearn_core_GL, and increases with each round of training.
     
     This function *must return* an action index in [0,3] where
        0 - move up
        1 - move right
        2 - move down
        3 - move left
        
     QLearn_core_GL will print a warning if your action makes the mouse cross a wall, or if it makes
     the mouse leave the map - this should not happen. If you see a warning, fix the code in this
     function!
     
   The Q-table has been pre-allocated and initialized to 0. The Q-table has
   a size of
   
        graph_size^3 x 4
        
   This is because the table requires one entry for each possible state, and
   the state is comprised of the position of the mouse, cat, and cheese. 
   Since each of these agents can be in one of graph_size positions, all
   possible combinations yield graph_size^3 states.
   
   Now, for each state, the mouse has up to 4 possible moves (up, right,
   down, and left). We ignore here the fact that some moves are not possible
   from some states (due to walls) - it is up to the QLearn_action() function
   to make sure the mouse never crosses a wall. 
   
   So all in all, you have a big table.
        
   For example, on an 8x8 maze, the Q-table will have a size of
   
       64^3 x 4  entries
       
       with 
       
       size_X = 8		<--- size of one side of the maze
       graph_size = 64		<--- Total number of nodes in the graph
       
   Indexing within the Q-table woQsa(weights, features)rks as follows:
   
     say the mouse is at   i,j
         the cat is at     k,l
         the cheese is at  m,n
         
     state = (i+(j*size_X)) + ((k+(l*size_X))*graph_size) + ((m+(n*size_X))*graph_size*graph_size)
     ** Make sure you undestand the state encoding above!
     
     Entries in the Q-table for this state are

     *(QTable+(4*state)+a)      <-- here a is the action in [0,3]
     
     (yes, it's a linear array, no shorcuts with brackets!)
     
     NOTE: There is only one cat and once cheese, so you only need to use cats[0][:] and cheeses[0][:]
   */
  
  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/  

  int choice;

  // lord, do i do rng
  double rng = rngesus();
  if(rng > pct) {
    do {
      choice = rand() % 4;
    } while(!isConnected(mouse_pos[0][0] + (size_X * mouse_pos[0][1]), choice, gr));
    return choice;
  }

  // pick best move
  double max = -inf;
  int state = getState(mouse_pos[0], cats[0], cheeses[0], size_X, graph_size);

  for(int i = 0; i < 4; i++) {
    if(!isConnected(mouse_pos[0][0] + (size_X * mouse_pos[0][1]), i, gr))
      continue;

    if(getQTable(QTable, state, i) > max) {
      max = getQTable(QTable, state, i);
      choice = i;
    }
  }

  return choice;		// <--- of course, you will change this! will i tho
  
}

double QLearn_reward(double gr[max_graph_size][4], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
{
  /*
    This function computes and returns a reward for the state represented by the input mouse, cat, and
    cheese position. 
    
    You can make this function as simple or as complex as you like. But it should return positive values
    for states that are favorable to the mouse, and negative values for states that are bad for the 
    mouse.
    
    I am providing you with the graph, in case you want to do some processing on the maze in order to
    decide the reward. 
        
    This function should return a maximim/minimum reward when the mouse eats/gets eaten respectively.      
   */

   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/ 

  double reward = 0;
  if(!distSet) {
    distSet = 1;
    fwInit(gr, size_X, graph_size);
  }

  if(isSameSpot(mouse_pos[0], cats[0])) {
    reward -= 100;
  }
  if(isSameSpot(mouse_pos[0], cheeses[0])) {
    reward += 100;
  }

  // if(reward)
  //   return reward;
  
  // int mouse = mouse_pos[0][0] + (size_X * mouse_pos[0][1]);
  // int otloc = cheeses[0][0] + (size_X * cheeses[0][1]);
  // reward += 200 * (graph_size - dists[mouse][otloc])/graph_size;
  // otloc = cats[0][0] + (size_X * cats[0][1]);
  // reward -= 200 * ((graph_size - dists[mouse][otloc]) / graph_size);

  return reward;		// <--- of course, you will change this as well! no.
}

void feat_QLearn_update(double gr[max_graph_size][4],double weights[25], double reward, int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
{
  /*
    This function performs the Q-learning adjustment to all the weights associated with your
    features. Unlike standard Q-learning, you don't receive a <s,a,r,s'> tuple, instead,
    you receive the current state (mouse, cats, and cheese potisions), and the reward 
    associated with this action (this is called immediately after the mouse makes a move,
    so implicit in this is the mouse having selected some action)
    
    Your code must then evaluate the update and apply it to the weights in the weight array.    
   */
  
   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/

    double features[25] = {0};
    evaluateFeatures(gr, features, mouse_pos, cats, cheeses, size_X, graph_size);
    double maxU = -dinf;
    int maxA = -1;
    maxQsa(gr, weights, mouse_pos, cats, cheeses, size_X, graph_size, &maxU, &maxA);
    double qs = Qsa(weights, features);

    for(int i = 0; i < 25; i++) {
      // wi = wi + alpha    [r      + gamma    Q(s’) - Q(s)] * fi(s)
      weights[i] += alpha * (reward + lambda * maxU - qs) * features[i];
    }
}

int feat_QLearn_action(double gr[max_graph_size][4],double weights[25], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], double pct, int size_X, int graph_size)
{
  /*
    Similar to its counterpart for standard Q-learning, this function returns the index of the next
    action to be taken by the mouse.
    
    Once more, the 'pct' value controls the percent of time that the function chooses an optimal
    action given the current policy.
    
    E.g. if 'pct' is .15, then 15% of the time the function uses the current weights and chooses
    the optimal action. The remaining 85% of the time, a random action is chosen.
    
    As before, the mouse must never select an action that causes it to walk through walls or leave
    the maze.    
   */

  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/        

  // lord, do i do rng
  if(rngesus() > pct) {
    int choice;
    do {
      choice = rand() % 4;
    } while(!isConnected(mouse_pos[0][0] + (size_X * mouse_pos[0][1]), choice, gr));
    return choice;
  }
  
  // get max qsa
  int choice = -1;
  double maxU = -dinf;

  maxQsa(gr, weights, mouse_pos, cats, cheeses, size_X, graph_size, &maxU, &choice);

  return choice;		// <--- replace this while you're at it!

}

void evaluateFeatures(double gr[max_graph_size][4], double features[25], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
{
  /*
   This function evaluates all the features you defined for the game configuration given by the input
   mouse, cats, and cheese positions. You are free to define up to 25 features. This function will
   evaluate each, and return all the feature values in the features[] array.
   
   Take some time to think about what features would be useful to have, the better your features, the
   smarter your mouse!
   
   Note that instead of passing down the number of cats and the number of cheese chunks (too many parms!)
   the arrays themselves will tell you what are valid cat/cheese locations.
   
   You can have up to 5 cats and up to 5 cheese chunks, and array entries for the remaining cats/cheese
   will have a value of -1 - check this when evaluating your features!
  */

   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/      
   if(!distSet) {
     distSet = 1;
     fwInit(gr, size_X, graph_size);

     for(int i = 0; i < max_graph_size; i++)
      deadend[i] = -1;
   }

  int i = 0;
  int mouse = mouse_pos[0][0] + (size_X * mouse_pos[0][1]);
  int otloc;

  double minCheese = inf;
  // dist to cheese
  for(int j = 0; j < 5 && i < 25; j++) {
    if(cheeses[j][0] == -1) {
      features[i++] = 0;
      continue;
    }
    otloc = cheeses[j][0] + (size_X * cheeses[j][1]);
    features[i] = (double) 1/ (double)(1 + (dists[mouse][otloc]));
    if(minCheese > features[i])
      minCheese = features[i];
    i++;
  }

  int catC = 0;
  int catD = 0;
  int minCat = inf;
  // dist to cats
  for(int j = 0; j < 5 && i < 25; j++) {
    if(cats[j][0] == -1) {
      features[i++] = 0;
      continue;
    }
    otloc = cats[j][0] + (size_X * cats[j][1]);
    features[i] = (double) 1/ (double)(1 + (dists[mouse][otloc]));
    catC++;
    catD += features[i];
    if(minCat > features[i])
      minCat = features[i];
    i++;
  }

  features[i++] = minCat;
  features[i++] = minCheese;
  features[i++] = (double)catD / (double)catC > size_X / 2.5 ? 1 : 0;

  features[i++] = hasCat(mouse_pos[0], cats, 0, size_X);
  features[i++] = hasCat(mouse_pos[0], cats, 1, size_X);
  features[i++] = hasCat(mouse_pos[0], cats, 2, size_X);
  features[i++] = hasCat(mouse_pos[0], cats, 3, size_X);


  features[i++] = isDeadend(mouse_pos[0][0], mouse_pos[0][1], size_X, gr) ? 1 : 0;


  while(i < 25) {
    features[i++] = 0;
  }

}

double Qsa(double weights[25], double features[25])
{
  /*
    Compute and return the Qsa value given the input features and current weights
   */

  /***********************************************************************************************
  * TO DO: Complete this function
  ***********************************************************************************************/  
  double sum = 0;

  for(int i = 0; i < 25; i++) {
    sum += weights[i] * features[i];
  }
  return sum;		// compute and return the Qsa value
}

void maxQsa(double gr[max_graph_size][4],double weights[25], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size, double *maxU, int *maxA)
{
 /*
   Given the state represented by the input positions for mouse, cats, and cheese, this function evaluates
   the Q-value at all possible neighbour states and returns the max. The maximum value is returned in maxU
   and the index of the action corresponding to this value is returned in maxA.
   
   You should make sure the function does not evaluate moves that would make the mouse walk through a
   wall. 
  */
 
   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/  
 
  double features[25] = {0};

  *maxU = -dinf;
  double temp;
  int nmouse[1][2] = {0};

  for(int i = 0; i < 4; i++) {
    // skip wall
    // always have 1 spot to go by def of maze
    if(!isConnected(mouse_pos[0][0] + (size_X * mouse_pos[0][1]), i, gr))
      continue;

    if(!i) {
      nmouse[0][0] = mouse_pos[0][0];
      nmouse[0][1] = mouse_pos[0][1] - 1;
    } else if (i == 1) {
      nmouse[0][0] = mouse_pos[0][0] + 1;
      nmouse[0][1] = mouse_pos[0][1];
    } else if (i == 2) {
      nmouse[0][0] = mouse_pos[0][0];
      nmouse[0][1] = mouse_pos[0][1] + 1;
    } else {
      nmouse[0][0] = mouse_pos[0][0] - 1;
      nmouse[0][1] = mouse_pos[0][1];
    }
    
    evaluateFeatures(gr, features, nmouse, cats, cheeses, size_X, graph_size);
    temp = Qsa(weights, features);
    if(temp > *maxU) {
      *maxU = temp;
      *maxA = i;
    }
  }

  return;   
}

/***************************************************************************************************
 *  Add any functions needed to compute your features below 
 *                 ---->  THIS BOX <-----
 * *************************************************************************************************/

double rngesus () {
  return (double) rand() / (double) (RAND_MAX);
}

double getQTable(double *QTable, int s, int a) {
  return *(QTable + (4 * s) + a);
}

int getState(int mouse_loc[2], int cat_loc[2], int cheese_loc[2], int size_X, int graph_size) {
  // say the mouse is at   i,j
  //        the cat is at     k,l
  //        the cheese is at  m,n
         
  //    state = (i+(j*size_X)) + ((k+(l*size_X))*graph_size) + ((m+(n*size_X))*graph_size*graph_size)
  int state = (mouse_loc[0] + (mouse_loc[1] * size_X));
  state += ((cat_loc[0] + (cat_loc[1] * size_X)) * graph_size);
  state += ((cheese_loc[0] + (cheese_loc[1] * size_X)) * graph_size * graph_size);
  return state;
}

int isSameSpot(int a[2], int b[2]) {
  return a[0] == b[0] && a[1] == b[1];
}

bool isConnected(int a, int b, double gr[max_graph_size][4])
{
  return gr[a][b];
}
 
void fwInit(double gr[max_graph_size][4], int size_X, int graph_size)
{
	for (int x = 0; x < graph_size; x++)
	{
		for (int y = 0; y < graph_size; y++)
		{
			dists[x][y] = inf;
		}
	}

	// init connected
	for (int i = 0; i < graph_size; i++)
	{
		dists[i][i] = 0;
		if (i - 1 >= 0 && isConnected(i, 3, gr))
		{ // L
			dists[i][i - 1] = 1;
		}
		if (i - size_X >= 0 && isConnected(i, 0, gr))
		{ // T
			dists[i][i - size_X] = 1;
		}
		if (i + 1 < graph_size && isConnected(i, 1, gr))
		{ // R
			dists[i][i + 1] = 1;
		}
		if (i + size_X < graph_size && isConnected(i, 2, gr))
		{ // D
			dists[i][i + size_X] = 1;
		}
	}

	for (int k = 0; k < graph_size; k++)
	{
		for (int i = 0; i < graph_size; i++)
		{
			for (int j = 0; j < graph_size; j++)
			{
				if (dists[i][j] > dists[i][k] + dists[k][j])
				{
					dists[i][j] = dists[i][k] + dists[k][j];
				}
			}
		}
	}
}

int manDist(int x1, int y1, int x2, int y2)
{
	return (abs(x1 - x2) + abs(y1 - y2));
}

bool isDeadend(int x, int y, int size_X, double gr[max_graph_size][4]) {
	int pos = x + y * size_X;
	if(deadend[pos] == -1) {
		deadend[pos] = 1;
		for(int i = 0; i < 4; i++)
			deadend[pos] += isConnected(pos, i, gr);

    if(deadend[pos] == 1)
      deadend[pos] = 0;
	}

	return deadend[pos];
}

double hasCat(int mouse[2], int cats[5][2], int dir, int size_X) {

  int ret = 0;

  for(int i = 0; i < 5 && !ret; i++) {
    if(dir == 3)
      ret += cats[i][0] > mouse[0] - 5;
    else if(dir == 0)
      ret += cats[i][1] > mouse[1] - 5;
    else if(dir == 1)
      ret += cats[i][0] < mouse[0] + 5;
    else if(dir == 2)
      ret += cats[i][1] < mouse[1] + 5;
  }

  return (double)ret / 10;
}