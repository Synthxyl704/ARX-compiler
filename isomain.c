#include "./arxtokenizer.h" // ./out basically
#include "arxtokenizer.c"  

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "std:fileinclusion:error::argv[1]=found_absent:::include_file\n");      // maybe better than puts/printf since its for my bash terminal
    return EXIT_FAILURE;
  }

  FILE *fileToRead = fopen(argv[1], "r");
  if (!fileToRead) {                                           // cool way to type fileToRead == NULL
    fprintf(stderr, "std:fileread:error::argv[1].filecontent=NULL:::writedata=true\n");
    return EXIT_FAILURE;
  }

  char charFromFile;
  int charNumCounter = 0;
  while ((charFromFile = fgetc(fileToRead)) != EOF) {          
    tokenize(charFromFile);
    charNumCounter += 1;
    if (charFromFile == '\n') {                                // removed fgets(<code>); because it caused segfaults
      lineNumber += 1; 
    }
  }

  finalizeAlphaToken();

  /* temporary parser confirmation */
  printTokens();

  // example: std:syntax:error::[TC3/L1]::[retx6]:::[_ret]
  // TC[X] = TokenCount (in file) at [X] position horizontally wrt L[X]
  // L[X]  = Line count (in file) at [X] position vertically
  // : = program standard (intrinsic to program)
  // :: = file standard (may be intrinsic to both program + the file read)
  // ::: = program standard amelioration tweak string

  fclose(fileToRead);
  return EXIT_SUCCESS;
}
