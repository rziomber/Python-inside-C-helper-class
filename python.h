#ifndef PythonHelperClass_h
#define PythonHelperClass_h
#include <Python.h>
#include <string>

class PyHelper {
public:
    PyHelper() {
        Py_Initialize();
        PyRun_SimpleString(R"rawliteral(def helperFunctionForPyCPP(runThisCommand):
        return str(eval(runThisCommand)))rawliteral");
        pModule_ = PyImport_AddModule("__main__");
    }
    
    ~PyHelper() {
        Py_Finalize();
    }
    
    std::string runPythonFunction(const std::string& cmd) {
        PyObject* pName = PyUnicode_DecodeFSDefault("helperFunctionForPyCPP");
        PyObject* pFunc = PyObject_GetAttr(pModule_, pName);
        Py_DECREF(pName);
        
        if (pFunc && PyCallable_Check(pFunc)) {
            PyObject* pArg = Py_BuildValue("(s)", cmd.c_str());
            PyObject* pResult = PyObject_CallObject(pFunc, pArg);
            Py_DECREF(pArg);
            if (pResult) {
                char* resultStr;
                PyArg_Parse(pResult, "s", &resultStr);
                std::string result(resultStr);
                Py_DECREF(pResult);
                return result;
            }
            Py_XDECREF(pResult);
        }
        Py_XDECREF(pFunc);
        return "";
    }
    
    PyObject *convertVariableToPyObject(bool var)
    {
        return PyBool_FromLong((long)var);
    }
    PyObject *convertVariableToPyObject(int8_t var)
    {
        return PyLong_FromLong((long)var);
    }
    PyObject *convertVariableToPyObject(uint8_t var)
    {
        return PyLong_FromUnsignedLong((unsigned long)var);
    }
    PyObject *convertVariableToPyObject(int16_t var)
    {
        return PyLong_FromLong((long)var);
    }
    PyObject *convertVariableToPyObject(uint16_t var)
    {
        return PyLong_FromUnsignedLong((unsigned long)var);
    }
    PyObject *convertVariableToPyObject(int var)
    {
        return PyLong_FromLong((long)var);
    }
    PyObject *convertVariableToPyObject(unsigned int var)
    {
        return PyLong_FromUnsignedLong((unsigned long)var);
    }
    PyObject *convertVariableToPyObject(long var)
    {
        return PyLong_FromLong(var);
    }
    PyObject *convertVariableToPyObject(unsigned long var)
    {
        return PyLong_FromUnsignedLong(var);
    }
    PyObject *convertVariableToPyObject(long long var)
    {
        return PyLong_FromLongLong(var);
    }
    PyObject *convertVariableToPyObject(unsigned long long var)
    {
        return PyLong_FromUnsignedLongLong(var);
    }
    PyObject *convertVariableToPyObject(double var)
    {
        return PyFloat_FromDouble(var);
    }
    PyObject *convertVariableToPyObject(char *var)
    {
        return Py_BuildValue("y#", var, strlen(var));
    }
    
    template <typename... T>
    PyObject *convertArgumentsToPyTuple(T &&... multi_inputs) //https://stackoverflow.com/questions/7230621/how-can-i-iterate-over-a-packed-variadic-template-argument-list
    {
        PyObject *pTupleWithArguments = PyTuple_New(sizeof...(T));
        int i = 0;
        ([&](auto &input) {
            PyObject *pArgument = convertVariableToPyObject(input);
            PyTuple_SetItem(pTupleWithArguments, i, pArgument);
            //Py_DECREF(pArgument);
            ++i;
        }(multi_inputs),
         ...);
        return pTupleWithArguments;
    }
    
    #define PREPARE_CALLBACK                                           \
    PyObject *moduleString = PyUnicode_FromString(moduleName);     \
    PyObject *module = PyImport_Import(moduleString);              \
    PyObject *func = PyObject_GetAttrString(module, functionName); \
    PyObject *args = convertArgumentsToPyTuple(multi_inputs...);   \
    PyObject *myResult = PyObject_CallObject(func, args);
    
    #define FINISH_CALLBACK      \
    Py_DECREF(moduleString); \
    Py_DECREF(module);       \
    Py_DECREF(func);         \
    Py_DECREF(args);         \
    Py_DECREF(myResult);     \
    return result;
    
    template <typename... T>
    long runPythonFunctionLong(const char *moduleName, const char *functionName, T &&... multi_inputs)
    {
        PREPARE_CALLBACK
        long result = PyLong_AsLong(myResult);
        FINISH_CALLBACK
    }
    
    template <typename... T>
    unsigned long runPythonFunctionUnsignedLong(const char *moduleName, const char *functionName, T &&... multi_inputs)
    {
        PREPARE_CALLBACK
        unsigned long result = PyLong_AsUnsignedLong(myResult);
        FINISH_CALLBACK
    }
    
    template <typename... T>
    long long runPythonFunctionLongLong(const char *moduleName, const char *functionName, T &&... multi_inputs)
    {
        PREPARE_CALLBACK
        long long result = PyLong_AsLongLong(myResult);
        FINISH_CALLBACK
    }
    
    template <typename... T>
    unsigned long long runPythonFunctionLongLong(const char *moduleName, const char *functionName, T &&... multi_inputs)
    {
        PREPARE_CALLBACK
        unsigned long long result = PyLong_AsUnsignedLongLong(myResult);
        FINISH_CALLBACK
    }
    
    template <typename... T>
    double runPythonFunctionDouble(const char *moduleName, const char *functionName, T &&... multi_inputs)
    {
        PREPARE_CALLBACK
        double result = PyFloat_AsDouble(myResult);
        FINISH_CALLBACK
    }
private:
    PyObject* pModule_ = nullptr;
} py;

#endif
