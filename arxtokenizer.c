#include "arxtokenizer.h"
#include <stdbool.h>

token tokenArray[MAX_TOKENS_USAGE];

char alphaDetectionBuffer[MAX_BUFFER_SIZE] = {0};
char linearOneLineSize[0x400];

int bufferIndex = 0;
int tokenCount = 0;
int lineNumber = 1;
int columnNumber = 1;

typedef struct {
    int errorCount;
    int warningCount;
    char lastErrorToken[MAX_BUFFER_SIZE];
    int lastErrorLine;
    int lastErrorColumn;
} ErrorContext;

static ErrorContext errorCtx = {0};

int getErrorCount(void) {
    return errorCtx.errorCount;
}

int getWarningCount(void) {
    return errorCtx.warningCount;
}

void reportSyntaxError(const char *found, const char *expected, const char *suggestion) {
    fprintf(stderr, "std:syntax_error::[TC%d/L%d:C%d]::[%s]:::[%s]:::[suggestion: %s]\n",
            tokenCount, lineNumber, columnNumber, found, expected, suggestion);
    errorCtx.errorCount += 1;
    strcpy(errorCtx.lastErrorToken, found);
    errorCtx.lastErrorLine = lineNumber;
    errorCtx.lastErrorColumn = columnNumber;
}

void reportWarning(const char *tokenStr, const char *warning) {
    fprintf(stderr, "std:warning::[TC%d/L%d:C%d]::[%s]:::[%s]\n",
            tokenCount, lineNumber, columnNumber, tokenStr, warning);
    errorCtx.warningCount += 1;
}

// e+21
// xE21
// Xe21
bool isValidScientificNotation(const char *buffer, int length) {
    int ePos = (-1);
    bool hasDigitsBeforeE = false;
    bool hasDigitsAfterE = false;
    int dotCount = 0;
    bool exponentSignSeen = false;

    for (int inx = 0; inx < length; inx += 1) {
        char c = buffer[inx];

        if (c == 'E' || c == 'e') {
            if (ePos != (-1)) {
                return false;
            } ePos = inx;
            
            continue;
        }

        // ch
        if (c == '.') {
            dotCount += 1;
            if (dotCount > 1) { return false; } 
            if (ePos != (-1)) { return false; }

            continue;
        }

        if (isdigit((unsigned char)c)) {
            if (ePos == -1 || inx < ePos) hasDigitsBeforeE = true;
            else hasDigitsAfterE = true;
            
            continue;
        }

        if (c == '+' || c == '-') {
            if (ePos == -1 || inx != ePos + 1) { return false; }
            if (exponentSignSeen) { return false; } // not int
            exponentSignSeen = true;
            continue;
        }

        // else
        return false;
    }

    if (ePos == 0 || ePos == length - 1) { return false; }
    if (!hasDigitsBeforeE || !hasDigitsAfterE) { return false; }
    
    return true;
}

void finalizeAlphaToken(void) {
    if (bufferIndex > 0 && tokenCount < MAX_TOKENS_USAGE) {
        alphaDetectionBuffer[bufferIndex] = '\0';

        if (strcmp(alphaDetectionBuffer, "_prov") == 0) {
            tokenArray[tokenCount].datatype = _main;
            goto store_token; // IM SOS ORRY I DONT KNWO3N3 WHAT ELSEFTO DO 2
        }
        if (strcmp(alphaDetectionBuffer, "_ret") == 0) {
            tokenArray[tokenCount].datatype = _return;
            goto store_token;
        }
        if (strcmp(alphaDetectionBuffer, "_subprog") == 0) {
            tokenArray[tokenCount].datatype = _subprog;
            goto store_token;
        }
        if (strcmp(alphaDetectionBuffer, "subScope") == 0) {
            tokenArray[tokenCount].datatype = subScope;
            goto store_token;
        }
        if (strcmp(alphaDetectionBuffer, "extScope") == 0) {
            tokenArray[tokenCount].datatype = extScope;
            goto store_token;
        }

        if (strcmp(alphaDetectionBuffer, "mv_int") == 0) {
            tokenArray[tokenCount].datatype = intDatatype;
            goto store_token;
        }
        if (strcmp(alphaDetectionBuffer, "mv_flt") == 0) {
            tokenArray[tokenCount].datatype = floatDatatype;
            goto store_token;
        }
        if (strcmp(alphaDetectionBuffer, "mv_chr") == 0) {
            tokenArray[tokenCount].datatype = charDatatype;
            goto store_token;
        }
        if (strcmp(alphaDetectionBuffer, "mv_dbl") == 0) {
            tokenArray[tokenCount].datatype = doubleDatatype;
            goto store_token;
        }
        if (strcmp(alphaDetectionBuffer, "mv_class") == 0) {
            tokenArray[tokenCount].datatype = classDatatype;
            goto store_token;
        }

        // if (strcmp(alphaDetectionBuffer, "mv_int*") == 0) {
        //     tokenArray[tokenCount].datatype = intPointerDatatype;
        //     goto store_token;
        // }
        // if (strcmp(alphaDetectionBuffer, "mv_flt*") == 0) {
        //     tokenArray[tokenCount].datatype = floatPointerDatatype;
        //     goto store_token;
        // }
        // if (strcmp(alphaDetectionBuffer, "mv_dbl*") == 0) {
        //     tokenArray[tokenCount].datatype = doublePointerDatatype;
        //     goto store_token;
        // }
        // if (strcmp(alphaDetectionBuffer, "mv_chr*") == 0) {
        //     tokenArray[tokenCount].datatype = charPointerDatatype;
        //     goto store_token;
        // }

        // if (strcmp(alphaDetectionBuffer, "mv_class*") == 0) {
        //     tokenArray[tokenCount].datatype = classPointerDatatype;
        //     goto store_token;
        // }

        if (strstr(alphaDetectionBuffer, "prov") != NULL && strcmp(alphaDetectionBuffer, "_prov") != 0) {
            reportSyntaxError(alphaDetectionBuffer, "_prov", "use underscore prefix for keywords");
            tokenArray[tokenCount].datatype = variable;
            goto store_token;
        }
        if (strstr(alphaDetectionBuffer, "ret") != NULL && strcmp(alphaDetectionBuffer, "_ret") != 0) {
            reportSyntaxError(alphaDetectionBuffer, "_ret", "use underscore prefix for return keyword");
            tokenArray[tokenCount].datatype = variable;
            goto store_token;
        }
        if (strstr(alphaDetectionBuffer, "subprog") != NULL && strcmp(alphaDetectionBuffer, "_subprog") != 0) {
            reportSyntaxError(alphaDetectionBuffer, "_subprog", "use underscore prefix for function keyword");
            tokenArray[tokenCount].datatype = variable;
            goto store_token;
        }

        if ((strstr(alphaDetectionBuffer, "mvint") != NULL || strstr(alphaDetectionBuffer, "mv_int") != NULL) &&
            strcmp(alphaDetectionBuffer, "mv_int") != 0
            // strcmp(alphaDetectionBuffer, "mv_int *") != 0 &&
            // strcmp(alphaDetectionBuffer, "mv_int*") != 0
            // this is not going to work this way, i need to learn AST
            ) {
            reportSyntaxError(alphaDetectionBuffer, "mv_int", "correct syntax is mv_int for integer datatype");
            tokenArray[tokenCount].datatype = variable;
            goto store_token;
        }

        int ePos = (-1);
        for (int inx = 0; inx < bufferIndex; inx += 1) {
            if (alphaDetectionBuffer[inx] == 'E' || alphaDetectionBuffer[inx] == 'e') {
                ePos = inx;
                break;
            }
        }

        if (ePos != -1) {
            if (isValidScientificNotation(alphaDetectionBuffer, bufferIndex)) {
                bool hasDecimal = false;
                for (int inx = 0; inx < ePos; inx++) {
                    if (alphaDetectionBuffer[inx] == '.') { hasDecimal = true; break; }
                }
                tokenArray[tokenCount].datatype = hasDecimal ? floatingPointLiteral : intLiteral;
            } else {
                reportSyntaxError(alphaDetectionBuffer, "valid scientific notation",
                                  "format: [digits][.digits]E[+/-][digits]");
                tokenArray[tokenCount].datatype = variable;
            }

            goto store_token;
        }

        bool allDigitsOrDot = true;
        int dotCount = 0;
        bool startsWithDigit = isdigit((unsigned char)alphaDetectionBuffer[0]);

        for (int inx = 0; inx < bufferIndex; (inx)++) /* i hate unary increment, its like lemonade without sugar or something */ {
            if (alphaDetectionBuffer[inx] == '.') {   // we should call [++var] as "backshot increment" instead LOL
                dotCount += 1;                        // that means [var++] is missionary XD
                if (dotCount > 1) {
                    reportSyntaxError(alphaDetectionBuffer, "single decimal point",
                                                            "remove extra decimal points"
                    );

                    allDigitsOrDot = false;
                    break;
                }
            } else if (!isdigit(/* its a shock this is even valid */(unsigned char)alphaDetectionBuffer[inx])) {
                allDigitsOrDot = false;
                break;
            }
        }

        if (allDigitsOrDot && dotCount == 0 && startsWithDigit) {
            tokenArray[tokenCount].datatype = intLiteral;
            goto store_token;
        }

        if (allDigitsOrDot && dotCount == 1 && startsWithDigit) {
            if (alphaDetectionBuffer[bufferIndex-1] == '.') {
                reportWarning(alphaDetectionBuffer,
                              "incomplete float literal - add digits after decimal point");
            }
            tokenArray[tokenCount].datatype = floatingPointLiteral;
            goto store_token;
        }

        if (startsWithDigit && !allDigitsOrDot) {
            reportSyntaxError(alphaDetectionBuffer, "valid identifier",
                              "identifiers cannot start with digits");
        }
        tokenArray[tokenCount].datatype = variable;

    store_token:
        strcpy(tokenArray[tokenCount].lexicalBuffer, alphaDetectionBuffer);
        tokenCount += 1;
        bufferIndex = 0;
        memset(alphaDetectionBuffer, 0, sizeof(alphaDetectionBuffer));
    }
}

void tokenize(char charFromTheFile) {
    // static bool inCharLiteral = false;
    // static char charLitBuffer[4] = {0};
    // static int charLitIndex = 0;

    if (charFromTheFile == '\'') {
        // inCharLiteral = true;
        // charLitIndex = 0;
        return;
    }

    if (charFromTheFile == '\n') {
        lineNumber += 1;
        columnNumber = 1;
        finalizeAlphaToken();
        return;
    } columnNumber += 1;

    if (isspace(charFromTheFile)) {
        finalizeAlphaToken();
        return;
    }

    if (isalpha(charFromTheFile) || charFromTheFile == '_' ||
        (bufferIndex > 0 && isalnum(charFromTheFile))) {
        if (bufferIndex < MAX_BUFFER_SIZE - 1) {
            alphaDetectionBuffer[bufferIndex++] = charFromTheFile;
        } else {
            reportSyntaxError("token_too_long", "shorter_identifier",
                              "maximum_token_length_exceeded");
        }

        return;
    }

    if (isdigit(charFromTheFile)) {
        if (bufferIndex < MAX_BUFFER_SIZE - 1) {
            alphaDetectionBuffer[bufferIndex++] = charFromTheFile;
        }
        return;
    }

    if ((charFromTheFile == '.' || charFromTheFile == 'E' || charFromTheFile == 'e')
        && bufferIndex > 0) {
        if (bufferIndex < MAX_BUFFER_SIZE - 1) {
            alphaDetectionBuffer[bufferIndex++] = charFromTheFile;
        }
        return;
    }

    if (tokenCount >= MAX_TOKENS_USAGE) {
        fprintf(stderr, "std:fatal_error::TOKENS_OVERFLOW_COUNT_REACHED:::optimize_code_or_increase_limit\n");
        return;
    }

    switch (charFromTheFile) {
        case ',': case ';': case '+': case '-': case '*':
        case '/': case '%': case '=': case '(': case ')':
        case '{': case '}': case '[': case ']': {
            finalizeAlphaToken();
            enum tokenType t = (
                        charFromTheFile == ',' ? comma :
                        charFromTheFile == ';' ? semicolon :
                        charFromTheFile == '+' ? plus :
                        charFromTheFile == '-' ? minus :
                        charFromTheFile == '*' ? multiply :
                        charFromTheFile == '/' ? divide :
                        charFromTheFile == '%' ? modulo :
                        charFromTheFile == '=' ? assignment :
                        charFromTheFile == '(' ? LHcircleBracket :
                        charFromTheFile == ')' ? RHcircleBracket :
                        charFromTheFile == '{' ? LHcurlyBracket :
                        charFromTheFile == '}' ? RHcurlyBracket :
                        charFromTheFile == '[' ? LHsquareBracket : RHsquareBracket
            );

            tokenArray[tokenCount].datatype = t;
            tokenArray[tokenCount].lexicalBuffer[0] = charFromTheFile;
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;
            break;
        }
        default:
            finalizeAlphaToken();
            reportWarning(&charFromTheFile, "unknown character ignored");
            break;
    }
}

const char *tokenTypeToStr(enum tokenType d_type) {
    switch (d_type) {
        case intDatatype:    return "integer datatype";
        case floatDatatype:  return "floating point datatype";
        case doubleDatatype: return "double precision datatype";
        case charDatatype:   return "character datatype";

        case classDatatype:           return "class datatype";
        case intPointerDatatype:      return "integer pointer datatype";
        case floatPointerDatatype:    return "floating point pointer datatype";
        case doublePointerDatatype:   return "double precision pointer datatype";
        case charPointerDatatype:     return "character pointer datatype";
        case classPointerDatatype:    return "class pointer datatype";

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

        // uwu sempai its sooooOOOOoooOoOoo big~~
        case semicolon:    return "semicolon";
        case comma:        return "comma";
        case plus:         return "plus";
        case multiply:     return "multiply";
        case minus:        return "minus";
        case modulo:       return "modulo";
        case divide:       return "divide";
        case assignment:   return "assignment";

        case _main:        return "_main";
        case _return:      return "_return";
        case _subprog:     return "_subprog";

        default:           return "unknown_token";
    }
}

/* temporary working confirmation */
void printTokens(void) {
    printf("\n=== TOKENIZATION RESULTS ===\n");
    printf("Total tokens: %d\n", tokenCount);
    printf("Errors: %d, Warnings: %d\n\n", errorCtx.errorCount, errorCtx.warningCount);

    for (int inc = 0; inc < tokenCount; inc += 1) {
        printf("token [%d]: type = [%s], value = '%s'\n",
               inc,
               tokenTypeToStr(tokenArray[inc].datatype),
               tokenArray[inc].lexicalBuffer);
    }

    if (errorCtx.errorCount > 0) {
        printf("\n=== COMPILATION FAILED ===\n");
        printf("[%d] Errors found\n", errorCtx.errorCount);
    } else {
        printf("\n=== TOKENIZATION SUCCESSFUL ===\n");
    }
}
