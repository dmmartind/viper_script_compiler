//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////
#pragma once
#ifndef GLOBAL_
#define GLOBAL_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>


#define VERSION_MAJOR               0           // Major version number
#define VERSION_MINOR               8           // Minor version number

// ---- Filename --------------------------------------------------------------------------

#define MAX_FILENAME_SIZE           2048        // Maximum filename length

#define SOURCE_FILE_EXT             ".VSS"      // Extension of a source code file
#define OUTPUT_FILE_EXT             ".VASM"     // Extension of an output assembly file

// ---- Source Code -----------------------------------------------------------------------

#define MAX_SOURCE_LINE_SIZE        4096        // Maximum source line length

#define MAX_IDENT_SIZE              256        // Maximum identifier size

// ---- Priority Types --------------------------------------------------------------------

#define PRIORITY_NONE               0           // A priority wasn't specified
#define PRIORITY_USER               1           // User-defined priority
#define PRIORITY_LOW                2           // Low priority
#define PRIORITY_MED                3           // Medium priority
#define PRIORITY_HIGH               4           // High priority

#define PRIORITY_LOW_KEYWORD        "Low"       // Low priority keyword
#define PRIORITY_MED_KEYWORD        "Med"       // Low priority keyword
#define PRIORITY_HIGH_KEYWORD       "High"      // Low priority keyword

// ---- Functions -------------------------------------------------------------------------

#define MAIN_FUNC_NAME				"_Main"		// _Main ()'s name

// ---- Register Codes---------------------------------------------------------------------

#define REG_CODE_RETVAL             0           // _RetVal

// ---- Internal Script Entities ----------------------------------------------------------

#define TEMP_VAR_0                  "_T0"       // Temporary variable 0
#define TEMP_VAR_1                  "_T1"       // Temporary variable 1

	// ---- I-Code Node Types -----------------------------------------------------------------

#define ICODE_NODE_INSTR        0               // An I-code instruction
#define ICODE_NODE_SOURCE_LINE  1               // Source-code annotation
#define ICODE_NODE_JUMP_TARGET  2               // A jump target

// ---- I-Code Instruction Opcodes --------------------------------------------------------

#define INSTR_MOV               0

#define INSTR_ADD               1
#define INSTR_SUB               2
#define INSTR_MUL               3
#define INSTR_DIV               4
#define INSTR_MOD               5
#define INSTR_EXP               6
#define INSTR_NEG               7
#define INSTR_INC               8
#define INSTR_DEC               9

#define INSTR_AND               10
#define INSTR_OR                11
#define INSTR_XOR               12
#define INSTR_NOT               13
#define INSTR_SHL               14
#define INSTR_SHR               15

#define INSTR_CONCAT            16
#define INSTR_GETCHAR           17
#define INSTR_SETCHAR           18

#define INSTR_JMP               19
#define INSTR_JE                20
#define INSTR_JNE               21
#define INSTR_JG                22
#define INSTR_JL                23
#define INSTR_JGE               24
#define INSTR_JLE               25

#define INSTR_PUSH              26
#define INSTR_POP               27

#define INSTR_CALL              28
#define INSTR_RET               29
#define INSTR_CALLHOST          30

#define INSTR_PAUSE             31
#define INSTR_EXIT              32

// ---- Operand Types ---------------------------------------------------------------------

#define OP_TYPE_INT                 0           // Integer literal value
#define OP_TYPE_FLOAT               1           // Floating-point literal value
#define OP_TYPE_STRING_INDEX        2           // String literal value
#define OP_TYPE_VAR                 3           // Variable
#define OP_TYPE_ARRAY_INDEX_ABS     4           // Array with absolute index
#define OP_TYPE_ARRAY_INDEX_VAR     5           // Array with relative index
#define OP_TYPE_JUMP_TARGET_INDEX   6           // Jump target index
#define OP_TYPE_FUNC_INDEX          7           // Function index
#define OP_TYPE_REG                 9           // Register


#define MAX_LEXEME_SIZE                 1024    // Maximum individual lexeme size

// ---- Operators -------------------------------------------------------------------------

#define MAX_OP_STATE_COUNT              32      // Maximum number of operator
												// states

// ---- Delimiters ------------------------------------------------------------------------

#define MAX_DELIM_COUNT                 24      // Maximum number of delimiters

// ---- Lexer States ----------------------------------------------------------------------

#define LEX_STATE_UNKNOWN               0       // Unknown lexeme type

#define LEX_STATE_START                 1       // Start state

#define LEX_STATE_INT                   2       // Integer
#define LEX_STATE_FLOAT                 3       // Float

#define LEX_STATE_IDENT                 4       // Identifier


#define LEX_STATE_OP                    5       // Operator
#define LEX_STATE_DELIM                 6       // Delimiter    

#define LEX_STATE_STRING                7       // String value
#define LEX_STATE_STRING_ESCAPE         8       // Escape sequence
#define LEX_STATE_STRING_CLOSE_QUOTE    9       // String closing quote

// ---- Token Types -----------------------------------------------------------------------

#define TOKEN_TYPE_END_OF_STREAM        0       // End of the token stream
#define TOKEN_TYPE_INVALID              1       // Invalid token

#define TOKEN_TYPE_INT                  2       // Integer
#define TOKEN_TYPE_FLOAT                3       // Float

#define TOKEN_TYPE_IDENT                4       // Identifier

#define TOKEN_TYPE_RSRVD_VAR            5       // var/var []
#define TOKEN_TYPE_RSRVD_TRUE           6       // true
#define TOKEN_TYPE_RSRVD_FALSE          7       // false
#define TOKEN_TYPE_RSRVD_IF             8       // if
#define TOKEN_TYPE_RSRVD_ELSE           9       // else
#define TOKEN_TYPE_RSRVD_BREAK          10      // break
#define TOKEN_TYPE_RSRVD_CONTINUE       11      // continue
#define TOKEN_TYPE_RSRVD_FOR            12      // for
#define TOKEN_TYPE_RSRVD_WHILE          13      // while
#define TOKEN_TYPE_RSRVD_FUNC           14      // func
#define TOKEN_TYPE_RSRVD_RETURN         15      // return
#define TOKEN_TYPE_RSRVD_HOST           16      // host

#define TOKEN_TYPE_OP                   18      // Operator

#define TOKEN_TYPE_DELIM_COMMA          19      // ,
#define TOKEN_TYPE_DELIM_OPEN_PAREN     20      // (
#define TOKEN_TYPE_DELIM_CLOSE_PAREN    21      // )
#define TOKEN_TYPE_DELIM_OPEN_BRACE     22      // [
#define TOKEN_TYPE_DELIM_CLOSE_BRACE    23      // ]
#define TOKEN_TYPE_DELIM_OPEN_CURLY_BRACE   24  // {
#define TOKEN_TYPE_DELIM_CLOSE_CURLY_BRACE  25  // }
#define TOKEN_TYPE_DELIM_SEMICOLON      26      // ;

#define TOKEN_TYPE_STRING               27      // String

// ---- Operators -------------------------------------------------------------------------

	// ---- Arithmetic

#define OP_TYPE_ADD                     0       // +
#define OP_TYPE_SUB                     1       // -
#define OP_TYPE_MUL                     2       // *
#define OP_TYPE_DIV                     3       // /
#define OP_TYPE_MOD                     4       // %
#define OP_TYPE_EXP                     5       // ^
#define OP_TYPE_CONCAT                  35       // $

#define OP_TYPE_INC                     15      // ++
#define OP_TYPE_DEC                     17      // --

#define OP_TYPE_ASSIGN_ADD              14      // +=
#define OP_TYPE_ASSIGN_SUB              16      // -=
#define OP_TYPE_ASSIGN_MUL              18      // *=
#define OP_TYPE_ASSIGN_DIV              19      // /=
#define OP_TYPE_ASSIGN_MOD              20      // %=
#define OP_TYPE_ASSIGN_EXP              21      // ^=
#define OP_TYPE_ASSIGN_CONCAT           36      // $=

// ---- Bitwise

#define OP_TYPE_BITWISE_AND             6       // &
#define OP_TYPE_BITWISE_OR              7       // |
#define OP_TYPE_BITWISE_XOR             8       // #
#define OP_TYPE_BITWISE_NOT             9       // ~
#define OP_TYPE_BITWISE_SHIFT_LEFT      30      // <<
#define OP_TYPE_BITWISE_SHIFT_RIGHT     32      // >>

#define OP_TYPE_ASSIGN_AND              22      // &=
#define OP_TYPE_ASSIGN_OR               24      // |=
#define OP_TYPE_ASSIGN_XOR              26      // #=
#define OP_TYPE_ASSIGN_SHIFT_LEFT       33      // <<=
#define OP_TYPE_ASSIGN_SHIFT_RIGHT      34      // >>=

// ---- Logical

#define OP_TYPE_LOGICAL_AND             23      // &&
#define OP_TYPE_LOGICAL_OR              25      // ||
#define OP_TYPE_LOGICAL_NOT             10      // !

// ---- Relational

#define OP_TYPE_EQUAL                   28      // ==
#define OP_TYPE_NOT_EQUAL               27      // !=
#define OP_TYPE_LESS                    12      // <
#define OP_TYPE_GREATER                 13      // >
#define OP_TYPE_LESS_EQUAL              29      // <=
#define OP_TYPE_GREATER_EQUAL           31      // >=

// ---- Assignment

#define OP_TYPE_ASSIGN                  11      // =

// ---- Data Structures -----------------------------------------------------------------------

typedef int Token;                                  // Token type

#define SCOPE_GLOBAL                    0       // Global scope

	// ---- Symbol Types ----------------------------------------------------------------------

#define SYMBOL_TYPE_VAR                 0       // Variable
#define SYMBOL_TYPE_PARAM               1       // Parameter

#define TAB_STOP_WIDTH                      8       // The width of a tab stop in
														// characters

//----------------------extra extra extra
#define TOKEN_TYPE_RSRVD_SWITCH			40
#define TOKEN_TYPE_RSRVD_CASE			41
#define TOKEN_TYPE_RSRVD_DEFAULT		42
#define TOKEN_TYPE_DELIM_COLON          43      // :
#define TOKEN_TYPE_RSRVD_COLON    48










#endif