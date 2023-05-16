#ifndef PythonHelperClass_h
#define PythonHelperClass_h
#include <Python.h>
#include <string>
#include <iostream>
#include <memory>
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

    std::string runPythonFunction(const std::string_view& cmd) {
        PyObject* pName = PyUnicode_DecodeFSDefault("helperFunctionForPyCPP");
        PyObject* pFunc = PyObject_GetAttr(pModule_, pName);
        Py_DECREF(pName);
        
        if (pFunc && PyCallable_Check(pFunc)) {
            PyObject* pArg = Py_BuildValue("(s)", cmd.data());
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
    PyObject* convertArgumentsToPyTuple(const T&... multi_inputs)
    {
        PyObject* pTupleWithArguments = PyTuple_New(sizeof...(T));
        int i = 0;
        (PyTuple_SetItem(pTupleWithArguments, i++, convertVariableToPyObject(multi_inputs)), ...);
        return pTupleWithArguments;
    }

    template <typename R, typename... T>
    R runPythonFunction(const char *moduleName, const char *functionName, T &&... multi_inputs)
    {
        std::unique_ptr<PyObject, decltype(&Py_DECREF)> moduleString(PyUnicode_FromString(moduleName), Py_DECREF);
        std::unique_ptr<PyObject, decltype(&Py_DECREF)> module(PyImport_Import(moduleString.get()), Py_DECREF);
        std::unique_ptr<PyObject, decltype(&Py_DECREF)> func(PyObject_GetAttrString(module.get(), functionName), Py_DECREF);
        std::unique_ptr<PyObject, decltype(&Py_DECREF)> args(convertArgumentsToPyTuple(multi_inputs...), Py_DECREF);
        std::unique_ptr<PyObject, decltype(&Py_DECREF)> myResult(PyObject_CallObject(func.get(), args.get()), Py_DECREF);
        if (!myResult) {
            std::cerr << "PythonHelper: bad result!"<< std::endl;
            return R();
        }
        R result;
        if constexpr (std::is_same<R, unsigned long long>::value)
            result = PyLong_AsUnsignedLongLong(myResult.get());
        else if constexpr (std::is_same<R, long long>::value)
            result = PyLong_AsLongLong(myResult.get());
        else if constexpr (std::is_unsigned<R>::value)
            result = PyLong_AsUnsignedLong(myResult.get());
        else if constexpr (std::is_floating_point_v<R>)
            result = PyFloat_AsDouble(myResult.get());
        else if constexpr (std::is_same<R, const char*>::value || std::is_same<R, std::string>::value)
            result = PyUnicode_AsUTF8(myResult.get());
        else
            result = PyLong_AsLong(myResult.get());

        return result;
    }
private:
    PyObject* pModule_ = nullptr;
} py;

#endif
