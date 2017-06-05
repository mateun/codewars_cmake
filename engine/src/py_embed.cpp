#include <py_embed.h>
#include <Windows.h>

static PyObject *pName, *pModule, *pDict, *pFunc;
static PyObject *pArgs, *pValue;

void callPyFunc() {
	
	//Py_SetProgramName(L"TESTPYTHON");
	int res = PyImport_AppendInittab("codewars", &PyInit_emb);
	Py_Initialize();
	
	//PySys_SetPath(L"C:/Users/martin/Documents/Projects/C++/codewars/build/win32_dist/x86-Debug/Debug");
	PySys_SetPath(L"./py_scripts");
	//PyObject* sysPath = PySys_GetObject("path");
	//PyList_Append(sysPath, PyUnicode_FromString("."));
	pName = PyUnicode_DecodeFSDefault("testprog");
	try {
		pModule = PyImport_Import(pName);
	} catch (...) {
		PyObject *ptype, *pvalue, *ptraceback;
		PyErr_Fetch(&ptype, &pvalue, &ptraceback);
		//pvalue contains error message
		//ptraceback contains stack snapshot and many other information
		//(see python traceback structure)

		//Get error message
		//char *pStrErrorMessage = PyBytes_FromString(pvalue);
		
	}
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

/// Embedding stuff
PyMethodDef EmbMethods[] = {
	{"get_player_health", emb_getHealth, METH_VARARGS,
	"Return the current health value of the player given by its number."},
	{NULL, NULL, 0, NULL}
};

PyModuleDef EmbModule = {
	PyModuleDef_HEAD_INIT, "codewars", NULL, -1, EmbMethods,
	//NULL, NULL, NULL, NULL
};

PyObject* PyInit_emb(void) {
	try {
		PyObject* m = PyModule_Create(&EmbModule);
		if (m) {
			OutputDebugString("loaded python module\n");
			return m;
		}
	}
	catch (...) {
		OutputDebugString("caught exception while calling PyModuleCreate\n");
	}
}

PyObject* emb_getHealth(PyObject* self, PyObject* args) {
	OutputDebugString("emb_getHealth called from python!\n");
	int playerNr = 0;
	int playerHealth = 100;
	if (!PyArg_ParseTuple(args, "i", &playerNr)) {
		return nullptr;
	}

	// TODO make fake impl real
	// e.g. lookup health for player somewhere
	return PyLong_FromLong(playerHealth);
}