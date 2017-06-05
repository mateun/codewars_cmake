#pragma once
#include <Python.h>

/// Call to python
void callPyFunc();


/// Callable from python

// Module Initializer
PyObject* PyInit_emb(void);

/**
	Get health for player
	params: 
		int => player number
*/
PyObject* emb_getHealth(PyObject* self, PyObject* args);