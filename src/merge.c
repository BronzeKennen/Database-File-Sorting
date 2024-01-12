#include "merge.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "chunk.h"  // Include the necessary header file for CHUNK operations


void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc); // We skip first block(HP_info) but we start from 0 so it balances out.
    int totalChunks = (int)ceil((double)totalBlocks / chunkSize); // Return the result round-up so we don't miss any chunks.
    int newFileChunkNum = (int)ceil((double)totalChunks / bWay);
    int newFileChunkSize = chunkSize * bWay;
    int totalRecords = 0;


    CHUNK_Iterator chunk_iterator = CHUNK_CreateIterator(input_FileDesc, chunkSize);
    CHUNK chunk;

    // Initialize iterators for each input chunk
    CHUNK_RecordIterator rec_iterators[totalChunks];
    Record records[bWay];

    // Create first chunk and record iterator.
    chunk.blocksInChunk = chunkSize;
    chunk.file_desc = input_FileDesc;
    chunk.from_BlockId = 1;
    chunk.to_BlockId = chunkSize;
    chunk.recordsInChunk = MAX_RECORDS_PER_BLOCK * chunkSize;
    rec_iterators[0] = CHUNK_CreateRecordIterator(&chunk);
    totalRecords += chunk.recordsInChunk;


    // Create iterators
    for (int i = 1; i < totalChunks; i++) {
        if (CHUNK_GetNext(&chunk_iterator, &chunk) != 0) {
            printf("Error retrieving chunk!\n");
            return;
        };
        rec_iterators[i] = CHUNK_CreateRecordIterator(&chunk);
        totalRecords += chunk.recordsInChunk;
    }

    // Merge logic
    int insertedRecords = 0;
    for(int k = 0; k < newFileChunkNum; k++) {
        printf("New chunk\n");
        for(int j = 0; j < newFileChunkSize * MAX_RECORDS_PER_BLOCK; j++) {
            if(insertedRecords == totalRecords) {
                return;
            }
            printf("Current record: %d\n", insertedRecords);
            if(j == 0) { // Load the new chunks for bway merge.
                for (int i = 0; i < bWay; i++) {
                    if(i + (k*bWay) >= totalChunks) {
                        printf("Exceeded chunk number!\n");
                        Record dummy;
                        dummy.id = -1;
                        records[i] = dummy;
                        continue;
                    }
                    if (CHUNK_GetNextRecord(&rec_iterators[i + (k*bWay)], &records[i]) != 0) {
                        printf("Failed to get record.\n");
                        return;
                    }
                }
            }
            // Find the smallest record among the loaded records
            int minIndex = -1;
            for (int i = 0; i < bWay; i++) {
                if(records[i].id == -1) continue;
                if (minIndex == -1 || shouldSwap(&records[i], &records[minIndex])) {
                    minIndex = i;
                }            
            }
            printf("Getting record from position %d which is iterator %d\n", minIndex, (minIndex + (k*bWay)));

            // Insert entry in new file
            printf("Trying to insert record with id = %d\n", records[minIndex].id);
            if(HP_InsertEntry(output_FileDesc,records[minIndex]) == -1) {
                printf("Error inserting entry!\n");
                return;
            }
            printf("Inserted record!\n");
            insertedRecords++;


            // Check if the current chunk is finished and there is space to load another record
            if (CHUNK_GetNextRecord(&rec_iterators[minIndex + (k*bWay)], &records[minIndex]) != 0) {
                printf("Couldn't retrieve record. Dummy inserted.\n");
                Record dummy;
                dummy.id = -1;
                records[minIndex] = dummy;
            }
        }
    }
    for(int i = 0 ; i < totalBlocks; i++) {
        //TEMP SOLUTION
        HP_Unpin(input_FileDesc,i);
    }
}