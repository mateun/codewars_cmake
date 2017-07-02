#include <game.h>
#include <Windows.h>

ScriptGameModule::ScriptGameModule(const std::string& name) {

	// We assume that the basic Python initialization is already done here
	// and the paths etc. are set, so we can just init the module given by the name. 
	PyObject* pName = PyUnicode_DecodeFSDefault(name.c_str());

	_pModule = PyImport_Import(pName);

	if (_pModule == nullptr) {
		OutputDebugString("failed to load module\n");
	}

	_pFuncDoFrame = PyObject_GetAttrString(_pModule, "doFrame");
	_pFuncInit = PyObject_GetAttrString(_pModule, "initGameModule");
}

void ScriptGameModule::CallDoFrame() {
	PyObject_CallObject(_pFuncDoFrame, nullptr);
}

void ScriptGameModule::CallInit() {
	PyObject_CallObject(_pFuncInit, nullptr);
}
