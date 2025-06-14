#include "tokenizer.h"

// need to know when to extern them or else its gonna break again
token tokenArray[MAX_TOKENS_USAGE];
char alphaDetectionBuffer[MAX_BUFFER_SIZE] = {0};
char linearOneLineSize[0x400];      // 1024
int bufferIndex = 0;
int tokenCount = 0;
int lineNumber = 1;

void finalizeAlphaToken() {

  /*
   
  if (bufferIndex > 0 && tokenCount < MAX_TOKENS_USAGE) {
    alphaDetectionBuffer[bufferIndex] = '\0'; 
    tokenArray[tokenCount].datatype = strcmp(alphaDetectionBuffer, "_ret") == 0 ? _return 
                                                                                : variable;

    if (alphaDetectionBuffer[bufferIndex - 1] != isspace(alphaDetectionBuffer[bufferIndex - 1])) { 
      tokenArray[tokenCount].datatype = isdigit(alphaDetectionBuffer[bufferIndex - 1]) ? intLiteral : variable;
    } 

    */

  if (bufferIndex > 0 && tokenCount < MAX_TOKENS_USAGE) {
    alphaDetectionBuffer[bufferIndex] = '\0';
    
    if (strcmp(alphaDetectionBuffer, "_ret") == 0) {
        tokenArray[tokenCount].datatype = _return;
    } 

    else if(strstr(alphaDetectionBuffer, "ret") != NULL) {
        // tokenArray[tokenCount].datatype = variable;
        fprintf(stderr, "\nstd:syntax:error::[TC%d/L%d]::[%s]:::[_ret]\n", tokenCount, lineNumber, alphaDetectionBuffer, "_ret");
        tokenArray[tokenCount].datatype = variable;  // Still tokenize it as variable
      } 

    else {
      int allDigits = 1;
        for (int inc = 0; inc < bufferIndex; inc += 1) {
          if (!isdigit(alphaDetectionBuffer[inc])) {
            allDigits = 0;
            break;
          }  // why is this shit ending before the keyword LOL
        }
        
      if (allDigits) {
        tokenArray[tokenCount].datatype = intLiteral;
      } else {
        tokenArray[tokenCount].datatype = variable;                              // note to self: cannot rewrite [else if] statements after [else] is already written (wdift)
      }
    }
    
    strcpy(tokenArray[tokenCount].lexicalBuffer, alphaDetectionBuffer);          // copy finalized token to alphaBuffer for (a) finalized reading
    tokenCount += 1;

    bufferIndex = 0;              
    memset(alphaDetectionBuffer, 0, sizeof(alphaDetectionBuffer));               // reset the buffer back -- memset(where, how_much, sizeof());
  }
}

void tokenize(char charFromTheFile) {
  if (isspace(charFromTheFile)) {
    finalizeAlphaToken();
  }

  else if (isalpha(charFromTheFile) || charFromTheFile == '_' || (bufferIndex > 0 && isalnum(charFromTheFile))) {
    if (bufferIndex < MAX_BUFFER_SIZE - 1) {
      alphaDetectionBuffer[bufferIndex] = charFromTheFile;
      bufferIndex += 1;
    }
  }

  /* ---------------------------------------------------------------- //
 
      else if (isalpha(charFromTheFile)) {
        alphaDetectionBuffer[bufferIndex += 1] = charFromTheFile; 
      }
  // ---------------------------------------------------------------- */ 
  
  else if(isdigit(charFromTheFile)) {
    alphaDetectionBuffer[bufferIndex] = charFromTheFile;
    bufferIndex += 1;
  }

  // -----------------
  /* OPERATORS HERE */
  
  // if its a semicolon ;
  else if (charFromTheFile == ';' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = semicolon;
    tokenArray[tokenCount].lexicalBuffer[0] = ';';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  }

  // if its a plus operator +
  else if (charFromTheFile == '+' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = plus;
    tokenArray[tokenCount].lexicalBuffer[0] = '+';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  }
  
  // if its a minus operator - ?
  else if (charFromTheFile == '-' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = minus;
    tokenArray[tokenCount].lexicalBuffer[0] = '-';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  }
  
  // if its a multiplication operator *
  else if (charFromTheFile == '*' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = multiply;
    tokenArray[tokenCount].lexicalBuffer[0] = '*';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  }
  
  // if its a division operator /
  else if (charFromTheFile == '/' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = divide;
    tokenArray[tokenCount].lexicalBuffer[0] = '/';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  }
  
  // if its a modulo operator %
  else if (charFromTheFile == '%' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = modulo;
    tokenArray[tokenCount].lexicalBuffer[0] = '%';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  }
  
  // if its an assignment operator =
  else if (charFromTheFile == '=' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = assignment;
    tokenArray[tokenCount].lexicalBuffer[0] = '=';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  } // pretty repetitive but at least now its concrete
}

/* temporary parser confirmation */
void printTokens(void) {
  for (int inc = 0; inc < tokenCount; inc += 1) {
    printf("token [%d]: type = [%d], value = [%s]\n", inc, tokenArray[inc].datatype, tokenArray[inc].lexicalBuffer);
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
