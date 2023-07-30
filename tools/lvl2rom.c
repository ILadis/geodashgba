
#define NOGBA

#include <game/collection.h>
#include <stdio.h>

int main(int argc, char **argv) {
  FILE *log = stderr;

  if(argc != 3) {
    fprintf(log, "Invalid number of arguments given.\n");
    return 1;
  }

  FILE *romfile = fopen(argv[2], "rb+");

  FileReader file;
  if (!FileReader_From(&file, romfile)) {
    fprintf(log, "Could not open romfile.\n");
    return 1;
  }

  Reader *reader = FileReader_AsReader(&file);

  Collection *collection = Collection_DefineWithUsableSpace(300 * 1024);
  if (!Collection_ReadFrom(collection, reader)) {
    fprintf(log, "Could not find collection in romfile.\n");
    // TODO close file handle
    return 1;
  }

  // TODO implement remaining stuff
}
