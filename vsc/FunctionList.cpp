//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////


#include "FunctionList.h"
#include "Global.h"
#include "ScriptHeader.h"

FunctionList *FunctionList::g_Function = 0;

FunctionList::FunctionList()
{
	this->g_ScriptHeader = new ScriptHeader();
}

FunctionList::~FunctionList()
{

}


int FunctionList::AddFunc(char* pstrName, int iIsHostAPI)
{

	if (this->GetFuncByName(pstrName))
		return -1;

	ICodeStream<ICodeNode>* _ICodeStream = new ICodeStream<ICodeNode>();
	FuncNode pNewFunc;

	pNewFunc.Init(_ICodeStream);

	pNewFunc.CopyPSTR( pstrName );

	pNewFunc.SetIndex(this->getCount() + 1);

	pNewFunc.SetIsHostAPI(iIsHostAPI);

	pNewFunc.SetParamCount(0);

	pNewFunc.GetCodeStream()->setCount(0);

	this->Append(pNewFunc);

	if (stricmp(pstrName, MAIN_FUNC_NAME) == 0)
	{
		g_ScriptHeader->SetMainFuncPresent(true);
		g_ScriptHeader->SetMainFuncIndex(this->getCount());
	}

	return this->getCount();
}


FuncNode* FunctionList::GetFuncByName(char* pstrName)
{
	FuncNode* pCurrFunc;

	for (int iCurrFuncIndex = 1; iCurrFuncIndex <= this->getCount(); ++iCurrFuncIndex)
	{
		pCurrFunc = this->GetFuncByIndex(iCurrFuncIndex);

		if (pCurrFunc && pCurrFunc->CompiPSTRName(pstrName) == 0)
			return pCurrFunc;
	}
	return NULL;
}


FuncNode* FunctionList::GetFuncByIndex(int iIndex)
{
	if (!this->getCount())
		return NULL;

	DLItr<FuncNode> pCurrNode = this->GetIt();

	for (pCurrNode.Start(); pCurrNode.Valid(); pCurrNode.Forth())
	{
		FuncNode* pCurrFunc = &pCurrNode.Item();
		if (iIndex == pCurrFunc->GetIndex())
			return pCurrFunc;
	}
	return NULL;

}

void FunctionList::SetFuncParamCount(int iIndex, int iParamCount)
{
	FuncNode* pFunc = this->GetFuncByIndex(iIndex);

	pFunc->SetParamCount(iParamCount);
}