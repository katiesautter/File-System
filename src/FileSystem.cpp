#include "stdafx.h"
#include "FileSystem.h"

#include <iostream>
#include <string>
#include <fstream>
#define _CRT_SECURE_NO_WARNINGS

using namespace std;

FileSystem::FileSystem(string diskName) {
	// Open the file with name diskName
	// Read the first 1KB and parse it to structs/objecs representing the super block
	//      An easy way to work with the 1KB memory chunk is to move a pointer to a
	// position where a struct/object begins. You can use the sizeof operator to help
	// cleanly determine the position. Next, cast the pointer to a pointer of the
	// struct/object type.
	// Be sure to close the file in a destructor or otherwise before
	// the process exits.

	disk = new fstream(diskName);
	readSuperBlock();
}
FileSystem::~FileSystem() {
	disk->close();
	delete disk;
}
int FileSystem::createf(char name[8], int size) { 
  //create a file with this name and this size
  // high level pseudo code for creating a new file
  // Step 1: Look for a free inode by searching the collection of objects
  // representing inodes within the super block object.
  // If none exist, then return an error.
  // Also make sure that no other file in use with the same name exists.
  // Step 2: Look for a number of free blocks equal to the size variable
  // passed to this method. If not enough free blocks exist, then return an error.
  // Step 3: Now we know we have an inode and free blocks necessary to
  // create the file. So mark the inode and blocks as used and update the rest of
  // the information in the inode.
  // Step 4: Write the entire super block back to disk.
  //  An easy way to do this is to seek to the beginning of the disk
  //  and write the 1KB memory chunk.
	
	// Check size
	if (size > 8) {
		cout << "The maximum file size is 8 blocks" << endl;
		return -1;
	}

	// Look for a free inode
	int inode_num = -1;
	for (int i = 0; i < NUM_INODES; i++) {
		if (inodes[i].used == 0) {
			inode_num = i;
			break;
		}
	}
	if (inode_num == -1) {
		cout << "The maximum number of files has already been reached." << endl;
		return -1;
	}

	// Check for existing file
	for (int i = 0; i < NUM_INODES; i++) {
		if (strncmp(inodes[i].name, name, 8) == 0) {
			cout << "A file with that name already exists." << endl;
			return -1;
		}
	}

	// Lock the inode
	open_files[inode_num].wait();

	// Find a starting block
	free_block_lock.wait();
	int *blockPtrs = new int[size];
	blockPtrs[0] = -1;
	for (int i = 0; i < FREE_BLOCK_LIST_SIZE; i++) {
		if (free_block_list[i] == 0) {
			blockPtrs[0] = i;
			break;
		}
	}
	if (blockPtrs[0] == -1) {
		cout << "There are no free blocks left." << endl;
		delete[] blockPtrs;
		open_files[inode_num].notify();
		free_block_lock.notify();
		return -1;
	}

	// Find the rest of the blocks
	blockPtrs[size - 1] = -1;
	int curr_block = 1;
	for (int i = (blockPtrs[0]+1); i < FREE_BLOCK_LIST_SIZE; i++) {
		if (free_block_list[i] == 0) {
			blockPtrs[curr_block] = i;
			curr_block++;
		}
		if (curr_block == size)
			break;
	}
	if (blockPtrs[size - 1] == -1) {
		cout << "There are insufficient free blocks left." << endl;
		delete [] blockPtrs;
		open_files[inode_num].notify();
		free_block_lock.notify();
		return -1;
	}

	// Store the information for the file
		// Update the free block list
	for (int i = 0; i < size; i++)
		free_block_list[blockPtrs[i]] = 1;
	free_block_lock.notify();
		// Update the entry for the inode
	strncpy(inodes[inode_num].name, name, 8);
	for (int i = 0; i < size; i++)
		inodes[inode_num].blockPointers[i] = blockPtrs[i];
	inodes[inode_num].size = size;
	inodes[inode_num].used = 1;
	open_files[inode_num].notify();

	saveSuperBlock();

	delete [] blockPtrs;
	cout << "Created file with name " << string(inodes[inode_num].name) << endl;
	return 1;
}

int FileSystem::deletef(char name[8]) {
	// Delete the file with this name
	// Step 1: Look for an inode that is in use with given name
	// by searching the collection of objects
	// representing inodes within the super block object.
	// If it does not exist, then return an error.
	// Step 2: Free blocks of the file being deleted by updating
	// the object representing the free block list in the super block object.
	// Step 3: Mark inode as free.
	// Step 4: Write the entire super block back to disk.

	// Find the inode corresponding to the file
	int inode_num = -1;
	for (int i = 0; i < NUM_INODES; i++) {
		if (strncmp(inodes[i].name, name, 8) == 0) {
			inode_num = i;
			break;
		}
	}
	if (inode_num == -1) {
		cout << "A file with that name was not found." << endl;
		return -1;
	}

	// lock the file
	open_files[inode_num].wait();

	// Free the blocks in the block list
	free_block_lock.wait();
	for (int i = 0; i < inodes[inode_num].size; i++)
		free_block_list[inodes[inode_num].blockPointers[i]] = 0;
	free_block_lock.notify();

	// Free the inode
	inodes[inode_num].used = 0;
	open_files[inode_num].notify();
	
	cout << "Deleted file with name " << string(name) << endl;
	saveSuperBlock();
	return 1;
}

int FileSystem::ls(void) {
	// List names of all files on disk
	// Step 1: Print the name and size fields of all used inodes.
	cout << "Files Currently on Drive: " << endl;
	for (int i = 0; i < NUM_INODES; i++) {
		if (inodes[i].used == 1)
			cout << "\t" << inodes[i].name << ": " << inodes[i].size << " blocks" << endl;
	}
	cout << endl;

	return 1;
}

int FileSystem::readf(char name[8], int blockNum, char buf[1024]) {
	// read this block from this file
	// Return an error if and when appropriate. For instance, make sure
	// blockNum does not exceed size-1.
	// Step 1: Locate the inode for this file as in Step 1 of delete.
	// Step 2: Seek to blockPointers[blockNum] and read the block
	// from disk to buf.
	
	// Find the inode corresponding to the file
	int inode_num = -1;
	for (int i = 0; i < NUM_INODES; i++) {
		if (strncmp(inodes[i].name, name, 8) == 0) {
			inode_num = i;
			break;
		}
	}
	if (inode_num == -1) {
		cout << "A file with that name was not found." << endl;
		return -1;
	}

	// Check blockNum
	if (blockNum > (inodes[inode_num].size - 1)) {
		cout << "The requested file does not have that many blocks." << endl;
		return -1;
	}

	// Move to the start position and read it
	int blockPos = inodes[inode_num].blockPointers[blockNum];
	int bufSpace = blockPos * BLOCK_SIZE;
	disk->seekg(bufSpace, ios::beg);
	disk->read(buf, BLOCK_SIZE);

	cout << "Read data from block " << blockNum << " of file named " << string(name) 
		<< " located at block " << blockPos << " of the disk" << endl;
	cout << "Data: " << string(buf) << endl;
	return 1;
}

int FileSystem::writef(char name[8], int blockNum, char buf[1024]) {
	// write this block to this file
	// Return an error if and when appropriate.
	// Step 1: Locate the inode for this file as in Step 1 of delete.
	// Step 2: Seek to blockPointers[blockNum] and write buf to disk.

	// Find the inode corresponding to the file
	int inode_num = -1;
	for (int i = 0; i < NUM_INODES; i++) {
		if (strncmp(inodes[i].name, name, 8) == 0) {
			inode_num = i;
			break;
		}
	}
	if (inode_num == -1) {
		cout << "A file with that name was not found." << endl;
		return -1;
	}

	// Check blockNum
	if (blockNum > (inodes[inode_num].size - 1)) {
		cout << "The requested file does not have that many blocks." << endl;
		return -1;
	}

	// Aquire the file
	open_files[inode_num].wait();

	// Move to the start position and write it
	int blockPos = inodes[inode_num].blockPointers[blockNum];
	int bufSpace = blockPos * BLOCK_SIZE;
	disk->seekg(bufSpace, ios::beg);
	disk->write(buf, BLOCK_SIZE);

	// flag block for having changes
	modified_block_lock.wait();
	modified_block_list[blockPos] = 1;
	modified_block_lock.notify();

	cout << "Wrote data from block " << blockNum << " of file named " << string(name)
		<< " located at block " << blockPos << " of the disk" << endl;
	cout << "Data: " << string(buf) << endl;

	// Release the lock and return
	open_files[inode_num].notify();
	return 1;
} // end write

/**
	Cast a char[4] to an int
*/
int FileSystem::convertCharToInt(char toConvert[4]) {
	return *(int *)toConvert;
}
/**
	Convert an int to a char[4] and store it in `output`
	Note: not portable due to endianness
*/
void FileSystem::convertIntToChar(int toConvert, char output[4]) {
	output[3] = (toConvert >> 24) & 0xFF;
	output[2] = (toConvert >> 16) & 0xFF;
	output[1] = (toConvert >> 8) & 0xFF;
	output[0] = toConvert & 0xFF;
}

void FileSystem::readSuperBlock() {
	char * super_block = new char[BLOCK_SIZE];
	disk->seekg(0, ios::beg);
	disk->read(super_block, BLOCK_SIZE);

	// Read the free block list
	free_block_lock.wait();
	for (int i = 0; i < FREE_BLOCK_LIST_SIZE; i++)
		free_block_list[i] = super_block[i];
	free_block_lock.notify();

	// Read and parse the inodes
	for (int inode_counter = 0; inode_counter < NUM_INODES; inode_counter++) {
		int inode_start_offset = FREE_BLOCK_LIST_SIZE + (INODE_SIZE * inode_counter);

		// collect 4 char and cast to an int to find used value
		char int_char[4];
		for (int i = 0; i < 4; i++)
			int_char[i] = super_block[(inode_start_offset + i)];
		inodes[inode_counter].used = convertCharToInt(int_char);
		inode_start_offset += 4;
		// if the node is unused, then we don't need to finish reading it
		if (inodes[inode_counter].used == 0)
			continue;

		// collect next 8 characters as the name
		for (int i = 0; i < 8; i++)
			inodes[inode_counter].name[i] = super_block[(inode_start_offset + i)];
		inode_start_offset += 8;

		// collect 4 char and convert to int for the size
		for (int i = 0; i < 4; i++)
			int_char[i] = super_block[(inode_start_offset + i)];
		inodes[inode_counter].size = convertCharToInt(int_char);
		inode_start_offset += 4;

		// collect 4 char and convert to int 8 times for the pointers
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 4; j++) {
				int_char[j] = super_block[(inode_start_offset + 4 * i + j)];
			}
			inodes[inode_counter].blockPointers[i] = convertCharToInt(int_char);
		}
		inode_start_offset += (4 * 8);
	}

	// Read the modified block list
	modified_block_lock.wait();
	for (int i = 0; i < FREE_BLOCK_LIST_SIZE; i++) {
		int start_offset = FREE_BLOCK_LIST_SIZE + (INODE_SIZE * NUM_INODES) + i;
		modified_block_list[i] = super_block[start_offset];
	}
	modified_block_lock.notify();

	delete [] super_block;
}

void FileSystem::saveSuperBlock() {
	modified_block_lock.wait();
	modified_block_list[0] = 1; // modification of the super block
	modified_block_lock.notify();

	char * super_block = new char[BLOCK_SIZE];

	// save the free block list to the buffer
	free_block_lock.wait();
	for (int i = 0; i < FREE_BLOCK_LIST_SIZE; i++)
		super_block[i] = free_block_list[i];
	free_block_lock.notify();

	// serialize to char[] and save the inodes to the buffer
	for (int inode_counter = 0; inode_counter < NUM_INODES; inode_counter++) {
		// get access to file before writing the meta data
		open_files[inode_counter].wait();

		// calculate the offset in buffer for current inode
		int inode_start_offset = FREE_BLOCK_LIST_SIZE + (INODE_SIZE * inode_counter);

		// convert used int to char[] and save it to the buffer
		char * int_conv = new char[4];
		convertIntToChar(inodes[inode_counter].used, int_conv);
		for (int i = 0; i < 4; i++)
			super_block[(inode_start_offset + i)] = int_conv[i];
		inode_start_offset += 4;

		// save the name by character to the buffer
		for (int i = 0; i < 8; i++)
			super_block[(inode_start_offset + i)] = inodes[inode_counter].name[i];
		inode_start_offset += 8;

		// convert size int to char[] and save it to the buffer
		convertIntToChar(inodes[inode_counter].size, int_conv);
		for (int i = 0; i < 4; i++)
			super_block[(inode_start_offset + i)] = int_conv[i];
		inode_start_offset += 4;

		// convert each of the int block pointers to char[] and save them to the buffer
		for (int i = 0; i < 8; i++) {
			convertIntToChar(inodes[inode_counter].blockPointers[i], int_conv);
			for (int j = 0; j < 4; j++) {
				super_block[(inode_start_offset + 4 * i + j)] = int_conv[j];
			}
		}
		delete[] int_conv;

		open_files[inode_counter].notify();
	}

	// Save the modified block list
	modified_block_lock.wait();
	for (int i = 0; i < FREE_BLOCK_LIST_SIZE; i++) {
		int start_offset = FREE_BLOCK_LIST_SIZE + (INODE_SIZE * NUM_INODES) + i;
		super_block[start_offset] = modified_block_list[i];
	}
	modified_block_lock.notify();

	disk->seekg(0, ios::beg);
	disk->write(super_block, BLOCK_SIZE);
	delete[] super_block;
}

int FileSystem::init_backup(string diskName) {
	fstream *backup = new fstream(diskName);

	// read the first block of the backup
	char * block = new char[BLOCK_SIZE];
	backup->seekg(0, ios::beg);
	backup->read(block, BLOCK_SIZE);

	// check if it has previously been saved
	int offset = FREE_BLOCK_LIST_SIZE + (INODE_SIZE * NUM_INODES);
	char flag_bit = block[offset];

	backup->close();
	delete backup;

	// start the proper type of backup
	if (flag_bit == 1)
		modifiedBackup(diskName);
	else
		newBackup(diskName);

	modified_block_lock.wait();
	for (int i = 0; i < FREE_BLOCK_LIST_SIZE; i++)
		modified_block_list[i] = 0;
	modified_block_lock.notify();

	return 1;
}

void FileSystem::newBackup(string diskName) {
	fstream *backup = new fstream(diskName);
	char * block = new char[BLOCK_SIZE];
	Inode * inodes_copy = new Inode[NUM_INODES];

	for (int inode_counter = 0; inode_counter < NUM_INODES; inode_counter++) {
		// aquire the file
		open_files[inode_counter].wait();

		// make a deep copy of the inode
		strcpy(inodes_copy[inode_counter].name, inodes[inode_counter].name);
		for (int i = 0; i < 8; i++)
			inodes_copy[inode_counter].blockPointers[i] = inodes[inode_counter].blockPointers[i];
		inodes_copy[inode_counter].size = inodes[inode_counter].size;
		inodes_copy[inode_counter].used = inodes[inode_counter].used;

		// copy each block
		for (int i = 0; i < inodes[inode_counter].size; i++) {
			// read the data from the current disk
			readf(inodes[inode_counter].name, i, block);
			// Move to the start position and write it
			int blockPos = inodes[inode_counter].blockPointers[i];
			int bufSpace = blockPos * BLOCK_SIZE;
			backup->seekg(bufSpace, ios::beg);
			backup->write(block, BLOCK_SIZE);
		}

		// release the file
		open_files[inode_counter].notify();
	}

	backupSuperBlock(backup, inodes_copy);

	backup->close();
	delete backup;
	delete[] block;
	delete[] inodes_copy;
}
void FileSystem::modifiedBackup(string diskName) {

}
void FileSystem::backupSuperBlock(fstream* backup, Inode inodes_copy[]) {
	char * super_block = new char[BLOCK_SIZE];

	// save the free block list to the buffer
	free_block_lock.wait();
	for (int i = 0; i < FREE_BLOCK_LIST_SIZE; i++)
		super_block[i] = free_block_list[i];
	free_block_lock.notify();

	// serialize to char[] and save the inodes to the buffer
	for (int inode_counter = 0; inode_counter < NUM_INODES; inode_counter++) {
		// calculate the offset in buffer for current inode
		int inode_start_offset = FREE_BLOCK_LIST_SIZE + (INODE_SIZE * inode_counter);

		// convert used int to char[] and save it to the buffer
		char * int_conv = new char[4];
		convertIntToChar(inodes_copy[inode_counter].used, int_conv);
		for (int i = 0; i < 4; i++)
			super_block[(inode_start_offset + i)] = int_conv[i];
		inode_start_offset += 4;

		// save the name by character to the buffer
		for (int i = 0; i < 8; i++)
			super_block[(inode_start_offset + i)] = inodes_copy[inode_counter].name[i];
		inode_start_offset += 8;

		// convert size int to char[] and save it to the buffer
		convertIntToChar(inodes_copy[inode_counter].size, int_conv);
		for (int i = 0; i < 4; i++)
			super_block[(inode_start_offset + i)] = int_conv[i];
		inode_start_offset += 4;

		// convert each of the int block pointers to char[] and save them to the buffer
		for (int i = 0; i < 8; i++) {
			convertIntToChar(inodes_copy[inode_counter].blockPointers[i], int_conv);
			for (int j = 0; j < 4; j++) {
				super_block[(inode_start_offset + 4 * i + j)] = int_conv[j];
			}
		}
		delete[] int_conv;
	}

	// Flag that this is already a backup
	int offset = FREE_BLOCK_LIST_SIZE + (INODE_SIZE * NUM_INODES);
	super_block[offset] = 1;

	backup->seekg(0, ios::beg);
	backup->write(super_block, BLOCK_SIZE);
	
	delete[] super_block;
}