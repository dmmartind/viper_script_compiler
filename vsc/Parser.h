//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////

#pragma once
#ifndef PARSER_
#define PARSER_


#include "FunctionList.h"
#include "Global.h"
#include "Stack.h"
class Lexer;
class ScriptHeader;
class SymbolList;





class Loop
{
public:
	void init()
	{
		iScopeIndex = 0;
		iInstrIndex = 0;
		iSymbolIndex = 0;
		iOperator = 0;
		iStartTargetIndex = 0;
		iEndTargetIndex = 0;
		iInstruction = 0;
		ffloatValue = 0.0f;
		tToken = 0;
		iLoopScope = 0;
		

	}
	void SetStartTargetIndex(int input)
	{
		this->iStartTargetIndex = input;
	}
	void SetEndTargetIndex(int input)
	{
		iEndTargetIndex = input;
	}
	int GetStartTargetIndex()
	{
		return iStartTargetIndex;
	}
	int GetEndTargetIndex()
	{
		return iEndTargetIndex;
	}
	void SetILoopScope(int data)
	{
		iLoopScope = data;
	}
	int GetILoopScope()
	{
		return iLoopScope;
	}
	int GetOperator()
	{
		return this->iOperator;
	}

	int getOperation()
	{
		return this->iOperation;
	}

	void setOperation(int input)
	{
		this->iOperation = input;
	}

	Token getToken()
	{
		return this->tToken;
	}

	void setToken(Token input)
	{
		this->tToken = input;
	}

	void setLexeme(int input)
	{
		this->lexeme = input;
	}

	int getLexeme()
	{
		return this->lexeme;
	}
	
private:

	int iScopeIndex;
	int iInstrIndex;
	int iSymbolIndex;
	int iOperator;
	int iInstruction;
	int iStartTargetIndex;
	int iEndTargetIndex;
	float ffloatValue;
	int iOperation;
	Token tToken;
	int iLoopScope;
	int lexeme;
};



//extern LStack<Loop*> g_LoopStack;
#define MAX_FUNC_DECLARE_PARAM_COUNT 32

class Parser
{
public:

	Parser::Parser(Lexer* _g_Lexer, ScriptHeader* _g_ScriptHeader, SymbolList* _g_SymbolList, ICodeStream<ICodeNode>* _g_ICode, DList<char*> _g_StringTable, Stack<Loop*> g_stack);
	~Parser();
	void ParseFunc();
	void ParseSourceCode();
	int ReadToken(Token ReqToken);
	void ParseStatement(int flag, char* synt);
	void ParseHost();
	void ParseBlock(int flag = 0);
	void ParseVar();
	void ParseWhile();
	void ParseExpr();
	void ParseSubExpr();
	int IsOpRelational(int iOpType);
	int IsOpLogical(int iOpType);
	void ParseTerm();
	void ParseFactor();
	void ParseFuncCall();
	void ParseReturn();
	int ParseAssign(int flag);
	void ParseIf();
	void ParseFor();
	void ParseForExpr();
	void ParseContinue();
	void ParseBreak(int flag);
	void ParseSwitch();
	void ParseCase(int flag);
	void ParseDefault();
	void ParseSwitchStatement(int flag);
	void ParseForStatement(int flag);
	void ParseForFactor();
	
	DList<char*>& GetStringTable()
	{
		return g_StringTable;
	}
	
	int GetAX()
	{
		return g_iTempVar0SymbolIndex;
	}
	int GetBX()
	{
		return g_iTempVar1SymbolIndex;
	}
	void SetAX(int data)
	{
		g_iTempVar0SymbolIndex = data;
	}
	void SetBX(int data)
	{
		g_iTempVar1SymbolIndex = data;
	}

	void setSyntax(int constant)
	{
		currentSyntax = constant;
	}

	int getSyntax()
	{
		return currentSyntax;
	}

	int getExtraCurrentOp()
	{
		return this->iExtraCurrentOp;
	}
	void setExtraCurrentOp(int token)
	{
		this->iExtraCurrentOp = token;
	}

	


private:
	int g_iTempVar0SymbolIndex;                     // Temporary variable symbol indices
	int g_iTempVar1SymbolIndex;
	int g_iCurrScope;
	int currentSyntax;
	//FunctionList* g_function;
	Lexer* g_Lexer;
	ScriptHeader* g_ScriptHeader;
	SymbolList* g_SymbolList;
	ICodeStream<ICodeNode>* g_ICode;
	DList<char*> g_StringTable;
	Stack<Loop*> g_Stack;
	int iExtraCurrentOp;


};


#endif