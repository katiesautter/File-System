#pragma once
class Cache
{
public:
	Cache();
	void saveToCache(int blockNum, char buf[1024]);

	~Cache();
};

