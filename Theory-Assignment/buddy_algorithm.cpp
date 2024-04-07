#include <stdio.h>
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int max_power = 10, min_power = 4, number_of_diff_sizes, total_allocated = 0;
bool sort_cmp(const pair<int, int> &a, const pair<int, int> &b)
{
    return a.first < b.first;
}
void initialize_buddy_free_list(vector<vector<pair<int, int>>> &buddy_free_list, int min_power, int max_power)
{
    number_of_diff_sizes = max_power - min_power + 1;
    for (int i = 0; i < number_of_diff_sizes; i++)
    {
        buddy_free_list.push_back(vector<pair<int, int>>());
    }
    buddy_free_list[buddy_free_list.size() - 1] = {{0, int(pow(2, max_power) - 1)}};
}

void allocate(vector<vector<pair<int, int>>> &buddy_free_list, unordered_map<string, vector<vector<int>>> &process_allocation_mapping, unordered_map<int, int> &global_mapping, string process_name, int req_memory)
{
    int index = ceil(log(req_memory) / log(2));
    
    if (index < min_power)
    {
        index = min_power;
    }

    int real_index = index - min_power;
    index = real_index;

    int flag = 0;

    for (; index < number_of_diff_sizes; index++)
    {
        if (buddy_free_list[index].size() > 0)
        {
            flag = 1;
            break;
        }
    }
    if (!flag)
    {
        cout << "could not allocate memory" << endl;
        return;
    }

    pair<int, int> pair_found = buddy_free_list[index][0];
    auto it = buddy_free_list[index].begin();
    buddy_free_list[index].erase(it);

    int l_, r_;

    for (index--; index >= real_index; index--)
    {
        l_ = pair_found.first;
        r_ = (pair_found.second - pair_found.first) / 2 + pair_found.first;

        buddy_free_list[index].push_back(make_pair(l_, r_));

        l_ = pair_found.second - (pair_found.second - pair_found.first) / 2;
        r_ = pair_found.second;

        buddy_free_list[index].push_back(make_pair(l_, r_));

        pair_found = buddy_free_list[index][0];
        auto it = buddy_free_list[index].begin();
        buddy_free_list[index].erase(it);
    }
    vector<int> to_push = {pair_found.first, pair_found.second - pair_found.first + 1};
    process_allocation_mapping[process_name].push_back({pair_found.first, req_memory});
    global_mapping[to_push[0]] = to_push[1];
}

void deallocate(vector<vector<pair<int, int>>> &buddy_free_list, unordered_map<string, vector<vector<int>>> &process_allocation_mapping, unordered_map<int, int> &global_mapping, string process_name, int start_address)
{

    int real_index = log(global_mapping[start_address]) / log(2);
    int index = real_index - min_power;
    int block_size = global_mapping[start_address];
    buddy_free_list[index].push_back(make_pair(start_address, start_address + block_size - 1));
    int block_number = start_address / block_size;
    int break_flag = 0;

    while (index < number_of_diff_sizes)
    {
        int adj_address = block_number % 2 ? start_address - block_size : start_address + block_size;
        for (int i = 0; i < buddy_free_list[index].size(); i++)
        {
            if (buddy_free_list[index][i].first != adj_address)
            {
                break_flag = 0;
            }
            else
            {
                break_flag = 1;
                if (block_number % 2 != 0)
                {
                    buddy_free_list[index + 1].push_back(make_pair(adj_address, adj_address + (block_size * 2) - 1));
                }
                else
                {
                    buddy_free_list[index + 1].push_back(make_pair(adj_address - block_size, adj_address + block_size - 1));
                }
                auto it = buddy_free_list[index].begin() + i;
                buddy_free_list[index].erase(it);
                for (auto &vec : buddy_free_list)
                {
                    sort(vec.begin(), vec.end(), sort_cmp);
                }
                it = buddy_free_list[index].begin() + i;
                buddy_free_list[index].erase(it);

                break;
            }
        }
        index++;
        block_size = 2 * block_size;
        block_number = start_address / block_size;
        if (!break_flag)
            break;
    }
    for (auto &vec : buddy_free_list)
    {
        sort(vec.begin(), vec.end(), sort_cmp);
    }
    
}

int main(int argc, char *argv[])
{
    ifstream input_file(argv[1]);

    int tests;
    input_file >> tests;

    while (tests--)
    {
        string newline;
        if (newline != "")
        {
            getline(input_file, newline);
        }

        input_file >> max_power >> min_power;

        vector<vector<pair<int, int>>> buddy_free_list;

        initialize_buddy_free_list(buddy_free_list, min_power, max_power);

        unordered_map<string, vector<vector<int>>> process_allocation_mapping;
        unordered_map<int, int> global_mapping;
        map<int, pair<string, int>> to_print_list;

        getline(input_file, newline);
        getline(input_file, newline);
        if (newline == "")
        {
            ;
        }

        while (true)
        {
            string process_name;
            int req_memory;
            getline(input_file, newline);

            if (newline == "")
            {
                break;
            }
            
            istringstream ss_line(newline);
            ss_line >> process_name;
            ss_line >> req_memory;
            if (req_memory != 0)
            {
                total_allocated++;
                allocate(buddy_free_list, process_allocation_mapping, global_mapping, process_name, req_memory);
            }
            else
            {
                total_allocated--;
                for (auto &i : process_allocation_mapping[process_name])
                {

                    deallocate(buddy_free_list, process_allocation_mapping, global_mapping, process_name, i[0]);
                }
                auto it = process_allocation_mapping.find(process_name);
                process_allocation_mapping.erase(it);
            }
            if (input_file.eof())
            {
                break;
            }
        }

        for (int i = 0; i < buddy_free_list.size(); i++)
        {
            for (int j = 0; j < buddy_free_list[i].size(); j++)
            {
                to_print_list[buddy_free_list[i][j].first] = {"FREE", buddy_free_list[i][j].second - buddy_free_list[i][j].first + 1};
            }
        }
        
        for (const auto &pair : process_allocation_mapping)
        {
            vector<vector<int>> vec = pair.second;
            for (const auto &elem : vec)
            {

                to_print_list[elem[0]] = {pair.first, elem[1]};
            }
        }
        for (const auto &pair : to_print_list)
        {
            if (pair.second.first == "FREE")
            {
                cout << "Free Block: ";
            }
            else
            {
                cout << pair.second.first << ": ";
            }
            cout << pair.second.second;
            cout << endl;
        }
        
        if (input_file.eof())
        {
            break;
        }
        else
        {
            cout << endl;
        }
    }

    return 0;
}
