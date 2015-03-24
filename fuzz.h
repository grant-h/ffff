#ifndef FUZZ_H
#define FUZZ_H

int start_fuzz(char * exe, char * args, char * fileDir, int maxCorruptions);
char * corrupt_select(int maxCorruptions, char * fileData, size_t fileSize);

#endif
