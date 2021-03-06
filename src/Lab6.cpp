// Lab6.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FileSystem.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <time.h>

using namespace std;

/*
	Saves random letters to output to use as a buffer
*/
void createRandomBuffer(char output[1024]) {
	for (int i = 0; i < 1024; i++) {
		output[i] = 'a' + rand() % 26;
	}
}

vector<string> tokenize(string const &str, const char delim) {
	vector<string> out;
	stringstream ss(str);
	string s;
	while (getline(ss, s, delim)) {
		if (!s.empty()) // ignore empty tokens
			out.push_back(s);
	}

	return out;
}

int main() {
	/*ifstream infile;
	infile.open("commands.txt");
	srand(time(NULL));

	if (infile.good()) {
		string line;

		// create the file system from the disk name on the first line
		getline(infile, line);
		vector<string> line_split = tokenize(line, ' ');
		FileSystem fileSys(line_split[0]);

		while (getline(infile, line)) {
			// split line by space, ignoring empty tokens
			vector<string> line_split = tokenize(line, ' ');

			// do the command specified on each other line
			if (line_split[0].compare("C") == 0) {
				char * name = (char *)line_split[1].c_str();
				fileSys.createf(name, stoi(line_split[2]));
			}
			else if (line_split[0].compare("D") == 0) {
				char * name = (char *)line_split[1].c_str();
				fileSys.deletef(name);
			}
			else if (line_split[0].compare("R") == 0) {
				char * buff = new char[fileSys.BLOCK_SIZE];
				char * name = (char *)line_split[1].c_str();
				int block_num = stoi(line_split[2]);
				fileSys.readf(name, block_num, buff);
				delete[] buff;
			}
			else if (line_split[0].compare("W") == 0) {
				char * buff = new char[fileSys.BLOCK_SIZE];
				createRandomBuffer(buff);
				char * name = (char *)line_split[1].c_str();
				int block_num = stoi(line_split[2]);
				fileSys.writef(name, block_num, buff);
				delete[] buff;
			}
			else {
				fileSys.ls();
			}
		}
	}*/

	FileSystem sys("disk0");
	sys.init_backup("disk1");

	system("pause");
	return 0;
}

