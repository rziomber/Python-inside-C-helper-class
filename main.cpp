//g++ main.cpp $(python3-config --embed --ldflags) -fpie $(python3-config --cflags --embed) -std=c++17
#include "./python.h"
#include <iostream>
#include <string>

int main()
{
    const char *pythonCode = R"rawliteral(
import time
print('Hello World from Embedded Python!!!')
def getFive():
   return 5
def getString():
   return "string"
def getProduct(a,b):
   return a*b
)rawliteral";

    PyRun_SimpleString(pythonCode);

    int result = py.runPythonFunction<long>("__main__", "getFive");
    double result2 = py.runPythonFunction<double>("__main__", "getProduct", 2.1, 4);
    std::string result3 = py.runPythonFunction("time.time()");
    long result4 = py.runPythonFunction<long>("__main__", "getProduct", 2, 4);
    const char* result5 = py.runPythonFunction<const char*>("__main__", "getString");
    std::cout << result << "\n"
              << result2 << "\n"
              << result3 << "\n"
              << result4 << "\n"
              << result5 << "\n";

    return 0;
}
