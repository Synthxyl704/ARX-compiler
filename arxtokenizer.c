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
            if (exponentSignSeen) { return false; } // not found, but that does not make it invalid
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

        // known tokens
        // read tokens
        // if known = read = that d_t
        const char *knownTokens[] = {
            "_prov", "_ret", "_subprog", "subScope", "extScope",
            "mv_int", "mv_flt", "mv_chr", "mv_dbl", "mv_class" // this looks way clear too, ty @riz.dscrd
        };

        // enum tokentype { ... }
        enum tokenType knownDatatypes[] = {
            _main, _return, _subprog, subScope, extScope,
            intDatatype, floatDatatype, charDatatype, doubleDatatype, classDatatype
        };

        // entire array size / one
        // (10 apples) / (1 apple) = (10){count}
        int numKnownTokens = sizeof(knownTokens) / sizeof(knownTokens[0]);
        signed int matchedIndex = (-1);
        
        for (int inx = 0; inx < numKnownTokens; inx += 1) {
            if (strcmp(alphaDetectionBuffer, knownTokens[inx]) == 0) {
                matchedIndex = inx;
                break;
            }
        }

        if (matchedIndex != (-1)) {
            tokenArray[tokenCount].datatype = knownDatatypes[matchedIndex];
            goto store_token;
        }

        // -------------- SYNTAX ERRORS -----------------

        // syntax error handling, should a parser make this more efficient?
        if (strstr(alphaDetectionBuffer, "prov") && strcmp(alphaDetectionBuffer, "_prov") != 0) {
            reportSyntaxError(alphaDetectionBuffer, "_prov", "prepend underscore (_) prefix for main/prov keyword");
            tokenArray[tokenCount].datatype = variable;
            goto store_token;
        }
        if (strstr(alphaDetectionBuffer, "ret") && strcmp(alphaDetectionBuffer, "_ret") != 0) {
            reportSyntaxError(alphaDetectionBuffer, "_ret", "prepend underscore (_) prefix for return keyword");
            tokenArray[tokenCount].datatype = variable;
            goto store_token;
        }
        if (strstr(alphaDetectionBuffer, "subprog") && strcmp(alphaDetectionBuffer, "_subprog") != 0) {
            reportSyntaxError(alphaDetectionBuffer, "_subprog", "prepend underscore (_) prefix for function keyword");
            tokenArray[tokenCount].datatype = variable;
            goto store_token;
        }
        if ((strstr(alphaDetectionBuffer, "mvint") != NULL || strstr(alphaDetectionBuffer, "mv_int") != NULL) &&
            strcmp(alphaDetectionBuffer, "mv_int") != 0) {
            reportSyntaxError(alphaDetectionBuffer, "mv_int", "syntax seems wrong, did you mean \"mv_int?\"");
            tokenArray[tokenCount].datatype = variable;
            goto store_token;
        }

        int ePos = -1;
        for (int inx = 0; inx < bufferIndex; inx += 1) {
            if (alphaDetectionBuffer[inx] == 'E' || alphaDetectionBuffer[inx] == 'e') {
                ePos = inx;
                break;
            }
        }

        if (ePos != -1) {
            if (isValidScientificNotation(alphaDetectionBuffer, bufferIndex)) {
                bool hasDecimal = false;
                for (int inx = 0; inx < ePos; inx += 1) {
                    if (alphaDetectionBuffer[inx] == '.') {
                        hasDecimal = true;
                        break;
                    }
                } tokenArray[tokenCount].datatype = hasDecimal ? floatingPointLiteral : intLiteral;
            } else {
                reportSyntaxError(
                    alphaDetectionBuffer, 
                    "valid scientific notation",
                    "format: [digits][.digits]E[+/-][digits]"
                ); tokenArray[tokenCount].datatype = variable;
            } goto store_token;
        } // this is so shit i hope i never work with this again

        bool allDigitsOrDot = true;
        int dotCount = 0;
        bool startsWithDigit = isdigit((unsigned char)alphaDetectionBuffer[0]);

        for (int inx = 0; inx < bufferIndex; inx += 1) {
            if (alphaDetectionBuffer[inx] == '.') {
                dotCount += 1; // i like longhand notational equivalents
                if (dotCount > 1) {
                    reportSyntaxError(alphaDetectionBuffer, "single decimal point", "remove extra decimal points");
                    allDigitsOrDot = false;
                    break;
                }
            } else if (!isdigit((unsigned char)alphaDetectionBuffer[inx])) {
                allDigitsOrDot = false;
                break;
            }
        }

        if (allDigitsOrDot && dotCount == 0 && startsWithDigit) {
            tokenArray[tokenCount].datatype = intLiteral;
            goto store_token;
        }

        if (allDigitsOrDot && dotCount == 1 && startsWithDigit) {
            if (alphaDetectionBuffer[bufferIndex - 1] == '.') {
                reportWarning(alphaDetectionBuffer, "incomplete float literal - add digits after decimal point");
            }
            tokenArray[tokenCount].datatype = floatingPointLiteral;
            goto store_token;
        }

        if (startsWithDigit && !allDigitsOrDot) {
            reportSyntaxError(alphaDetectionBuffer, "valid identifier", "identifiers cannot start with digits");
        }

        tokenArray[tokenCount].datatype = variable;

    store_token:
        strcpy(tokenArray[tokenCount].lexicalBuffer, alphaDetectionBuffer);
        (tokenCount)++; // like look at this stupid unary increment its so bad
        bufferIndex = 0;
        
        // memset(memblock, value, byteSize);
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
            alphaDetectionBuffer[bufferIndex++] = charFromTheFile; // [++bufferIndex]
        } else {
            // void reportSyntaxError(const char *found, const char *expected, const char *suggestion);
            reportSyntaxError( 
                "token_too_long", 
                "shorter_identifier",
                "maximum_token_length_exceeded"
            );
        } return;
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

            enum tokenType tex = (
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

            // not the LaTeX's { TeX }
            tokenArray[tokenCount].datatype = tex;
            tokenArray[tokenCount].lexicalBuffer[0] = charFromTheFile;
            tokenArray[tokenCount].lexicalBuffer[1] = '\0';
            tokenCount += 1;

            break;
        }

        default:
            finalizeAlphaToken();
            reportWarning(&charFromTheFile, "unknown_character_eluded");
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
