#include "arxtokenizer.h"
#include <stdbool.h>

token tokenArray[MAX_TOKENS_USAGE];

char alphaDetectionBuffer[MAX_BUFFER_SIZE] = {0};
char linearOneLineSize[0x400];      // 1024

int bufferIndex = 0;
int tokenCount = 0;
int lineNumber = 1;
int columnNumber = 1; 

// err tracc
typedef struct {
    int errorCount;
    int warningCount;
    char lastErrorToken[MAX_BUFFER_SIZE];
    int lastErrorLine;
    int lastErrorColumn;
} ErrorContext;

// holy sigmastyle is that a kracc bacc reference!!1!1!!1

static ErrorContext errorCtx = {0};

void reportSyntaxError(const char *found, const char *expected, const char *suggestion) {
    fprintf(stderr, "std:syntax_error::[TC%d/L%d:C%d]::[%s]:::[%s]:::[suggestion: %s]\n", 
            tokenCount, lineNumber, columnNumber, found, expected, suggestion);
    errorCtx.errorCount += 1;
    strcpy(errorCtx.lastErrorToken, found);
    errorCtx.lastErrorLine = lineNumber;
    errorCtx.lastErrorColumn = columnNumber;
}

// warning <
void reportWarning(const char *token, const char *warning) {
    fprintf(stderr, "std:warning::[TC%d/L%d:C%d]::[%s]:::[%s]\n", 
            tokenCount, lineNumber, columnNumber, token, warning);
    errorCtx.warningCount += 1;
}

bool isValidScientificNotation(const char *buffer, int length) {
    int ePos = -1;
    int dotCount = 0;
    bool hasDigitsBeforeE = false;
    bool hasDigitsAfterE = false;
    
    // epoxy resin -> structure validation
    // huh?
    for (int inc = 0; inc < length; inc += 1) {
        if (buffer[inc] == 'E' || buffer[inc] == 'e') {
            if (ePos != (-1)) { return false; } // multiple E's
            ePos = inc;
            continue;
        }
        
        if (buffer[inc] == '.') {
            dotCount += 1;
            if (dotCount > 1) return false; // multiple dots
            if (ePos != -1) return false;  // dot after E
            continue;
        }

        // this hurts my brain kinda
        
        if (isdigit(buffer[inc])) {
            if (ePos == (-1)) { hasDigitsBeforeE = true; }
            else { hasDigitsAfterE = true; }
        } else if (buffer[inc] != '+' && buffer[inc] != '-') {
            return false; // invalid character
        }
    }
    
    // validate E position and surrounding digits
    if (ePos == 0 || ePos == (length - 1)) { return false; } // E at start/end
    if (!hasDigitsBeforeE || (ePos != (-1) && !hasDigitsAfterE)) { return false; }
    
    return true;
}

void finalizeAlphaToken() {
    if (bufferIndex > 0 && tokenCount < MAX_TOKENS_USAGE) {
        alphaDetectionBuffer[bufferIndex] = '\0';

        // ------------------- COMPILER SYNTAX AND ERROR SECTION START --------------------- // 
        
        // exact matches parsing
        // TODO: migrate lexer conditionals to switch statx
        if (strcmp(alphaDetectionBuffer, "_prov") == 0) {
            tokenArray[tokenCount].datatype = _main;
        } 
        else if (strcmp(alphaDetectionBuffer, "_ret") == 0) {
            tokenArray[tokenCount].datatype = _return;
        }
        else if (strcmp(alphaDetectionBuffer, "_subprog") == 0) {
            tokenArray[tokenCount].datatype = _subprog;
        }
        else if (strcmp(alphaDetectionBuffer, "subScope") == 0) {
            tokenArray[tokenCount].datatype = subScope;
        }
        else if (strcmp(alphaDetectionBuffer, "extScope") == 0) {
            tokenArray[tokenCount].datatype = extScope;
        }
        else if (strcmp(alphaDetectionBuffer, "mv_int") == 0) {
            tokenArray[tokenCount].datatype = intDatatype;
        }
        else if (strcmp(alphaDetectionBuffer, "mv_flt") == 0) {
            tokenArray[tokenCount].datatype = floatDatatype;
        }
        else if (strcmp(alphaDetectionBuffer, "mv_chr") == 0) {
            tokenArray[tokenCount].datatype = charDatatype; 
        }
        else if (strcmp(alphaDetectionBuffer, "mv_dbl") == 0) {
            tokenArray[tokenCount].datatype = doubleDatatype;
        }
        
        // --- FUNC6N HANDLING ---
        // geniunely dont know how to make this better
        else if (strstr(alphaDetectionBuffer, "prov") != NULL && strcmp(alphaDetectionBuffer, "_prov") != 0) {
            reportSyntaxError(alphaDetectionBuffer, "_prov", "use underscore prefix for keywords");
        } 
        else if (strstr(alphaDetectionBuffer, "ret") != NULL && strcmp(alphaDetectionBuffer, "_ret") != 0) {
            reportSyntaxError(alphaDetectionBuffer, "_ret", "use underscore prefix for return keyword");
        }
        else if (strstr(alphaDetectionBuffer, "subprog") != NULL && strcmp(alphaDetectionBuffer, "_subprog") != 0) {
            reportSyntaxError(alphaDetectionBuffer, "_subprog", "use underscore prefix for function keyword");
        }
        
        // --- DATATYPE HANDLING ---
        else if ((strstr(alphaDetectionBuffer, "mvint") != NULL || strstr(alphaDetectionBuffer, "mv_int") != NULL) && strcmp(alphaDetectionBuffer, "mv_int") != 0) {
            reportSyntaxError(alphaDetectionBuffer, "mv_int", "correct syntax is mv_int for integer datatype");
        }
        else if ((strstr(alphaDetectionBuffer, "mvflt") != NULL || strstr(alphaDetectionBuffer, "mv_flt") != NULL) && strcmp(alphaDetectionBuffer, "mv_flt") != 0) {
            reportSyntaxError(alphaDetectionBuffer, "mv_flt", "correct syntax is mv_flt for float datatype");
        }
        else if ((strstr(alphaDetectionBuffer, "mvchr") != NULL || strstr(alphaDetectionBuffer, "mv_chr") != NULL) && strcmp(alphaDetectionBuffer, "mv_chr") != 0) {
            reportSyntaxError(alphaDetectionBuffer, "mv_chr", "correct syntax is mv_chr for char datatype");
        }
        else if ((strstr(alphaDetectionBuffer, "mvdbl") != NULL || strstr(alphaDetectionBuffer, "mv_dbl") != NULL) && strcmp(alphaDetectionBuffer, "mv_dbl") != 0) {
            reportSyntaxError(alphaDetectionBuffer, "mv_dbl", "correct syntax is mv_dbl for double datatype");
        }
        
        // ------------------- LITERAL AND VARIABLE DETECTION SECTION --------------------- //
        else {
          // E+X notation shit
            int hasViewedE = 0;
            int E_Position = -1;

            for (int inc = 0; inc < bufferIndex; inc += 1) {
                if (alphaDetectionBuffer[inc] == 'E' || alphaDetectionBuffer[inc] == 'e') {
                    hasViewedE = 1;
                    E_Position = inc;
                    break;
                }
            }

            if (hasViewedE) {
                if (isValidScientificNotation(alphaDetectionBuffer, bufferIndex)) {
                    // check if it's complete (has digits after E)
                    if (E_Position == bufferIndex - 1) {
                        reportSyntaxError(alphaDetectionBuffer, "complete scientific notation", "add exponent value after E");
                        tokenArray[tokenCount].datatype = variable; // treat as invalid variable
                    } else {
                      // if dec == true == flt
                        bool hasDecimal = false;
                        for (int i = 0; i < E_Position; i++) {
                            if (alphaDetectionBuffer[i] == '.') {
                                hasDecimal = true;
                                break;
                            }
                        }
                        tokenArray[tokenCount].datatype = hasDecimal ? floatingPointLiteral : intLiteral;
                    }
                } else {
                    reportSyntaxError(alphaDetectionBuffer, "valid scientific notation", "format: [digits][.digits]E[+/-][digits]");
                    tokenArray[tokenCount].datatype = variable;
                }
            } else { 
                // enhanced: improved regular number validation
                int8_t allDigitsOrSingleDot = 1;
                int8_t hasSeenDecimalDot = 0;
                bool startsWithDigit = isdigit(alphaDetectionBuffer[0]);

                for (int inc = 0; inc < bufferIndex; inc += 1) {
                    if (alphaDetectionBuffer[inc] == '.') {
                        hasSeenDecimalDot += 1;
                        if (hasSeenDecimalDot > 1) {
                            reportSyntaxError(alphaDetectionBuffer, "single decimal point", "remove extra decimal points");
                            allDigitsOrSingleDot = 0;
                            break;
                        }
                    } else if (!isdigit((unsigned char)alphaDetectionBuffer[inc])) {
                        allDigitsOrSingleDot = 0;
                        break;
                    }
                }

                // enhanced: better literal classification
                if (allDigitsOrSingleDot == 1 && hasSeenDecimalDot == 0 && startsWithDigit) {
                    tokenArray[tokenCount].datatype = intLiteral;
                } else if (allDigitsOrSingleDot == 1 && hasSeenDecimalDot == 1 && startsWithDigit) {
                    // check for incomplete float (ends with dot)
                    if (alphaDetectionBuffer[bufferIndex-1] == '.') {
                        reportWarning(alphaDetectionBuffer, "incomplete float literal - add digits after decimal point");
                    }
                    tokenArray[tokenCount].datatype = floatingPointLiteral;
                } else {
                    // enhanced: validate variable names
                    if (startsWithDigit && !allDigitsOrSingleDot) {
                        reportSyntaxError(alphaDetectionBuffer, "valid identifier", "identifiers cannot start with digits");
                    }
                    tokenArray[tokenCount].datatype = variable;
                }
            }
        }

        strcpy(tokenArray[tokenCount].lexicalBuffer, alphaDetectionBuffer);
        tokenCount += 1;

        bufferIndex = 0;
        memset(alphaDetectionBuffer, 0, sizeof(alphaDetectionBuffer));
    }
}

void tokenize(char charFromTheFile) {
    // enhanced: track column position
    if (charFromTheFile == '\n') {
        columnNumber = 1;
    } else {
        columnNumber++;
    }
    
    if (isspace(charFromTheFile)) {
        finalizeAlphaToken();
        if (charFromTheFile == '\n') {
            lineNumber += 1;
        }
        return;
    }
    
    // enhanced: better character classification
    if (isalpha(charFromTheFile) || charFromTheFile == '_' || (bufferIndex > 0 && isalnum(charFromTheFile))) {
        if (bufferIndex < MAX_BUFFER_SIZE - 1) {
            alphaDetectionBuffer[bufferIndex] = charFromTheFile;
            bufferIndex += 1;
        } else {
            reportSyntaxError("token_too_long", "shorter identifier", "maximum token length exceeded");
        }
        return;
    }
    
    if (isdigit(charFromTheFile)) {
        if (bufferIndex < MAX_BUFFER_SIZE - 1) {
            alphaDetectionBuffer[bufferIndex] = charFromTheFile;
            bufferIndex += 1;
        }
        return;
    }
    
    if ((charFromTheFile == '.' || charFromTheFile == 'E' || charFromTheFile == 'e') && bufferIndex > 0) {
        if (bufferIndex < MAX_BUFFER_SIZE - 1) {
            alphaDetectionBuffer[bufferIndex] = charFromTheFile;
            bufferIndex += 1;
        }
        return;
    }

    if (tokenCount >= MAX_TOKENS_USAGE) {
        printf("\n");
        fprintf(stderr, "std:fatal_error::TOKENS_OVERFLOW_COUNT_REACHED:::optimize_code_or_increase_limit\n");
        printf("\n");
        return; 
    }
    
    switch (charFromTheFile) {
        case ',':
            finalizeAlphaToken();
            tokenArray[tokenCount].datatype = comma;
            tokenArray[tokenCount].lexicalBuffer[0] = ',';
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
            
        case ';':
            finalizeAlphaToken();
            tokenArray[tokenCount].datatype = semicolon;
            tokenArray[tokenCount].lexicalBuffer[0] = ';';
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
            
        case '+':
            finalizeAlphaToken();
            tokenArray[tokenCount].datatype = plus;
            tokenArray[tokenCount].lexicalBuffer[0] = '+';
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
            
        case '-':
            finalizeAlphaToken();
            tokenArray[tokenCount].datatype = minus;
            tokenArray[tokenCount].lexicalBuffer[0] = '-';
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
            
        case '*':
            finalizeAlphaToken();
            tokenArray[tokenCount].datatype = multiply;
            tokenArray[tokenCount].lexicalBuffer[0] = '*';
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
            
        case '/':
            finalizeAlphaToken();
            tokenArray[tokenCount].datatype = divide;
            tokenArray[tokenCount].lexicalBuffer[0] = '/';
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
            
        case '%':
            finalizeAlphaToken();
            tokenArray[tokenCount].datatype = modulo;
            tokenArray[tokenCount].lexicalBuffer[0] = '%';
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
            
        case '=':
            finalizeAlphaToken();
            tokenArray[tokenCount].datatype = assignment;
            tokenArray[tokenCount].lexicalBuffer[0] = '=';
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
            
        case '(':
            finalizeAlphaToken();
            tokenArray[tokenCount].datatype = LHcircleBracket;
            tokenArray[tokenCount].lexicalBuffer[0] = '(';
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
            
        case ')':
            finalizeAlphaToken();
            tokenArray[tokenCount].datatype = RHcircleBracket;
            tokenArray[tokenCount].lexicalBuffer[0] = ')';
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
            
        case '{':
            finalizeAlphaToken();
            tokenArray[tokenCount].datatype = LHcurlyBracket;
            tokenArray[tokenCount].lexicalBuffer[0] = '{';
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
            
        case '}':
            finalizeAlphaToken();
            tokenArray[tokenCount].datatype = RHcurlyBracket;
            tokenArray[tokenCount].lexicalBuffer[0] = '}';
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
            
        case '[':
            finalizeAlphaToken();
            tokenArray[tokenCount].datatype = LHsquareBracket;
            tokenArray[tokenCount].lexicalBuffer[0] = '[';
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
            
        case ']':
            finalizeAlphaToken();
            tokenArray[tokenCount].datatype = RHsquareBracket;
            tokenArray[tokenCount].lexicalBuffer[0] = ']';
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
            
        default:
            finalizeAlphaToken();
            reportWarning(&charFromTheFile, "unknown character ignored");
            break;
    }
}

/* temporary parser confirmation snippet */
void printTokens(void) {
    printf("\n=== TOKENIZATION RESULTS ===\n");
    printf("Total tokens: %d\n", tokenCount);
    printf("Errors: %d, Warnings: %d\n\n", errorCtx.errorCount, errorCtx.warningCount);
    
    for (int inc = 0; inc < tokenCount; inc += 1) {
        printf("token [%d]: type = [%s], value = '%s'\n",
            inc,
            tokenTypeToStr(tokenArray[inc].datatype),
            tokenArray[inc].lexicalBuffer
            // tokenArray[inc].lineNum,
            // tokenArray[inc].columnNum
        );
    }
    
    if (errorCtx.errorCount > 0) {
        printf("\n=== COMPILATION FAILED ===\n");
        printf("Fix [%d] error(s) before proceeding.\n", errorCtx.errorCount);
    } else {
        printf("\n=== TOKENIZATION SUCCESSFUL ===\n");
    }
}

// enhanced: comprehensive reset with error context
// void resetTokenizer(void) {
    // tokenCount = 0;
    // bufferIndex = 0;
    // lineNumber = 1;
    // columnNumber = 1;
    // memset(alphaDetectionBuffer, 0, sizeof(alphaDetectionBuffer));
    // memset(tokenArray, 0, sizeof(tokenArray));
    // memset(&errorCtx, 0, sizeof(ErrorContext));
// }
// 
// enhanced: new function to get error statistics
// int getErrorCount(void) {
    // return errorCtx.errorCount;
// }
// 
// int getWarningCount(void) {
    // return errorCtx.warningCount;
// }
