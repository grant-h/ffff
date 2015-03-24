#ifndef CORRUPT_H
#define CORRUPT_H

#define CORRUPT_START_MIN 5

enum corrupt_type
{
  CORRUPT_NONE = 0,
  CORRUPT_ZEROS,
  CORRUPT_EDGENUM,
  CORRUPT_BITS,
  CORRUPT_FLIPSIGN,
  CORRUPT_ASCII,
  CORRUPT_MAX
};

typedef char * (*corruptorFunc)(char*, size_t*);

char * do_corrupt(enum corrupt_type type, char * data, size_t * dataSz);
char * corruptor_str(enum corrupt_type type);

// parameters
void corrupt_max_offset(size_t maxOff);

// corruptors
char * corrupt_zeros(char * data, size_t * dataSz);
char * corrupt_edgenum(char * data, size_t * dataSz);
char * corrupt_bits(char * data, size_t * dataSz);
char * corrupt_flipsign(char * data, size_t * dataSz);
char * corrupt_letters(char * data, size_t * dataSz);

#endif
