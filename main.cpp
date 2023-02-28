#include "./python.h"
#include <iostream>
#include <string>

int main()
{
    std::string pythonCode = R"rawliteral(
import time
print('Hello World from Embedded Python!!!')
def getFive():
   return 5
def getProduct(a,b):
   return a*b
)rawliteral";

    PyRun_SimpleString(pythonCode.c_str());

    int result = py.runPythonFunctionLong("__main__", "getFive");
    double result2 = py.runPythonFunctionDouble("__main__", "getProduct", 2.8, 4.2);
    std::string result3 = py.runPythonFunction("time.time()");
    std::cout << result << "\n"
              << result2 << "\n"
              << result3 << "\n";

    return 0;
}
