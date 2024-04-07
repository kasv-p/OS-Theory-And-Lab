#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>
using namespace std;
using std::ifstream;
int current_time = 0;
int time_slice = 3;
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
    ready_procs.clear();
    std::copy(below_cur_time.begin(), below_cur_time.end(), std::back_inserter(ready_procs));
    std::copy(after_cur_time.begin(), after_cur_time.end(), std::back_inserter(ready_procs));
}
bool sort_priority(const vector<int> &v1, const vector<int> &v2)
{
    return (v1[1] == v2[1] && v1[4] > v2[4]);
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
    vector<vector<int>> input_array;
    map<int, vector<vector<int>>> process_data;

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
        temp = {i, input_array[i][0], input_array[i][1], 2, 1, time_slice, time_slice};
        burst_times[i] = cpuBurstTime(input_array[i]);
        cpu_queue.push_back(temp);
    }

    input_file.close();
    filter_time(cpu_queue);
    sort(cpu_queue.begin(), cpu_queue.end(), sort_priority);

    int exit_count = 0;
    // 0 - index, 1 - arrival time, 2 - burst time, 3 - step size to check all bursts till now are done are not, 4 - to tell new bit or not
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
                        cout << 1 + cpu_running[0] << " "
                             << "current time " << current_time << " "
                             << "exited"
                             << "\n";
                        exit_count += 1;
                    }
                    else
                    {
                        vector<int> new_io;
                        new_io = {cpu_running[0], current_time, input_array[cpu_running[0]][cpu_running[3]], cpu_running[3] + 1, 1, time_slice};
                        io_queue.push_back(new_io);
                    }
                    cpu_queue.erase(cpu_queue.begin());
                    filter_time(cpu_queue);
                    sort(cpu_queue.begin(), cpu_queue.end(), sort_priority);
                    if (cpu_queue.size() > 0)
                    {
                        if (response_time.find(cpu_queue[0][0]) == response_time.end())
                        {

                            response_time[cpu_queue[0][0]] = current_time - input_array[cpu_queue[0][0]][0];
                        }
                        cpu_queue[0][2] -= 1;
                        cpu_queue[0][5] -= 1;
                    }
                }
                else if (cpu_running[2] != 0 && cpu_running[5] == 0)
                {

                    vector<int> new_cpu;
                    new_cpu = {cpu_running[0], current_time, cpu_running[2], cpu_running[3], 0, time_slice};
                    cpu_queue.push_back(new_cpu);
                    cpu_queue.erase(cpu_queue.begin());
                    filter_time(cpu_queue);
                    sort(cpu_queue.begin(), cpu_queue.end(), sort_priority);
                    if (cpu_queue.size() > 0)
                    {
                        if (response_time.find(cpu_queue[0][0]) == response_time.end())
                        {
                            response_time[cpu_queue[0][0]] = current_time - input_array[cpu_queue[0][0]][0];
                        }
                        cpu_queue[0][2] -= 1;
                        cpu_queue[0][5] -= 1;
                    }
                }
                else
                {
                    cpu_queue[0][2] -= 1;
                    cpu_queue[0][5] -= 1;
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
                    vector<int> new_cpu;
                    new_cpu = {io_running[0], current_time, input_array[io_running[0]][io_running[3]], io_running[3] + 1, 1, time_slice};
                    cpu_queue.push_back(new_cpu);
                    if (cpu_queue.size() == 1)
                    {
                        if (response_time.find(cpu_queue[0][0]) == response_time.end())
                        {
                            response_time[cpu_queue[0][0]] = current_time - input_array[cpu_queue[0][0]][0];
                        }
                        cpu_queue[0][2] -= 1;
                        cpu_queue[0][5] -= 1;
                    }
                    io_queue.erase(io_queue.begin());
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
        int responseTime = response_time[i];
        int turnAroundTime = exit_timing[i] - input_array[i][0];
        int comepletionTime = exit_timing[i];
        int arrivalTime = input_array[i][0];
        int cpuBurst = burst_times[i];
        int waitingTime = turnAroundTime - cpuBurst;
        int totalBurstTime = sumBurstTime(input_array[i]);
        float penaltyRatio = (turnAroundTime)*1.0 / (cpuBurst);
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
