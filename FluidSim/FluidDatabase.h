#pragma once
#include "sqlite3.h"
#include <string>
#include <map>

class FluidDatabase {
public:
	FluidDatabase(const std::string& dbPath);
	~FluidDatabase();

	bool open();
	void close();
	bool createTables();
	// Methods for saving/aloading fluid types, simulation states, etc.

	// Loads simulation parameters from the database.
	// Returns a map of parameter names to their values (as stings).
	bool loadSimulationParameters(std::map<std::string, std::string>& parameters);

	// Saves simulation parameters to the database.
	// Accepts a map of parameter names to their values (as strings).
	bool saveSimulationParameters(const std::map<std::string, std::string>& parameters);

private:
	std::string path;
	sqlite3* db;
};