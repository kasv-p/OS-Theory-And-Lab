#include <stdio.h>
#include <bits/stdc++.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <chrono>
#include <ctime>

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

#define BUFFER_SIZE 3*sizeof(int)
#define THRESHOLD 128
int pid;
int transfer[3];
int pipeFileDescriptor[2];

void RGB_to_GRAY(vector<vector<vector<int>>> &data_arg, vector<int> &dim_arg)
{
    float weights[3] = {0.2989, 0.5870, 0.1140};

    close(pipeFileDescriptor[0]);
    for (int i = 0; i < dim_arg[1]; i++)
    {
        for (int j = 0; j < dim_arg[0]; j++)
        {
            float temp = 0;
            for (int k = 0; k < 3; k++)
            {
                temp += (float)data_arg[i][j][k] * weights[k] * 1.0;
            }

            for (int k = 0; k < 3; k++)
            {
                data_arg[i][j][k] = (int)temp;
                transfer[k] = data_arg[i][j][k];
            }
            write(pipeFileDescriptor[1], transfer, BUFFER_SIZE);
        }
    }
    close(pipeFileDescriptor[1]);
}

void THRESHOLDING(vector<vector<vector<int>>> &data_arg, vector<int> &dim_arg)
{

    close(pipeFileDescriptor[1]);

    for (int i = 0; i < dim_arg[1]; i++)
    {
        for (int j = 0; j < dim_arg[0]; j++)
        {
            read(pipeFileDescriptor[0], transfer, BUFFER_SIZE);
            for (int k = 0; k < 3; k++)
            {
                data_arg[i][j][k] = transfer[k] < THRESHOLD ? 0 : 255;
            }
        }
    }
    close(pipeFileDescriptor[0]);
}

int main(int argc, char **argv)
{

    fstream newfile;

    newfile.open(argv[1], ios::in);
    int max_cap;
    vector<int> dimensions(2);
    string type;

    int lineCount = 0;

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


    int pipeReturn;
    pipeReturn = pipe(pipeFileDescriptor);
    if (pipeReturn == -1)
    {
        perror("Failed pipe detected.\n");
    }

    pid = fork();

    auto const start = std::chrono::system_clock::now();

    if (pid == 0)
    {
        RGB_to_GRAY(data, dimensions);
    }

    else if (pid > 0)
    {
        THRESHOLDING(data, dimensions);
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
