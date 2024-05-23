//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////



#include "Lexer.h"
#include "SourceCode.h"


struct OpState
{
	char cChar;
	int iSubStateIndex;
	int iSubStateCount;
	int iIndex;
};


LexerState g_CurrLexerState;
LexerState g_PrevLexerState;


OpState g_OpChars0[MAX_OP_STATE_COUNT] = { { '+', 0, 2, 0 }, { '-', 2, 2, 1 }, { '*', 4, 1, 2 }, { '/', 5, 1, 3 },
{ '%', 6, 1, 4 }, { '^', 7, 1, 5 }, { '&', 8, 2, 6 }, { '|', 10, 2, 7 },
{ '#', 12, 1, 8 }, { '~', 0, 0, 9 }, { '!', 13, 1, 10 }, { '=', 14, 1, 11 },
{ '<', 15, 2, 12 }, { '>', 17, 2, 13 }, { '$', 19, 1, 35 } };

OpState g_OpChars1[MAX_OP_STATE_COUNT] = { { '=', 0, 0, 14 }, { '+', 0, 0, 15 },     // +=, ++
{ '=', 0, 0, 16 }, { '-', 0, 0, 17 },     // -=, --
{ '=', 0, 0, 18 },                        // *=
{ '=', 0, 0, 19 },                        // /=
{ '=', 0, 0, 20 },                        // %=
{ '=', 0, 0, 21 },                        // ^=
{ '=', 0, 0, 22 }, { '&', 0, 0, 23 },     // &=, &&
{ '=', 0, 0, 24 }, { '|', 0, 0, 25 },     // |=, ||
{ '=', 0, 0, 26 },                        // #=
{ '=', 0, 0, 27 },                        // !=
{ '=', 0, 0, 28 },                        // ==
{ '=', 0, 0, 29 }, { '<', 0, 1, 30 },     // <=, <<
{ '=', 0, 0, 31 }, { '>', 1, 1, 32 },     // >=, >>
{ '=', 0, 0, 36 } };                      // $=



OpState g_OpChars2[MAX_OP_STATE_COUNT] = { { '=', 0, 0, 33 }, { '=', 0, 0, 34 } }; // <<=, >>=



char cDelims[MAX_DELIM_COUNT] = { ',', '(', ')', '[', ']', '{', '}', ';' };


LexerState::LexerState()
{
	iCurrLineIndex = 0;
	CurrToken = 0;
	pstrCurrLexeme[MAX_LEXEME_SIZE] = { 0 };
	iCurrLexemeStart = 0;
	iCurrLexemeEnd = 0;
	iCurrOp = 0;

}

LexerState::~LexerState()
{

}



DLItr<char*>& LexerState::GetCurrLine()
{
	return this->pCurrLine;
}

void LexerState::SetCurrLine(DLItr<char*> data)
{
	this->pCurrLine = data;
}

int LexerState::GetCurrLexemeStart()
{
	return this->iCurrLexemeStart;
}

int LexerState::GetCurrLexemeEnd()
{
	return this->iCurrLexemeEnd;
}

Token LexerState::GetCurrToken()
{
	return this->CurrToken;

}

void LexerState::SetCurrLexemeStart(int data)
{
	this->iCurrLexemeStart = data;
}

void LexerState::SetCurrLexemeEnd(int data)
{
	this->iCurrLexemeEnd = data;
}


int LexerState::GetCurrOp()
{
	return this->iCurrOp;
}

void LexerState::SetCurrOp(int data)
{
	this->iCurrOp = data;
}

void LexerState::SetCurrToken(Token Data)
{
	this->CurrToken = Data;
}

Lexer::Lexer(LexerState* _g_CurrLexerState, LexerState* _g_PrevLexerState, SourceCode* _g_SourceCode)
{
	this->g_CurrLexerState = _g_CurrLexerState;
	this->g_PrevLexerState = _g_PrevLexerState;
	this->g_SourceCode = _g_SourceCode;

}

Lexer::~Lexer()
{

}


void Lexer::ResetLexer()
{


	g_CurrLexerState->iCurrLineIndex = 0;
	g_CurrLexerState->SetCurrLine(g_SourceCode->GetIt());



	g_CurrLexerState->SetCurrLexemeStart(0);
	g_CurrLexerState->SetCurrLexemeEnd(0);



	g_CurrLexerState->SetCurrOp(0);
}

void Lexer::CopyLexerState(LexerState* pDestState, LexerState* pSourceState)
{


	pDestState->iCurrLineIndex = pSourceState->iCurrLineIndex;
	pDestState->SetCurrLine(pSourceState->GetCurrLine());
	pDestState->SetCurrToken(pSourceState->GetCurrToken());
	strcpy(pDestState->pstrCurrLexeme, pSourceState->pstrCurrLexeme);
	pDestState->SetCurrLexemeStart(pSourceState->GetCurrLexemeStart());
	pDestState->SetCurrLexemeEnd(pSourceState->GetCurrLexemeEnd());
	pDestState->SetCurrOp(pSourceState->GetCurrOp());
}



int Lexer::GetOpStateIndex(char cChar, int iCharIndex, int iSubStateIndex, int iSubStateCount)
{
	int iStartStateIndex;
	int iEndStateIndex;



	if (iCharIndex == 0)
	{


		iStartStateIndex = 0;
		iEndStateIndex = MAX_OP_STATE_COUNT;
	}
	else
	{


		iStartStateIndex = iSubStateIndex;
		iEndStateIndex = iStartStateIndex + iSubStateCount;
	}



	for (int iCurrOpStateIndex = iStartStateIndex; iCurrOpStateIndex < iEndStateIndex; ++iCurrOpStateIndex)
	{


		char cOpChar;
		switch (iCharIndex)
		{
		case 0:
			cOpChar = g_OpChars0[iCurrOpStateIndex].cChar;
			break;

		case 1:
			cOpChar = g_OpChars1[iCurrOpStateIndex].cChar;
			break;

		case 2:
			cOpChar = g_OpChars2[iCurrOpStateIndex].cChar;
			break;
		}



		if (cChar == cOpChar)
			return iCurrOpStateIndex;
	}



	return -1;
}



int Lexer::IsCharOpChar(char cChar, int iCharIndex)
{


	for (int iCurrOpStateIndex = 0; iCurrOpStateIndex < MAX_OP_STATE_COUNT; ++iCurrOpStateIndex)
	{


		char cOpChar;
		switch (iCharIndex)
		{
		case 0:
			cOpChar = g_OpChars0[iCurrOpStateIndex].cChar;
			break;

		case 1:
			cOpChar = g_OpChars1[iCurrOpStateIndex].cChar;
			break;

		case 2:
			cOpChar = g_OpChars2[iCurrOpStateIndex].cChar;
			break;
		}



		if (cChar == cOpChar)
			return true;
	}



	return false;
}



OpState Lexer::GetOpState(int iCharIndex, int iStateIndex)
{
	OpState State;



	switch (iCharIndex)
	{
	case 0:
		State = g_OpChars0[iStateIndex];
		break;

	case 1:
		State = g_OpChars1[iStateIndex];
		break;

	case 2:
		State = g_OpChars2[iStateIndex];
		break;
	}

	return State;
}


int Lexer::IsCharDelim(char cChar)
{


	for (int iCurrDelimIndex = 0; iCurrDelimIndex < MAX_DELIM_COUNT; ++iCurrDelimIndex)
	{


		if (cChar == cDelims[iCurrDelimIndex])
			return true;
	}

	return false;
}


int Lexer::IsCharWhitespace(char cChar)
{


	if (cChar == ' ' || cChar == '\t' || cChar == '\n' || cChar == '\r')
		return true;
	else
		return false;
}


int Lexer::IsCharNumeric(char cChar)
{


	if (cChar >= '0' && cChar <= '9')
		return true;
	else
		return false;
}


int Lexer::IsCharIdent(char cChar)
{


	if ((cChar >= '0' && cChar <= '9') ||
		(cChar >= 'A' && cChar <= 'Z') ||
		(cChar >= 'a' && cChar <= 'z') ||
		cChar == '_')
		return true;
	else
		return false;
}


char Lexer::GetNextChar()
{

	char * pstrCurrLine;
	if (g_CurrLexerState->GetCurrLine().Valid())
		pstrCurrLine = (char *)g_CurrLexerState->GetCurrLine().Item();
	else
		return '\0';

	if (g_CurrLexerState->GetCurrLexemeEnd() >= (int)strlen(pstrCurrLine))
	{
		g_CurrLexerState->GetCurrLine().Forth();

		if (g_CurrLexerState->GetCurrLine().Valid())
		{
			pstrCurrLine = (char *)g_CurrLexerState->GetCurrLine().Item();

			++g_CurrLexerState->iCurrLineIndex;
			g_CurrLexerState->SetCurrLexemeStart(0);
			g_CurrLexerState->SetCurrLexemeEnd(0);
		}
		else
		{

			return '\0';
		}

	}

	return pstrCurrLine[g_CurrLexerState->iCurrLexemeEnd++];
}



Token Lexer::GetNextToken(int flag)
{
	CopyLexerState(g_PrevLexerState, g_CurrLexerState);

	g_CurrLexerState->SetCurrLexemeStart(g_CurrLexerState->GetCurrLexemeEnd());

	int iCurrLexState = LEX_STATE_START;

	int iCurrOpCharIndex = 0;
	int iCurrOpStateIndex = 0;
	OpState CurrOpState;

	int iLexemeDone = false;

	char cCurrChar;

	int iNextLexemeCharIndex = 0;

	int iAddCurrChar;

	while (true)
	{
		cCurrChar = GetNextChar();
		if (cCurrChar == '\0')
			break;

		iAddCurrChar = true;

		switch (iCurrLexState)
		{
		case LEX_STATE_UNKNOWN:

			iLexemeDone = true;

			break;

		case LEX_STATE_START:

			if (IsCharWhitespace(cCurrChar))
			{
				++g_CurrLexerState->iCurrLexemeStart;
				iAddCurrChar = false;
			}

			else if (IsCharNumeric(cCurrChar))
			{
				iCurrLexState = LEX_STATE_INT;
			}

			else if (cCurrChar == '.')
			{
				iCurrLexState = LEX_STATE_FLOAT;
			}

			else if (IsCharIdent(cCurrChar))
			{
				iCurrLexState = LEX_STATE_IDENT;
			}

			else if (IsCharDelim(cCurrChar))
			{
				iCurrLexState = LEX_STATE_DELIM;
			}

			else if (IsCharOpChar(cCurrChar, 0))
			{
				iCurrOpStateIndex = GetOpStateIndex(cCurrChar, 0, 0, 0);
				if (iCurrOpStateIndex == -1)
					return TOKEN_TYPE_INVALID;

				CurrOpState = GetOpState(0, iCurrOpStateIndex);

				iCurrOpCharIndex = 1;

				g_CurrLexerState->SetCurrOp(CurrOpState.iIndex);

				iCurrLexState = LEX_STATE_OP;
			}

			else if (cCurrChar == '"')
			{
				iAddCurrChar = false;
				iCurrLexState = LEX_STATE_STRING;
			}
			else if (cCurrChar == ':')
			{
				iAddCurrChar = true;
				iLexemeDone = false;
				iCurrLexState = LEX_STATE_DELIM;
			}

			else
				iCurrLexState = LEX_STATE_UNKNOWN;

			break;

		case LEX_STATE_INT:

			if (IsCharNumeric(cCurrChar))
			{
				iCurrLexState = LEX_STATE_INT;
			}

			else if (cCurrChar == '.')
			{
				iCurrLexState = LEX_STATE_FLOAT;
			}
			else if (cCurrChar == ':')
			{
				iCurrLexState = LEX_STATE_INT;
				iAddCurrChar = false;
				iLexemeDone = true;
			}

			else if (IsCharWhitespace(cCurrChar) || IsCharDelim(cCurrChar))
			{
				iAddCurrChar = false;
				iLexemeDone = true;
			}

			else
				iCurrLexState = LEX_STATE_UNKNOWN;

			break;

		case LEX_STATE_FLOAT:

			if (IsCharNumeric(cCurrChar))
			{
				iCurrLexState = LEX_STATE_FLOAT;
			}

			else if (IsCharWhitespace(cCurrChar) || IsCharDelim(cCurrChar))
			{
				iLexemeDone = true;
				iAddCurrChar = false;
			}

			else
				iCurrLexState = LEX_STATE_UNKNOWN;

			break;

		case LEX_STATE_IDENT:

			if (IsCharIdent(cCurrChar))
			{
				iCurrLexState = LEX_STATE_IDENT;
			}

			else if (IsCharWhitespace(cCurrChar) || IsCharDelim(cCurrChar))
			{
				iAddCurrChar = false;
				iLexemeDone = true;
			}
			else if (cCurrChar == ':')
			{
				iAddCurrChar = false;
				iLexemeDone = true;
			}

			else
				iCurrLexState = LEX_STATE_UNKNOWN;

			break;

		case LEX_STATE_OP:

			if (CurrOpState.iSubStateCount == 0)
			{
				iAddCurrChar = false;
				iLexemeDone = true;
				break;
			}

			if (IsCharOpChar(cCurrChar, iCurrOpCharIndex))
			{
				iCurrOpStateIndex = GetOpStateIndex(cCurrChar, iCurrOpCharIndex, CurrOpState.iSubStateIndex, CurrOpState.iSubStateCount);
				if (iCurrOpStateIndex == -1)
				{
					iCurrLexState = LEX_STATE_UNKNOWN;
				}
				else
				{
					CurrOpState = GetOpState(iCurrOpCharIndex, iCurrOpStateIndex);

					++iCurrOpCharIndex;

					g_CurrLexerState->SetCurrOp(CurrOpState.iIndex);
				}
			}

			else
			{
				iAddCurrChar = false;
				iLexemeDone = true;
			}

			break;

		case LEX_STATE_DELIM:

			iAddCurrChar = false;
			iLexemeDone = true;

			break;

		case LEX_STATE_STRING:

			if (cCurrChar == '"')
			{
				iAddCurrChar = false;
				iCurrLexState = LEX_STATE_STRING_CLOSE_QUOTE;
			}

			else if (cCurrChar == '\n')
			{
				iAddCurrChar = false;
				iCurrLexState = LEX_STATE_UNKNOWN;
			}

			else if (cCurrChar == '\\')
			{
				iAddCurrChar = false;
				iCurrLexState = LEX_STATE_STRING_ESCAPE;
			}



			break;



		case LEX_STATE_STRING_ESCAPE:



			iCurrLexState = LEX_STATE_STRING;

			break;



		case LEX_STATE_STRING_CLOSE_QUOTE:



			iAddCurrChar = false;
			iLexemeDone = true;

			break;
		}



		if (iAddCurrChar)
		{
			g_CurrLexerState->pstrCurrLexeme[iNextLexemeCharIndex] = cCurrChar;
			++iNextLexemeCharIndex;
		}



		if (iLexemeDone)
			break;
	}



	g_CurrLexerState->pstrCurrLexeme[iNextLexemeCharIndex] = '\0';



	--g_CurrLexerState->iCurrLexemeEnd;



	Token TokenType;
	switch (iCurrLexState)
	{


	case LEX_STATE_UNKNOWN:
		TokenType = TOKEN_TYPE_INVALID;
		break;



	case LEX_STATE_INT:
		TokenType = TOKEN_TYPE_INT;
		break;



	case LEX_STATE_FLOAT:
		TokenType = TOKEN_TYPE_FLOAT;
		break;



	case LEX_STATE_IDENT:



		TokenType = TOKEN_TYPE_IDENT;





		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "var") == 0)
			TokenType = TOKEN_TYPE_RSRVD_VAR;



		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "true") == 0)
			TokenType = TOKEN_TYPE_RSRVD_TRUE;



		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "false") == 0)
			TokenType = TOKEN_TYPE_RSRVD_FALSE;



		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "if") == 0)
			TokenType = TOKEN_TYPE_RSRVD_IF;



		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "else") == 0)
			TokenType = TOKEN_TYPE_RSRVD_ELSE;



		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "break") == 0)
			TokenType = TOKEN_TYPE_RSRVD_BREAK;



		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "continue") == 0)
			TokenType = TOKEN_TYPE_RSRVD_CONTINUE;



		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "for") == 0)
			TokenType = TOKEN_TYPE_RSRVD_FOR;



		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "while") == 0)
			TokenType = TOKEN_TYPE_RSRVD_WHILE;



		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "func") == 0)
			TokenType = TOKEN_TYPE_RSRVD_FUNC;



		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "return") == 0)
			TokenType = TOKEN_TYPE_RSRVD_RETURN;

		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "host") == 0)
			TokenType = TOKEN_TYPE_RSRVD_HOST;

		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "switch") == 0)
			TokenType = TOKEN_TYPE_RSRVD_SWITCH;

		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "case") == 0)
			TokenType = TOKEN_TYPE_RSRVD_CASE;

		if (stricmp(g_CurrLexerState->pstrCurrLexeme, "default") == 0)
			TokenType = TOKEN_TYPE_RSRVD_DEFAULT;

		break;



	case LEX_STATE_DELIM:

		switch (g_CurrLexerState->pstrCurrLexeme[0])
		{
		case ',':
			TokenType = TOKEN_TYPE_DELIM_COMMA;
			break;

		case '(':
			TokenType = TOKEN_TYPE_DELIM_OPEN_PAREN;
			break;

		case ')':
			TokenType = TOKEN_TYPE_DELIM_CLOSE_PAREN;
			break;

		case '[':
			TokenType = TOKEN_TYPE_DELIM_OPEN_BRACE;
			break;

		case ']':
			TokenType = TOKEN_TYPE_DELIM_CLOSE_BRACE;
			break;

		case '{':
			TokenType = TOKEN_TYPE_DELIM_OPEN_CURLY_BRACE;
			break;

		case '}':
			TokenType = TOKEN_TYPE_DELIM_CLOSE_CURLY_BRACE;
			break;

		case ';':
			TokenType = TOKEN_TYPE_DELIM_SEMICOLON;
			break;
		case ':':
			TokenType = TOKEN_TYPE_DELIM_COLON;
			break;

		}

		break;

	case LEX_STATE_OP:
			TokenType = TOKEN_TYPE_OP;
		break;

	case LEX_STATE_STRING_CLOSE_QUOTE:
		TokenType = TOKEN_TYPE_STRING;
		break;

	default:
		TokenType = TOKEN_TYPE_END_OF_STREAM;
	}

	g_CurrLexerState->SetCurrToken(TokenType);
	return TokenType;
}



void Lexer::RewindTokenStream()
{
	CopyLexerState(g_CurrLexerState, g_PrevLexerState);
}



Token Lexer::GetCurrToken()
{
	return g_CurrLexerState->GetCurrToken();
}



char * Lexer::GetCurrLexeme()
{
	return g_CurrLexerState->pstrCurrLexeme;
}



void Lexer::CopyCurrLexeme(char * pstrBuffer)
{
	strcpy(pstrBuffer, g_CurrLexerState->pstrCurrLexeme);
}



int Lexer::GetCurrOp()
{
	return g_CurrLexerState->GetCurrOp();
}



char Lexer::GetLookAheadChar()
{
	LexerState* PrevLexerState = new LexerState;
	CopyLexerState(PrevLexerState, g_CurrLexerState);

	char cCurrChar;
	while (true)
	{
		cCurrChar = GetNextChar();
		if (!IsCharWhitespace(cCurrChar))
			break;
	}


	CopyLexerState(g_CurrLexerState, PrevLexerState);



	return cCurrChar;
}



char * Lexer::GetCurrSourceLine()
{
	if (g_CurrLexerState->GetCurrLine().Valid())
		return (char *)g_CurrLexerState->GetCurrLine().Item();
	else
		return NULL;
}



int Lexer::GetCurrSourceLineIndex()
{
	return g_CurrLexerState->iCurrLineIndex;
}



int Lexer::GetLexemeStartIndex()
{
	return g_CurrLexerState->GetCurrLexemeStart();
}
