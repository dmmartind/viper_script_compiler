//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////


#include "Error.h"
#include "Compiler.h"
#include "Lexer.h"


void ExitOnCodeError(char* pstrErrorMssg, Lexer* g_Lexer)
{


	printf("Error: %s.\n\n", pstrErrorMssg);
	printf("Line %d\n", g_Lexer->GetCurrSourceLineIndex());

	char pstrSourceLine[MAX_SOURCE_LINE_SIZE];

	char* pstrCurrSourceLine = g_Lexer->GetCurrSourceLine();
	if (pstrCurrSourceLine)
		strcpy(pstrSourceLine, pstrCurrSourceLine);
	else
		pstrSourceLine[0] = '\0';
	int iLastCharIndex = strlen(pstrSourceLine) - 1;
	if (pstrSourceLine[iLastCharIndex] == '\n')
		pstrSourceLine[iLastCharIndex] = '\0';

	for (unsigned int iCurrCharIndex = 0; iCurrCharIndex < strlen(pstrSourceLine); ++iCurrCharIndex)
	if (pstrSourceLine[iCurrCharIndex] == '\t')
		pstrSourceLine[iCurrCharIndex] = ' ';

	printf("%s\n", pstrSourceLine);

	for (int iCurrSpace = 0; iCurrSpace < g_Lexer->GetLexemeStartIndex(); ++iCurrSpace)
		printf(" ");
	printf("^\n");

	printf("Could not compile %s. ", Compiler::getInstance()->GetSource());

	Compiler::getInstance()->Exit();

}