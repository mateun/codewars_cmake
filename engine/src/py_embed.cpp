#include <py_embed.h>
#include <Windows.h>

static PyObject *pName, *pModule, *pDict, *pFunc;
static PyObject *pArgs, *pValue;

void callPyFunc() {
	//PyImport_AppendInittab("emb", &PyInit_emb);
	Py_SetProgramName(L"TESTPYTHON");
	Py_Initialize();
	//PySys_SetPath(L"C:/Users/martin/Documents/Projects/C++/codewars/build/win32_dist/x86-Debug/Debug");
	PySys_SetPath(L"./py_scripts");
	//PyObject* sysPath = PySys_GetObject("path");
	//PyList_Append(sysPath, PyUnicode_FromString("."));
	pName = PyUnicode_DecodeFSDefault("testprog");
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	if (pModule == nullptr) {
		OutputDebugString("python module import failed/n");
		exit(1);
	}

	pFunc = PyObject_GetAttrString(pModule, "doFrame");
	if (pFunc && PyCallable_Check(pFunc)) {
		pArgs = PyTuple_New(1);
		pValue = PyLong_FromLong(1);
		PyTuple_SetItem(pArgs, 0, pValue);
		pValue = PyObject_CallObject(pFunc, pArgs);
		long val = PyLong_AsLong(pValue);
		OutputDebugString("Val: ");
	}
	
	Py_FinalizeEx();
}