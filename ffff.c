#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

#include "util.h"
#include "corrupt.h"

#define OPTSTRING "n:s:m:"

void usage(char * reason, char * exe);
void banner();
int corrupt_file(char * filename, char * outFilename, int numCorruptions);
char * corrupt_select(int numCorruptions, char * data, size_t *dataSz);

void usage(char * reason, char * exe)
{
  if(reason && strlen(reason))
    fprintf(stderr, "error: %s\n", reason);

  fprintf(stderr, "Usage: %s [-n num corruptions] [-s seed32] [-m max offset] inFile outFile\n", exe);

  exit(EXIT_FAILURE);
}

void banner()
{
  fprintf(stderr, " ______   ______   ______   ______  \n");
  fprintf(stderr, "/\\  ___\\ /\\  ___\\ /\\  ___\\ /\\  ___\\ \n");
  fprintf(stderr, "\\ \\  __\\ \\ \\  __\\ \\ \\  __\\ \\ \\  __\\ \n");
  fprintf(stderr, " \\ \\_\\    \\ \\_\\    \\ \\_\\    \\ \\_\\   \n");
  fprintf(stderr, "  \\/_/     \\/_/     \\/_/     \\/_/   \n");
  fprintf(stderr, "\n");
  fprintf(stderr, "   FFFF - File Fuzzing For Fun\n");
  fprintf(stderr, "\n");
}

int main(int argc, char * argv[])
{
  int opt;
  char * exe;
  uint32_t seed = 0;
  uint32_t numCorruptions = 1;
  char * inFile = NULL, *outFile = NULL;
  size_t maxOff = 0;

  if(argc == 0)
    return -1;

  exe = argv[0];

  // check it out!
  banner();

  if(argc == 1)
    usage("", exe);

  while((opt = getopt(argc, argv, OPTSTRING)) != -1)
  {
    switch(opt)
    {
    case 's':
      seed = atoi(optarg);
      break;
    case 'n':
      numCorruptions = atoi(optarg);
      break;
    case 'm':
      maxOff = atoi(optarg);
      break;
    default:
      usage("", exe);
    }
  }

  if(argc - optind < 2)
  {
    usage("In file and out file required", exe);
  }
  else
  {
    inFile = argv[optind];
    outFile = argv[optind+1];
  }

  if(!seed)
    seed = time(NULL);
  else
    fprintf(stderr, "Using supplied seed 0x%08X\n", seed);

  if(maxOff)
  {
    fprintf(stderr, "Limiting corruption within the first %zu bytes\n", maxOff);
    corrupt_max_offset(maxOff);
  }

  // seed the beginnings of a new future under the machines
  seedCorruptors(seed);

  corrupt_file(inFile, outFile, numCorruptions);

  return 0;
}

int corrupt_file(char * filename, char * outFilename, int numTimes)
{
  size_t fileSz = 0;
  char * fileData = NULL;

  fileData = readFile("", filename, &fileSz);

  if(!fileData)
    die("failed to read file \'%s\'\n", filename);

  if(numTimes > 1)
    fprintf(stderr, "Corrupting file %s (size %zu) %d times\n", filename, fileSz, numTimes);
  else
    fprintf(stderr, "Corrupting file %s (size %zu)\n", filename, fileSz);

  char * corruptedFile = corrupt_select(numTimes, fileData, &fileSz);
  writeFile("", outFilename, corruptedFile, fileSz);

  if(corruptedFile)
    free(corruptedFile);

  return 0;
}

char * corrupt_select(int numCorruptions, char * data, size_t *dataSz)
{
  char * corrupted = data;
  int i;

  if(!corrupted)
    die("out of memory when corrupting file");

  for(i = 0; i < numCorruptions; i++)
  {
    //int corruptType = randomInt(CORRUPT_NONE+1, CORRUPT_MAX-1);
    int corruptType = randomInt(CORRUPT_NONE+1, CORRUPT_FLIPSIGN); // TESTING
    corrupted = do_corrupt(corruptType, corrupted, dataSz);
  }

  return corrupted;
}
