#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define MAX_TOKENS_USAGE 0x7F6E
#define MAX_BUFFER_SIZE 1024

enum tokenType {
  intDatatype,
  floatDatatype,
  doubleDatatype,
  charDatatype,


  intLiteral,           // 1 | mv_int
  charLiteral,          // 2 | mv_chr
  floatingPointLiteral, // 3 | mv_flt
  doublePointLiteral,   // 4 | mv_dbl

  variable,             // 5
  subScope,             // 6 | mv_subScope<d_t> <value>, {{ variable_name }}
  extScope,             // 7 | mv_extScope<d_t> <value>, {{ variable_name }}

  LHcircleBracket,      // 8
  RHcircleBracket,      // 9

  LHcurlyBracket,       // 10 | for _prov ONLY
  RHcurlyBracket,       // 11 | 
   
  LHsquareBracket,      // 12 | function/_subprog[parameters]
  RHsquareBracket,      // 13 |


  semicolon,            // 14 | end statements

  plus,                 // 15
  multiply,             // 16
  minus,                // 17
  modulo ,              // 18
  divide ,              // 19
  assignment ,          // 20

  _main,                // 22 [MAIN!] || Provenance = start of something
  _return,              // 23 [RETURN!] || ret = return
  _subprog,             // 24 [FUNCTION!] || function[parameters]
};


// enum datatype {
//   _int,
//   _float,
//   _char, 
//   _double
// }

const char* tokenTypeToStr(enum tokenType d_type) {
  switch (d_type) {
    case intDatatype:    return "integer datatype";
    case floatDatatype:  return "floating point datatype";
    case doubleDatatype: return "double precision datatype";
    case charDatatype:   return "character datatype";

    case intLiteral:            return "integer literal";
    case charLiteral:           return "character literal";
    case floatingPointLiteral:  return "floating point literal";
    case doublePointLiteral:    return "double precision literal";

    case variable:     return "variable";
    case subScope:     return "subScope";
    case extScope:     return "extScope";

    case LHcircleBracket:  return "LHcircleBracket";
    case RHcircleBracket:  return "RHcircleBracket";
    case LHcurlyBracket:   return "LHcurlyBracket";
    case RHcurlyBracket:   return "RHcurlyBracket";
    case LHsquareBracket:  return "LHsquareBracket";
    case RHsquareBracket:  return "RHsquareBracket";

    case semicolon:    return "semicolon";
    case plus:         return "plus";
    case multiply:     return "multiply";
    case minus:        return "minus";
    case modulo:       return "modulo";
    case divide:       return "divide";
    case assignment:   return "assignment";

    case _main:        return "_main";
    case _return:      return "_return";
    case _subprog:     return "_subprog";
    default:           return "unknown";
  }
}

typedef struct {
  enum tokenType datatype;
  char lexicalBuffer[MAX_BUFFER_SIZE];
} token;

// extern em
extern token tokenArray[MAX_TOKENS_USAGE];

extern char alphaDetectionBuffer[MAX_BUFFER_SIZE];
extern char linearOneLineSize[0x400];

extern int bufferIndex;
extern int tokenCount;
extern int lineNumber;
extern int actualDatatypeClass[MAX_BUFFER_SIZE];

void finalizeAlphaToken(void);
void tokenize(char charFromTheFile);
void printTokens(void);
void resetTokenizer(void);

#endif
