#!/bin/bash
g++ -std=c++11 -I.. -L/usr/lib -L/user/local/lib -L/lib -o test_lambda_queue test_lambda_queue.cpp -lpthread && ./test_lambda_queue 
