#include "arxtokenizer.h"
// #include "arxparser.h"

#include <stdbool.h>

int main(int argc, char **argv) {
  if (argc != 2) {
      fprintf(stderr, "std:fileinclusion_error::argv[1]=found_absent:::include_file\n");
    return EXIT_FAILURE;
  }

  char (*checkARX_argv1) = argv[1];

  size_t len = strlen(checkARX_argv1);
  if (len < 4 || strcmp(checkARX_argv1 + len - 4, ".arx") != 0) {
      fprintf(stderr, "std::file_Inclusion_Error::invalid_arx_extension:::<file>.arx\n");
      return EXIT_FAILURE;
  }

  FILE *fileToRead = fopen(argv[1], "r");
  if (!fileToRead || fileToRead == NULL) {
    fprintf(stderr, "std:fileread_error::argv[1].filecontent=NULL:::writedataARX=true\n");
    return EXIT_FAILURE;
  }

  char lineBuffer[1024];
  while (fgets(lineBuffer, sizeof(lineBuffer), fileToRead)) {
      for (char (*p) = lineBuffer; (*p) != '\0'; p += 1) {
          tokenize(*p);
      }
  }

  finalizeAlphaToken();

  /* temporary lexer/tokenizer/parser confirmation */
  printTokens();

  fclose(fileToRead);
  return EXIT_SUCCESS;
}
