//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////

#include "Parser.h"
#include "Compiler.h"
#include "Lexer.h"
#include "Error.h"
#include "ScriptHeader.h"
#include "SymbolList.h"



Parser::Parser(Lexer* _g_Lexer, ScriptHeader* _g_ScriptHeader, SymbolList* _g_SymbolList, ICodeStream<ICodeNode>* _g_ICode, DList<char*> _g_StringTable, Stack<Loop*> _g_stack)
{
	g_Lexer = _g_Lexer;
	g_ScriptHeader = _g_ScriptHeader;
	g_SymbolList = _g_SymbolList;
	g_ICode = _g_ICode;
	g_StringTable = _g_StringTable;
	g_Stack = _g_stack;
	currentSyntax = 0;
	this->SetAX(g_SymbolList->AddSymbol(TEMP_VAR_0, 1, SCOPE_GLOBAL, SYMBOL_TYPE_VAR));
	this->SetBX(g_SymbolList->AddSymbol(TEMP_VAR_1, 1, SCOPE_GLOBAL, SYMBOL_TYPE_VAR));
	iExtraCurrentOp = 0;

}

Parser::~Parser()
{

}
// ---- Functions -----------------------------------------------------------------------------

   int Parser::ReadToken ( Token ReqToken )
    {
       

		if (g_Lexer->GetNextToken(0) != ReqToken)
        {
            

            char pstrErrorMssg [ 256 ];
            switch ( ReqToken )
            {   
                

                case TOKEN_TYPE_INT:
                    strcpy ( pstrErrorMssg, "Integer" );
                    break;

                

                case TOKEN_TYPE_FLOAT:
                    strcpy ( pstrErrorMssg, "Float" );
                    break;

                

                case TOKEN_TYPE_IDENT:
                    strcpy ( pstrErrorMssg, "Identifier" );
                    break;

                

                case TOKEN_TYPE_RSRVD_VAR:
                    strcpy ( pstrErrorMssg, "var" );
                    break;

                

                case TOKEN_TYPE_RSRVD_TRUE:
                    strcpy ( pstrErrorMssg, "true" );
                    break;

                

                case TOKEN_TYPE_RSRVD_FALSE:
                    strcpy ( pstrErrorMssg, "false" );
                    break;

                

                case TOKEN_TYPE_RSRVD_IF:
                    strcpy ( pstrErrorMssg, "if" );
                    break;

                

                case TOKEN_TYPE_RSRVD_ELSE:
                    strcpy ( pstrErrorMssg, "else" );
                    break;

                

                case TOKEN_TYPE_RSRVD_BREAK:
                    strcpy ( pstrErrorMssg, "break" );
                    break;

                

                case TOKEN_TYPE_RSRVD_CONTINUE:
                    strcpy ( pstrErrorMssg, "continue" );
                    break;

                

                case TOKEN_TYPE_RSRVD_FOR:
                    strcpy ( pstrErrorMssg, "for" );
                    break;

                

                case TOKEN_TYPE_RSRVD_WHILE:
                    strcpy ( pstrErrorMssg, "while" );
                    break;

                

                case TOKEN_TYPE_RSRVD_FUNC:
                    strcpy ( pstrErrorMssg, "func" );
                    break;

                

                case TOKEN_TYPE_RSRVD_RETURN:
                    strcpy ( pstrErrorMssg, "return" );
                    break;

                

                case TOKEN_TYPE_RSRVD_HOST:
                    strcpy ( pstrErrorMssg, "host" );
                    break;

                

                case TOKEN_TYPE_OP:
                    strcpy ( pstrErrorMssg, "Operator" );
                    break;

                

                case TOKEN_TYPE_DELIM_COMMA:
                    strcpy ( pstrErrorMssg, "," );
                    break;

                

                case TOKEN_TYPE_DELIM_OPEN_PAREN:
                    strcpy ( pstrErrorMssg, "(" );
                    break;

                

                case TOKEN_TYPE_DELIM_CLOSE_PAREN:
                    strcpy ( pstrErrorMssg, ")" );
                    break;

                

                case TOKEN_TYPE_DELIM_OPEN_BRACE:
                    strcpy ( pstrErrorMssg, "[" );
                    break;

                // Close brace

                case TOKEN_TYPE_DELIM_CLOSE_BRACE:
                    strcpy ( pstrErrorMssg, "]" );
                    break;

                // Open curly brace

                case TOKEN_TYPE_DELIM_OPEN_CURLY_BRACE:
                    strcpy ( pstrErrorMssg, "{" );
                    break;

                // Close curly brace

                case TOKEN_TYPE_DELIM_CLOSE_CURLY_BRACE:
                    strcpy ( pstrErrorMssg, "}" );
                    break;

                // Semicolon

                case TOKEN_TYPE_DELIM_SEMICOLON:
                    strcpy ( pstrErrorMssg, ";" );
                    break;

                // String

                case TOKEN_TYPE_STRING:
                    strcpy ( pstrErrorMssg, "String" );
                    break;

				case TOKEN_TYPE_DELIM_COLON:
					strcpy(pstrErrorMssg, "Colon");
				default:
					strcpy(pstrErrorMssg, "Something happened that wasn't expected!!!!");
            }

            // Finish the message

            strcat ( pstrErrorMssg, " expected" );

            // Display the error

            ExitOnCodeError( pstrErrorMssg, g_Lexer );
			return 0;
        }
		else
			return 1;
    }

    /******************************************************************************************
    *
    *   IsOpRelational ()
    *
    *   Determines if the specified operator is a relational operator.
    */

    int Parser::IsOpRelational ( int iOpType )
    {
        if ( iOpType != OP_TYPE_EQUAL &&
             iOpType != OP_TYPE_NOT_EQUAL &&
             iOpType != OP_TYPE_LESS && 
             iOpType != OP_TYPE_GREATER &&
             iOpType != OP_TYPE_LESS_EQUAL && 
             iOpType != OP_TYPE_GREATER_EQUAL )
            return false;
        else
            return true;
    }

    /******************************************************************************************
    *
    *   IsOpLogical ()
    *
    *   Determines if the specified operator is a logical operator.
    */

    int Parser::IsOpLogical ( int iOpType )
    {
        if ( iOpType != OP_TYPE_LOGICAL_AND &&
             iOpType != OP_TYPE_LOGICAL_OR &&
             iOpType != OP_TYPE_LOGICAL_NOT )
            return false;
        else
            return true;
    }

    /******************************************************************************************
    *
    *   ParseSourceCode ()
    *
    *   Parses the source code from start to finish, generating a complete I-code
    *   representation.
    */

    void Parser::ParseSourceCode ()
    {
        // Reset the lexer

        g_Lexer->ResetLexer();

        // Initialize the loop stack

        //InitStack ( & g_LoopStack );
		Loop* data = new Loop;
		g_Stack.Push(data);

        // Set the current scope to global

        g_iCurrScope = SCOPE_GLOBAL;

        // Parse each line of code

        while ( true )
        {
            // Parse the next statement and ignore an end of file marker

            ParseStatement( 0, "a" );

            // If we're at the end of the token stream, break the parsing loop

            if ( g_Lexer->GetNextToken(0) == TOKEN_TYPE_END_OF_STREAM )
                break;
            else
                g_Lexer->RewindTokenStream();
        }

        // Free the loop stack

		g_Stack.Pop();
    }

	/******************************************************************************************
	*
	*   ParseSwitch ()
	*
	*   Parses a switch block.
	*
	*       switch( <variable> ) case <value>: <statement> break; default: <statement> break;
	*/

	void Parser::ParseSwitch()//write the icode for switch
	{
		int iInstrIndex;
		int casenum = 0;
		if (g_iCurrScope == SCOPE_GLOBAL)
			ExitOnCodeError("if illegal in global scope", this->g_Lexer);

		g_ICode->AddICodeSourceLine(g_iCurrScope, g_Lexer->GetCurrSourceLine());

		ReadToken(TOKEN_TYPE_DELIM_OPEN_PAREN);

		ParseExpr();

		ReadToken(TOKEN_TYPE_DELIM_CLOSE_PAREN);

		ReadToken(TOKEN_TYPE_DELIM_OPEN_CURLY_BRACE);

		ParseSwitchStatement(1);

		ReadToken(TOKEN_TYPE_DELIM_CLOSE_CURLY_BRACE);
				
	}

	/******************************************************************************************
	*
	*   ParseCase ()
	*
	*   Parses a case block.
	*
	*       case <value>: <statement> break;
	*/
	void Parser::ParseCase(int flag)
	{
		//Queue<Array2D<int>> this->g_Lexer->cst = g_Lexer->cst;
		Token test;
		int counter = 0;
		int iInstrIndex;
		if (g_iCurrScope == SCOPE_GLOBAL)
			ExitOnCodeError("for illegal on scope global", this->g_Lexer);
		//int width = this->g_Lexer->cst.Front().Width();
		//int height = this->g_Lexer->cst.Front().Height();
		//Array2D<int> caseInfo(width, height);
		//caseInfo = this->g_Lexer->cst.Front();
		//caseInfo.this->g_Lexer->cst.Front();
		//if (caseInfo.Size() >= 0)
			g_ICode->AddICodeJumpTarget(g_iCurrScope, g_ICode->GetNextJumpTargetIndex());
		if (flag == 1)
			ReadToken(TOKEN_TYPE_RSRVD_CASE);
		ParseExpr();
		ReadToken(TOKEN_TYPE_DELIM_COLON);

		while (test = g_Lexer->GetNextToken(0))
		{
			if (test == TOKEN_TYPE_RSRVD_BREAK || test == TOKEN_TYPE_RSRVD_CASE)
				break;
			g_Lexer->RewindTokenStream();
			this->ParseStatement(0, "vvv");
		}
		

		//while (g_Lexer->GetLookAheadChar() != )
		//{
		//	if (test == TOKEN_TYPE_RSRVD_BREAK || test == TOKEN_TYPE_RSRVD_CASE)
		//		break;
		//	g_Lexer->RewindTokenStream();
		//	//if( ReadToken( TOKEN_TYPE_RSRVD_BREAK ) )
		//	//	break;
		//	ParseStatement(3);
		//}
		g_Lexer->RewindTokenStream();
		ReadToken(TOKEN_TYPE_RSRVD_BREAK);
		ReadToken(TOKEN_TYPE_DELIM_SEMICOLON);
		//Compiler::getInstance()->IncCaseCount();

	}

	/******************************************************************************************
	*
	*   ParseDefault()
	*
	*   Parses a default block.
	*
	*       default: <statement> break;
	*/
	void Parser::ParseDefault()
	{

		int iInstrIndex;
		if (g_iCurrScope == SCOPE_GLOBAL)
			ExitOnCodeError("for illegal on scope global", this->g_Lexer);
		ReadToken(TOKEN_TYPE_DELIM_COLON);
		g_ICode->AddICodeJumpTarget(g_iCurrScope, g_ICode->GetNextJumpTargetIndex());
		while (g_Lexer->GetNextToken(0) != TOKEN_TYPE_RSRVD_BREAK)
		{
			g_Lexer->RewindTokenStream();
			this->ParseStatement(0, "vvv");
		}
		g_Lexer->RewindTokenStream();
		ReadToken(TOKEN_TYPE_RSRVD_BREAK);
		ReadToken(TOKEN_TYPE_DELIM_SEMICOLON);

	}


    void Parser::ParseStatement(int flag, char* synt)
    {
        

        if ( g_Lexer->GetLookAheadChar() == ';' )
        {
            ReadToken( TOKEN_TYPE_DELIM_SEMICOLON );
            return;
        }

        

        Token InitToken = g_Lexer->GetNextToken( 0 );

        

        switch ( InitToken )
        {
            

            case TOKEN_TYPE_END_OF_STREAM:
                ExitOnCodeError( "Unexpected end of file", g_Lexer );
                break;

            

            case TOKEN_TYPE_DELIM_OPEN_CURLY_BRACE:
                ParseBlock ();
                break;

            

            case TOKEN_TYPE_RSRVD_VAR:
                ParseVar ();
                break;

            

            case TOKEN_TYPE_RSRVD_HOST:
                ParseHost ();
                break;

            

            case TOKEN_TYPE_RSRVD_FUNC:
                ParseFunc ();
                break;

            

            case TOKEN_TYPE_RSRVD_IF:
                ParseIf ();
                break;

            

            case TOKEN_TYPE_RSRVD_WHILE:
                ParseWhile ();
                break;

			case TOKEN_TYPE_RSRVD_SWITCH:
				ParseSwitch();
				break;

            

            case TOKEN_TYPE_RSRVD_FOR:
				this->setSyntax(TOKEN_TYPE_RSRVD_FOR);
                ParseFor ();
                break;

            

            case TOKEN_TYPE_RSRVD_BREAK:
                ParseBreak (0);
                break;

            

            case TOKEN_TYPE_RSRVD_CONTINUE:
                ParseContinue ();
                break;

            

            case TOKEN_TYPE_RSRVD_RETURN:
                ParseReturn ();
                break;

			

            case TOKEN_TYPE_IDENT:
            {
				

				if (synt == "for")
				{
					char pstrIdent[MAX_LEXEME_SIZE];
					g_Lexer->CopyCurrLexeme(pstrIdent);
					int iSize = atoi(g_Lexer->GetCurrLexeme());
					if (iSize == 0)
						iSize = 1;

					g_SymbolList->AddSymbol(pstrIdent, iSize, g_iCurrScope, SYMBOL_TYPE_VAR);
					int lexemeIndex = g_SymbolList->getCount() - 2;
					Loop* loop = g_Stack.Top();
					g_Stack.Pop();
					loop->setLexeme(lexemeIndex);
					g_Stack.Push(loop);
				}
                

				if (g_SymbolList->GetSymbolByIdent(g_Lexer->GetCurrLexeme(), g_iCurrScope))
                {
                   

                    ParseAssign( flag );
                }
				else if (FunctionList::getInstance()->GetFuncByName(g_Lexer->GetCurrLexeme()))
                {
                    

					g_ICode->AddICodeSourceLine(g_iCurrScope, g_Lexer->GetCurrSourceLine());
                    ParseFuncCall ();

                    

                    ReadToken ( TOKEN_TYPE_DELIM_SEMICOLON );
                }
                else
                {
                   

                    ExitOnCodeError ( "Invalid identifier", g_Lexer );
                }

                break;
            }

            

            default:
                ExitOnCodeError ( "Unexpected input", g_Lexer );
                break;
        }
    }

    

    void Parser::ParseBlock(int flag)//replicate this functionality for the for loop
    {
        

        if ( g_iCurrScope == SCOPE_GLOBAL )
            ExitOnCodeError ( "Code blocks illegal in global scope", g_Lexer );

       

		while ( g_Lexer->GetLookAheadChar() != '}' )
            ParseStatement( 0, "vvv" );

        

        ReadToken ( TOKEN_TYPE_DELIM_CLOSE_CURLY_BRACE );
    }

   

    void Parser::ParseVar ()
    {
        

        ReadToken ( TOKEN_TYPE_IDENT );

        

        char pstrIdent [ MAX_LEXEME_SIZE ];
		g_Lexer->CopyCurrLexeme(pstrIdent);

       

        int iSize = 1;

        

		if (g_Lexer->GetLookAheadChar() == '[')
        {
            

            ReadToken ( TOKEN_TYPE_DELIM_OPEN_BRACE );

            

            ReadToken ( TOKEN_TYPE_INT );

            

			iSize = atoi(g_Lexer->GetCurrLexeme());

            

            ReadToken ( TOKEN_TYPE_DELIM_CLOSE_BRACE );
        }

        

		if (g_SymbolList->AddSymbol(pstrIdent, iSize, g_iCurrScope, SYMBOL_TYPE_VAR) == -1)
			ExitOnCodeError("Identifier redefinition", g_Lexer);

        

        ReadToken ( TOKEN_TYPE_DELIM_SEMICOLON );
    }

    

    void Parser::ParseHost ()
    {
        

        ReadToken ( TOKEN_TYPE_IDENT );

        

		if (FunctionList::getInstance()->AddFunc(g_Lexer->GetCurrLexeme(), true) == -1)
			ExitOnCodeError("Function redefinition", g_Lexer);

        

        ReadToken ( TOKEN_TYPE_DELIM_OPEN_PAREN );
        ReadToken ( TOKEN_TYPE_DELIM_CLOSE_PAREN );

        

        ReadToken ( TOKEN_TYPE_DELIM_SEMICOLON );
    }

   

    void Parser::ParseFunc ()
    {
        

        if ( g_iCurrScope != SCOPE_GLOBAL )
			ExitOnCodeError("Nested functions illegal", g_Lexer);

        

        ReadToken ( TOKEN_TYPE_IDENT );

        

		int iFuncIndex = FunctionList::getInstance()->AddFunc(g_Lexer->GetCurrLexeme(), false);

       

        if ( iFuncIndex == -1 )
			ExitOnCodeError("Function redefinition", g_Lexer);

        

        g_iCurrScope = iFuncIndex;

       

        ReadToken ( TOKEN_TYPE_DELIM_OPEN_PAREN );
       
        

		if (g_Lexer->GetLookAheadChar() != ')')
        {   
            

			if (g_ScriptHeader->GetMainFuncPresent() &&
                 g_ScriptHeader->GetMainFuncIndex() == iFuncIndex )
            {
                ExitOnCodeError ( "_Main () cannot accept parameters", g_Lexer );
            }

            

            int iParamCount = 0;

            

            char ppstrParamList [ MAX_FUNC_DECLARE_PARAM_COUNT ][ MAX_IDENT_SIZE ];

            

            while ( true )
            {
                

                ReadToken ( TOKEN_TYPE_IDENT );

                

				g_Lexer->CopyCurrLexeme(ppstrParamList[iParamCount]);

                

                ++ iParamCount;

                

                if ( g_Lexer->GetLookAheadChar () == ')' )
                    break;

                

                ReadToken ( TOKEN_TYPE_DELIM_COMMA );           
            }

           

			FunctionList::getInstance()->SetFuncParamCount(g_iCurrScope, iParamCount);

            

            while ( iParamCount > 0 )
            {
                -- iParamCount;

                

				g_SymbolList->AddSymbol(ppstrParamList[iParamCount], 1, g_iCurrScope, SYMBOL_TYPE_PARAM);
            }
        }

        

        ReadToken ( TOKEN_TYPE_DELIM_CLOSE_PAREN );

        

        ReadToken ( TOKEN_TYPE_DELIM_OPEN_CURLY_BRACE );

        

        ParseBlock ();

        

        g_iCurrScope = SCOPE_GLOBAL;
    }

    

    void Parser::ParseExpr ()
    {
        int iInstrIndex;

       

        int iOpType = -500;

		

        ParseSubExpr ();

        

        while ( true )
        {
           

			if (g_Lexer->GetNextToken(0) != TOKEN_TYPE_OP ||
				(!IsOpRelational(g_Lexer->GetCurrOp()) &&
				!IsOpLogical(g_Lexer->GetCurrOp())))
			{
				g_Lexer->RewindTokenStream();
				break;
			}

            
			iOpType = g_Lexer->GetCurrOp();
            
			
			if(this->getExtraCurrentOp() == TOKEN_TYPE_RSRVD_IF)
			{
				ParseSubExpr();
			}
			else
			{
				ParseExpr();
			}
			/////////?????????????????????????????????????
            
			//
            

			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar1SymbolIndex);

            

			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar0SymbolIndex);

           

            if ( IsOpRelational ( iOpType ) )
            {
                
				int iTrueJumpTargetIndex = 0;
				int iExitJumpTargetIndex = 0;
				

				//check this
				if (this->getSyntax() == TOKEN_TYPE_RSRVD_FOR)
				{
					iTrueJumpTargetIndex = g_ICode->GetNextJumpTargetIndex(),
					iExitJumpTargetIndex = this->g_Stack.Top()->GetEndTargetIndex();
				}
				else
				{
					iTrueJumpTargetIndex = g_ICode->GetNextJumpTargetIndex(),
					iExitJumpTargetIndex = g_ICode->GetNextJumpTargetIndex();
				}

				

                

                switch ( iOpType )
                {
                    

                    case OP_TYPE_EQUAL:
                    {
                        

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JE);
                        break;
                    }

                   

                    case OP_TYPE_NOT_EQUAL:
                    {
                        

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JNE);
                        break;
                    }

                    

                    case OP_TYPE_GREATER:
                    {
                        

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JG);
                        break;
                    }

                    

                    case OP_TYPE_LESS:
                    {
                        

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JL);
                        break;
                    }

                    

                    case OP_TYPE_GREATER_EQUAL:
                    {
                        

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JGE);
                        break;
                    }

                    

                    case OP_TYPE_LESS_EQUAL:
                    {
                       

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JLE);
                        break;
                    }
                }

               

				g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar0SymbolIndex);
				g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar1SymbolIndex);
				g_ICode->AddJumpTargetICodeOp(g_iCurrScope, iInstrIndex, iTrueJumpTargetIndex);

                

				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
				g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, 0);

               

				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JMP);
				g_ICode->AddJumpTargetICodeOp(g_iCurrScope, iInstrIndex, iExitJumpTargetIndex);

				//int test = this->g_Stack.Top()->GetEndTargetIndex();

				g_ICode->AddICodeJumpTarget(g_iCurrScope, iTrueJumpTargetIndex);

                

				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
				g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, 1);

                
				if (this->getSyntax() != TOKEN_TYPE_RSRVD_FOR)
				{
					g_ICode->AddICodeJumpTarget(g_iCurrScope, iExitJumpTargetIndex);
				}
				
				
            }
            else
            {
                

                switch ( iOpType )
                {
                    

                    case OP_TYPE_LOGICAL_AND:
                    {
							int iFalseJumpTargetIndex = g_ICode->GetNextJumpTargetIndex(),
								iExitJumpTargetIndex = g_ICode->GetNextJumpTargetIndex();

                        

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JE);
						g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar0SymbolIndex);
						g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, 0);
						g_ICode->AddJumpTargetICodeOp(g_iCurrScope, iInstrIndex, iFalseJumpTargetIndex);

                        

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JE);
						g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar1SymbolIndex);
						g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, 0);
						g_ICode->AddJumpTargetICodeOp(g_iCurrScope, iInstrIndex, iFalseJumpTargetIndex);

                        

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
						g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, 1);

                        

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JMP);
						g_ICode->AddJumpTargetICodeOp(g_iCurrScope, iInstrIndex, iExitJumpTargetIndex);

                        

						g_ICode->AddICodeJumpTarget(g_iCurrScope, iFalseJumpTargetIndex);

                        

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
						g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, 0);

                        

						g_ICode->AddICodeJumpTarget(g_iCurrScope, iExitJumpTargetIndex);

                        break;
                    }

                    

                    case OP_TYPE_LOGICAL_OR:
                    {
                        

							int iTrueJumpTargetIndex = g_ICode->GetNextJumpTargetIndex(),
								iExitJumpTargetIndex = g_ICode->GetNextJumpTargetIndex();

                        

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JNE);
						g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar0SymbolIndex);
						g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, 0);
						g_ICode->AddJumpTargetICodeOp(g_iCurrScope, iInstrIndex, iTrueJumpTargetIndex);

                        

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JNE);
						g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar1SymbolIndex);
						g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, 0);
						g_ICode->AddJumpTargetICodeOp(g_iCurrScope, iInstrIndex, iTrueJumpTargetIndex);

                       

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
						g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, 0);

                       

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JMP);
						g_ICode->AddJumpTargetICodeOp(g_iCurrScope, iInstrIndex, iExitJumpTargetIndex);

                        

						g_ICode->AddICodeJumpTarget(g_iCurrScope, iTrueJumpTargetIndex);

                        

						iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
						g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, 1);

                        

						g_ICode->AddICodeJumpTarget(g_iCurrScope, iExitJumpTargetIndex);

                        break;
                    }
                }
            }
			
			
        }
    }

   

    void Parser::ParseSubExpr ()
    {
        int iInstrIndex;

        

        int iOpType;

        

        ParseTerm (); 

        

        while ( true )
        {
            

			if (g_Lexer->GetNextToken( 0 ) != TOKEN_TYPE_OP ||
				(g_Lexer->GetCurrOp() != OP_TYPE_ADD &&
				g_Lexer->GetCurrOp() != OP_TYPE_SUB &&
				g_Lexer->GetCurrOp() != OP_TYPE_CONCAT))
            {
				g_Lexer->RewindTokenStream();
                break;
            }

            

			iOpType = g_Lexer->GetCurrOp();

            

            ParseTerm ();

            

			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar1SymbolIndex);

            

			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar0SymbolIndex);

            

            int iOpInstr;
            switch ( iOpType )
            {
               

                case OP_TYPE_ADD:
                    iOpInstr = INSTR_ADD;
                    break;

                

                case OP_TYPE_SUB:
                    iOpInstr = INSTR_SUB;
                    break;

                

                case OP_TYPE_CONCAT:
                    iOpInstr = INSTR_CONCAT;
                    break;
            }
			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, iOpInstr);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar0SymbolIndex);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar1SymbolIndex);

            

			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar0SymbolIndex);
        }
    }

    

    void Parser::ParseTerm ()
    {
        int iInstrIndex;

        

        int iOpType;

        

        ParseFactor (); 

        

        while ( true )
        {
            

            if ( g_Lexer->GetNextToken ( 0 ) != TOKEN_TYPE_OP ||
                 ( g_Lexer->GetCurrOp () != OP_TYPE_MUL &&
				 g_Lexer->GetCurrOp() != OP_TYPE_DIV &&
				 g_Lexer->GetCurrOp() != OP_TYPE_MOD &&
				 g_Lexer->GetCurrOp() != OP_TYPE_EXP &&
				 g_Lexer->GetCurrOp() != OP_TYPE_BITWISE_AND &&
				 g_Lexer->GetCurrOp() != OP_TYPE_BITWISE_OR &&
				 g_Lexer->GetCurrOp() != OP_TYPE_BITWISE_XOR &&
				 g_Lexer->GetCurrOp() != OP_TYPE_BITWISE_SHIFT_LEFT &&
				 g_Lexer->GetCurrOp() != OP_TYPE_BITWISE_SHIFT_RIGHT))
            {
				g_Lexer->RewindTokenStream();
                break;
            }

           

			iOpType = g_Lexer->GetCurrOp();

            
			ParseExpr();
            //ParseFactor ();

            

			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar1SymbolIndex);

           

			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar0SymbolIndex);

            

            int iOpInstr;
            switch ( iOpType )
            {
                

                case OP_TYPE_MUL:
                    iOpInstr = INSTR_MUL;
                    break;

               

                case OP_TYPE_DIV:
                    iOpInstr = INSTR_DIV;
                    break;

                

                case OP_TYPE_MOD:
                    iOpInstr = INSTR_MOD;
                    break;

                

                case OP_TYPE_EXP:
                    iOpInstr = INSTR_EXP;
                    break;

                

                case OP_TYPE_BITWISE_AND:
                    iOpInstr = INSTR_AND;
                    break;

               

                case OP_TYPE_BITWISE_OR:
                    iOpInstr = INSTR_OR;
                    break;

                

                case OP_TYPE_BITWISE_XOR:
                    iOpInstr = INSTR_XOR;
                    break;

                

                case OP_TYPE_BITWISE_SHIFT_LEFT:
                    iOpInstr = INSTR_SHL;
                    break;

                

                case OP_TYPE_BITWISE_SHIFT_RIGHT:
                    iOpInstr = INSTR_SHR;
                    break;
            }

			
			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, iOpInstr);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar0SymbolIndex);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar1SymbolIndex);

            

			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar0SymbolIndex);
        }
    }

   

    void Parser::ParseFactor ()
    {  
        int iInstrIndex;
        int iUnaryOpPending = false;
        int iOpType;
		char* nString = 0;
		int iStringIndex = 0;
		int tok = 0;


		SymbolNode * pSymbol = g_SymbolList->GetSymbolByIdent(g_Lexer->GetCurrLexeme(), g_iCurrScope);

        

		if (g_Lexer->GetNextToken( 0 ) == TOKEN_TYPE_OP &&
             ( g_Lexer->GetCurrOp () == OP_TYPE_ADD ||
               g_Lexer->GetCurrOp () == OP_TYPE_SUB ||
               g_Lexer->GetCurrOp () == OP_TYPE_BITWISE_NOT ||
			   g_Lexer->GetCurrOp() == OP_TYPE_LOGICAL_NOT ||
			   g_Lexer->GetCurrOp() == OP_TYPE_INC ||
			   g_Lexer->GetCurrOp() == OP_TYPE_DEC ))
        {
            

            iUnaryOpPending = true;
			iOpType = g_Lexer->GetCurrOp();
			
        }
        else
        {
           

			g_Lexer->RewindTokenStream();
        }

			
		


			switch (g_Lexer->GetNextToken(0))
			{
				

			case TOKEN_TYPE_RSRVD_TRUE:
			case TOKEN_TYPE_RSRVD_FALSE:
				iInstrIndex = this->g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
				g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, g_Lexer->GetCurrToken() == TOKEN_TYPE_RSRVD_TRUE ? 1 : 0);
				break;

				

			case TOKEN_TYPE_INT:
				iInstrIndex = this->g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
				this->g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, atoi(g_Lexer->GetCurrLexeme()));
				break;

				

			case TOKEN_TYPE_FLOAT:
				iInstrIndex = this->g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
				this->g_ICode->AddFloatICodeOp(g_iCurrScope, iInstrIndex, (float)atof(g_Lexer->GetCurrLexeme()));
				break;

				

			case TOKEN_TYPE_STRING:
			{
									  
									  g_StringTable.TrapAppend(g_Lexer->GetCurrLexeme());
									  iStringIndex = g_StringTable.getCount() - 1;
									  iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
									  g_ICode->AddStringICodeOp(g_iCurrScope, iInstrIndex, iStringIndex);
									  break;
			}

				

			case TOKEN_TYPE_IDENT:
			{
									 

									 SymbolNode * pSymbol = this->g_SymbolList->GetSymbolByIdent(g_Lexer->GetCurrLexeme(), g_iCurrScope);
									 if (pSymbol)
									 {
										 

										 if (g_Lexer->GetLookAheadChar() == '[')
										 {
											 

											 if (pSymbol->GetSize() == 1)
												 ExitOnCodeError("Invalid array", g_Lexer);

											

											 ReadToken(TOKEN_TYPE_DELIM_OPEN_BRACE);

											 

											 if (g_Lexer->GetLookAheadChar() == ']')
												 ExitOnCodeError("Invalid expression", g_Lexer);

											 

											 ParseExpr();

											 

											 ReadToken(TOKEN_TYPE_DELIM_CLOSE_BRACE);

											 

											 iInstrIndex = this->g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
											 this->g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, g_iTempVar0SymbolIndex);

											 

											 iInstrIndex = this->g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
											 this->g_ICode->AddArrayIndexVarICodeOp(g_iCurrScope, iInstrIndex, pSymbol->GetIndex(), g_iTempVar0SymbolIndex);
										 }
										 else
										 {

											 if (pSymbol->GetSize() == 1)
											 {
												 iInstrIndex = this->g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
												 this->g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, pSymbol->GetIndex());
											 }
											 else
											 {
												 ExitOnCodeError("Arrays must be indexed", g_Lexer);
											 }
										 }
									 }
									 else
									 {
										

										 if (FunctionList::getInstance()->GetFuncByName(g_Lexer->GetCurrLexeme()))
										 {
											 

											 ParseFuncCall();

											 

											 iInstrIndex = this->g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
											 this->g_ICode->AddRegICodeOp(g_iCurrScope, iInstrIndex, REG_CODE_RETVAL);
										 }
									 }

									 break;
			}

				

			case TOKEN_TYPE_DELIM_OPEN_PAREN:
				ParseExpr();
				ReadToken(TOKEN_TYPE_DELIM_CLOSE_PAREN);
				break;

				

			default:
				if (!iUnaryOpPending)
					ExitOnCodeError("Invalid input", g_Lexer);
				else
					g_Lexer->RewindTokenStream();
				break;
			}
		

       

        if ( iUnaryOpPending )
        {
            
			if (iOpType != OP_TYPE_INC && iOpType != OP_TYPE_DEC)
			{
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
				g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar0SymbolIndex);
			}            

            if ( iOpType == OP_TYPE_LOGICAL_NOT )
            {
               

				int iTrueJumpTargetIndex = g_ICode->GetNextJumpTargetIndex(),
					iExitJumpTargetIndex = g_ICode->GetNextJumpTargetIndex();

                

				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JE);
				g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar0SymbolIndex);
				g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, 0);
				g_ICode->AddJumpTargetICodeOp(g_iCurrScope, iInstrIndex, iTrueJumpTargetIndex);

                

				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
				g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, 0);

                

				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JMP);
				g_ICode->AddJumpTargetICodeOp(g_iCurrScope, iInstrIndex, iExitJumpTargetIndex);

                

				g_ICode->AddICodeJumpTarget(g_iCurrScope, iTrueJumpTargetIndex);

                

				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
				g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, 1);

                

				g_ICode->AddICodeJumpTarget(g_iCurrScope, iExitJumpTargetIndex);
            }
			else if (iOpType == OP_TYPE_INC)
			{
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_INC);
				this->g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, pSymbol->GetIndex());
			}
			else if (iOpType == OP_TYPE_DEC)
			{
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_DEC);
				g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, pSymbol->GetIndex());

			}
            else
            {
                int iOpIndex = 0;
                switch ( iOpType )
                {
                   

                    case OP_TYPE_SUB:
                        iOpIndex = INSTR_NEG;
                        break;

                    

                    case OP_TYPE_BITWISE_NOT:
                        iOpIndex = INSTR_NOT;
                        break;
                }

                

				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, iOpIndex);
				g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar0SymbolIndex);

                

				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
				g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->g_iTempVar0SymbolIndex);
            }
        }
    }

   

    void Parser::ParseIf ()
    {
        int iInstrIndex;

       

        if ( g_iCurrScope == SCOPE_GLOBAL )
            ExitOnCodeError ( "if illegal in global scope", g_Lexer );

       

		g_ICode->AddICodeSourceLine(g_iCurrScope, g_Lexer->GetCurrSourceLine());

        

		int iFalseJumpTargetIndex = g_ICode->GetNextJumpTargetIndex();

		this->setExtraCurrentOp(TOKEN_TYPE_RSRVD_IF);

        ReadToken ( TOKEN_TYPE_DELIM_OPEN_PAREN );

		

        ParseExpr ();

        

        ReadToken ( TOKEN_TYPE_DELIM_CLOSE_PAREN );

		this->setExtraCurrentOp(0);

        

		iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
		g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->GetAX());

        

		iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JE);
		g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->GetAX());
		g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, 0);
		g_ICode->AddJumpTargetICodeOp(g_iCurrScope, iInstrIndex, iFalseJumpTargetIndex);

        

        ParseStatement ( 0,"vvv" );

        

		if (g_Lexer->GetNextToken( 0 ) == TOKEN_TYPE_RSRVD_ELSE)
        {
            

			int iSkipFalseJumpTargetIndex = g_ICode->GetNextJumpTargetIndex();
            iInstrIndex = g_ICode->AddICodeInstr ( g_iCurrScope, INSTR_JMP );
            g_ICode->AddJumpTargetICodeOp ( g_iCurrScope, iInstrIndex, iSkipFalseJumpTargetIndex );

            

			g_ICode->AddICodeJumpTarget(g_iCurrScope, iFalseJumpTargetIndex);

            

            ParseStatement ( 0, "vvv" );

            

			g_ICode->AddICodeJumpTarget(g_iCurrScope, iSkipFalseJumpTargetIndex);
        }
        else
        {
            

            g_Lexer->RewindTokenStream ();

            

			g_ICode->AddICodeJumpTarget(g_iCurrScope, iFalseJumpTargetIndex);
        }
    }

    

    void Parser::ParseWhile ()
    {
        int iInstrIndex;

        // Make sure we're inside a function

        if ( g_iCurrScope == SCOPE_GLOBAL )
            ExitOnCodeError ( "Statement illegal in global scope", g_Lexer );

        // Annotate the line

        g_ICode->AddICodeSourceLine ( g_iCurrScope, g_Lexer->GetCurrSourceLine () );
        
        // Get two jump targets; for the top and bottom of the loop

        int iStartTargetIndex = g_ICode->GetNextJumpTargetIndex (),
            iEndTargetIndex = g_ICode->GetNextJumpTargetIndex ();

        // Set a jump target at the top of the loop

        g_ICode->AddICodeJumpTarget ( g_iCurrScope, iStartTargetIndex );

        // Read the opening parenthesis

        ReadToken ( TOKEN_TYPE_DELIM_OPEN_PAREN );

        // Parse the expression and leave the result on the stack

        ParseExpr ();

        // Read the closing parenthesis

        ReadToken ( TOKEN_TYPE_DELIM_CLOSE_PAREN );

        // Pop the result into _T0 and jump out of the loop if it's nonzero

        iInstrIndex = g_ICode->AddICodeInstr ( g_iCurrScope, INSTR_POP );
		g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->GetAX());

        iInstrIndex = g_ICode->AddICodeInstr ( g_iCurrScope, INSTR_JE );
        g_ICode->AddVarICodeOp ( g_iCurrScope, iInstrIndex, this->GetAX() );    
        g_ICode->AddIntICodeOp ( g_iCurrScope, iInstrIndex, 0 );
        g_ICode->AddJumpTargetICodeOp ( g_iCurrScope, iInstrIndex, iEndTargetIndex );

        // Create a new loop instance structure

        //Loop * pLoop = ( Loop * ) malloc ( sizeof ( Loop ) );

		Loop* pLoop = new Loop;

        // Set the starting and ending jump target indices

        pLoop->SetStartTargetIndex(iStartTargetIndex);
        pLoop->SetEndTargetIndex(iEndTargetIndex);

        // Push the loop structure onto the stack
		this->g_Stack.Push(pLoop);
        
        // Parse the loop body

        ParseStatement ( 0,"vvv" );

        // Pop the loop instance off the stack

		this->g_Stack.Pop();

        // Unconditionally jump back to the start of the loop

        iInstrIndex = g_ICode->AddICodeInstr ( g_iCurrScope, INSTR_JMP );
        g_ICode->AddJumpTargetICodeOp ( g_iCurrScope, iInstrIndex, iStartTargetIndex );

        // Set a jump target for the end of the loop

        g_ICode->AddICodeJumpTarget ( g_iCurrScope, iEndTargetIndex );
    }

    

    void Parser::ParseFor ()
    {
		int iInstrIndex = 0;

		if ( g_iCurrScope == SCOPE_GLOBAL )
            ExitOnCodeError ( "for illegal in global scope", g_Lexer );

		// Annotate the line
		g_ICode->AddICodeSourceLine ( g_iCurrScope, g_Lexer->GetCurrSourceLine () );

		int iEndTargetIndex = g_ICode->GetNextJumpTargetIndex();

		Loop* pLoop = new Loop();

		pLoop->SetEndTargetIndex(iEndTargetIndex);

		int iStartTargetIndex = g_ICode->GetNextJumpTargetIndex();

		pLoop->SetStartTargetIndex(iStartTargetIndex);

		this->g_Stack.Push(pLoop);  ///push 1

		ReadToken(TOKEN_TYPE_DELIM_OPEN_PAREN);

		
		ParseForExpr();

		ReadToken(TOKEN_TYPE_DELIM_CLOSE_PAREN);
		ReadToken(TOKEN_TYPE_DELIM_OPEN_CURLY_BRACE);

		

		this->ParseBlock();

		//pLoop = this->g_Stack.Top();

		

		pLoop = this->g_Stack.Top();

		this->g_Stack.Pop();

		int op = pLoop->getOperation();
		Token tToken = pLoop->getToken();

		int lexme = pLoop->getLexeme();


		//get index from loop//needs to set it in the parse for expression
		SymbolNode* pSymbol = this->g_SymbolList->GetSymbolByIndex(lexme);
		

		if (op == OP_TYPE_INC)
		{
			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_INC);
			this->g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, pSymbol->GetIndex());

		}
		else if (op == OP_TYPE_DEC)
		{
			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_DEC);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, pSymbol->GetIndex());
		}
		else if (op == OP_TYPE_ASSIGN_ADD)
		{
			iInstrIndex = this->g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
			this->g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, tToken);
			/////Push		10
			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->GetAX());
			////////POP _TO
			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_ADD);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, pSymbol->GetIndex());
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->GetAX());
			/////////Add		i, _T0
		}
		else if (op == OP_TYPE_ASSIGN_SUB)
		{
			iInstrIndex = this->g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
			this->g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, tToken);
			/////Push		10
			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->GetAX());
			////////POP _TO
			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_SUB);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, pSymbol->GetIndex());
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->GetAX());
			/////////Add		i, _T0

		}
		else if (op == OP_TYPE_ASSIGN_MUL)
		{
			iInstrIndex = this->g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
			this->g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, tToken);
			/////Push		10
			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->GetAX());
			////////POP _TO
			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_MUL);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, pSymbol->GetIndex());
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->GetAX());
			/////////Add		i, _T0

		}
		else if (op = OP_TYPE_ASSIGN_DIV)
		{
			iInstrIndex = this->g_ICode->AddICodeInstr(g_iCurrScope, INSTR_PUSH);
			this->g_ICode->AddIntICodeOp(g_iCurrScope, iInstrIndex, tToken);
			/////Push		10
			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->GetAX());
			////////POP _TO
			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_DIV);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, pSymbol->GetIndex());
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->GetAX());
			/////////Add		i, _T0

		}

		iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JMP);

		g_ICode->AddJumpTargetICodeOp(g_iCurrScope, iInstrIndex, pLoop->GetStartTargetIndex());    

		this->g_ICode->AddICodeJumpTarget(g_iCurrScope, iEndTargetIndex);
    }

	
	void Parser::ParseForExpr()
	{
		int iStatementCounter = 1;
		int flag = 0;

		Loop* pLoop = this->g_Stack.Top(); //top

		while ( (g_Lexer->GetLookAheadChar() != TOKEN_TYPE_DELIM_CLOSE_PAREN && iStatementCounter < 5) )
		{
			//g_Lexer->RewindTokenStream();
			if (iStatementCounter == 1)
			{
				this->ParseStatement(0, "for"); // eval
				//g_ICode->AddICodeJumpTarget(g_iCurrScope, pLoop->GetStartTargetIndex());
				

				g_ICode->AddICodeJumpTarget(g_iCurrScope, pLoop->GetStartTargetIndex());
				//stack
			}
			else if (iStatementCounter == 2)
			{				
				//int NextToken = g_Lexer->GetNextToken(0);
				char pstrIdent[MAX_LEXEME_SIZE];
				g_Lexer->CopyCurrLexeme(pstrIdent);
				//g_Lexer->GetNextToken(0);
				//this->ParseStatement(0,"vvv"); // eval
				this->ParseExpr();
				//int iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_JMP);

				//g_ICode->AddJumpTargetICodeOp(g_iCurrScope, iInstrIndex, pLoop->GetEndTargetIndex());
				flag = 1;

				//stack
			}				
			else if (iStatementCounter == 3)
			{
				int test = 0;
				int count = 0;
				Loop* loop;
				int op;
				Token fr;
				bool nextToken = false;
				bool tokenAlreadySet = false;
				
				while (test = g_Lexer->GetNextToken(0) != TOKEN_TYPE_DELIM_CLOSE_PAREN)
				{
					op = g_Lexer->GetCurrOp();
					fr = atoi(g_Lexer->GetCurrLexeme());


					if (op == OP_TYPE_INC || op == OP_TYPE_DEC || op == OP_TYPE_ASSIGN_ADD || op == OP_TYPE_ASSIGN_SUB || op == OP_TYPE_ASSIGN_MUL || op == OP_TYPE_ASSIGN_DIV)
					{
						

						
						loop = this->g_Stack.Top();//
						this->g_Stack.Pop();  //1-1 = 0
						loop->setOperation(op);
						if (nextToken == true)
						{
							loop->setToken(fr);
							tokenAlreadySet = true;

						}
						this->g_Stack.Push(loop); // 1
						if (op == OP_TYPE_ASSIGN_ADD || op == OP_TYPE_ASSIGN_SUB || op == OP_TYPE_ASSIGN_MUL || op == OP_TYPE_ASSIGN_DIV)
						{
							if (tokenAlreadySet == true)
								nextToken = false;
							else
								nextToken = true;
						}
						//break;
					}
						

					
				}
			}
				
				//if (flag == 1)
				//{
					//iStatementCounter--;
					//iStatementCounter--;
					//this->ParseStatement(5,"vvv");
					//int NextToken = g_Lexer->GetNextToken(0);
					//flag = 0;
				//}
				//else
				//{
					//g_Lexer->RewindTokenStream();
				//	this->ParseStatement(5,"NO");
				//}
			iStatementCounter++;
		}
			
			//ReadToken(TOKEN_TYPE_DELIM_CLOSE_PAREN);

		g_Lexer->RewindTokenStream();
	}

	


    void Parser::ParseBreak(int flag)
    {
        // Make sure we're in a loop
		//need to add for case statements

        if ( g_Stack.IsStackEmpty() )
            ExitOnCodeError ( "break illegal outside loops", g_Lexer );

        // Annotate the line

        g_ICode->AddICodeSourceLine ( g_iCurrScope, g_Lexer->GetCurrSourceLine () );

        // Attempt to read the semicolon

        ReadToken ( TOKEN_TYPE_DELIM_SEMICOLON );

        // Get the jump target index for the end of the loop

        int iTargetIndex = g_Stack.Top()->GetStartTargetIndex();

        // Unconditionally jump to the end of the loop

        int iInstrIndex = g_ICode->AddICodeInstr ( g_iCurrScope, INSTR_JMP );
        g_ICode->AddJumpTargetICodeOp ( g_iCurrScope, iInstrIndex, iTargetIndex );        
    }

   

    void Parser::ParseContinue ()
    {
        // Make sure we're inside a function

        if ( g_Stack.IsStackEmpty() )
            ExitOnCodeError ( "continue illegal outside loops", g_Lexer );

        // Annotate the line

        g_ICode->AddICodeSourceLine ( g_iCurrScope, g_Lexer->GetCurrSourceLine () );

        // Attempt to read the semicolon

        ReadToken ( TOKEN_TYPE_DELIM_SEMICOLON );

        // Get the jump target index for the start of the loop

        int iTargetIndex = g_Stack.Top()->GetStartTargetIndex();

        // Unconditionally jump to the end of the loop

        int iInstrIndex = g_ICode->AddICodeInstr ( g_iCurrScope, INSTR_JMP );
        g_ICode->AddJumpTargetICodeOp ( g_iCurrScope, iInstrIndex, iTargetIndex );
    }

    

    void Parser::ParseReturn ()
    {
        int iInstrIndex;

        // Make sure we're inside a function

        if ( g_iCurrScope == SCOPE_GLOBAL )
            ExitOnCodeError ( "return illegal in global scope", g_Lexer);

        // Annotate the line

        g_ICode->AddICodeSourceLine ( g_iCurrScope, g_Lexer->GetCurrSourceLine () );

        // If a semicolon doesn't appear to follow, parse the expression and place it in
        // _RetVal

        if ( g_Lexer->GetLookAheadChar () != ';' )
        {
            // Parse the expression to calculate the return value and leave the result on the stack.

            ParseExpr ();

            // Determine which function we're returning from

            if ( g_ScriptHeader->GetMainFuncPresent() && 
                 g_ScriptHeader->GetMainFuncIndex() == g_iCurrScope )
            {
                // It is _Main (), so pop the result into _T0

                iInstrIndex = g_ICode->AddICodeInstr ( g_iCurrScope, INSTR_POP );
                g_ICode->AddVarICodeOp ( g_iCurrScope, iInstrIndex, this->GetAX() );
            }
            else
            {
                // It's not _Main, so pop the result into the _RetVal register

                iInstrIndex = g_ICode->AddICodeInstr ( g_iCurrScope, INSTR_POP );
                g_ICode->AddRegICodeOp ( g_iCurrScope, iInstrIndex, REG_CODE_RETVAL );
            }
        }
        else
        {
            // Clear _T0 in case we're exiting _Main ()

            if ( g_ScriptHeader->GetMainFuncPresent() && 
                 g_ScriptHeader->GetMainFuncIndex() == g_iCurrScope )
            {
            
                iInstrIndex = g_ICode->AddICodeInstr ( g_iCurrScope, INSTR_MOV );
                g_ICode->AddVarICodeOp ( g_iCurrScope, iInstrIndex, this->GetAX() );
                g_ICode->AddIntICodeOp ( g_iCurrScope, iInstrIndex, 0 );
            }
        }

        if ( g_ScriptHeader->GetMainFuncPresent() && 
             g_ScriptHeader->GetMainFuncIndex() == g_iCurrScope )
        {
            // It's _Main, so exit the script with _T0 as the exit code

            iInstrIndex = g_ICode->AddICodeInstr ( g_iCurrScope, INSTR_EXIT );
            g_ICode->AddVarICodeOp ( g_iCurrScope, iInstrIndex, this->GetAX() );
        }
        else
        {
            // It's not _Main, so return from the function

            g_ICode->AddICodeInstr ( g_iCurrScope, INSTR_RET );
        }
    }

   

    int Parser::ParseAssign(int flag)
    {
        // Make sure we're inside a function

        if ( g_iCurrScope == SCOPE_GLOBAL )
            ExitOnCodeError ( "Assignment illegal in global scope", g_Lexer );

        int iInstrIndex = 0;
		int NextToken = 0;

        // Assignment operator

        int iAssignOp;

        // Annotate the line

		if (this->getSyntax() != TOKEN_TYPE_RSRVD_FOR)
		{
			g_ICode->AddICodeSourceLine(g_iCurrScope, g_Lexer->GetCurrSourceLine());
		}
        

        // ---- Parse the variable or array

		SymbolNode * pSymbol = g_SymbolList->GetSymbolByIdent(g_Lexer->GetCurrLexeme(), g_iCurrScope);

        // Does an array index follow the identifier?

        int iIsArray = false;
        if ( g_Lexer->GetLookAheadChar () == '[' )
        {
            // Ensure the variable is an array

            if ( pSymbol->GetSize() == 1 )
				ExitOnCodeError("Invalid array", g_Lexer);

            // Verify the opening brace

            ReadToken ( TOKEN_TYPE_DELIM_OPEN_BRACE );

            // Make sure an expression is present

            if ( g_Lexer->GetLookAheadChar () == ']' )
				ExitOnCodeError("Invalid expression", g_Lexer);

            // Parse the index as an expression

            ParseExpr ();

            // Make sure the index is closed

            ReadToken ( TOKEN_TYPE_DELIM_CLOSE_BRACE );
            
            // Set the array flag

            iIsArray = true;
        }
        else
        {
            // Make sure the variable isn't an array

            if ( pSymbol->GetSize() > 1 )
				ExitOnCodeError("Arrays must be indexed", g_Lexer);
        }
		NextToken = g_Lexer->GetNextToken(0);

        // ---- Parse the assignment operator
        
		if (NextToken == TOKEN_TYPE_OP &&
			(g_Lexer->GetCurrOp() == OP_TYPE_ADD ||
			g_Lexer->GetCurrOp() == OP_TYPE_SUB ||
			g_Lexer->GetCurrOp() == OP_TYPE_BITWISE_NOT ||
			g_Lexer->GetCurrOp() == OP_TYPE_LOGICAL_NOT ||
			g_Lexer->GetCurrOp() == OP_TYPE_INC ||
			g_Lexer->GetCurrOp() == OP_TYPE_DEC))
		{
			g_Lexer->RewindTokenStream();
			iAssignOp = 0;
		}
		else if (NextToken != TOKEN_TYPE_OP &&
             ( g_Lexer->GetCurrOp() != OP_TYPE_ASSIGN &&
			 g_Lexer->GetCurrOp() != OP_TYPE_ASSIGN_ADD &&
			 g_Lexer->GetCurrOp() != OP_TYPE_ASSIGN_SUB &&
			 g_Lexer->GetCurrOp() != OP_TYPE_ASSIGN_MUL &&
			 g_Lexer->GetCurrOp() != OP_TYPE_ASSIGN_DIV &&
			 g_Lexer->GetCurrOp() != OP_TYPE_ASSIGN_MOD &&
			 g_Lexer->GetCurrOp() != OP_TYPE_ASSIGN_EXP &&
			 g_Lexer->GetCurrOp() != OP_TYPE_ASSIGN_CONCAT &&
			 g_Lexer->GetCurrOp() != OP_TYPE_ASSIGN_AND &&
			 g_Lexer->GetCurrOp() != OP_TYPE_ASSIGN_OR &&
			 g_Lexer->GetCurrOp() != OP_TYPE_ASSIGN_XOR &&
			 g_Lexer->GetCurrOp() != OP_TYPE_ASSIGN_SHIFT_LEFT &&
			 g_Lexer->GetCurrOp() != OP_TYPE_ASSIGN_SHIFT_RIGHT))
			 ExitOnCodeError("Illegal assignment operator", g_Lexer);
        else
			iAssignOp = g_Lexer->GetCurrOp();

       

		if (flag == 5)
		{
			ParseSubExpr();
			flag = 1;
		}
		else
		{
			ParseExpr();
		}
		
        
        

		if (flag != 1)
		{
			ReadToken(TOKEN_TYPE_DELIM_SEMICOLON);
		}        

       
		if (iAssignOp != 0)
		{
			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->GetAX());
		}
        

        

        if ( iIsArray )
        {
			iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_POP);
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->GetBX());
        }


        

        switch ( iAssignOp )
        {
            

            case OP_TYPE_ASSIGN:
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_MOV);
                break;

            
//			case OP_TYPE_ASSIGN_INC:
            case OP_TYPE_ASSIGN_ADD:
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_ADD);
                break;

            
//			case OP_TYPE_ASSIGN_DEC:
            case OP_TYPE_ASSIGN_SUB:
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_SUB);
                break;

            

            case OP_TYPE_ASSIGN_MUL:
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_MUL);
                break;

            

            case OP_TYPE_ASSIGN_DIV:
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_DIV);
                break;

            

            case OP_TYPE_ASSIGN_MOD:
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_MOD);
                break;

            

            case OP_TYPE_ASSIGN_EXP:
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_EXP);
                break;

            

            case OP_TYPE_ASSIGN_CONCAT:
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_CONCAT);
                break;

            

            case OP_TYPE_ASSIGN_AND:
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_AND);
                break;

            

            case OP_TYPE_ASSIGN_OR:
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_OR);
                break;

            

            case OP_TYPE_ASSIGN_XOR:
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_XOR);
                break;

            

            case OP_TYPE_ASSIGN_SHIFT_LEFT:
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_SHL);
                break;

            

            case OP_TYPE_ASSIGN_SHIFT_RIGHT:
				iInstrIndex = g_ICode->AddICodeInstr(g_iCurrScope, INSTR_SHR);
                break;
        }

       

        if ( iIsArray )
			g_ICode->AddArrayIndexVarICodeOp(g_iCurrScope, iInstrIndex, pSymbol->GetIndex(), this->GetBX());
		else
		{
			if (iInstrIndex != 0)
			{
				g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, pSymbol->GetIndex());
			}
		}
			

		if (iInstrIndex != 0)
		{
			g_ICode->AddVarICodeOp(g_iCurrScope, iInstrIndex, this->GetAX());
		}
		

		return 1;
    }

   

    void Parser::ParseFuncCall ()//*********************************************
    {
       //proceed on the thir try for debug

		FuncNode * pFunc = FunctionList::getInstance()->GetFuncByName(g_Lexer->GetCurrLexeme());


        int iParamCount = 0;

        

        ReadToken ( TOKEN_TYPE_DELIM_OPEN_PAREN );

        

        while ( true )
        {
            

            if ( g_Lexer->GetLookAheadChar () != ')' )
            {
                

                ParseExpr ();

               

                ++ iParamCount;
                if ( ! pFunc->GetIsHostAPI() && iParamCount > pFunc->GetParamCount() )
                    ExitOnCodeError ( "Too many parameters", g_Lexer );

                

                if ( g_Lexer->GetLookAheadChar () != ')' )
                    ReadToken ( TOKEN_TYPE_DELIM_COMMA );
            }
            else
            {
                

                break;
            }
        }

        

        ReadToken ( TOKEN_TYPE_DELIM_CLOSE_PAREN );

        

        if ( ! pFunc->GetIsHostAPI() && iParamCount < pFunc->GetParamCount() )
            ExitOnCodeError ( "Too few parameters", g_Lexer );

       

        int iCallInstr = INSTR_CALL;
        if ( pFunc->GetIsHostAPI() )
            iCallInstr = INSTR_CALLHOST;

        int iInstrIndex = g_ICode->AddICodeInstr ( g_iCurrScope, iCallInstr );
		g_ICode->AddFuncICodeOp(g_iCurrScope, iInstrIndex, pFunc->GetIndex());
    }

	

	void Parser::ParseSwitchStatement(int flag)
	{

		if (g_Lexer->GetLookAheadChar() == ';')
		{
			ReadToken(TOKEN_TYPE_DELIM_SEMICOLON);
			return;
		}
		char* test;
		char* ret;
		int result = 0;

		

		while (g_Lexer->GetLookAheadChar() != '}')
		{
			Token InitToken = g_Lexer->GetNextToken(0);
			switch (InitToken)
			{
			case TOKEN_TYPE_END_OF_STREAM:
				ExitOnCodeError("Unexpected end of file", this->g_Lexer);
				break;
			case TOKEN_TYPE_DELIM_OPEN_CURLY_BRACE:
				ParseCase(1);
				break;
			case TOKEN_TYPE_RSRVD_BREAK:
				ParseBreak(1);
				break;

			case TOKEN_TYPE_RSRVD_CASE:
				ParseCase(0);
				break;
			case TOKEN_TYPE_RSRVD_DEFAULT:
				ParseDefault();
				break;
			default:
				ExitOnCodeError("Unexpected input", this->g_Lexer);
				break;
			}
		}

		
	}