#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merge.h"

#define RECORDS_NUM 500 // you can change it if you want
#define FILE_NAME "data.db"

int createAndPopulateHeapFile(char* filename);


int main() {
    BF_Init(LRU);
    int file_desc = createAndPopulateHeapFile(FILE_NAME);
    CHUNK_Iterator iterator = CHUNK_CreateIterator(file_desc, 5);
    CHUNK chunk = {file_desc, 1, 5, 5*MAX_RECORDS_PER_BLOCK, 5};

    CHUNK_Print(chunk);
    CHUNK_GetNext(&iterator,&chunk);
    CHUNK_Print(chunk);
    CHUNK_GetNext(&iterator,&chunk);
    CHUNK_Print(chunk);
    CHUNK_GetNext(&iterator,&chunk);
    CHUNK_Print(chunk);
    CHUNK_GetNext(&iterator,&chunk);
    CHUNK_Print(chunk);

}


int createAndPopulateHeapFile(char* filename){
  HP_CreateFile(filename);
  
  int file_desc;
  HP_OpenFile(filename, &file_desc);

  Record record;
  srand(12569874);
  for (int id = 0; id < RECORDS_NUM; ++id)
  {
    record = randomRecord();
    HP_InsertEntry(file_desc, record);
  }
  return file_desc;
}