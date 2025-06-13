#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define MAX_TOKENS_USAGE 0x7F6E
#define MAX_BUFFER_SIZE 1024

enum tokenType {
  _return,
  intLiteral,
  semicolon,
  variable, 
};

typedef struct {
  enum tokenType datatype;
  char lexicalBuffer[MAX_BUFFER_SIZE];
} token;

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
    } else if(strstr(alphaDetectionBuffer, "ret") != NULL) {
        // tokenArray[tokenCount].datatype = variable;
        fprintf(stderr, "\nstd:syntax:error::[TC%d/L%d]::[%s]:::[%s]\n", tokenCount, lineNumber, alphaDetectionBuffer, "_ret");
    } else {
        tokenArray[tokenCount].datatype = variable;
    }
}

  else if (bufferIndex > 0 && alphaDetectionBuffer[bufferIndex - 1] != ' ' && (!isspace(alphaDetectionBuffer[bufferIndex - 1]))) {
    if (isdigit(alphaDetectionBuffer[bufferIndex - 1])) {
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


void tokenize(char charFromTheFile) {
  if (isspace(charFromTheFile)) {
    finalizeAlphaToken();
  }

  else if (isalpha(charFromTheFile) || (bufferIndex > 0 && isalnum(charFromTheFile))) {
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
  
  // if its a semicolon -
  else if (charFromTheFile == ';' && tokenCount < MAX_TOKENS_USAGE) {
    finalizeAlphaToken();
    tokenArray[tokenCount].datatype = semicolon;
    tokenArray[tokenCount].lexicalBuffer[0] = ';';
    tokenArray[tokenCount].lexicalBuffer[1] = '\0';
    tokenCount += 1;
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "incorrect argc passage detected\n");      // maybe better than puts/printf since its for my bash terminal
    return EXIT_FAILURE;
  }

  FILE *fileToRead = fopen(argv[1], "r");
  if (!fileToRead) {                                           // cool way to type fileToRead == NULL
    fprintf(stderr, "[ERR]: argv[1] == found_absent\n");
    return EXIT_FAILURE;
  }

  char charFromFile;
  int charNumCounter = 0;
  while ((charFromFile = fgetc(fileToRead)) != EOF) {          
    tokenize(charFromFile);
    charNumCounter += 1;
    if (charFromFile == '\n') {
      lineNumber += 1;
    }
  }

  /* temporary parser confirmation */

  printf("\n0 - return\n1 - integer literal\n2 - semicolon\n3 - variable\n\n");

  for (int inc = 0; inc < tokenCount; inc += 1) {
    printf("token [%d]: type = [%d], value = [%s]\n", inc, tokenArray[inc].datatype, tokenArray[inc].lexicalBuffer);
  }

  finalizeAlphaToken();
  fclose(fileToRead);

  return EXIT_SUCCESS;
}
