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

    if (strcmp(alphaDetectionBuffer, "_prov") == 0) {
      tokenArray[tokenCount].datatype = _main;
    } 
    
    else if (strcmp(alphaDetectionBuffer, "_ret") == 0) {
      tokenArray[tokenCount].datatype = _return;
    } 
    
    else if (strcmp(alphaDetectionBuffer, "mv_int") == 0) {
      tokenArray[tokenCount].datatype = intDatatype;
    } 

    else if (strcmp(alphaDetectionBuffer, "mv_flt") == 0) {
      tokenArray[tokenCount].datatype = floatDatatype;
    } 
    
    else if (strstr(alphaDetectionBuffer, "prov") != NULL) {
      fprintf(stderr, "\nstd:syntax_error::[TC%d/L%d]::[%s]:::[%s]", tokenCount, lineNumber, alphaDetectionBuffer, "_prov");
      tokenArray[tokenCount].datatype = variable;
    } 
    
    else if (strstr(alphaDetectionBuffer, "ret") != NULL) {
      fprintf(stderr, "\nstd:syntax_error::[TC%d/L%d]::[%s]:::[%s]\n\n", tokenCount, lineNumber, alphaDetectionBuffer, "_ret");
      tokenArray[tokenCount].datatype = variable;
    } else {


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

    if (allDigitsAfterE && allDigitsB4E) {
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

  else { // only runs if NOT scientific notation - balances
    int8_t allDigits = 1;
    int8_t hasSeenDecimalDot = 0;
    for (int inc = 0; inc < bufferIndex; inc += 1) {
      if (alphaDetectionBuffer[inc] == '.') {
        hasSeenDecimalDot += 1;
      } else if (!isdigit((unsigned char)alphaDetectionBuffer[inc])) {
        allDigits = 0;
        break;
      }
    }
    if (allDigits == 1 && hasSeenDecimalDot == 0) {
      tokenArray[tokenCount].datatype = intLiteral;
    } else if (allDigits == 1 && hasSeenDecimalDot == 1) {
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
  } else if (isalpha(charFromTheFile) || charFromTheFile == '_' || (bufferIndex > 0 && isalnum(charFromTheFile))) {
    if (bufferIndex < MAX_BUFFER_SIZE - 1) {
      alphaDetectionBuffer[bufferIndex] = charFromTheFile;
      bufferIndex += 1;
    }
  } else if (isdigit(charFromTheFile)) {
    alphaDetectionBuffer[bufferIndex] = charFromTheFile;
    bufferIndex += 1;
  } else if (charFromTheFile == ';' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = semicolon;
    tokenArray[tokenCount].lexicalBuffer[0] = ';';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  } else if (charFromTheFile == '+' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = plus;
    tokenArray[tokenCount].lexicalBuffer[0] = '+';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  } else if (charFromTheFile == '-' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = minus;
    tokenArray[tokenCount].lexicalBuffer[0] = '-';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  } else if (charFromTheFile == '*' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = multiply;
    tokenArray[tokenCount].lexicalBuffer[0] = '*';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  } else if (charFromTheFile == '/' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = divide;
    tokenArray[tokenCount].lexicalBuffer[0] = '/';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  } else if (charFromTheFile == '%' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = modulo;
    tokenArray[tokenCount].lexicalBuffer[0] = '%';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  } else if (charFromTheFile == '=' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = assignment;
    tokenArray[tokenCount].lexicalBuffer[0] = '=';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  } else if (charFromTheFile == '(' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = LHcircleBracket;
    tokenArray[tokenCount].lexicalBuffer[0] = '(';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  } else if (charFromTheFile == ')' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = RHcircleBracket;
    tokenArray[tokenCount].lexicalBuffer[0] = ')';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  } else if (charFromTheFile == '{' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = LHcurlyBracket;
    tokenArray[tokenCount].lexicalBuffer[0] = '{';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  } else if (charFromTheFile == '}' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = RHcurlyBracket;
    tokenArray[tokenCount].lexicalBuffer[0] = '}';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  } else if (charFromTheFile == '[' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = LHsquareBracket;
    tokenArray[tokenCount].lexicalBuffer[0] = '[';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  } else if (charFromTheFile == ']' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = RHsquareBracket;
    tokenArray[tokenCount].lexicalBuffer[0] = ']';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  }
}

/* temporary parser confirmation */
void printTokens(void) {
  for (int inc = 0; inc < tokenCount; inc += 1) {
    printf("token [%d]: type = [%s], value = [%s]\n",
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
