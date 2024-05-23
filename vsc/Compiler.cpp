//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////


#include "Compiler.h"
#include "ScriptHeader.h"
#include "SourceCode.h"
#include "SymbolList.h"
#include "Lexer.h"
#include "Parser.h"
#include <cctype>


Compiler *Compiler::g_Compiler = 0;

struct OpState
{
	char cChar;
	int iSubStateIndex;
	int iSubStateCount;
	int iIndex;
};


Compiler::Compiler()
{
	this->g_iPreserveOutputFile = 0;
	this->g_ptrSourceFilename[MAX_FILENAME_SIZE] = { 0 };	// Source code filename
	this->g_ptrOutputFilename[MAX_FILENAME_SIZE] = { 0 };	// Executable filename
	this->g_iGenerateVSE = 0;
	//this->g_iTempVar0SymbolIndex = 0;
	//this->g_iTempVar1SymbolIndex = 0;
	this->caseCount = -1;
}
Compiler::~Compiler()
{

}
void Compiler::PrintLogo()
{
	printf("VSC\n");
	printf("ViperScript Compiler Version %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
	printf("Developed by David Martin\n");
	printf("\n");
}

void Compiler::PrintUsage()
{
	printf("Usage:\tXSC Source.VSS [Output.VASM] [Options]\n");
	printf("\n");
	printf("\t-S:Size      Sets the stack size (must be decimal integer value)\n");
	printf("\t-P:Priority  Sets the thread priority: Low, Med, High or timeslice\n");
	printf("\t             duration (must be decimal integer value)\n");
	printf("\t-A           Preserve assembly output file\n");
	printf("\t-N           Don't generate .VSE (preserves assembly output file)\n");
	printf("\n");
	printf("Notes:\n");
	printf("\t- File extensions are not required.\n");
	printf("\t- Executable name is optional; source name is used by default.\n");
	printf("\n");
}
void Compiler::Init(ScriptHeader* g_SH)
{
	g_SH->SetMainFuncPresent(false);
	g_SH->SetStackSize(0);
	g_SH->SetPriorityType(PRIORITY_NONE);

	g_iPreserveOutputFile = false;
	g_iGenerateVSE = true;
}

void Compiler::Exit()
{
	ShutDown();
	exit(0);
}
void Compiler::ShutDown()
{
	
}
void Compiler::VerifyFilenames(int argc, char * argv[])
{
	strcpy(this->g_ptrSourceFilename, argv[1]);
	strupr(this->g_ptrSourceFilename);

	if (!strstr(this->g_ptrSourceFilename, SOURCE_FILE_EXT))
	{
		strcat(this->g_ptrSourceFilename, SOURCE_FILE_EXT);
	}
	if (argv[2] && argv[2][0] != '-')
	{
		strcpy(this->g_ptrOutputFilename, argv[2]);
		strupr(this->g_ptrOutputFilename);
		if (!strstr(this->g_ptrOutputFilename, OUTPUT_FILE_EXT))
		{
			strcat(this->g_ptrOutputFilename, OUTPUT_FILE_EXT);
		}
	}
	else
	{
		int ExtOffset = strrchr(this->g_ptrSourceFilename, '.') - this->g_ptrSourceFilename;
		strncpy(this->g_ptrOutputFilename, this->g_ptrSourceFilename, ExtOffset);
		this->g_ptrOutputFilename[ExtOffset] = '\0';
		strcat(this->g_ptrOutputFilename, OUTPUT_FILE_EXT);
	}
}

void Compiler::ReadCmmndLineParams(int argc, char * argv[], ScriptHeader* g_ScriptHeader)
{
	char pstrCurrOption[32];
	char pstrCurrValue[32];
	char pstrErrorMssg[256];

	for (int iCurrOptionIndex = 0; iCurrOptionIndex < argc; ++iCurrOptionIndex)
	{
		strupr(argv[iCurrOptionIndex]);

		if (argv[iCurrOptionIndex][0] == '-')
		{
			int iCurrCharIndex;
			int iOptionSize;
			char cCurrChar;

			iCurrCharIndex = 1;
			while (true)
			{
				cCurrChar = argv[iCurrOptionIndex][iCurrCharIndex];
				if (cCurrChar == ':' || cCurrChar == '\0')
					break;
				else{
					pstrCurrOption[iCurrCharIndex - 1] = cCurrChar;
					pstrCurrOption[iCurrCharIndex] = '\0';
				}
					
				++iCurrCharIndex;
			}
			pstrCurrOption[iCurrCharIndex - 1] = '\0';

			if (strstr(argv[iCurrOptionIndex], ":"))
			{
				++iCurrCharIndex;
				iOptionSize = iCurrCharIndex;

				pstrCurrValue[0] = '\0';
				while (true)
				{
					if (iCurrCharIndex > (int)strlen(argv[iCurrOptionIndex]))
						break;
					else
					{
						cCurrChar = argv[iCurrOptionIndex][iCurrCharIndex];
						pstrCurrValue[iCurrCharIndex - iOptionSize] = cCurrChar;
					}
					++iCurrCharIndex;
				}
				pstrCurrValue[iCurrCharIndex - iOptionSize] = '\0';

				if (!strlen(pstrCurrValue))
				{
					sprintf(pstrErrorMssg, "Invalid value for -%s option", pstrCurrOption);
					ExitOnError(pstrErrorMssg);
				}
			}
			if (stricmp(pstrCurrOption, "S") == 0)
			{
				g_ScriptHeader->SetStackSize(atoi(pstrCurrValue));
			}
			else if (stricmp(pstrCurrOption, "P") == 0)
			{
				if (stricmp(pstrCurrValue, PRIORITY_LOW_KEYWORD) == 0)
				{
					g_ScriptHeader->SetPriorityType(PRIORITY_LOW);
				}
				else if (stricmp(pstrCurrValue, PRIORITY_MED_KEYWORD) == 0)
				{
					g_ScriptHeader->SetPriorityType(PRIORITY_MED);
				}
				else if (stricmp(pstrCurrValue, PRIORITY_HIGH_KEYWORD) == 0)
				{
					g_ScriptHeader->SetPriorityType(PRIORITY_HIGH);
				}
				else
				{
					g_ScriptHeader->SetPriorityType(PRIORITY_USER);
					g_ScriptHeader->SetUserPriority(atoi(pstrCurrValue));
				}
			}
			else if (stricmp(pstrCurrOption, "A") == 0)
			{
				this->g_iGenerateVSE = false;
				this->g_iPreserveOutputFile = true;
			}
			else
			{
				sprintf(pstrErrorMssg, "Unrecognized option: \"%s\"", pstrCurrOption);
				ExitOnError(pstrErrorMssg);
			}
		}
	}
}
void Compiler::ExitOnError(char* pstrErrorMssg)
{
	printf("Fatal Error: %s.\n", pstrErrorMssg);

	Exit();
}
void Compiler::LoadSourceFile(SourceCode* g_SourceCode)
{
	FILE* g_pSourceFile;
	if (fopen_s(&g_pSourceFile, this->g_ptrSourceFilename, "rb"))
		this->ExitOnError("Could not open file");
	while (!feof(g_pSourceFile))
	{
		char* pstrCurrLine = (char*)malloc(MAX_SOURCE_LINE_SIZE + 1);
		pstrCurrLine[0] = '\0';
		fgets(pstrCurrLine, MAX_SOURCE_LINE_SIZE, g_pSourceFile);
		g_SourceCode->Append(pstrCurrLine);
	}
}

void Compiler::testBuffer(SourceCode* g_SourceCode)
{
	int iInBlockComment = false;
	int iInString = false;
	char* temp = 0;
	char* test1 = 0;

	DLItr<char*> pNode = g_SourceCode->GetIt();

	while (true)
	{
		char* pstrCurrLine = pNode.Item();
		pNode.Forth();
		if (!pNode.Valid())
			break;
	}

	//Array2D<int>* fun1 = new Array2D<int>();
	//fun1->setArr(this->caseQueue.Front());
	//int fun2 = fun1->Get(2, 0);
}
void Compiler::PreprocessSourceFile(SourceCode* g_SourceCode, Lexer* g_Lexer)
{
	
	this->c_Lexer = g_Lexer;
	int iInBlockComment = false;
	int iInString = false;
	int* temp = 0;
	char* test1 = 0;
	int length2 = 0;
	int test = 0;
	int tcase = 0;
	int caseCounter = 0;
	int number = 0;
	Array2D<int> caseArr(10, 2);
	int* hold = 0;
	int expressionx = 0;

	DLItr<char*> pNode = g_SourceCode->GetIt();

	while (true)
	{
		char* pstrCurrLine = pNode.Item();
		for (int iCurrCharIndex = 0; iCurrCharIndex < (int)strlen(pstrCurrLine); ++iCurrCharIndex)
		{
			if (pstrCurrLine[iCurrCharIndex] == '"')
			{
				if (iInString)
					iInString = false;
				else
					iInString = true;
			}
			if (pstrCurrLine[iCurrCharIndex] == '/' &&
				pstrCurrLine[iCurrCharIndex + 1] == '/' &&
				!iInString && !iInBlockComment)
			{
				pstrCurrLine[iCurrCharIndex] = '\n';
				pstrCurrLine[iCurrCharIndex + 1] = '\0';
				break;
			}
			if (pstrCurrLine[iCurrCharIndex] == '/' &&
				pstrCurrLine[iCurrCharIndex + 1] == '*' &&
				!iInString && !iInBlockComment)
			{
				iInBlockComment = true;
			}
			if (pstrCurrLine[iCurrCharIndex] == '*' &&
				pstrCurrLine[iCurrCharIndex + 1] == '/' &&
				iInBlockComment)//
			{
				pstrCurrLine[iCurrCharIndex] = ' ';
				pstrCurrLine[iCurrCharIndex + 1] = ' ';
				iInBlockComment = false;
			}
			if (iInBlockComment)
			{
				if (pstrCurrLine[iCurrCharIndex] != '\n')
					pstrCurrLine[iCurrCharIndex] = ' ';
			}
		}
		this->setBuffer(pstrCurrLine);//ident not good
		test = this->GetNextToken(0);

		if (test == TOKEN_TYPE_RSRVD_SWITCH || tcase == 1)
		{

			tcase = 1;
			//test = this->GetNextToken();
			
			if (test == TOKEN_TYPE_RSRVD_CASE)
			{
				this->caseCount++;
				//get the value from the buffer before getnexttoken
				number = this->GetNumber();
				int token = this->GetNextToken(0);
				
				switch (token)
				{
				case TOKEN_TYPE_STRING:
					printf("case statement cannot be a string");
					break;
				case TOKEN_TYPE_INT:
					caseArr.Get(this->caseCount, 0) = number;
					break;

				}
			}

			if (test == TOKEN_TYPE_RSRVD_DEFAULT)
			{
				//hold = caseArr.GetArr();
				g_Lexer->cst.Enqueue(caseArr);
				//this->caseCount = 0;
				//this->c_Lexer->
				tcase = 0;

			}


		}

		pNode.Forth();
		//this->clearBuffer();
		if (!pNode.Valid())
			break;
	}
}

int Compiler::GetNumber()
{
	int length = strlen(this->buffer);
	//int test = ':';
	char* newBuffer = (char*)malloc(length + 2);
	int counter = 0;
	int flag = 0;
	while (counter < length)
	{
		if (flag == 1)
		{
			counter++;
			continue;
		}
		if (this->buffer[counter] != ':')
			newBuffer[counter] = this->buffer[counter];
		else
		{
			flag = 1;
			newBuffer[counter] = '\0';

		}


		counter++;
	}
	length = strlen(newBuffer);
	//newBuffer[length] = '\0';
	int icase = atoi(newBuffer);
	free(newBuffer);
	return icase;
}

/******************************************************************************************
*
*   GetNextToken ()
*
*   Returns the next token in the source buffer.
*/

Token Compiler::GetNextToken(int flag)
{
	// Save the current lexer state for future rewinding

	//CopyLexerState(g_PrevLexerState, g_CurrLexerState);

	// Start the new lexeme at the end of the last one

	//g_CurrLexerState->SetCurrLexemeStart(g_CurrLexerState->GetCurrLexemeEnd());

	// Set the initial state to the start state

	int iCurrLexState = LEX_STATE_START;

	// Set the current operator state

	int iCurrOpCharIndex = 0;
	int iCurrOpStateIndex = 0;
	OpState CurrOpState;
	char pstrCurrLexeme[MAX_LEXEME_SIZE];

	// Flag to determine when the lexeme is done

	int iLexemeDone = false;

	// ---- Loop until a token is completed

	// Current character

	char cCurrChar;

	// Current position in the lexeme string buffer

	int iNextLexemeCharIndex = 0;

	// Should the current character be included in the lexeme?

	int iAddCurrChar;

	// Begin the loop

	while (true)
	{
		// Read the next character, and exit if the end of the source has been reached

		cCurrChar = GetNextChar();
		if (cCurrChar == '\0')
			break;

		// Assume the character will be added to the lexeme

		iAddCurrChar = true;

		// Depending on the current state of the lexer, handle the incoming character

		switch (iCurrLexState)
		{
			// If an unknown state occurs, the token is invalid, so exit

		case LEX_STATE_UNKNOWN:

			iLexemeDone = true;

			break;

			// The start state

		case LEX_STATE_START:

			// Just loop past whitespace, and don't add it to the lexeme

			if (this->c_Lexer->IsCharWhitespace(cCurrChar))
			{
				//++g_CurrLexerState->iCurrLexemeStart;
				iAddCurrChar = false;
			}

			// An integer is starting

			else if (this->c_Lexer->IsCharNumeric(cCurrChar))
			{
				iCurrLexState = LEX_STATE_INT;
			}

			// A float is starting

			else if (cCurrChar == '.')
			{
				iCurrLexState = LEX_STATE_FLOAT;
			}

			// An identifier is starting

			else if (this->c_Lexer->IsCharIdent(cCurrChar))
			{
				iCurrLexState = LEX_STATE_IDENT;
			}

			// A delimiter has been read

			else if (this->c_Lexer->IsCharDelim(cCurrChar))
			{
				iCurrLexState = LEX_STATE_DELIM;
			}

			// An operator is starting

			else if (this->c_Lexer->IsCharOpChar(cCurrChar, 0))
			{
				// Get the index of the initial operand state

				iCurrOpStateIndex = this->c_Lexer->GetOpStateIndex(cCurrChar, 0, 0, 0);
				if (iCurrOpStateIndex == -1)
					return TOKEN_TYPE_INVALID;

				// Get the full state structure

				CurrOpState = this->c_Lexer->GetOpState(0, iCurrOpStateIndex);

				// Move to the next character in the operator (1)

				iCurrOpCharIndex = 1;

				// Set the current operator

				this->c_Lexer->g_CurrLexerState->SetCurrOp(CurrOpState.iIndex);

				iCurrLexState = LEX_STATE_OP;
			}

			// A string is starting, but don't add the opening quote to the lexeme

			else if (cCurrChar == '"')
			{
				iAddCurrChar = false;
				iCurrLexState = LEX_STATE_STRING;
			}

			// It's invalid

			else
				iCurrLexState = LEX_STATE_UNKNOWN;

			break;

			// Integer

		case LEX_STATE_INT:

			// If a numeric is read, keep the state as-is

			if (this->c_Lexer->IsCharNumeric(cCurrChar))
			{
				iCurrLexState = LEX_STATE_INT;
			}

			// If a radix point is read, the numeric is really a float

			else if (cCurrChar == '.')
			{
				iCurrLexState = LEX_STATE_FLOAT;
			}
			else if (cCurrChar == ':')
			{
				iCurrLexState = LEX_STATE_INT;
			}

			// If whitespace or a delimiter is read, the lexeme is done

			else if (this->c_Lexer->IsCharWhitespace(cCurrChar) || this->c_Lexer->IsCharDelim(cCurrChar))
			{
				iAddCurrChar = false;
				iLexemeDone = true;
			}

			// Anything else is invalid

			else
				iCurrLexState = LEX_STATE_UNKNOWN;

			break;

			// Floating-point

		case LEX_STATE_FLOAT:

			// If a numeric is read, keep the state as-is

			if (this->c_Lexer->IsCharNumeric(cCurrChar))
			{
				iCurrLexState = LEX_STATE_FLOAT;
			}

			// If whitespace or a delimiter is read, the lexeme is done

			else if (this->c_Lexer->IsCharWhitespace(cCurrChar) || this->c_Lexer->IsCharDelim(cCurrChar))
			{
				iLexemeDone = true;
				iAddCurrChar = false;
			}

			// Anything else is invalid

			else
				iCurrLexState = LEX_STATE_UNKNOWN;

			break;

			// Identifier

		case LEX_STATE_IDENT:

			// If an identifier character is read, keep the state as-is

			if (this->c_Lexer->IsCharIdent(cCurrChar))
			{
				iCurrLexState = LEX_STATE_IDENT;
			}

			// If whitespace or a delimiter is read, the lexeme is done

			else if (this->c_Lexer->IsCharWhitespace(cCurrChar) || this->c_Lexer->IsCharDelim(cCurrChar))
			{
				iAddCurrChar = false;
				iLexemeDone = true;
			}
			else if (cCurrChar == ':')
			{
				iCurrLexState = LEX_STATE_IDENT;
				iAddCurrChar = false;
				iLexemeDone = true;
			}

			// Anything else is invalid

			else
				iCurrLexState = LEX_STATE_UNKNOWN;

			break;

			// Operator

			// Delimiter

		case LEX_STATE_DELIM:

			// Don't add whatever comes after the delimiter to the lexeme, because
			// it's done

			iAddCurrChar = false;
			iLexemeDone = true;

			break;

			// String

		case LEX_STATE_STRING:

			// If the current character is a closing quote, finish the lexeme

			if (cCurrChar == '"')
			{
				iAddCurrChar = false;
				iCurrLexState = LEX_STATE_STRING_CLOSE_QUOTE;
			}

			// If it's a newline, the string token is invalid

			else if (cCurrChar == '\n')
			{
				iAddCurrChar = false;
				iCurrLexState = LEX_STATE_UNKNOWN;
			}

			// If it's an escape sequence, switch to the escape state and don't add the
			// backslash to the lexeme

			else if (cCurrChar == '\\')
			{
				iAddCurrChar = false;
				iCurrLexState = LEX_STATE_STRING_ESCAPE;
			}

			// Anything else gets added to the string

			break;

			// Escape sequence

		case LEX_STATE_STRING_ESCAPE:

			// Immediately switch back to the string state, now that the character's
			// been added

			iCurrLexState = LEX_STATE_STRING;

			break;

			// String closing quote

		case LEX_STATE_STRING_CLOSE_QUOTE:

			// Finish the string lexeme

			iAddCurrChar = false;
			iLexemeDone = true;

			break;
		}

		// Add the next character to the lexeme and increment the index

		if (iAddCurrChar)
		{
			pstrCurrLexeme[iNextLexemeCharIndex] = cCurrChar;
			++iNextLexemeCharIndex;
		}

		// If the lexeme is complete, exit the loop

		if (iLexemeDone)
			break;
	}

	pstrCurrLexeme[iNextLexemeCharIndex] = '\0';
	//--iCurrLexemeEnd;

	Token TokenType = 0;
	switch (iCurrLexState)
	{
		// Unknown

	case LEX_STATE_UNKNOWN:
		TokenType = TOKEN_TYPE_INVALID;
		break;

		// Integer

	case LEX_STATE_INT:
		TokenType = TOKEN_TYPE_INT;
		break;

		// Float

	case LEX_STATE_FLOAT:
		TokenType = TOKEN_TYPE_FLOAT;
		break;

		// Identifier/Reserved Word

	case LEX_STATE_IDENT:

		// Set the token type to identifier in case none of the reserved words match

		TokenType = TOKEN_TYPE_IDENT;

		// ---- Determine if the "identifier" is actually a reserved word

		// var/var []

		if (stricmp(pstrCurrLexeme, "var") == 0)
			TokenType = TOKEN_TYPE_RSRVD_VAR;
		if (stricmp(pstrCurrLexeme, "true") == 0)
			TokenType = TOKEN_TYPE_RSRVD_TRUE;
		if (stricmp(pstrCurrLexeme, "false") == 0)
			TokenType = TOKEN_TYPE_RSRVD_FALSE;
		if (stricmp(pstrCurrLexeme, "if") == 0)
			TokenType = TOKEN_TYPE_RSRVD_IF;
		if (stricmp(pstrCurrLexeme, "else") == 0)
			TokenType = TOKEN_TYPE_RSRVD_ELSE;
		if (stricmp(pstrCurrLexeme, "break") == 0)
			TokenType = TOKEN_TYPE_RSRVD_BREAK;
		if (stricmp(pstrCurrLexeme, "continue") == 0)
			TokenType = TOKEN_TYPE_RSRVD_CONTINUE;
		if (stricmp(pstrCurrLexeme, "for") == 0)
			TokenType = TOKEN_TYPE_RSRVD_FOR;
		if (stricmp(pstrCurrLexeme, "while") == 0)
			TokenType = TOKEN_TYPE_RSRVD_WHILE;
		if (stricmp(pstrCurrLexeme, "func") == 0)
			TokenType = TOKEN_TYPE_RSRVD_FUNC;
		if (stricmp(pstrCurrLexeme, "return") == 0)
			TokenType = TOKEN_TYPE_RSRVD_RETURN;
		if (stricmp(pstrCurrLexeme, "host") == 0)
			TokenType = TOKEN_TYPE_RSRVD_HOST;
		if (stricmp(pstrCurrLexeme, "switch") == 0)
			TokenType = TOKEN_TYPE_RSRVD_SWITCH;
		if (stricmp(pstrCurrLexeme, "case") == 0)
			TokenType = TOKEN_TYPE_RSRVD_CASE;
		if (stricmp(pstrCurrLexeme, "default") == 0)
			TokenType = TOKEN_TYPE_RSRVD_DEFAULT;
		if (stricmp(pstrCurrLexeme, "colon") == 0)
			TokenType = TOKEN_TYPE_RSRVD_COLON;

		else
		{
			if (flag == 1)
				TokenType = TOKEN_TYPE_INT;
			strcpy(this->ident, pstrCurrLexeme);
		}

		break;

		// Delimiter

	case LEX_STATE_DELIM:
		switch (pstrCurrLexeme[0])
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

			// Operators

		case LEX_STATE_OP:
			TokenType = TOKEN_TYPE_OP;
			break;

			// Strings

		case LEX_STATE_STRING_CLOSE_QUOTE:
			TokenType = TOKEN_TYPE_STRING;
			break;

			// All that's left is whitespace, which means the end of the stream

		default:
			TokenType = TOKEN_TYPE_END_OF_STREAM;
		}
		break;
	}

	// Return the token type and set the global copy

	//g_CurrLexerState->SetCurrToken(TokenType);
	return TokenType;
}


char Compiler::GetNextChar()
{
	this->index++;
	return *(this->buffer++);
}

char Compiler::GetPrevChar()
{
	this->index--;
	return *(this->buffer--);
}

////

void Compiler::CompileSourceFile(SymbolList* g_SL)
{
	// Add two temporary variables for evaluating expressions

	//this->g_iTempVar0SymbolIndex = g_SL->AddSymbol(TEMP_VAR_0, 1, SCOPE_GLOBAL, SYMBOL_TYPE_VAR);
	//this->g_iTempVar1SymbolIndex = g_SL->AddSymbol(TEMP_VAR_1, 1, SCOPE_GLOBAL, SYMBOL_TYPE_VAR);
	// Parse the source file to create an I-code representation
	//Parser* g_Parser = new Parser;

}


//Compiler& Compiler::getInstance()
//{

//}
