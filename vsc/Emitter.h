//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////

#pragma once
#ifndef EMITTER
#define EMITTER

#include "stdio.h"

class SymbolList;
class FunctionList;
class FuncNode;
class Parser;
class ScriptHeader;

class Emitter
{
public:

	Emitter(ScriptHeader* _g_ScriptHeader, SymbolList* _g_SymbolList, Parser* _g_Parser);
	~Emitter();
	void BuildXSE();
	void EmitHeader();
	void EmitDirectives();
	void EmitScopeSymbols(int iScope, int iType);
	void EmmitFunctions();
	void EmitFunc(FuncNode* pFunc);

private:
	FILE * g_pOutputFile;
	ScriptHeader* g_ScriptHeader;
	SymbolList* g_SymbolList;
	//FunctionList* g_Function;
	Parser* g_Parser;


};

#endif