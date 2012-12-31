g++ -c -std=c++11 -I.. -I/usr/include test_utils.cpp -o test_utils.o && \
g++ test_utils.o -o test_utils -L/usr/lib -L/usr/local/lib -lOpenCL -lctemplate -lpthread && \
CL_LOG_ERRORS=stdout ./test_utils

