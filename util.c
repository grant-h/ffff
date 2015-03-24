#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/types.h>
#include <dirent.h>

#include "util.h"

static int isSeeded = 0;

void die(char * fmt, ...)
{
  va_list va;

  va_start(va, fmt);

  char dieBuf[512];

  vsnprintf(dieBuf, sizeof(dieBuf)-1, fmt, va);

  fprintf(stderr, "fatal: %s", dieBuf);

  exit(EXIT_FAILURE);
}

void seedCorruptors(unsigned int seed)
{
  srandom(seed);
  isSeeded = seed;
}

int randomInt(int min, int max)
{
  if(min >= max)
    return min;

  assert(isSeeded);

  return ((float) random() / RAND_MAX) * max + min;
}

char * readFile(char * dir, char * file, size_t * fileSz)
{
  char * fullPath;
  FILE * fp;
  size_t bytesWritten = 0;
  char * data = NULL;
  size_t bufSize = 0;
  size_t bytesRead = 0;

  fullPath = concatDirElem(dir, file);
  if(!fullPath)
    return NULL;

  fp = fopen(fullPath, "rb");
  free(fullPath);

  if(!fp)
    return NULL;

  while(1)
  {
    if(!data || (bufSize - bytesRead) < 0x100)
    {
      bufSize += 0x1000;
      data = realloc(data, bufSize);
    }

    size_t actualRead = fread(data+bytesRead, sizeof(char),
                              bufSize-bytesRead, fp);

    if(actualRead == 0)
    {
      if(feof(fp))
      {
        break;
      }
      else
      {
        fclose(fp);
        free(data);
        return NULL;
      }
    }
    else 
    {
      bytesRead += actualRead;
    }
  }

  *fileSz = bytesRead;
  return data;
}

int writeFile(char * dir, char * file, char * data, size_t dataSz)
{
  char * fullPath;
  FILE * fp;
  size_t bytesWritten = 0;

  fullPath = concatDirElem(dir, file);
  if(!fullPath)
    return 0;

  fp = fopen(fullPath, "wb");
  free(fullPath);

  if(!fp)
    return 0;

  while(bytesWritten < dataSz)
  {
    size_t writeRet = fwrite(data+bytesWritten, sizeof(char), dataSz-bytesWritten, fp);

    bytesWritten += writeRet;
  }

  fclose(fp);
  return bytesWritten;
}

int numFilesInDir(char * dirPath)
{
  DIR * dir = opendir(dirPath);
  int numFiles = 0;

  if(!dir)
    return 0;

  struct dirent * ent = NULL; 

  while(ent = readdir(dir))
  {
    if(ent->d_type == DT_REG)
      numFiles++;
  }

  closedir(dir);

  return numFiles;
}

char * concatDirElem(char * dir, char *filename)
{
  size_t dirLen = strlen(dir);
  size_t fileLen = strlen(filename);
  char * concat = NULL;
  size_t totalSize = 0;

  if(!dirLen)
    return strdup(filename);

  if(dir[dirLen-1] == '/')
  {
    totalSize = dirLen+fileLen+1;
    concat = calloc(totalSize, sizeof(char));

    if(!concat)
      return NULL;

    strncat(concat, filename, totalSize-1);
  }
  else
  {
    totalSize = dirLen+1+fileLen+1;
    concat = calloc(totalSize, sizeof(char));

    if(!concat)
      return NULL;

    strncat(concat, dir, totalSize-1);
    strncat(concat, "/", totalSize-1-dirLen);
    strncat(concat, filename, totalSize-2-dirLen);
  }

  return concat;
}

char * insertBytes(char * data, size_t * dataSz, char * insertData, size_t insertSz, size_t offset)
{
  if(offset >= *dataSz)
    return data;

  size_t newSz = *dataSz + insertSz;
  char * newBuf = malloc(newSz);

  if(!newBuf)
    return data;

  // [old data] [inserted data] [old data tail]
  //           ^ offset
  memcpy(newBuf, data, offset);
  memcpy(newBuf+offset, insertData, insertSz);
  memcpy(newBuf+offset+insertSz, data+offset, *dataSz - offset);

  *dataSz = newSz;
  free(data);

  return newBuf;
}

char * deleteBytes(char * data, size_t * dataSz, size_t deleteSz, size_t offset)
{
  if(!deleteSz || *dataSz)
    return data;

  // [       existing data       ]
  // 0  1  2      ...       dataSz-1
  if(*dataSz - 1 > (deleteSz + offset)) // deleting too much
    return data;

  size_t newSz = *dataSz - deleteSz; 
  char * newBuf = malloc(newSz);

  if(!newBuf)
    return data;

  memcpy(newBuf, data, offset);
  memcpy(newBuf+offset, data+offset+deleteSz, *dataSz - deleteSz - offset);

  *dataSz = newSz;
  free(data);

  return newBuf;
}

char * replaceBytes(char * data, size_t * dataSz, char * replaceBytes, size_t replaceSz, size_t offset)
{
  if(!*dataSz || !replaceSz || offset > *dataSz - 1)
    return data;

  if(replaceSz+offset < *dataSz-1)
    replaceSz = *dataSz - offset;

  // we are only replacing data, we don't need new memory
  memcpy(data+offset, replaceBytes, replaceSz);

  return data;
}
