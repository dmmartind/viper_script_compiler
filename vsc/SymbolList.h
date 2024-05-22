//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////


#pragma once
#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include "global.h"
#include "LinkedList.h"

class SymbolNode
{
public:
	int GetIndex()
	{
		return this->iIndex;
	}
	char* GetIndent()
	{
		return this->pstrIdent;
	}
	int GetSize()
	{
		return this->iSize;
	}
	int GetScope()
	{
		return this->iScope;
	}
	int GetType()
	{
		return this->iType;
	}
	void SetSize(int data)
	{
		this->iSize = data;
	}
	void SetScope(int data)
	{
		this->iScope = data;
	}
	void SetType(int data)
	{
		this->iType = data;
	}
	void SetIndex(int data)
	{
		this->iIndex = data;
	}
	void CopyIdent(char* pstrIdent)
	{
		strcpy(this->pstrIdent, pstrIdent);
	}

	int CmpIdent(char* pstrIdent)
	{
		return stricmp(this->pstrIdent, pstrIdent);
	}	


private:
	int iIndex;
	char pstrIdent[MAX_IDENT_SIZE];
	int iSize;
	int iScope;
	int iType;
};


class SymbolList : public DList<SymbolNode*>
{

public:
	SymbolList::SymbolList(){}
	SymbolList::~SymbolList(){}


	/******************************************************************************************
	*
	*   GetSymbolByIndex ()
	*
	*   Returns a pointer to the symbol structure corresponding to the index.
	*/
	SymbolNode* SymbolList::GetSymbolByIndex(int iIndex)
	{
		if (!this->getCount())
			return NULL;

		DLItr<SymbolNode*> pCurrNode = this->GetIt();

		for (pCurrNode.Start(); pCurrNode.Valid(); pCurrNode.Forth())
		{
			SymbolNode* pCurrSymbol = pCurrNode.Item();
			if (iIndex == pCurrSymbol->GetIndex())
				return pCurrSymbol;
		}
		return NULL;
	}

	/******************************************************************************************
	*
	*   AddSymbol ()
	*
	*   Adds a symbol to the symbol table.
	*/
	int SymbolList::AddSymbol(char* pstrIdent, int iSize, int iScope, int iType)
	{
		if (this->GetSymbolByIdent(pstrIdent, iScope))
			return -1;

		SymbolNode *pNewSymbol = new SymbolNode();

		pNewSymbol->CopyIdent(pstrIdent);
		pNewSymbol->SetSize(iSize);
		pNewSymbol->SetScope(iScope);
		pNewSymbol->SetType(iType);

		this->Append(pNewSymbol);

		pNewSymbol->SetIndex(this->getCount() - 1);

		return pNewSymbol->GetIndex();
	}

	/******************************************************************************************
	*
	*   GetSymbolByIdent ()
	*
	*   Returns a pointer to the symbol structure corresponding to the identifier and scope.
	*/
	SymbolNode* SymbolList::GetSymbolByIdent(char* pstrIdent, int iScope)
	{
		SymbolNode* pCurrSymbol = 0;

		for (int iCurrSymbolIndex = 0; iCurrSymbolIndex < this->getCount(); ++iCurrSymbolIndex)
		{
			pCurrSymbol = this->GetSymbolByIndex(iCurrSymbolIndex);
			if (pCurrSymbol && pCurrSymbol->CmpIdent(pstrIdent) == 0 &&
				(pCurrSymbol->GetScope() == iScope || pCurrSymbol->GetScope() == 0))
				return pCurrSymbol;
		}
		return NULL;
	}

};


#endif