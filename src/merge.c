#include "merge.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "chunk.h"  // Include the necessary header file for CHUNK operations


void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc); 
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
    for(int k = 0; k < newFileChunkNum; k++) {  // For k chunks.
        for(int j = 0; j < newFileChunkSize * MAX_RECORDS_PER_BLOCK; j++) { // For j records per chunk.
            if(insertedRecords == totalRecords) {
                break;
            }

            if(j == 0) { // Load the new chunks for bway merge.
                for (int i = 0; i < bWay; i++) {
                    if(i + (k*bWay) >= totalChunks) {
                        Record dummy;
                        dummy.id = -1;
                        records[i] = dummy;
                        continue;
                    }
                    if (CHUNK_GetNextRecord(&rec_iterators[i + (k*bWay)], &records[i]) != 0) {
                        return;
                    }
                }
            }

            // Find the smallest among the loaded records.
            int minIndex = -1;
            for (int i = 0; i < bWay; i++) {
                if(records[i].id == -1) continue;
                if (minIndex == -1 || shouldSwap(&records[i], &records[minIndex])) {
                    minIndex = i;
                }            
            }

            // Insert entry in new file
            if(HP_InsertEntry(output_FileDesc,records[minIndex]) == -1) {
                printf("Error inserting entry. Returning.\n");
                return;
            }
            insertedRecords++;

            // Free memory.
            HP_Unpin(output_FileDesc,HP_GetIdOfLastBlock(output_FileDesc));

            // Get next record from chunk. If chunk ends, replace array position with a dummy record.
            if (CHUNK_GetNextRecord(&rec_iterators[minIndex + (k*bWay)], &records[minIndex]) != 0) {
                Record dummy;
                dummy.id = -1;
                records[minIndex] = dummy;

                // Free memory of the entire chunk if it reached the end.
                for(int i = rec_iterators[minIndex + (k*bWay)].chunk.from_BlockId; i <= rec_iterators[minIndex + (k*bWay)].chunk.to_BlockId; i++) {
                    HP_Unpin(input_FileDesc,i);
                }
            }
        }
    }
}