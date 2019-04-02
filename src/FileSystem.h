#pragma once
#include <fstream>
#include "Semaphore.h"
using namespace std;

class FileSystem
{
public:
	FileSystem(string diskName);
	~FileSystem();
	int createf(char name[8], int size);
	int deletef(char name[8]);
	int ls(void);
	int readf(char name[8], int blockNum, char buf[1024]);
	int writef(char name[8], int blockNum, char buf[1024]);
	int init_backup(string diskName);

	static const int BLOCK_SIZE = 1024;
	static const int FREE_BLOCK_LIST_SIZE = 128;
	static const int NUM_INODES = 16;
	static const int INODE_SIZE = 48;

private:
	struct Inode {
	public:
		Inode() {
			for (int i = 0; i < 8; i++) {
				name[i] = 0;
				blockPointers[i] = 0;
			}
			size = 0;
			used = 0;
		}
		char name[8];
		int size;
		int blockPointers[8];
		int used;
	};

	fstream * disk; // simulation keeps the "disk" on a file
	char free_block_list[FREE_BLOCK_LIST_SIZE]; // array to track which blocks are free
	char modified_block_list[FREE_BLOCK_LIST_SIZE]; // array to track blocks modified since last backup
	Inode inodes[NUM_INODES];
	
	Semaphore open_files[NUM_INODES]; // semaphore to control file access
	Semaphore free_block_lock; // semaphore to control access to the free block list
	Semaphore modified_block_lock; // semaphore to control access to modified block list

	int convertCharToInt(char toConvert[4]);
	void convertIntToChar(int toConvert, char output[4]);
	void readSuperBlock();
	void saveSuperBlock();
	
	void newBackup(string diskName); // start a complete backup
	void modifiedBackup(string diskName); // start a backup of only modified files
	void backupSuperBlock(fstream* backup, Inode inodes_copy[]);

};

