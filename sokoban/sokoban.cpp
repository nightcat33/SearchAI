#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <stack>
#include <set>
#include <queue>
#include <map>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

using namespace std;

#define WALL 		'%'
#define BOX  		'b'
#define PLAYER 		'P'
#define BOX_ON_GOAL 'B'
#define STORAGE 	'.'

#define UP 		1
#define DOWN 	2
#define LEFT 	3
#define RIGHT 	4

vector<pair<int,int>> storages;
vector<vector<char>> board;
vector<pair<int,int>> walls;

// only box and player's locations is getting changed
class BoardState {
	// row , col coordinates
public:
	vector<pair<int,int>> box;
	pair<int,int> player;
	// BoardState *parent;

	bool operator!=(BoardState &other) const{
		if(player != other.player){
			return true;
		}

		for (int i = 0; i < (int)storages.size(); i++)
		{
			if (box[i] != other.box[i]){
				return true;
			}
		}
		return false;
	}
};


class ComparatorState
{
public:
	bool operator() (const BoardState& state1, const BoardState& state2) const
	{
		pair<int,int> player1 = state1.player;
		pair<int,int> player2 = state2.player;
		if (player1 != player2){
			return player1 < player2;
		}
		for (int i = 0; i < (int)storages.size(); i++)
		{
			if (state1.box[i] != state2.box[i]){
				return state1.box[i] < state2.box[i];
			}
		}
		return false;
	}
};

vector<vector<char>> read_maze(string filename){
	ifstream infile;
	infile.open(filename, ios::binary);
	char c;
	vector<char> line;
	vector<vector<char>> maze;

	while(!infile.eof()){

		infile >> noskipws >> c;
		
		if(c == WALL || c == ' ' || c == PLAYER || c == BOX || c == BOX_ON_GOAL || c == STORAGE){
			line.push_back(c);
		}else{
			if(c == '\n'){
				maze.push_back(line);
				line.clear();
			}
		}

		if(infile.eof()){
			maze.push_back(line);
		}
	}
	infile.close();
	return maze;
}

vector<pair<int,int>> get_walls(vector<vector<char>> maze){
	size_t row = maze.size();
	size_t col = maze[0].size();
	vector<pair<int, int>> Boxes;

	for(size_t i=0; i<row; i++){
		for(size_t j=0; j<col; j++){
			if(maze[i][j] == WALL){
				Boxes.push_back(make_pair(i,j));
			}
		}
	}
	return Boxes;
}

vector<pair<int,int>> get_box_locs(vector<vector<char>> maze) {
	size_t row = maze.size();
	size_t col = maze[0].size();
	vector<pair<int, int>> Boxes;

	for(size_t i=0; i<row; i++){
		for(size_t j=0; j<col; j++){
			if(maze[i][j] == BOX || maze[i][j] == BOX_ON_GOAL){
				Boxes.push_back(make_pair(i,j));
			}
		}
	}
	return Boxes;
}

vector<pair<int,int>> get_storage_locs(vector<vector<char>> maze) {
	size_t row = maze.size();
	size_t col = maze[0].size();
	vector<pair<int, int>> Storages;

	for(size_t i=0; i<row; i++){
		for(size_t j=0; j<col; j++){
			if(maze[i][j] == BOX_ON_GOAL || maze[i][j] == STORAGE){
				Storages.push_back(make_pair(i,j));
			}
		}
	}
	return Storages;
}

pair<int,int> get_player(vector<vector<char>> maze){
	size_t row = maze.size();
	size_t col = maze[0].size();
	pair<int, int> player;

	for(size_t i=0; i<row; i++){
		for(size_t j=0; j<col; j++){
			if(maze[i][j] == 'P'){
				player = make_pair(i,j);
			}
		}
	}
	return player;
}

// true, if box, otherwise false
bool boxCheck(BoardState state, pair<int,int> loc){
	for(auto i : state.box){
		if(i == loc){
			return true;
		}
	}
	return false;
}

void updateCoordinate(pair<int,int> &loc, int direction){
	switch(direction){
		case UP:
			loc.first -= 1;
			break;
		case DOWN:
			loc.first += 1;
			break;
		case LEFT:
			loc.second -= 1;
			break;
		case RIGHT:
			loc.second += 1;
			break;
	}
	return;
}


// wall return true, otherwise return false
bool checkWall(pair<int,int> loc){
	if(board[loc.first][loc.second] == WALL){
		return true;
	}else{
		return false;
	}
}

bool checkBound(pair<int,int> loc){
	if(loc.first >= 0 && loc.first < (int)board.size() && loc.second >= 0 && loc.second < (int)board[0].size()){
		return true;
	}else{
		return false;
	}
}

bool canMove(BoardState &currState, int direction){
	pair<int,int> player = currState.player;

	//update player's loc based on direction
	updateCoordinate(player, direction);
	// check if player's new loc is wall
	if(checkBound(player) && !checkWall(player)){
		// check if player's new loc is box
		if(boxCheck(currState, player)){
			// we need to update box's location too
			pair<int,int> box_loc = player;
			updateCoordinate(box_loc, direction);

			// no wall and no another box
			if(checkBound(box_loc) && !checkWall(box_loc) && !boxCheck(currState, box_loc)){
				// find out which box to update
				vector<pair<int,int>> boxes = currState.box;
				for(size_t i=0; i<boxes.size(); i++){
					if(boxes[i] == player){
						boxes[i] = box_loc;
						break;
					}
				}
				// update state
				currState.box = boxes;
				currState.player = player;
				return true;

			}else{
				return false;
			}
		}else{
			// just move player
			// update state
			currState.player = player;
			return true;
		}

	}else{
		return false;
	}
}

bool checkSolved(BoardState state){
	vector<pair<int,int>> box = state.box;
	size_t count = 0;
	for(auto i : box){
		for(auto j : storages){
			if(i.first == j.first && i.second == j.second){
				count++;
				break;
			}
		}
	}
	if(count != box.size()){
		return false;
	}else{
		return true;
	}
}

stack<BoardState> BFS(BoardState init_state){
	queue<BoardState> q;
	set<BoardState, ComparatorState> visited;
	// used for backtracing
	map<BoardState, BoardState, ComparatorState> path;
	path[init_state] = init_state;
	q.push(init_state);
	int n = 0;
	BoardState currState;
	BoardState nextState;
	clock_t t;
	t = clock();
	while(!q.empty()){
		currState = q.front();
		q.pop();
		n++;
		if(checkSolved(currState)){
			break;
		}
		for(int i=1; i<=4; i++){
			bool moveable;
			nextState = currState;
			moveable = canMove(nextState, i);

			// check movability and if visited before
			if(moveable == true && visited.find(nextState) == visited.end()){
				q.push(nextState);
				visited.insert(nextState);
				path[nextState] = currState;
			}
		}
	}

	t = clock() - t;
	printf ("It took me %lu clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
	cout << "BFS complete." << endl;
	cout << "# of expanded nodes: " << n << endl;

	vector<string> direction = {"up", "down", "left", "right"};
	stack<string> solution;
	stack<BoardState> board_change;
	// now current state is final state, so we need to backtrace
	while(currState != init_state){
		board_change.push(currState);
		BoardState prevState = path[currState];
		for(int i=1; i<=4; i++){
			pair<int,int> prevPlayer = prevState.player;
			updateCoordinate(prevPlayer, i);
			if(prevPlayer == currState.player){
				solution.push(direction[i-1]);
				break;
			}
		}
		currState = prevState;
	}
	cout << "# of movements: " << solution.size() << endl;
	while(!solution.empty()){
		auto i = solution.top();
		solution.pop();
		cout << i << " ";
	}
	cout << endl;

	return board_change;
}


class Node
{
public:
	int g;
	int h;
	BoardState state;
	int f;

	void update_f(void){
		this->f = g+h;
	}
};

class NodeComparator
{
	public:
	bool operator() (const Node& n1, const Node& n2) const
	{
		return n1.f > n2.f;
	}
};

// heuristic function for A star search
int heuristic_f(BoardState currState){
	queue<pair<int,int>> q;
	vector<vector<int>> distance;

	int row = board.size();
	int col = board[0].size();

	// initalize distance map
	for(int i=0; i<row; i++){
		vector<int> line;
		for(int j=0; j<col; j++){
			line.push_back(-1);
		}
		distance.push_back(line);
	}

	for(auto i : storages){
		distance[i.first][i.second] = 0;
		q.push(i);
	}
	pair<int,int> current;
	// using BFS to expand from storage's locations
	while(!q.empty()){
		current = q.front();
		q.pop();

		for(int i=1; i<=4; i++){
			pair<int,int> neighbor = current;
			updateCoordinate(neighbor, i);
			// wall return true
			if(!checkWall(neighbor) && distance[neighbor.first][neighbor.second] == -1){
				distance[neighbor.first][neighbor.second] = distance[current.first][current.second] + 1;
				q.push(neighbor);
			}
		}
	}
	// sum up box distance to nearst storage
	int h = 0;
	for(auto i : currState.box){
		h += distance[i.first][i.second];
	}
	return h;
}


stack<BoardState> A_Star_Search(BoardState init_state){
	map<BoardState, BoardState, ComparatorState> path;
	// mapping g value and used for closed Set
	map<BoardState, int, ComparatorState> distance; 
	priority_queue<Node, vector<Node>, NodeComparator> min_heap;

	Node init_node;

	init_node.g = 0;
	init_node.h = heuristic_f(init_state);
	init_node.update_f();
	init_node.state = init_state;

	path[init_state] = init_state;
	distance[init_state] = 0; // close set and tracking g value

	min_heap.push(init_node);

	Node currNode;
	BoardState currState;
	
	BoardState nextState;
	clock_t t;
	t = clock();
	int n = 0;
	while(!min_heap.empty()){
		currNode = min_heap.top();
		currState = currNode.state;
		min_heap.pop();
		n++; 

		// current node is bad, so continue, we did not remove bad one from min heap, 
		if(currNode.g > distance[currState]){
			continue;
		}

		if(checkSolved(currState)){
			break;
			// now currNode will be final node with final state
		}

		for(int i=1; i<=4; i++) {
			nextState = currState;
			bool moveable;
			moveable = canMove(nextState, i);
			if(moveable){
				// did not find in closeSet or current one is better
				if(distance.find(nextState) == distance.end() || distance[nextState] > currNode.g + 1){
					Node nextNode;
					// set up the next node and distance dict
					nextNode.state = nextState;
					distance[nextState] = currNode.g + 1;
					nextNode.g = distance[nextState];
					nextNode.h = heuristic_f(nextState);
					nextNode.update_f();

					path[nextState] = currState;
					min_heap.push(nextNode);
				}				

			}
		}
	}
	t = clock() - t;
	printf ("It took me %lu clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
	cout << "A star search complete." << endl;
	cout << "# of expanded nodes: " << n << endl;
	// cout << "# of expanded nodes: " << n << endl;

	vector<string> direction = {"up", "down", "left", "right"};
	stack<string> solution;
	stack<BoardState> board_change;
	// now current state is final state, so we need to backtrace
	while(currState != init_state){
		board_change.push(currState);
		BoardState prevState = path[currState];
		for(int i=1; i<=4; i++){
			pair<int,int> prevPlayer = prevState.player;
			updateCoordinate(prevPlayer, i);
			if(prevPlayer == currState.player){
				solution.push(direction[i-1]);
				break;
			}
		}
		currState = prevState;
	}
	cout << "# of movements: " << solution.size() << endl;
	while(!solution.empty()){
		auto i = solution.top();
		solution.pop();
		cout << i << " ";
	}
	cout << endl;

	return board_change;
}

void drawBoard(BoardState state){
	int col = board[0].size();
	int row = board.size();
	vector<vector<char>> blank_board;

	for(int i=0; i<row; i++){
		vector<char> line;
		for(int j=0; j<col; j++){
			line.push_back(' ');
		}
		blank_board.push_back(line);
	}
	
	for(auto loc : walls){
		blank_board[loc.first][loc.second] = '%';
	}

	for(auto loc : storages){
		blank_board[loc.first][loc.second] = '.';
	}

	for(auto loc : state.box){
		blank_board[loc.first][loc.second] = 'b';
	}

	blank_board[state.player.first][state.player.second] = 'P';

	for(int i=0; i<row; i++){
		for(int j=0; j<col; j++){
			cout << blank_board[i][j];
		}
		cout << endl;
	}
	cout << endl;
}

// test for min heap or max heap for set and priority queue
// class test_node {
// public:
// 	int a;
// };

// class test_comp{
// public:
// 	bool operator() (const test_node& n1, const test_node& n2) const
// 	{
		
// 		return n1.a > n2.a;
// 	}
// };


int main(void){


	// test_node n1, n2, n3;
	// n1.a = 3;
	// n2.a = 4;
	// n3.a = 5;
	// priority_queue<test_node, vector<test_node>, test_comp> list;

	
	// list.push(n2);
	// list.push(n3);
	// list.push(n1);

	// cout << (list.top()).a << endl;


	string filename;
	filename = "sokoban4.txt";

	vector<pair<int,int>> boxes;
	pair<int,int> player;

	board = read_maze(filename);

	cout << "row number: " << board.size() << endl;
	cout << "col number: " << board[3].size() << endl;

	storages = get_storage_locs(board);
	boxes = get_box_locs(board);
	walls = get_walls(board);

	cout << "Storage location: " << endl;
	for(auto each : storages){
		cout << "(" << each.first << "," << each.second << "), ";
		cout << endl;
	}
	cout << "Box location: " << endl;
	for(auto each : boxes){
		cout << "(" << each.first << "," << each.second << "), ";
		cout << endl;
	}

	player = get_player(board);
	cout << "Player loc: " << endl;
	cout << "(" << player.first << "," << player.second << "), ";
	cout << endl;

	BoardState init_state;
	init_state.box = boxes;
	init_state.player = player;

	// drawBoard(init_state);
	stack<BoardState> states = A_Star_Search(init_state);
	cout << "begin to draw solution" << endl;
	while(!states.empty()){
		BoardState curr = states.top();
		states.pop();
		drawBoard(curr);
		// for(int i=0; i<10000000; i++);
		sleep(1);
	}

	return 0;
}




