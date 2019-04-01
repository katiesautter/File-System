#include "stdafx.h"
#include "CacheNode.h"


CacheNode::CacheNode(int blockNum, char buf[1024])
{
	next = NULL;
	blockNumber = blockNum;
	blockData = buf;

}


CacheNode::~CacheNode()
{
}
