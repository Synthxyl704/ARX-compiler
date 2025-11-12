// #pragma once
#ifndef ARXTOKENIZER_H
#define ARXTOKENIZER_H

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

    classDatatype,
    intPointerDatatype,
    floatPointerDatatype,
    doublePointerDatatype,
    charPointerDatatype,
    classPointerDatatype,

    intLiteral,
    charLiteral,
    floatingPointLiteral,
    doublePointLiteral,

    variable,
    subScope,
    extScope,

    LHcircleBracket,
    RHcircleBracket,
    LHcurlyBracket,
    RHcurlyBracket,
    LHsquareBracket,
    RHsquareBracket,

    semicolon,
    comma,

    plus,
    multiply,
    minus,
    modulo,
    divide,
    assignment,

    _main,
    _return,
    _subprog,
};

const char *tokenTypeToStr(enum tokenType d_type);

typedef struct {
    enum tokenType datatype;
    char lexicalBuffer[MAX_BUFFER_SIZE];
} token;

extern token tokenArray[MAX_TOKENS_USAGE];
extern char alphaDetectionBuffer[MAX_BUFFER_SIZE];
extern char linearOneLineSize[0x400];
extern int bufferIndex;
extern int tokenCount;
extern int lineNumber;
extern int columnNumber;
extern int actualDatatypeClass[MAX_BUFFER_SIZE];

void finalizeAlphaToken(void);
void tokenize(char charFromTheFile);
void printTokens(void);
void resetTokenizer(void);

void reportSyntaxError(const char *found, const char *expected, const char *suggestion);
void reportWarning(const char *tokenStr, const char *warning);
int getErrorCount(void);
int getWarningCount(void);
bool isValidScientificNotation(const char *buffer, int length);

#endif
