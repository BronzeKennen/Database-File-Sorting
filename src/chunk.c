#include <merge.h>
#include <stdio.h>
#include "chunk.h"
#include "record.h"
#define MAX_RECORDS_PER_BLOCK (BF_BLOCK_SIZE / sizeof(Record))

CHUNK_Iterator CHUNK_CreateIterator(int fileDesc, int blocksInChunk) {
    CHUNK_Iterator iterator;
    iterator.file_desc = fileDesc;
    iterator.current = 1;  // Assuming we start from block 1
    iterator.lastBlocksID = blocksInChunk;  // Corrected value
    iterator.blocksInChunk = blocksInChunk;

    return iterator;
}

int CHUNK_GetNext(CHUNK_Iterator *iterator, CHUNK *chunk) {
    int blockNum = HP_GetIdOfLastBlock(iterator->file_desc);

    iterator->current += iterator->blocksInChunk; // Increment iterator      
    iterator->lastBlocksID += iterator->blocksInChunk; 
    if(iterator->current > blockNum) {
        return -1;
    }
    if(iterator->lastBlocksID >= blockNum) {
        iterator->lastBlocksID = blockNum;
        chunk->to_BlockId = blockNum;
        
    }
    iterator->blocksInChunk = iterator->lastBlocksID - iterator->current + 1;
    
    // Assign iterator info to the chunk variable
    chunk->file_desc = iterator->file_desc;
    chunk->from_BlockId = iterator->current;
    chunk->to_BlockId = iterator->lastBlocksID;

    // Assign iterator info to the chunk variable
    chunk->recordsInChunk = ((chunk->to_BlockId - chunk->from_BlockId) * MAX_RECORDS_PER_BLOCK) + HP_GetRecordCounter(iterator->file_desc, iterator->lastBlocksID);
    chunk->blocksInChunk = iterator->blocksInChunk;
    return 0;
}




int CHUNK_GetIthRecordInChunk(CHUNK* chunk,  int i, Record* record){
    if (i < 0 || i >= chunk->recordsInChunk) {
        return -1;  // Invalid position
    }

    // Calculate the block and position within the block for the record
    int blockId = chunk->from_BlockId + i / MAX_RECORDS_PER_BLOCK;
    int recordPosInBlock = i % MAX_RECORDS_PER_BLOCK;

    // Use your file access functions to retrieve the record from the block
    // For example, assuming HP_GetRecord is a function to retrieve a record from a heap file
    int file_desc = chunk->file_desc;
    int result = HP_GetRecord(file_desc, blockId, recordPosInBlock, record);
    HP_Unpin(file_desc,blockId);
    return result;  // Return the result of HP_GetRecord
}

int CHUNK_UpdateIthRecord(CHUNK* chunk,  int i, Record record){
    if (i < 0 || i >= chunk->recordsInChunk) {
        return -1;  // Invalid position
    }

    // Calculate the block and position within the block for the record
    int blockId = chunk->from_BlockId + i / MAX_RECORDS_PER_BLOCK;
    int recordPosInBlock = i % MAX_RECORDS_PER_BLOCK;

    // Use your file access functions to update the record in the block
    // For example, assuming HP_UpdateRecord is a function to update a record in a heap file
    int file_desc = chunk->file_desc;
    int result = HP_UpdateRecord(file_desc, blockId, recordPosInBlock, record);
    HP_Unpin(file_desc,blockId);
    return result;  // Return the result of HP_UpdateRecord
}


void CHUNK_Print(CHUNK chunk) {
    printf("Printing records in Chunk from Block %d to Block %d:\n", chunk.from_BlockId, chunk.to_BlockId);

    // Iterate through records in the chunk
    for (int i = 0; i < chunk.recordsInChunk; i++) {
        Record record;
        // Use the CHUNK_GetIthRecordInChunk function to retrieve each record
        if (CHUNK_GetIthRecordInChunk(&chunk, i, &record) == 0) {
            // Use the provided printRecord function to print the record
            printRecord(record);
        }
    }

    printf("End of Chunk Print\n");
}


CHUNK_RecordIterator CHUNK_CreateRecordIterator(CHUNK *chunk) {
    CHUNK_RecordIterator iterator;

    // Initialize the record iterator based on the chunk
    iterator.chunk = *chunk;
    iterator.currentBlockId = chunk->from_BlockId;
    iterator.cursor = 0;

    return iterator;
}


/* Function to get the next record from the iterator. */
int CHUNK_GetNextRecord(CHUNK_RecordIterator *iterator, Record* record) {
    // Check if the cursor has reached the end of the current block
    if (iterator->cursor > MAX_RECORDS_PER_BLOCK - 1) {
        // Move to the next block
        iterator->currentBlockId++;
        iterator->cursor = 0;
    }

    // Check if the iterator has reached the end of the chunk
    if (iterator->currentBlockId > iterator->chunk.to_BlockId) {
        return -1;  // Iterator reached the end
    }

    // Retrieve the record from the current block and cursor position
    if (HP_GetRecord(iterator->chunk.file_desc, iterator->currentBlockId, iterator->cursor, record) != 0) {
        return -1;  // Error retrieving record
    }

    // Move the cursor to the next record
    iterator->cursor++;

    return 0;  // Successful retrieval
}
