#pragma once
#include "sqlite3.h"
#include <string>
#include <map>
#include <iostream>

class FluidDatabase {
public:
	FluidDatabase(const std::string& dbPath);
	~FluidDatabase();

	bool open();
	void close();
	// Methods for saving/aloading fluid types, simulation states, etc.

	// Methods for StandardSimulationConfigs table
	bool saveSimulationParameters(const std::map<std::string, std::string>& parameters);
	bool loadSimulationParameters(const int ConfigID, std::map<std::string, std::string>& parameters);
	bool updateSimulationParameters(const int ConfigID, std::map<std::string, std::string>& parameters);

	// Methods for TypesOfLiquids table
	bool saveLiquidType(const std::string& name, double density, double viscosity,
	                    const std::string& color, const std::string& description,
	                    const std::string& otherPhysicalPropertiesJSON);
	bool loadLiquidType(int liquidID, std::map<std::string, std::string>& liquidData);
	bool updateLiquidType(int liquidID, std::map<std::string, std::string>& liquidData);

	// Methods for SavedSimulations table
	bool saveSimulation(const int configID, const std::string& dateTime,
	                    const std::string& resultFilePath, double duration,
	                    const std::string& notes, const std::string& user,
	                    int seed, const std::string& version,
	                    const std::string& otherMetadataJSON);
	bool loadSimulation(int simulationID, std::map<std::string, std::string>& simulationData);
	bool updateSimulation(int simulationID, std::map<std::string, std::string>& simulationData);

private:
	std::string path;
	sqlite3* db;
};