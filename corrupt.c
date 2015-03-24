#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#include "util.h"
#include "corrupt.h"

corruptorFunc corruptorFuncs[CORRUPT_MAX-1] = {
    corrupt_zeros,
    corrupt_edgenum,
    corrupt_letters,
    corrupt_bits,
    corrupt_flipsign
}; 

char * corruptorStrs[CORRUPT_MAX-1] = {
  "zeros",
  "edge numbers",
  "bits",
  "flip sign",
  "ascii padding"
};

uint32_t corruptVals32[] = {
  0,
  INT_MAX,
  INT_MIN,
  -1,
};


// global parameters
size_t gMaxOffset = 0;

// local functions
static size_t random_offset(size_t dataSize);

char * do_corrupt(enum corrupt_type type, char * data, size_t * dataSz)
{
  if(type <= CORRUPT_NONE || type >= CORRUPT_MAX)
    die("Corruptor out of range");

  // assert that dataSz will NEVER be zero
  if(!*dataSz)
    return data;

  fprintf(stderr, "Corruptor: %s\n", corruptor_str(type));

  return corruptorFuncs[type-1](data, dataSz);
}

char * corruptor_str(enum corrupt_type type)
{
  if(type <= CORRUPT_NONE || type >= CORRUPT_MAX)
    return NULL;

  return corruptorStrs[type-1];
}

///////////////////////////////////////
// Parameters
///////////////////////////////////////

void corrupt_max_offset(size_t maxOff)
{
  gMaxOffset = maxOff;
}

size_t random_offset(size_t dataSize)
{
  return randomInt(CORRUPT_START_MIN, gMaxOffset ? gMaxOffset : (dataSize-1));
}

///////////////////////////////////////
// Corruptors
///////////////////////////////////////

char * corrupt_zeros(char * data, size_t * dataSz)
{
  // Choose between zero corruption methods
  //   0. Insert zeros in to the bitstream
  //   1. Replace random bytes with zeros

  int choice = randomInt(0, 1);
  size_t corruptLoc = random_offset(*dataSz);
  size_t corruptSize = randomInt(1, 0x100);

  // we can't corrupt this crap TODO fix this
  if(*dataSz < CORRUPT_START_MIN)
    return data;

  char * replacement = calloc(corruptSize, sizeof(char));
  if(!replacement)
    return data;

  memset(replacement, 0x0, corruptSize);

  if(choice == 0)
  {
    data = insertBytes(data, dataSz, replacement,
                       corruptSize, corruptLoc);
  }
  else if(choice == 1)
  {
    data = replaceBytes(data, dataSz, replacement, 
                        corruptSize, corruptLoc);
  }

  return data;
}

char * corrupt_edgenum(char * data, size_t * dataSz)
{
  /* Corruptor: Edgenum
   *
   * Goal: tries to add values determined to be the 'edges'
   * of the common data types. Think 0, -1, MAX_INT, MAX_INT-1,
   * etc.
   *
   * This corruptor should only replace, never insert. We want
   * to mess up length fields.
   */

  size_t corruptLoc = random_offset(*dataSz);

  uint32_t corruptVal = randomInt(0, sizeof(corruptVals32) /
                                     sizeof(corruptVals32[0])-1);

  data = insertBytes(data, dataSz, (char *)&corruptVal, 
                     sizeof(uint32_t), corruptLoc);

  return data;
}

char * corrupt_bits(char * data, size_t * dataSz)
{
  size_t bitsToCorrupt = randomInt(0, 20);
  size_t i;

  // Flip the MSB of a random amount of bytes
  
  for(i = 0; i < bitsToCorrupt; i++)
  {
    size_t off = random_offset(*dataSz);
    size_t bit = randomInt(0, 7);

    // flip the bit
    data[off] = data[off] ^ (1 << bit);
  }

  return data;
}

char * corrupt_flipsign(char * data, size_t * dataSz)
{
  size_t bitsToFlip = randomInt(0, 20);
  size_t i;

  // Flip the MSB of a random amount of bytes
  
  for(i = 0; i < bitsToFlip; i++)
  {
    size_t off = random_offset(*dataSz);

    // flip the MSB
    data[off] = data[off] ^ 0x80;
  }

  return data;
}

char * corrupt_letters(char * data, size_t * dataSz)
{
  return data;
}

