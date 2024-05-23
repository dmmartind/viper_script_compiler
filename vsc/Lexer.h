//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved.
/////////////////////////////////////////////////////////////////////

#ifndef LEXER
#define LEXER
#pragma once

#include "global.h"
#include "Queue.h";
#include "Array2D.h";

extern struct OpState;
class SourceCode;

class LexerState
{
public:
	LexerState();
	~LexerState();

	int iCurrLineIndex;
	DLItr<char*>& GetCurrLine();
	int GetCurrLexemeStart();
	void SetCurrLexemeStart(int data);

	int GetCurrLexemeEnd();
	void SetCurrLexemeEnd(int data);
	char pstrCurrLexeme[MAX_LEXEME_SIZE];
	void SetCurrLine(DLItr<char*> data);
	Token GetCurrToken();
	void SetCurrToken(Token Data);
	int GetCurrOp();
	void SetCurrOp(int data);
	//int IncCurrLexemeStart();
	//int IncCurrLexemeEnd();
	//int DecCurrLexemeEnd();
	int iCurrLexemeStart;
	int iCurrLexemeEnd;
	

private:
	
	DLItr<char*> pCurrLine;
	Token CurrToken;
	int iCurrOp;
	static int counter;
};

class Lexer
{
public:
	friend class Compiler;
	Lexer(LexerState* _g_CurrLexerState, LexerState* _g_PrevLexerState, SourceCode* _g_SourceCode);
	~Lexer();
	LexerState* g_CurrLexerState;
	LexerState* g_PrevLexerState;
	SourceCode* g_SourceCode;

	void operator=(const char* data)
	{
		if (this->g_CurrLexerState->pstrCurrLexeme)
			delete this->g_CurrLexerState->pstrCurrLexeme;
		strcpy(this->g_CurrLexerState->pstrCurrLexeme, data);

	}
	
	int GetCurrSourceLineIndex();
	char* GetCurrSourceLine();
	int GetLexemeStartIndex();
	char* GetCurrLexeme();
	char GetLookAheadChar();
	void CopyLexerState(LexerState* pDestState, LexerState* pSourceState);
	char GetNextChar();
	int IsCharWhitespace(char cChar);
	void CopyCurrLexeme(char* pstrBuffer);
	void ResetLexer();
	Token GetNextToken(int flag);//todo
	int IsCharNumeric(char cChar);
	int IsCharIdent(char cChar);
	int IsCharDelim(char cChar);
	int IsCharOpChar(char cChar, int iCharIndex);
	int GetOpStateIndex(char cChar, int iCharIndex, int iSubStateIndex, int iSubStateCount);
	OpState GetOpState(int iCharIndex, int iStateIndex);
	int GetCurrOp();
	void RewindTokenStream();
	Token GetCurrToken();
	//Queue<char*>& GetIncStack()
	//{
		//return this->inc;
	//}

	//Queue<Array2D<int>>& GetCaseQueue()
	//{
		//return this->cst;
	//}

	//int GetCaseCount()
	//{
	//	return this->caseCount;
	//}

	//void IncCaseCount()
	//{
	//	this->caseCount++;
	//}
	//void CopyCurrLexeme(LexerState* Dest, LexerState* Source)
	//{
		//strcpy(Dest->pstrCurrLexeme, Source->pstrCurrLexeme);
	//}
	//char* CopyNString(char* nString, int size, char* Lexeme )
	//{
	//	strcpy_s(nString, strnlen(g_Lexer->GetCurrLexeme(), 1000) + 1, g_Lexer->GetCurrLexeme());
	//}
	void setError(int input)
	{
		this->hasError = input;
	}
	int getError()
	{
		return this->hasError;
	}
	Queue<Array2D<int>> cst;
	Queue<char*> inc;
private:
	
	int hasError;
};
#endif