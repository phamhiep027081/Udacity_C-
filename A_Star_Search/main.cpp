#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

enum class State {kEmpty, kObstacle, kClosed, kPath, kStart, kFinish};
const int neighbor[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

vector<State> ParseLine(string line){
  istringstream sline(line);
  int n;
  char c;
  vector<State> row{};
  while(sline >> n >> c && c == ','){
    if(n == 0){
      row.push_back(State::kEmpty);
    }else{
      row.push_back(State::kObstacle);
    }
  }
  return row;
}

vector<vector<State>> ReadingBoardFile(string path){
  ifstream my_file(path);

  vector<vector<State>> grid{};
  if(my_file){
    string line;
    vector<State> row;
    while(getline(my_file, line)){
      row = ParseLine(line);
      grid.push_back(row);
    }
  }
  return grid;
}

/**
 * Compare the F values of two cells.
 */
bool Compare(const vector<int> a, const vector<int> b){
  int f1 = a[2] + a[3]; //format: vector<int> v = {x, y, g, h}
  int f2 = b[2] + b[3];

  return f1 > f2;
}

void CellSort(vector<vector<int>> *v){
  sort(v->begin(), v->end(), Compare);
}

/** 
 * Calculate distance with two cell. 
 */
int Distance_Heuristic(int x1, int y1, int x2, int y2){
  return abs(x1 - x2) + abs(y1 - y2);
}
/** 
 * Check that a cell is valid: on the grid, not an obstacle, and clear. 
 */
bool CheckValidCell(int x, int y, vector<vector<State>> &grid){
  bool x_valid = (x >= 0 && x < grid.size());
  bool y_valid = (y >= 0 && y < grid[0].size());
  if(x_valid && y_valid)
    return grid[x][y] == State::kEmpty;
  
  return false;
}

/** 
 * Add a node to the open list and mark it as open. 
 */
void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &open, vector<vector<State>> &grid){

  open.push_back(vector<int>{x, y, g, h});
  grid[x][y] = State::kClosed;
}

void ExpandNeighbors(const vector<int> &curr, int goal[2], vector<vector<int>> &open, vector<vector<State>> &grid){
  /*Get information of current node*/
  int x = curr[0];
  int y = curr[1];
  int g = curr[2];

  /*Loop through current node's potential neighbors.*/
  for(int i = 0; i < 4; i++){
    int x2 = x + neighbor[i][0];
    int y2 = y + neighbor[i][1];

    if(CheckValidCell(x2, y2, grid)){
      int g2 = g + 1; /*Next node: +1*/
      int h2 = Distance_Heuristic(x2, y2, goal[0], goal[1]);

      AddToOpen(x2, y2, g2, h2, open, grid);
    }
  }
}
vector<vector<State>> Search(vector<vector<State>> &grid, int start[2], int goal[2]){
  /*Init open list: {x, y, g, h}*/
  vector<vector<int>> open = {};

  /*Get parameter of current node (start node)*/
  int x = start[0], y = start[1];
  int g = 0;  /*Distance from start node to current node. g = 0, because start node is current node*/
  int h = Distance_Heuristic(x, y, goal[0], goal[1]); /*Distance from current node to goal node*/

  /*Add current node to Open*/
  AddToOpen(x, y, g, h, open, grid);

  while(open.size() > 0){
    /*Get next node*/
    CellSort(&open);
    auto current = open.back();
    /*Remove from open*/
    open.pop_back();

    /*Coordinates of current node*/
    x = current[0];
    y = current[1];

    /*Assign cell in grid is path*/
    grid[x][y] = State::kPath;

    /*If current node is goal node => Return grid*/
    if(x == goal[0] && y == goal[1]){
      grid[start[0]][start[1]] = State::kStart;
      grid[x][y] = State::kFinish;
      return grid;
    }

    /*Expand to current node's neighbor*/
    ExpandNeighbors(current, goal, open, grid);
  }
  return vector<vector<State>> {};
}

string CellString(State cell) {
  switch(cell) {
    case State::kObstacle: return "X   ";
    case State::kPath: return "*   ";
    case State::kStart: return "S   ";
    case State::kFinish: return "G   ";
    default: return "0   "; 
  }
}

void PrintGrid(const vector<vector<State>> grid){
  for(int i = 0; i < grid.size(); i++){
    for(int j = 0; j < grid[i].size(); j++){
      cout << CellString(grid[i][j]);
    }
    cout << endl;
  }
}


int main(){
  
  vector<vector<State>> grid = ReadingBoardFile("board.txt");
  int start[2] = {0, 0};
  int goal[2] = {0, 4};

  PrintGrid(Search(grid, start, goal));

  return 0;
}
