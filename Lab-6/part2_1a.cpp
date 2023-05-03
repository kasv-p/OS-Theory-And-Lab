#include <stdio.h>
#include <bits/stdc++.h>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <cmath>
#include <chrono>
#include <ctime>
using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;
atomic_flag locked = ATOMIC_FLAG_INIT;
int done_till_now[2] = {0, 0};
bool flag = false;
void RGB_to_GRAY(vector<vector<vector<int>>> &data_arg, vector<int> &dim_arg)
{
    float weights[3] = {0.2989, 0.5870, 0.1140};
    for (int i = 0; i < dim_arg[1]; i++)
    {
        for (int j = 0; j < dim_arg[0]; j++)
        {
            while (atomic_flag_test_and_set(&locked))
                ;
            float temp = 0;
            for (int k = 0; k < 3; k++)
            {
                temp += (float)data_arg[i][j][k] * weights[k] * 1.0;
            }
            data_arg[i][j][0] = (int)temp;
            data_arg[i][j][1] = (int)temp;
            data_arg[i][j][2] = (int)temp;
            done_till_now[0] = i + 1;
            done_till_now[1] = j + 1;
            atomic_flag_clear(&locked);
        }
    }
}

void THRESHOLDING(vector<vector<vector<int>>> &data_arg, vector<int> &dim_arg)
{
    int threshold = 128;
    for (int i = 0; i < dim_arg[1]; i++)
    {
        for (int j = 0; j < dim_arg[0]; j++)
        {
            while (atomic_flag_test_and_set(&locked))
                ;
            if (i < done_till_now[0] && j < done_till_now[1])
            {
                for (int k = 0; k < 3; k++)
                {
                    data_arg[i][j][k] = data_arg[i][j][k] < threshold ? 0 : 255;
                }
                flag = true;
            }
            else
            {
                flag = false;
            }
            atomic_flag_clear(&locked);
            if (!flag)
            {
                j -= 1;
            }
        }
        if (!flag)
        {
            i -= 1;
        }
    }
}
int main(int argc, char **argv)
{
    fstream newfile;
    newfile.open(argv[1], ios::in);
    int max_cap;
    vector<int> dimensions(2);
    string type;
    time_t start, end;

    int lineCount = 0;
    if (newfile.is_open())
    {
        string line;
        newfile >> line;
        type = line;

        int width;
        newfile >> width;
        dimensions[0] = width;
        int height;
        newfile >> height;
        dimensions[1] = height;
        vector<vector<vector<int>>> data(dimensions[1], vector<vector<int>>(dimensions[0], vector<int>(3)));

        newfile >> max_cap;
        int f = 0;
        for (int i = 0; i < dimensions[1]; i++)
        {
            int pixel = 0;
            for (int j = 0; j < dimensions[0]; j++)
            {
                newfile >> pixel;
                data[i][j][0] = pixel;
                newfile >> pixel;
                data[i][j][1] = pixel;
                newfile >> pixel;
                data[i][j][2] = pixel;
            }
        }
        newfile.close();

        auto const start = std::chrono::system_clock::now();
        // unsync the I/O of C and C++.
        ios_base::sync_with_stdio(false);
        RGB_to_GRAY(std::ref(data), dimensions);
        THRESHOLDING(std::ref(data), dimensions);
        auto const end = std::chrono::system_clock::now();
        auto duration = duration_cast<microseconds>(end - start);

        cout << "Time taken to execute both the functions is " << duration.count()
             << " micro seconds " << endl;
        ofstream outputImage;

        outputImage.open(argv[2]);
        outputImage << type << endl;
        outputImage << dimensions[0] << " " << dimensions[1] << endl;
        outputImage << max_cap << endl;
        for (int i = 0; i < data.size(); i++)
        {
            for (int j = 0; j < data[i].size(); j++)
            {
                for (int k = 0; k < data[i][j].size(); k++)
                {
                    outputImage << data[i][j][k] << " ";
                }
            }
            outputImage << endl;
        }
    }

    return 0;
}