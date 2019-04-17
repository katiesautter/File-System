#include "stdafx.h"
#include "CacheNode.h"


CacheNode::CacheNode(char name[8], int blockNum, char buf[1024])
{
	next = NULL;
	blockNumber = blockNum;
	blockData = buf;
	fileName = name;

}
char * CacheNode::toString()
{
	char str[1000];
	sprintf_s(str, sizeof(str), "CacheNode: NumBlock= %d File Name = %s", blockNumber, fileName);
	return str;

}

CacheNode::~CacheNode()
{
}
