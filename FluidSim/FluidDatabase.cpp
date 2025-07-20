#include "FluidDatabase.h"


FluidDatabase::FluidDatabase(const std::string& dbPath)
    : path(dbPath), db(nullptr) {}

FluidDatabase::~FluidDatabase() {
    close();
}

bool FluidDatabase::open() {
    if (sqlite3_open(path.c_str(), &db) == SQLITE_OK) {
        return true;
    }
    db = nullptr;
    return false;
}

void FluidDatabase::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool FluidDatabase::saveSimulationParameters(const std::map<std::string, std::string>& parameters) {
     if (!db) return false;

     const char* sql =
          "INSERT INTO SimulationConfigs "
          "(ConfigID, Name, GridSize, ParticleCount, InflowParamsJSON, OutflowParamsJSON, "
          "Timestep, MethodOfComputation, FluidID, Description, IsStandard, OtherParamsJSON) "
          "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

     sqlite3_stmt* stmt = nullptr;
     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
          return false;

     sqlite3_bind_text(stmt, 1, parameters.at("Name").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 2, parameters.at("GridSize").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_int(stmt, 3, std::stoi(parameters.at("ParticleCount")));
     sqlite3_bind_text(stmt, 4, parameters.at("InflowParamsJSON").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 5, parameters.at("OutflowParamsJSON").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_double(stmt, 6, std::stod(parameters.at("Timestep")));
     sqlite3_bind_text(stmt, 7, parameters.at("MethodOfComputation").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_int(stmt, 8, std::stoi(parameters.at("FluidID")));
     sqlite3_bind_text(stmt, 9, parameters.at("Description").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_int(stmt, 10, std::stoi(parameters.at("IsStandard")));
     sqlite3_bind_text(stmt, 11, parameters.at("OtherParamsJSON").c_str(), -1, SQLITE_TRANSIENT);

     bool success = (sqlite3_step(stmt) == SQLITE_DONE);

     sqlite3_finalize(stmt);
     return success;
}

bool FluidDatabase::loadSimulationParameters(const int configID, std::map<std::string, std::string>& parameters) {
     if (!db) return false;

     const char* sql =
          "SELECT ConfigID, Name, GridSize, ParticleCount, InflowParamsJSON, OutflowParamsJSON, "
          "Timestep, MethodOfComputation, FluidID, Description, IsStandard, OtherParamsJSON "
          "FROM SimulationConfigs WHERE ConfigID = ?;";

     sqlite3_stmt* stmt = nullptr;
     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
          return false;

     if (sqlite3_bind_int(stmt, 1, configID) != SQLITE_OK) {
          sqlite3_finalize(stmt);
          return false;
     }

     bool found = false;
     if (sqlite3_step(stmt) == SQLITE_ROW) {
          parameters["ConfigID"] = std::to_string(sqlite3_column_int(stmt, 0));

          const unsigned char* nameText = sqlite3_column_text(stmt, 1);
          parameters["Name"] = nameText ? reinterpret_cast<const char*>(nameText) : "";

          const unsigned char* gridSizeText = sqlite3_column_text(stmt, 2);
          parameters["GridSize"] = gridSizeText ? reinterpret_cast<const char*>(gridSizeText) : "";

          parameters["ParticleCount"] = std::to_string(sqlite3_column_int(stmt, 3));

          const unsigned char* inflowText = sqlite3_column_text(stmt, 4);
          parameters["InflowParamsJSON"] = inflowText ? reinterpret_cast<const char*>(inflowText) : "";

          const unsigned char* outflowText = sqlite3_column_text(stmt, 5);
          parameters["OutflowParamsJSON"] = outflowText ? reinterpret_cast<const char*>(outflowText) : "";

          parameters["Timestep"] = std::to_string(sqlite3_column_double(stmt, 6));

          const unsigned char* methodText = sqlite3_column_text(stmt, 7);
          parameters["MethodOfComputation"] = methodText ? reinterpret_cast<const char*>(methodText) : "";

          parameters["FluidID"] = std::to_string(sqlite3_column_int(stmt, 8));

          const unsigned char* descText = sqlite3_column_text(stmt, 9);
          parameters["Description"] = descText ? reinterpret_cast<const char*>(descText) : "";

          parameters["IsStandard"] = std::to_string(sqlite3_column_int(stmt, 10));

          const unsigned char* otherParamsText = sqlite3_column_text(stmt, 11);
          parameters["OtherParamsJSON"] = otherParamsText ? reinterpret_cast<const char*>(otherParamsText) : "";
          found = true;
     }

     sqlite3_finalize(stmt);
     return found;
}

bool FluidDatabase::updateSimulationParameters(const int configID, std::map<std::string, std::string>& parameters) {
     if (!db) return false;

     const char* sql =
          "UPDATE SimulationConfigs SET "
          "Name = ?, "
          "GridSize = ?, "
          "ParticleCount = ?, "
          "InflowParamsJSON = ?, "
          "OutflowParamsJSON = ?, "
          "Timestep = ?, "
          "MethodOfComputation = ?, "
          "FluidID = ?, "
          "Description = ?, "
          "IsStandard = ?, "
          "OtherParamsJSON = ? "
          "WHERE ConfigID = ?;";

     sqlite3_stmt* stmt = nullptr;
     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
          return false;

     sqlite3_bind_text(stmt, 1, parameters.at("Name").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 2, parameters.at("GridSize").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_int(stmt, 3, std::stoi(parameters.at("ParticleCount")));
     sqlite3_bind_text(stmt, 4, parameters.at("InflowParamsJSON").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 5, parameters.at("OutflowParamsJSON").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_double(stmt, 6, std::stod(parameters.at("Timestep")));
     sqlite3_bind_text(stmt, 7, parameters.at("MethodOfComputation").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_int(stmt, 8, std::stoi(parameters.at("FluidID")));
     sqlite3_bind_text(stmt, 9, parameters.at("Description").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_int(stmt, 10, std::stoi(parameters.at("IsStandard")));
     sqlite3_bind_text(stmt, 11, parameters.at("OtherParamsJSON").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_int(stmt, 12, configID);

     bool success = (sqlite3_step(stmt) == SQLITE_DONE);

     sqlite3_finalize(stmt);
     return success;
}

bool FluidDatabase::saveLiquidType(const std::string& name, double density, double viscosity,
     const std::string& color, const std::string& description,
     const std::string& otherPhysicalPropertiesJSON) {
     if (!db) return false;

     const char* sql =
          "INSERT INTO TypesOfLiquids "
          "(Name, Density, Viscosity, Color, Description, OtherPhysicalPropertiesJSON) "
          "VALUES (?, ?, ?, ?, ?, ?);";

     sqlite3_stmt* stmt = nullptr;
     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
          return false;

     sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_double(stmt, 2, density);
     sqlite3_bind_double(stmt, 3, viscosity);
     sqlite3_bind_text(stmt, 4, color.c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 5, description.c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 6, otherPhysicalPropertiesJSON.c_str(), -1, SQLITE_TRANSIENT);

     bool success = (sqlite3_step(stmt) == SQLITE_DONE);

     sqlite3_finalize(stmt);
     return success;
}

bool FluidDatabase::loadLiquidType(int liquidID, std::map<std::string, std::string>& liquidData) {
     if (!db) return false;

     const char* sql =
          "SELECT LiquidID, Name, Density, Viscosity, Color, Description, OtherPhysicalPropertiesJSON "
          "FROM TypesOfLiquids WHERE LiquidID = ?;";

     sqlite3_stmt* stmt = nullptr;
     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
          return false;

     if (sqlite3_bind_int(stmt, 1, liquidID) != SQLITE_OK) {
          sqlite3_finalize(stmt);
          return false;
     }

     bool found = false;
     if (sqlite3_step(stmt) == SQLITE_ROW) {
          liquidData["LiquidID"] = std::to_string(sqlite3_column_int(stmt, 0));

          const unsigned char* nameText = sqlite3_column_text(stmt, 1);
          liquidData["Name"] = nameText ? reinterpret_cast<const char*>(nameText) : "";

          liquidData["Density"] = std::to_string(sqlite3_column_double(stmt, 2));
          liquidData["Viscosity"] = std::to_string(sqlite3_column_double(stmt, 3));

          const unsigned char* colorText = sqlite3_column_text(stmt, 4);
          liquidData["Color"] = colorText ? reinterpret_cast<const char*>(colorText) : "";

          const unsigned char* descText = sqlite3_column_text(stmt, 5);
          liquidData["Description"] = descText ? reinterpret_cast<const char*>(descText) : "";

          const unsigned char* otherPropsText = sqlite3_column_text(stmt, 6);
          liquidData["OtherPhysicalPropertiesJSON"] = otherPropsText ? reinterpret_cast<const char*>(otherPropsText) : "";

          found = true;
     }

     sqlite3_finalize(stmt);
     return found;
}

bool FluidDatabase::updateLiquidType(int liquidID, std::map<std::string, std::string>& liquidData) {
     if (!db) return false;

     const char* sql =
          "UPDATE TypesOfLiquids SET "
          "Name = ?, "
          "Density = ?, "
          "Viscosity = ?, "
          "Color = ?, "
          "Description = ?, "
          "OtherPhysicalPropertiesJSON = ? "
          "WHERE LiquidID = ?;";

     sqlite3_stmt* stmt = nullptr;
     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
          return false;

     sqlite3_bind_text(stmt, 1, liquidData.at("Name").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_double(stmt, 2, std::stod(liquidData.at("Density")));
     sqlite3_bind_double(stmt, 3, std::stod(liquidData.at("Viscosity")));
     sqlite3_bind_text(stmt, 4, liquidData.at("Color").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 5, liquidData.at("Description").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 6, liquidData.at("OtherPhysicalPropertiesJSON").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_int(stmt, 7, liquidID);

     bool success = (sqlite3_step(stmt) == SQLITE_DONE);

     sqlite3_finalize(stmt);
     return success;
}

bool FluidDatabase::saveSimulation(const int configID, const std::string& dateTime,
     const std::string& resultFilePath, double duration,
     const std::string& notes, const std::string& user,
     int seed, const std::string& version,
     const std::string& otherMetadataJSON) {
     if (!db) return false;

     const char* sql =
          "INSERT INTO SavedSimulations "
          "(ConfigID, DateTime, ResultFilePath, Duration, Notes, User, Seed, Version, OtherMetadataJSON) "
          "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";

     sqlite3_stmt* stmt = nullptr;
     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
          return false;

     sqlite3_bind_int(stmt, 1, configID);
     sqlite3_bind_text(stmt, 2, dateTime.c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 3, resultFilePath.c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_double(stmt, 4, duration);
     sqlite3_bind_text(stmt, 5, notes.c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 6, user.c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_int(stmt, 7, seed);
     sqlite3_bind_text(stmt, 8, version.c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 9, otherMetadataJSON.c_str(), -1, SQLITE_TRANSIENT);

     bool success = (sqlite3_step(stmt) == SQLITE_DONE);

     sqlite3_finalize(stmt);
     return success;
}

bool FluidDatabase::loadSimulation(int simulationID, std::map<std::string, std::string>& simulationData) {
     if (!db) return false;

     const char* sql =
          "SELECT SimulationID, ConfigID, DateTime, ResultFilePath, Duration, Notes, User, Seed, Version, OtherMetadataJSON "
          "FROM SavedSimulations WHERE SimulationID = ?;";

     sqlite3_stmt* stmt = nullptr;
     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
          return false;

     if (sqlite3_bind_int(stmt, 1, simulationID) != SQLITE_OK) {
          sqlite3_finalize(stmt);
          return false;
     }

     bool found = false;
     if (sqlite3_step(stmt) == SQLITE_ROW) {
          simulationData["SimulationID"] = std::to_string(sqlite3_column_int(stmt, 0));
          simulationData["ConfigID"] = std::to_string(sqlite3_column_int(stmt, 1));

          const unsigned char* dateTimeText = sqlite3_column_text(stmt, 2);
          simulationData["DateTime"] = dateTimeText ? reinterpret_cast<const char*>(dateTimeText) : "";

          const unsigned char* resultFilePathText = sqlite3_column_text(stmt, 3);
          simulationData["ResultFilePath"] = resultFilePathText ? reinterpret_cast<const char*>(resultFilePathText) : "";

          simulationData["Duration"] = std::to_string(sqlite3_column_double(stmt, 4));

          const unsigned char* notesText = sqlite3_column_text(stmt, 5);
          simulationData["Notes"] = notesText ? reinterpret_cast<const char*>(notesText) : "";

          const unsigned char* userText = sqlite3_column_text(stmt, 6);
          simulationData["User"] = userText ? reinterpret_cast<const char*>(userText) : "";

          simulationData["Seed"] = std::to_string(sqlite3_column_int(stmt, 7));

          const unsigned char* versionText = sqlite3_column_text(stmt, 8);
          simulationData["Version"] = versionText ? reinterpret_cast<const char*>(versionText) : "";

          const unsigned char* otherMetaText = sqlite3_column_text(stmt, 9);
          simulationData["OtherMetadataJSON"] = otherMetaText ? reinterpret_cast<const char*>(otherMetaText) : "";

          found = true;
     }

     sqlite3_finalize(stmt);
     return found;
}

bool FluidDatabase::updateSimulation(int simulationID, std::map<std::string, std::string>& simulationData) {
     if (!db) return false;

     const char* sql =
          "UPDATE SavedSimulations SET "
          "ConfigID = ?, "
          "DateTime = ?, "
          "ResultFilePath = ?, "
          "Duration = ?, "
          "Notes = ?, "
          "User = ?, "
          "Seed = ?, "
          "Version = ?, "
          "OtherMetadataJSON = ? "
          "WHERE SimulationID = ?;";

     sqlite3_stmt* stmt = nullptr;
     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
          return false;

     sqlite3_bind_int(stmt, 1, std::stoi(simulationData.at("ConfigID")));
     sqlite3_bind_text(stmt, 2, simulationData.at("DateTime").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 3, simulationData.at("ResultFilePath").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_double(stmt, 4, std::stod(simulationData.at("Duration")));
     sqlite3_bind_text(stmt, 5, simulationData.at("Notes").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 6, simulationData.at("User").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_int(stmt, 7, std::stoi(simulationData.at("Seed")));
     sqlite3_bind_text(stmt, 8, simulationData.at("Version").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 9, simulationData.at("OtherMetadataJSON").c_str(), -1, SQLITE_TRANSIENT);
     sqlite3_bind_int(stmt, 10, simulationID);

     bool success = (sqlite3_step(stmt) == SQLITE_DONE);

     sqlite3_finalize(stmt);
     return success;
}