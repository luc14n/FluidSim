#pragma once
#include "sqlite3.h"
#include <string>

class FluidDatabase {
public:
	FluidDatabase(const std::string& dbPath);
	~FluidDatabase();

	bool open();
	void close();
	bool createTables();
	// Methods for saving/aloading fluid types, simulation states, etc.

private:
	std::string path;
	sqlite3* db;
};