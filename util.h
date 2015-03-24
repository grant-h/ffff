#ifndef UTIL_H
#define UTIL_H

// misc
void die(char * fmt, ...);

// random numbers
void seedCorruptors(unsigned int seed);
// returns random integer in [min, max]
int randomInt(int min, int max);

// file handling
char * readFile(char * dir, char * file, size_t * fileSz);
// returns the number of bytes written
int writeFile(char * dir, char * file, char * data, size_t dataSz);
int numFilesInDir(char * dirPath);
char * concatDirElem(char * dir, char *filename);

// insertion and deletion
char * insertBytes(char * data, size_t * dataSz, char * insertData, size_t insertSz, size_t offset);
char * deleteBytes(char * data, size_t * dataSz, size_t deleteSz, size_t offset);
char * replaceBytes(char * data, size_t * dataSz, char * replaceBytes, size_t replaceSz, size_t offset);

#endif
