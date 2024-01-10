#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "chunk.h"  // Include the necessary header file for CHUNK operations


void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    CHUNK_Iterator chunk_iterator = CHUNK_CreateIterator(input_FileDesc, chunkSize);
    CHUNK chunk;

    // Initialize iterators for each input chunk
    CHUNK_RecordIterator rec_iterators[bWay];
    Record records[bWay];

    // Create first chunk and record iterator.
    chunk.blocksInChunk = chunkSize;
    chunk.file_desc = input_FileDesc;
    chunk.from_BlockId = 1;
    chunk.to_BlockId = chunkSize;
    chunk.recordsInChunk = MAX_RECORDS_PER_BLOCK * chunkSize;
    rec_iterators[0] = CHUNK_CreateRecordIterator(&chunk);

    // Create iterators
    for (int i = 1; i < bWay; i++) {
        CHUNK_GetNext(&chunk_iterator, &chunk);
        rec_iterators[i] = CHUNK_CreateRecordIterator(&chunk);
    }

    // Create a new chunk for the output file
    CHUNK newChunk;
    newChunk.file_desc = output_FileDesc;
    newChunk.from_BlockId = 1;  // Starting block for the new chunk
    newChunk.to_BlockId = chunkSize;  // Assuming each chunk has 'chunkSize' blocks
    newChunk.recordsInChunk = MAX_RECORDS_PER_BLOCK * chunkSize;
    newChunk.blocksInChunk = chunkSize;

    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc) - 1; 
    int totalChunks = (int)ceil((double)totalBlocks / chunkSize); // Return the result round-up so we don't miss any chunks.
    int newFileChunkNum = (int)ceil((double)totalChunks / bWay);
    int newFileChunkSize = chunkSize * bWay;

    // Load the first record from each input chunk
    for (int i = 0; i < bWay; i++) {
        if (CHUNK_GetNextRecord(&rec_iterators[i], &records[i]) != 0) {
            printf("Failed to get record\n");
            return;
        }
    }

    // Merge logic
    while(true) {
        // Find the smallest record among the loaded records
        int minIndex = -1;
        for (int i = 0; i < bWay; i++) {
            if (minIndex == -1 || shouldSwap(&records[i], &records[minIndex])) {
                minIndex = i;
            }
        }
        // minIndex is now the smallest record.

        // Update the new chunk with the smallest record
        CHUNK_UpdateIthRecord(&newChunk, 0, records[minIndex]);

        // Check if the current chunk is finished and there is space to load another record
        if (CHUNK_GetNextRecord(&rec_iterators[minIndex], &records[minIndex]) != 0) {
            // Handle end of chunk or iteration
            // You might need to adjust this part based on your logic
        }

        // You might need to adjust the logic here based on your requirements

        // Write the sorted chunk back to the file
        for (int i = 0; i < newChunk.recordsInChunk; i++) {
            // Use CHUNK_UpdateIthRecord to update the records in the corresponding chunk
            CHUNK_UpdateIthRecord(&newChunk, i, records[i]);
        }
    }

    // Cleanup resources (you might need additional cleanup logic here)
    for (int i = 0; i < bWay; i++) {
        // Cleanup each iterator
        // CHUNK_CloseRecordIterator(&iterators[i]);
    }
}