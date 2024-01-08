#include <merge.h>
#include <stdio.h>
#include <stdbool.h>

#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include "chunk.h"  // Include the necessary header file for CHUNK operations

#include <merge.h>
#include <stdio.h>
#include <stdbool.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {

    CHUNK_Iterator chunk_iterator = CHUNK_CreateIterator(input_FileDesc, chunkSize);
    CHUNK chunk;

    // Initialize iterators for each input chunk
    CHUNK_RecordIterator rec_iterators[bWay];
    Record records[bWay];

    // Create iterators
    for (int i = 0; i < bWay; i++) {
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

    int lastBlockId = HP_GetIdOfLastBlock(input_FileDesc); 
    printf("Last Block ID = %d\n", lastBlockId);
    int k = lastBlockId/chunkSize;
    printf("ChunkSize = %d\n", chunkSize);
    printf("K = %d\n", k);
    int newFileChunkSize = k/bWay;
    printf("New Chunk Num = %d\n", newFileChunkSize);

    // Load the first record from each input chunk
    for (int i = 0; i < bWay; i++) {
        if (CHUNK_GetNextRecord(&rec_iterators[i], &records[i]) != 0) {
            // Handle end of chunk or iteration
            // You might need to adjust this part based on your logic
        }
    }

    // Merge logic
    while (true) {

        // Find the smallest record among the loaded records
        int minIndex = -1;
        for (int i = 0; i < bWay; i++) {
            if (minIndex == -1 || shouldSwap(&records[i], &records[minIndex])) {
                minIndex = i;
            }
        }

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