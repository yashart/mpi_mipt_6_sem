//Equation: U_t - 20*U_X_X = 1/(1/h*1/tau - p - h)!
#include <stdlib.h>
#include <queue>
#include <utility>
#include <math.h>
#include <mutex>
#include <set>
#include <iostream>
#include <thread>


int threadNum = 0;

const int T_LENGTH = 100;
const int X_LENGTH = 100;

const double tau = 0.0001;
const double h = 0.1;

double grid[T_LENGTH][X_LENGTH] = {};
std::mutex taskMutex;
std::mutex solvedMutex;
std::queue<std::pair<int, int>> tasks = {};
std::set<std::pair<int, int>> tasksSet = {};
std::set<std::pair<int, int>> solved = {};
int isComplete = 0;

void grid_dump() {
  for (int i = 0; i < T_LENGTH; i++)
  {
    for (int j = 0; j < X_LENGTH; j++) {
      std::cout << grid[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

void initialisation_matrix() {
  for (int i = 0; i < T_LENGTH; i++) {
    grid[i][0] = rand() % 100/100.0;
    grid[i][X_LENGTH - 1] = rand() % 100/100.0;
    solved.insert(std::make_pair(i, 0));
    solved.insert(std::make_pair(i, X_LENGTH - 1));
  }
  for (int i = 0; i < X_LENGTH; i++) {
    grid[0][i] = rand() % 100/100.0;
    solved.insert(std::make_pair(0, i));
  }
}

void make_init_tasks() {
  for (int i = 1; i < X_LENGTH; i++) {
    tasks.push(std::make_pair(1, i));
    tasksSet.insert(std::make_pair(1, i));
  }
}

double calc_heterogeneity(int p, int m) {
  double nk = 1;
  for(int i = 2; i < T_LENGTH*X_LENGTH - p - m; i++){
    nk *= i;
  }
  return 1/nk;
}

void calc_one_task(std::pair<int, int> oneTask) {
  int p = oneTask.first;
  int m = oneTask.second;
  grid[p][m] = calc_heterogeneity(p-1, m) * tau +\
                grid[p-1][m] +\
                20 * tau * (grid[p-1][m-1] - 2 * grid[p-1][m] + grid[p-1][m+1])/(h*h);
  solvedMutex.lock();
  solved.insert(std::make_pair(p, m));
  if ((solved.find(std::make_pair(p, m-1)) != solved.end()) &&
      (solved.find(std::make_pair(p, m-2)) != solved.end()) &&
      p != T_LENGTH - 1 && (tasksSet.find(std::make_pair(p+1, m-1)) == tasksSet.end())){
        taskMutex.lock();
        tasks.push(std::make_pair(p+1, m-1));
        tasksSet.insert(std::make_pair(p+1, m-1));
        taskMutex.unlock();
  }
  if ((solved.find(std::make_pair(p, m-1)) != solved.end()) &&
      (solved.find(std::make_pair(p, m+1)) != solved.end()) &&
      p != T_LENGTH - 1 && (tasksSet.find(std::make_pair(p+1, m)) == tasksSet.end())){
        taskMutex.lock();
        tasks.push(std::make_pair(p+1, m));
        tasksSet.insert(std::make_pair(p+1, m));
        taskMutex.unlock();
  }
  if ((solved.find(std::make_pair(p, m+2)) != solved.end()) &&
      (solved.find(std::make_pair(p, m+1)) != solved.end()) &&
      p != T_LENGTH - 1 && (tasksSet.find(std::make_pair(p+1, m+1)) == tasksSet.end())){
        taskMutex.lock();
        tasks.push(std::make_pair(p+1, m+1));
        tasksSet.insert(std::make_pair(p+1, m+1));
        taskMutex.unlock();
  }

  solvedMutex.unlock();
}

void task_manager() {
  do {
    //grid_dump();
    taskMutex.lock();
    //std::cout << "tasks:" << tasks.front().first << " " << tasks.front().second << std::endl;
    //std::cout << "solved:" << solved.front().first << " " << solved.front().second << std::endl;


    if(tasks.empty()) {
      taskMutex.unlock();
      break;
    }
    std::pair<int, int> oneTask = tasks.front();
    tasks.pop();
    taskMutex.unlock();
    if(oneTask.first == T_LENGTH - 1 && oneTask.second == X_LENGTH - 1) {
      isComplete = 1;
    }
    calc_one_task(oneTask);
  }while (isComplete == 0);
  taskMutex.unlock();

}

void create_threads() {
  std::thread workers[threadNum];

  for (int k = 0; k < threadNum; k++) {
    workers[k] = std::thread(task_manager);
  }
  for (int k = 0; k < threadNum; k++) {
    workers[k].join();
  }
}

int main(int argc, char* argv[]) {
  threadNum = atoi(argv[1]);
  initialisation_matrix();
  make_init_tasks();
  create_threads();
  grid_dump();
}
