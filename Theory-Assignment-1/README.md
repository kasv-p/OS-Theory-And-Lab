## Usage

compiling the code - g++ -std=c++11 buddy_algorithm.cpp -o buddy <br>
to execute the code - ./buddy <file_name> <br>
ex - ./buddy input1.dat

Here, the executable being created is ./buddy and different request patterns are present in the files input1.dat, input2.dat, input3.dat, input4.dat and input5.dat

* input1.dat - this file have the given test file along with a test case in which sometimes free blocks on left side of the tree also exist.
* input2.dat - this file have the test cases like requests are made exactly in powers of 2 and they are being deallocated faster.
* input3.dat - this file contain bursty requests.
* input4.dat - this file contain data doing same requests.
* input5.dat - this file asks to allocate same amount of memory and deallocates and ask the different amount of memory but all the proccesses ask the same amount of memory.
