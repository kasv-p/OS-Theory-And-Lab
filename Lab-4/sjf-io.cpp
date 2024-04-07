#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <map>
using namespace std;
using std::ifstream;
int current_time = 0;
vector<vector<int>> input_array;
bool sort_order(const vector<int> &v1, const vector<int> &v2)
{

  return (v1[2] < v2[2]);
}
bool sort_equal(const vector<int> &v1, const vector<int> &v2)
{

  return ((v1[2] == v2[2]) && (input_array[v1[0]][0] < input_array[v2[0]][0]));
}

void filter_time(vector<vector<int>> &ready_procs)
{

  vector<vector<int>> below_cur_time;
  vector<vector<int>> after_cur_time;
  for (int i = 0; i < ready_procs.size(); i++)
  {
    if (ready_procs[i][1] > current_time)
    {

      after_cur_time.push_back(ready_procs[i]);
    }
    else
    {
      below_cur_time.push_back(ready_procs[i]);
    }
  }
  sort(below_cur_time.begin(), below_cur_time.end(), sort_order);
  sort(below_cur_time.begin(), below_cur_time.end(), sort_equal);
  sort(after_cur_time.begin(), after_cur_time.end(), sort_order);
  sort(after_cur_time.begin(), after_cur_time.end(), sort_equal);
  ready_procs.clear();
  copy(below_cur_time.begin(), below_cur_time.end(), back_inserter(ready_procs));
  copy(after_cur_time.begin(), after_cur_time.end(), back_inserter(ready_procs));
}

int sumBurstTime(vector<int> vec)
{
  int sum = 0;
  for (int i = 1; i < vec.size() - 1; i++)
  {
    sum += vec[i];
  }

  return sum;
}
int cpuBurstTime(vector<int> vec)
{
  int sum = 0;
  for (int i = 1; i < vec.size(); i += 2)
  {
    sum += vec[i];
  }
  return sum;
}
int main(int argc, char **argv)
{

  ifstream input_file;

  input_file.open(argv[1]);
  string s;

  vector<int> temp;
  int line_count = 0;
  while (getline(input_file, s))
  {
    line_count += 1;
    if (line_count <= 3)
    {
      continue;
    }
    if (s == "</pre></body></html>")
    {
      break;
    }
    stringstream ss(s);
    string val;
    while (ss >> val)
    {
      if (val == "-1")
      {
        temp.push_back(stoi(val));
        input_array.push_back(temp);
        temp.clear();
      }
      else
      {
        temp.push_back(stoi(val));
      }
    }
  }

  vector<vector<int>> cpu_queue;
  vector<vector<int>> io_queue;
  map<int, int> exit_timing;
  map<int, int> burst_times;
  map<int, int> response_time;
  for (int i = 0; i < input_array.size(); i++)
  {
    temp = {i, input_array[i][0], input_array[i][1], 2, sumBurstTime(input_array[i])};
    burst_times[i] = cpuBurstTime(input_array[i]);
    cpu_queue.push_back(temp);
  }
  input_file.close();
  // sort(cpu_queue.begin(), cpu_queue.end(), sort_order);
  filter_time(cpu_queue);
  int exit_count = 0;

  while (exit_count != input_array.size())
  {
    if (cpu_queue.size() > 0)
    {
      vector<int> cpu_running = cpu_queue.front();

      if (cpu_running[1] <= current_time)
      {
        if (response_time.find(cpu_running[0]) == response_time.end())
        {
          response_time[cpu_running[0]] = current_time - input_array[cpu_running[0]][0];
        }
        if (cpu_running[2] == 0)
        {
          if (input_array[cpu_running[0]][cpu_running[3]] == -1)
          {
            exit_timing[cpu_running[0]] = current_time;
            cout << cpu_running[0] + 1 << " "
                 << "exited"
                 << " current time " << current_time
                 << "\n";
            exit_count += 1;
          }
          else
          {
            vector<int> new_io = {cpu_running[0], current_time, input_array[cpu_running[0]][cpu_running[3]], cpu_running[3] + 1, cpu_running[4] - input_array[cpu_running[0]][cpu_running[3] - 1]};
            io_queue.push_back(new_io);
          }

          cpu_queue.erase(cpu_queue.begin());
          // sort(cpu_queue.begin(), cpu_queue.end(), sort_order);
          filter_time(cpu_queue);
          if (cpu_queue.size() > 0 && cpu_queue[0][1] <= current_time)
          {
            if (response_time.find(cpu_running[0]) == response_time.end())
            {
              response_time[cpu_queue[0][0]] = current_time - input_array[cpu_queue[0][0]][0];
            }
            cpu_queue[0][2] -= 1;
          }
        }
        else
        {
          cpu_queue[0][2] -= 1;
        }
      }
    }
    if (io_queue.size() > 0)
    {
      vector<int> io_running = io_queue.front();
      if (io_running[1] <= current_time)
      {
        if (io_running[2] == 0)
        {
          vector<int> new_cpu = {io_running[0], current_time, input_array[io_running[0]][io_running[3]], io_running[3] + 1, io_running[4] - input_array[io_running[0]][io_running[3] - 1]};
          cpu_queue.push_back(new_cpu);
          if (cpu_queue.size() == 1)
          {
            if (response_time.find(cpu_queue[0][0]) == response_time.end())
            {
              response_time[cpu_queue[0][0]] = current_time - input_array[cpu_queue[0][0]][0];
            }
            cpu_queue[0][2] -= 1;
          }
          io_queue.erase(io_queue.begin());
          if (io_queue.size() > 0)
          {
            io_queue[0][2] -= 1;
          }
        }
        else
        {
          io_queue[0][2] -= 1;
        }
      }
    }
    current_time += 1;
  }

  float avgResTime = 0;
  float avgTurnAroundTime = 0;
  float avgWaitingTime = 0;
  float avgPenaltyRatio = 0;
  cout << "\n\n\n";
  for (int i = 0; i < input_array.size(); i++)
  {
    int processId = i;

    int turnAroundTime = exit_timing[i] - input_array[i][0];
    int comepletionTime = exit_timing[i];
    int arrivalTime = input_array[i][0];
    int cpuBurst = burst_times[i];
    int waitingTime = turnAroundTime - cpuBurst;
    int responseTime = min(waitingTime, response_time[i]);
    int totalBurstTime = sumBurstTime(input_array[i]);
    float penaltyRatio = (waitingTime + totalBurstTime) * 1.0 / (cpuBurst);
    avgResTime += responseTime;
    avgTurnAroundTime += turnAroundTime;
    avgWaitingTime += waitingTime;
    avgPenaltyRatio += penaltyRatio;
    cout << "Process ID " << processId + 1 << " Response Time " << responseTime << " Waiting Time " << waitingTime << " Turn Around Time " << turnAroundTime << " Completion Time " << comepletionTime << " Arrival Time " << arrivalTime << " Cpu Burst Time " << cpuBurst << " Total Burst Time " << totalBurstTime << " Penalty Ratio " << penaltyRatio << endl;
  }
  cout << "\n\n\n";
  avgResTime /= (input_array.size());
  avgTurnAroundTime /= (input_array.size());
  avgWaitingTime /= (input_array.size());
  avgPenaltyRatio /= (input_array.size());
  cout << "Average Response Time " << avgResTime << " Average Turn Around Time " << avgTurnAroundTime << " Average Waiting Time " << avgWaitingTime << " Average Penalty Ratio " << avgPenaltyRatio << endl;
  cout << "\n\nThroughtput = " << input_array.size() * 1.0 / (current_time - 1) << "\n\n";
  return 0;
}
