#include "tokenizer.h"
#include <stdbool.h>

token tokenArray[MAX_TOKENS_USAGE];

char alphaDetectionBuffer[MAX_BUFFER_SIZE] = {0};
char linearOneLineSize[0x400];      // 1024

int bufferIndex = 0;
int tokenCount = 0;
int lineNumber = 1;

void finalizeAlphaToken() {
  if (bufferIndex > 0 && tokenCount < MAX_TOKENS_USAGE) {
    alphaDetectionBuffer[bufferIndex] = '\0';

    
    // ------------------- COMPILER SYNTAX AND ERROR SECTION END --------------------- // 
    
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

    else if(strcmp(alphaDetectionBuffer, "mv_dbl") == 0) {
      tokenArray[tokenCount].datatype = doubleDatatype;
    }

    // TODO: fix this shit
  
    else if (strstr(alphaDetectionBuffer, "prov") != NULL) {
      fprintf(stderr, "std:syntax_error::[TC%d/L%d]::[%s]:::[%s]\n", tokenCount, lineNumber, alphaDetectionBuffer, "_prov");
    } 
    
    else if (strstr(alphaDetectionBuffer, "ret") != NULL) {
      fprintf(stderr, "std:syntax_error::[TC%d/L%d]::[%s]:::[%s]\n", tokenCount, lineNumber, alphaDetectionBuffer, "_ret");
    }

    else if (strstr(alphaDetectionBuffer, "subprog") != NULL) {
      fprintf(stderr, "std:syntax_error::[TC%d/L%d]::[%s]:::[%s]\n", tokenCount, lineNumber, alphaDetectionBuffer, "_subprog");
    }

    else if (strstr(alphaDetectionBuffer, "mv_int") != NULL && strcmp(alphaDetectionBuffer, "mvint") == 0) {
    fprintf(stderr, "std:syntax_error::[TC%d/L%d]::[%s]:::[%s]\n",
        tokenCount, lineNumber, alphaDetectionBuffer, "mv_int::datatype_int");
}

    else if (strstr(alphaDetectionBuffer, "mvflt") != NULL) {
      fprintf(stderr, "std:syntax_error::[TC%d/L%d]::[%s]:::[%s]\n", tokenCount, lineNumber, alphaDetectionBuffer, "mv_flt::datatype_float"); 
    }

    else if (strstr(alphaDetectionBuffer, "mvchr") != NULL) {
      fprintf(stderr, "std:syntax_error::[TC%d/L%d]::[%s]:::[%s]\n", tokenCount, lineNumber, alphaDetectionBuffer, "mv_chr::datatype_char");
    }

    else if (strstr(alphaDetectionBuffer, "mvdbl") != NULL) {
      fprintf(stderr, "std:syntax_error::[TC%d/L%d]::[%s]:::[%s]\n", tokenCount, lineNumber, alphaDetectionBuffer, "mv_dbl::datatype_double");
    }

    // ------------------- COMPILER SYNTAX AND ERROR SECTION END --------------------- //

    else {
  // --------------------------------------------
  // scientific/exponentiational notation support
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
      int allDigitsB4E = 1;
      int hasSeenDecimalDot = 0;
      for (int inc = 0; inc < E_Position; inc += 1) {
        if (alphaDetectionBuffer[inc] == '.') {
            hasSeenDecimalDot += 1;
              if (hasSeenDecimalDot > 1) {
                allDigitsB4E = 0;
                fprintf(stderr, "\nstd:syntax_error::[TC%d/L%d]::[%s]:::[%s]\n\n", tokenCount, lineNumber, alphaDetectionBuffer, "multiple_decimal_digits=invalid");
                break;
              }
          } else if (!isdigit((unsigned char)alphaDetectionBuffer[inc])) {
            allDigitsB4E = 0;
            break;
        }
      }
      int allDigitsAfterE = 1;
      if (E_Position == bufferIndex - 1) {
        allDigitsAfterE = 0; // nothing after E/e
      }
      for (int inc = E_Position + 1; inc < bufferIndex; inc += 1) {
        if (!isdigit((unsigned char)alphaDetectionBuffer[inc])) {
          allDigitsAfterE = 0;
          break;
        }
      }

      if (allDigitsAfterE == 1 && allDigitsB4E == 1) {
        if (hasSeenDecimalDot == 0) {
            tokenArray[tokenCount].datatype = intLiteral;
        } else if (hasSeenDecimalDot == 1) {
            tokenArray[tokenCount].datatype = floatingPointLiteral;
        } else {
            tokenArray[tokenCount].datatype = variable;
        }
      } else {
          tokenArray[tokenCount].datatype = variable;
      }
  }

  else { // only runs if NOT scientific notation - balances shit
      int8_t allDigitsOrSingleDot = 1;
      int8_t hasSeenDecimalDot = 0;
      for (int inc = 0; inc < bufferIndex; inc += 1) {
          if (alphaDetectionBuffer[inc] == '.') {
              hasSeenDecimalDot += 1;
              if (hasSeenDecimalDot > 1) {
                  allDigitsOrSingleDot = 0;
                  break;
              }
          } else if (!isdigit((unsigned char)alphaDetectionBuffer[inc])) {
              allDigitsOrSingleDot = 0;
              break;
          }
      }
      if (allDigitsOrSingleDot == 1 && hasSeenDecimalDot == 0) {
          tokenArray[tokenCount].datatype = intLiteral;
      } else if (allDigitsOrSingleDot == 1 && hasSeenDecimalDot == 1) {
          tokenArray[tokenCount].datatype = floatingPointLiteral;
      } else {
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
  if (isspace(charFromTheFile)) {
    finalizeAlphaToken();
    return;
  }
  
  if (isalpha(charFromTheFile) || charFromTheFile == '_' || (bufferIndex > 0 && isalnum(charFromTheFile))) {
    if (bufferIndex < MAX_BUFFER_SIZE - 1) {
      alphaDetectionBuffer[bufferIndex] = charFromTheFile;
      bufferIndex += 1;
    }
    return;
  }
  
  if (isdigit(charFromTheFile)) {
    alphaDetectionBuffer[bufferIndex] = charFromTheFile;
    bufferIndex += 1;
    return;
  }
  
  if (charFromTheFile == '.' && bufferIndex > 0) {
    alphaDetectionBuffer[bufferIndex] = charFromTheFile;
    bufferIndex += 1;
    return;
  }

  if (tokenCount >= MAX_TOKENS_USAGE) {
    fprintf(stderr, "%s", "std::TOKENS_OVERFLOW_COUNT_REACHED:::improve_optimization");
    return; 
  }
  
  switch (charFromTheFile) {
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

      break;
  }
  // i want to kill myself
}

/* temporary  confirmation */
void printTokens(void) {\
  printf("\n");
  for (int inc = 0; inc < tokenCount; inc += 1) {

    // if (inc < 10) {
    //   printf(" token [%d]: type = [%s], value = %s\n",
    //   inc,
    //   tokenTypeToStr(tokenArray[inc].datatype),
    //   tokenArray[inc].lexicalBuffer
    // );

    // continue;
    // }

    printf("token [%d]: type = [%s], value = %s\n",
      inc,
      tokenTypeToStr(tokenArray[inc].datatype),
      tokenArray[inc].lexicalBuffer
    );
  }
}

// hell yeah
void resetTokenizer(void) {
  tokenCount = 0;
  bufferIndex = 0;
  lineNumber = 1;
  memset(alphaDetectionBuffer, 0, sizeof(alphaDetectionBuffer));
  memset(tokenArray, 0, sizeof(tokenArray));
}
