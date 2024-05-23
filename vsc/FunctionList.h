//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////


#pragma once
#ifndef FUNC_TABLE
#define FUNC_TABLE

#include "LinkedList.h"
#include "I_Code.h"
#define MAX_IDENT_SIZE     256

class ScriptHeader;



class FuncNode
{
public:

	FuncNode(ICodeStream<ICodeNode>* _ICodeStream)
	{
		this->iIndex = 0;
		this->iIsHostAPI = 0;
		this->pstrName[MAX_IDENT_SIZE] = { 0 };
		this->iParamCount = 0;
		this->ICodeStream = _ICodeStream;
	}

	FuncNode()
	{

	}

	template<class T>
	void Init(ICodeStream<T>* _ICodeStream)
	{
		this->iIndex = 0;
		this->iIsHostAPI = 0;
		this->pstrName[MAX_IDENT_SIZE] = { 0 };
		this->iParamCount = 0;
		this->ICodeStream = _ICodeStream;
	}
	int GetIsHostAPI()
	{
		return iIsHostAPI;
	}
	void SetIsHostAPI(int data)
	{
		iIsHostAPI = data;
	}
	int GetParamCount()
	{
		return iParamCount;
	}
	void SetParamCount(int data)
	{
		iParamCount = data;
	}
	ICodeStream<ICodeNode>* GetCodeStream()
	{
		return ICodeStream;
	}
	int GetIndex()
	{
		return this->iIndex;
	}
	void SetIndex(int data)
	{
		this->iIndex = data;
	}
	char* GetPSTRName()
	{
		return pstrName;
	}

	int CompPSTRName(char* pstrName)
	{
		return _stricmp(this->pstrName, pstrName);
	}

	int CompiPSTRName(char* pstrName)
	{
		return stricmp(this->pstrName, pstrName);
	}

	void CopyPSTR(char* pstrName)
	{
		strcpy(this->pstrName, pstrName);
	}	

private:
	int iIndex;
	char pstrName[MAX_IDENT_SIZE];
	int iIsHostAPI;
	int iParamCount;
	ICodeStream<ICodeNode>* ICodeStream;
};

//template<class T>
class FunctionList : public DList<FuncNode>
{
	friend class FuncNode;
public:
	FunctionList();
	~FunctionList();
	void Init(ScriptHeader* input)
	{
		g_ScriptHeader = input;
	}
	int AddFunc(char* pstrName, int iIsHostAPI);
	FuncNode* GetFuncByName(char* pstrName);
	FuncNode* GetFuncByIndex(int iIndex);
	void SetFuncParamCount(int iIndex, int iParamCount);
	static FunctionList* getInstance()
	{
		if (!g_Function)
			g_Function = new FunctionList();
		return g_Function;
	}
private:
	ScriptHeader* g_ScriptHeader;
	static FunctionList* g_Function;

};


#endif