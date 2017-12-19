#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <map>

using namespace std;

vector<vector<int>> getManhattanDistance(vector<vector<char>> maze, pair<int, int> goal);

class Node
{
public:
	int f;
	pair<int, int> coordinate;
	Node *parent;
	int step;
	int h;

	Node(int h, pair<int, int> coordinate, Node *parent, int step){
		this->h = h;
		this->coordinate = coordinate;
		this->parent = parent;
		this->step = step;
		this->f = step + h;
	};
};

class Comparator
{ // used for set to maintain min heap
public:
    bool operator()(const Node* s1, const Node* s2) const
    {
        return s1->f <= s2->f;
    }
};

// read maze
vector<vector<char>> getMaze(string filename){
	ifstream infile;
	infile.open(filename, ios::binary);
	char c;
	vector<char> line;
	vector<vector<char>> maze;

	while(!infile.eof()){

		infile >> noskipws >> c;
		
		if(c == '%' || c == ' ' || c == 'P' || c == '.'){
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

// get start location
pair<int, int> getStart(vector<vector<char>> maze){
	size_t row = maze.size();
	size_t col = maze[0].size();
	pair<int, int> start;
	for(size_t i=0; i<row; i++){
		for(size_t j=0; j<col; j++){
			if(maze[i][j] == 'P'){
				start = make_pair(i, j);
			}
		}
	}
	return start;
}

// get location of goals
vector<pair<int, int>> getDots(vector<vector<char>> maze){
	size_t row = maze.size();
	size_t col = maze[0].size();
	vector<pair<int, int>> dots;
	for(size_t i=0; i<row; i++){
		for(size_t j=0; j<col; j++){
			if(maze[i][j] == '.'){
				dots.push_back(make_pair(i, j));
			}
		}
	}
	return dots;
}


// find neighbors used for greedy
vector<Node*> findNeighborsGreedy(vector<vector<char>> maze, vector<vector<int>> distance, Node *parent, set<pair<int, int>> visited){
	int row = (parent->coordinate).first;
	int col = (parent->coordinate).second;
	// int step = parent->step + 1; // step from start to now
	vector<Node*> neighbors;

	if(visited.find(make_pair(row-1, col)) == visited.end() && row-1 >= 0 && maze[row-1][col] != '%'){
		Node *newNode = new Node(distance[row-1][col], make_pair(row-1, col), parent, 0);
		neighbors.push_back(newNode); 
	} //up

	if(visited.find(make_pair(row+1, col)) == visited.end() && row+1 < (int)maze.size() && maze[row+1][col] != '%'){
		Node *newNode = new Node(distance[row+1][col], make_pair(row+1, col), parent, 0);
		neighbors.push_back(newNode);
	} //down

	if(visited.find(make_pair(row, col-1)) == visited.end() && col-1 > 0 && maze[row][col-1] != '%'){
		Node *newNode = new Node(distance[row][col-1], make_pair(row, col-1), parent, 0);
		neighbors.push_back(newNode);
	} //left

	if(visited.find(make_pair(row, col+1)) == visited.end() && col+1 < (int)maze[0].size() && maze[row][col+1] != '%'){
		Node *newNode = new Node(distance[row][col+1], make_pair(row, col+1), parent, 0);
		neighbors.push_back(newNode);
	} //right
	return neighbors;
}

// greedy best first search
stack<pair<int,int>> gbfSearch(vector<vector<char>> maze, vector<vector<int>> distance, pair<int,int> start, pair<int,int> goal, int &n){
	set<Node*, Comparator> min_heap;
	Node *curNode = new Node(distance[start.first][start.second], start, NULL, 0);
	pair<int, int> curPos = start;
	stack<pair<int, int>> path; // path from start to goal
	set<pair<int, int>> visited; 
	n = 0;
	min_heap.insert(curNode);
	visited.insert(start);

	while(curPos != goal){
		n++;
		curNode = *(min_heap.begin());
		curPos = curNode->coordinate;
		min_heap.erase(min_heap.begin()); // remove it from min heap
		vector<Node*> neighbors = findNeighborsGreedy(maze, distance, curNode, visited);

		for(auto i : neighbors){
			min_heap.insert(i);
			visited.insert(curNode->coordinate);
		}
	}
	cout << "Complete Greedy Search.\n";

	while(curNode->parent != NULL){
		path.push(curNode->coordinate);
		curNode = curNode->parent;
	}
	return path;
}

// used for a star search and other basic path finding algorithm
vector<Node*> findNeighbors(vector<vector<char>> maze, vector<vector<int>> distance, Node *parent){
	int row = (parent->coordinate).first;
	int col = (parent->coordinate).second;
	int step = parent->step + 1; // step from start to now
	vector<Node*> neighbors;
	if(row-1 >= 0 && maze[row-1][col] != '%'){
		Node *newNode = new Node(distance[row-1][col]+step, make_pair(row-1, col), parent, step);
		neighbors.push_back(newNode); 
	} //up

	if(row+1 < (int)maze.size() && maze[row+1][col] != '%'){
		Node *newNode = new Node(distance[row+1][col] + step, make_pair(row+1, col), parent, step);
		neighbors.push_back(newNode);
	} //down

	if(col-1 > 0 && maze[row][col-1] != '%'){
		Node *newNode = new Node(distance[row][col-1]+step, make_pair(row, col-1), parent, step);
		neighbors.push_back(newNode);
	} //left

	if(col+1 < (int)maze[0].size() && maze[row][col+1] != '%'){
		Node *newNode = new Node(distance[row][col+1]+step, make_pair(row, col+1), parent, step);
		neighbors.push_back(newNode);
	} //right
	return neighbors;
}

int calculateManhattanDistance(pair<int,int> x1, pair<int,int> x2){
	return abs(x1.first-x2.first) + abs(x1.second-x2.second);
}


// A star search
stack<pair<int,int>> A_star_search(vector<vector<char>> maze, vector<vector<int>> distance, pair<int, int> start, pair<int, int> goal, int &n){
	n = 0;
	set<Node*, Comparator> openSet;

	Node *curNode = new Node(distance[start.first][start.second], start, NULL, 0);
	pair<int, int> curPos = start;
	stack<pair<int, int>> path; // path from start to goal

	openSet.insert(curNode);
	map<pair<int,int>, int> dist;
	dist[start] = 0;

	while(curPos != goal){
		n++;
		curNode = *(openSet.begin());
		curPos = curNode->coordinate;
		openSet.erase(openSet.begin()); // remove it from min heap

		if(curNode->step > dist[curNode->coordinate]){
			continue;
		}

		// update weight and return neighbors
		vector<Node*> neighbors = findNeighbors(maze, distance, curNode);

		for(auto i : neighbors){
			
			if(dist.find(i->coordinate) == dist.end() || dist[i->coordinate] > i->step){
				openSet.insert(i);
				dist[i->coordinate] = i->step;
			}
		}
	}
	cout << "Complete A star Search.\n";

	while(curNode->parent != NULL){
		path.push(curNode->coordinate);
		curNode = curNode->parent;
	}
	return path;
}


vector<stack<pair<int,int>>> A_star_search_multiple_suboptimal(vector<vector<char>> maze, pair<int, int> start, vector<pair<int, int>> goals, int &n){
	vector<stack<pair<int,int>>> total_path;
	n = 0;
	pair<int, int> curPos = start;
	while(goals.size() > 0){
		stack<pair<int,int>> path;
		pair<int, int> goal;
		// find the nearest goal based on current position
		if(goals.size() > 1){
			int minDistance = 10000;
			auto i = goals.begin();
			auto j = goals.begin();
			while(i != goals.end()){
				int manhattanDistance = calculateManhattanDistance(curPos, *i);
				if(manhattanDistance <= minDistance){
					// update new shortest goal
					j = i;
					goal = *i;
					minDistance = manhattanDistance;
				}
				i++;
			}
			goals.erase(j);
		}else{ // only one goal left
			goal = goals.front();
			goals.clear();
		}
		int n_expanded = 0;
		vector<vector<int>> distance;
		distance = getManhattanDistance(maze, goal);
		path = A_star_search(maze, distance, curPos, goal, n_expanded);
		n += n_expanded;
		total_path.push_back(path);
		curPos = goal;
	}
	return total_path;
}


stack<pair<int,int>> Search_bfs(vector<vector<char>> maze, pair<int, int> start, pair<int, int> goal, vector<vector<int>> distance, int &n){
	queue<Node*> q;
	n = 0;
	Node *newNode = new Node(0, start, NULL, 0);
	q.push(newNode);
	set<pair<int, int>> visited;
	visited.insert(newNode->coordinate);
	Node *curNode;
	pair<int, int> curPos = start;
	stack<pair<int, int>> path;

	while(curPos != goal){
		n++;
		curNode = q.front();
		q.pop();
		curPos = curNode->coordinate;
		// visited.insert(curPos);
		vector<Node*> neighbors = findNeighbors(maze, distance, curNode);
		for(auto i : neighbors){
			if(visited.find(i->coordinate) == visited.end()){
				q.push(i);
				visited.insert(i->coordinate);
			}
		}
	}
	
	cout << "Complete BFS Search.\n";

	while(curNode->parent != NULL){
		path.push(curNode->coordinate);
		curNode = curNode->parent;
	}
	return path;
}

stack<pair<int,int>> Search_dfs(vector<vector<char>> maze, pair<int, int> start, pair<int, int> goal, vector<vector<int>> distance, int &n){
	stack<Node*> s;
	n = 0;
	Node *head = new Node(0, start, NULL, 0);
	s.push(head);
	Node *curNode;
	pair<int, int> curPos = start;
	stack<pair<int, int>> path;
	set<pair<int, int>> visited;
	visited.insert(head->coordinate);

	while(curPos != goal){
		n++;
		curNode = s.top();
		s.pop();
		curPos = curNode->coordinate;
		// visited.insert(curPos);
		vector<Node*> neighbors = findNeighbors(maze, distance, curNode);
		for(auto i : neighbors){
			if(visited.find(i->coordinate) == visited.end()){
				s.push(i);
				visited.insert(i->coordinate);
			}
		}
	}

	cout << "Complete DFS Search.\n";

	while(curNode->parent != NULL){
		path.push(curNode->coordinate);
		curNode = curNode->parent;
	}
	return path;
}


vector<vector<int>> getManhattanDistance(vector<vector<char>> maze, pair<int, int> goal){
	vector<vector<int>> distance;
	vector<int> line;
	for(int row=0; row<(int)maze.size(); row++){
		line.clear();
		for(int col=0; col<(int)maze[0].size()-1; col++){
			
			if(maze[row][col] != '%'){
				line.push_back(abs(row - goal.first) + abs(col - goal.second));
			}else{
				line.push_back(-1);
			}
		}
		distance.push_back(line);
	}
	return distance;
}


void printMaze(stack<pair<int,int>> path, vector<vector<char>> maze, string filename, string method, int n){
	ofstream f;
	string output_file = "./outputs/" + filename + "_" + method + ".txt";
	f.open(output_file);
	f << "step size: " << path.size() << endl;
	f << "# of expanded nodes: " << n << endl;

	while(!path.empty()){
		pair<int,int> loc = path.top();
		path.pop();
		f << "(" << loc.first << "," << loc.second << "), ";
		maze[loc.first][loc.second] = '.';
	}
	f << endl;

	// draw solution on maze
	for(size_t i=0; i<maze.size(); i++){
		for(size_t j=0; j<maze[0].size(); j++){
			f << maze[i][j];
		}
		f << endl;
	}
	f.close();
	return;
}

void printMutipleMaze(vector<stack<pair<int,int>>> path, vector<vector<char>> maze, string filename, string method, int n){
	ofstream f;
	string output_file = "./outputs/" + filename + "_" + method + ".txt";
	f.open(output_file);
	int step = 0;

	for(size_t i=0; i<path.size(); i++){
		step += path[i].size();
	}

	f << "step size: " << step << endl;
	f << "# of expanded nodes: " << n << endl;
	string seq = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int idx = 0;
	for(size_t i=0; i<path.size(); i++){
		stack<pair<int,int>> p = path[i];
		while(!p.empty()){
			pair<int,int> curPos = p.top();
			p.pop();
			maze[curPos.first][curPos.second] = seq[idx];
			idx++;
			if(idx >= 62){
				idx = 0;
			}
		}
	}
	// draw solution on maze
	for(size_t i=0; i<maze.size(); i++){
		for(size_t j=0; j<maze[0].size(); j++){
			f << maze[i][j];
		}
		f << endl;
	}
	f.close();
	return;
}


int main(void){

	const string mazes[3] = {"mediumMaze", "bigMaze", "openMaze"};

	const string search_method[5] = {"bfs", "dfs", "gbfs", "ass", "suboptimal"};

	vector<vector<char>> maze;
	vector<pair<int, int>> dots_coord;
	pair<int, int> start;
	vector<vector<int>> manhattanDistance;

	for(auto each : mazes){
		int n = 0;
		maze = getMaze(each + ".txt");
		dots_coord = getDots(maze);
		start = getStart(maze);
		manhattanDistance = getManhattanDistance(maze, dots_coord[0]);
		stack<pair<int,int>> path;

		path = Search_bfs(maze, start, dots_coord[0], manhattanDistance, n);
		printMaze(path, maze, each, search_method[0], n);

		path = Search_dfs(maze, start, dots_coord[0], manhattanDistance, n);
		printMaze(path, maze, each, search_method[1], n);
		path = gbfSearch(maze, manhattanDistance, start, dots_coord[0], n);
		printMaze(path, maze, each, search_method[2], n);
		path = A_star_search(maze, manhattanDistance, start, dots_coord[0], n);
		printMaze(path, maze, each, search_method[3], n);
	}

	cout << "begin to search bigdots" << endl;

	int n = 0;
	maze = getMaze("bigDots.txt");
	cout << maze[0].size() << endl;
	dots_coord = getDots(maze);
	start = getStart(maze);
	vector<stack<pair<int,int>>> multiple_path;
	multiple_path = A_star_search_multiple_suboptimal(maze, start, dots_coord, n);
	printMutipleMaze(multiple_path, maze, "bigDots.txt", search_method[4], n);

	return 0;
}



