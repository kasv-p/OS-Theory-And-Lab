#include <stdio.h>
#include <bits/stdc++.h>
#include <thread>
#include <semaphore.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>
#include <fcntl.h>
using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;
sem_t BINARY_SEM;

int pid;
bool flag = false;
void RGB_to_GRAY(key_t sharedKey, key_t sh, vector<vector<vector<int>>> &data_arg, vector<int> &dim_arg)
{

    int shmID = shmget(sharedKey, dim_arg[0] * dim_arg[1] * 3 * sizeof(int), 0666 | IPC_CREAT);
    int *pixelData = (int *)shmat(shmID, NULL, 0);
    int shmID2 = shmget(sh, 2 * sizeof(int), 0666 | IPC_CREAT);
    int *done = (int *)shmat(shmID2, NULL, 0);

    float weights[3] = {0.2989, 0.5870, 0.1140};
    for (int i = 0; i < dim_arg[1]; i++)
    {
        for (int j = 0; j < dim_arg[0]; j++)
        {
            sem_wait(&BINARY_SEM);

            float temp = 0;

            for (int k = 0; k < 3; k++)
            {
                temp += (float)data_arg[i][j][k] * weights[k] * 1.0;
            }

            pixelData[i * dim_arg[0] * 3 + j * 3 + 0] = (int)temp;
            pixelData[i * dim_arg[0] * 3 + j * 3 + 1] = (int)temp;
            pixelData[i * dim_arg[0] * 3 + j * 3 + 2] = (int)temp;
            done[0] = i + 1;
            done[1] = j + 1;
            sem_post(&BINARY_SEM);
        }
    }
}

void THRESHOLDING(key_t sharedKey, key_t sh, vector<vector<vector<int>>> &data_arg, vector<int> &dim_arg)
{

    int shmID = shmget(sharedKey, sizeof(int) * 3 * dim_arg[0] * dim_arg[1], 0666 | IPC_CREAT);
    int *pixelData = (int *)shmat(shmID, NULL, 0);
    int shmID2 = shmget(sh, 2 * sizeof(int), 0666 | IPC_CREAT);
    int *done = (int *)shmat(shmID2, NULL, 0);

    int threshold = 128;
    for (int i = 0; i < dim_arg[1]; i++)
    {
        for (int j = 0; j < dim_arg[0]; j++)
        {
            sem_wait(&BINARY_SEM);
            if (i < done[0] && j < done[1])
            {
                for (int k = 0; k < 3; k++)
                {

                    data_arg[i][j][k] = pixelData[i * dim_arg[0] * 3 + j * 3 + k] < threshold ? 0 : 255;
                }

                flag = true;
            }
            else
            {
                flag = false;
            }

            sem_post(&BINARY_SEM);
            if (!flag)
            {
                j-=1;
            }
        }
        if(!flag)
        {
            i-=1;
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

        key_t sh2 = 0x11111;
        key_t sharedKey = 0x1884;

        int shmID = shmget(sharedKey, sizeof(int) * 3 * dimensions[0] * dimensions[1], 0666 | IPC_CREAT);
        int *pixelData = (int *)shmat(shmID, NULL, 0);

        int shmID2 = shmget(sh2, sizeof(int) * 2, 0666 | IPC_CREAT);
        int *done = (int *)shmat(shmID2, NULL, 0);

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

        ios_base::sync_with_stdio(false);
        sem_init(&BINARY_SEM, 0, 1);
        pid = fork();
        auto const start = std::chrono::system_clock::now();
        if (pid == 0)
        {
            RGB_to_GRAY(sharedKey, sh2, data, dimensions);
        }
        else if (pid > 0)
        {

            THRESHOLDING(sharedKey, sh2, data, dimensions);

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
    }

    return 0;
}
