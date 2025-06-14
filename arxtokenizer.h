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
  _return,         // 0
  intLiteral,      // 1
  semicolon,       // 2
  variable,        // 3
  plus,            // 4
  multiply,        // 5
  minus,           // 6
  modulo,          // 7
  divide,          // 8
  assignment       // 9
};

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

// funcs are externed by default
void finalizeAlphaToken(void);
void tokenize(char charFromTheFile);
void printTokens(void);
void resetTokenizer(void);

#endif
