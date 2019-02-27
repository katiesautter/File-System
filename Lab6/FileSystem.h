#pragma once
#include <fstream>
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

	fstream * disk;
	char free_block_list[FREE_BLOCK_LIST_SIZE];
	Inode inodes[NUM_INODES];

	int convertCharToInt(char toConvert[4]);
	void convertIntToChar(int toConvert, char output[4]);
	void readSuperBlock();
	void saveSuperBlock();
};

