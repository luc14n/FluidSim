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
    if (validateDB()) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool FluidDatabase::saveSimulationParameters(const std::map<std::string, std::string>& parameters) {
    if (!validateDB()) return false;

    bool hasPK = parameters.count("ConfigID") && !parameters.at("ConfigID").empty();
    const char* sql = nullptr;
    sqlite3_stmt* stmt = nullptr;
    int bindIdx = 1;

    if (hasPK) {
        sql =
            "INSERT INTO SimulationConfigs "
            "(ConfigID, Name, GridSize, ParticleCount, InflowParamsJSON, OutflowParamsJSON, "
            "Timestep, MethodOfComputation, FluidID, Description, IsStandard, OtherParamsJSON) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    }
    else {
        sql =
            "INSERT INTO SimulationConfigs "
            "(Name, GridSize, ParticleCount, InflowParamsJSON, OutflowParamsJSON, "
            "Timestep, MethodOfComputation, FluidID, Description, IsStandard, OtherParamsJSON) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        printf("SQLite error: %s\n", sqlite3_errmsg(db));
        return false;
    }

    if (hasPK) {
        sqlite3_bind_int(stmt, bindIdx++, std::stoi(parameters.at("ConfigID")));
    }
    sqlite3_bind_text(stmt, bindIdx++, parameters.at("Name").c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, bindIdx++, parameters.at("GridSize").c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, bindIdx++, std::stoi(parameters.at("ParticleCount")));
    sqlite3_bind_text(stmt, bindIdx++, parameters.at("InflowParamsJSON").c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, bindIdx++, parameters.at("OutflowParamsJSON").c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, bindIdx++, std::stod(parameters.at("Timestep")));
    sqlite3_bind_text(stmt, bindIdx++, parameters.at("MethodOfComputation").c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, bindIdx++, std::stoi(parameters.at("FluidID")));
    sqlite3_bind_text(stmt, bindIdx++, parameters.at("Description").c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, bindIdx++, std::stoi(parameters.at("IsStandard")));
    sqlite3_bind_text(stmt, bindIdx++, parameters.at("OtherParamsJSON").c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return success;
}

bool FluidDatabase::loadSimulationParameters(const int configID, std::map<std::string, std::string>& parameters) {
     if (!validateDB()) return false;

     const char* sql =
          "SELECT ConfigID, Name, GridSize, ParticleCount, InflowParamsJSON, OutflowParamsJSON, "
          "Timestep, MethodOfComputation, FluidID, Description, IsStandard, OtherParamsJSON "
          "FROM SimulationConfigs WHERE ConfigID = ?;";

     sqlite3_stmt* stmt = nullptr;
     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
         printf("SQLite error: %s\n", sqlite3_errmsg(db));
         return false;
     }

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

bool FluidDatabase::loadAllSimulationParameters(std::vector<std::map<std::string, std::string>>& records) {
    if (!validateDB()) return false;
    const char* sql = "SELECT * FROM SimulationConfigs;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> row;
        row["ConfigID"] = std::to_string(sqlite3_column_int(stmt, 0));
        row["Name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        row["GridSize"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        row["ParticleCount"] = std::to_string(sqlite3_column_int(stmt, 3));
        row["InflowParamsJSON"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        row["OutflowParamsJSON"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        row["Timestep"] = std::to_string(sqlite3_column_double(stmt, 6));
        row["MethodOfComputation"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        row["FluidID"] = std::to_string(sqlite3_column_int(stmt, 8));
        row["Description"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        row["IsStandard"] = std::to_string(sqlite3_column_int(stmt, 10));
        row["OtherParamsJSON"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        records.push_back(row);
    }
    sqlite3_finalize(stmt);
    return true;
}

bool FluidDatabase::updateSimulationParameters(const int configID, std::map<std::string, std::string>& parameters) {
    if (!validateDB()) return false;

    // List of all possible columns (excluding PK)
    const char* allColumns[] = {
        "Name", "GridSize", "ParticleCount", "InflowParamsJSON", "OutflowParamsJSON",
        "Timestep", "MethodOfComputation", "FluidID", "Description", "IsStandard", "OtherParamsJSON"
    };
    std::string sql = "UPDATE SimulationConfigs SET ";
    std::vector<std::string> columnsToUpdate;
    std::vector<std::string> valuesToBind;

    // Build the SET clause only for non-empty values
    for (size_t i = 0; i < sizeof(allColumns) / sizeof(allColumns[0]); ++i) {
        auto it = parameters.find(allColumns[i]);
        if (it != parameters.end() && !it->second.empty()) {
            if (!columnsToUpdate.empty()) sql += ", ";
            sql += allColumns[i];
            sql += " = ?";
            columnsToUpdate.push_back(allColumns[i]);
            valuesToBind.push_back(it->second);
        }
    }

    if (columnsToUpdate.empty()) {
        // Nothing to update
        return false;
    }

    sql += " WHERE ConfigID = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    // Bind only the non-empty values
    int bindIdx = 1;
    for (size_t i = 0; i < columnsToUpdate.size(); ++i) {
        const std::string& col = columnsToUpdate[i];
        const std::string& val = valuesToBind[i];
        if (col == "ParticleCount" || col == "FluidID" || col == "IsStandard") {
            sqlite3_bind_int(stmt, bindIdx++, std::stoi(val));
        }
        else if (col == "Timestep") {
            sqlite3_bind_double(stmt, bindIdx++, std::stod(val));
        }
        else {
            sqlite3_bind_text(stmt, bindIdx++, val.c_str(), -1, SQLITE_TRANSIENT);
        }
    }
    // Bind the PK
    sqlite3_bind_int(stmt, bindIdx, configID);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return success;
}

bool FluidDatabase::saveLiquidType(const std::string& liquidID, const std::string& name, double density, double viscosity,
    const std::string& color, const std::string& description,
    const std::string& otherPhysicalPropertiesJSON) {
    if (!validateDB()) return false;

    bool hasPK = !liquidID.empty();
    const char* sql = nullptr;
    sqlite3_stmt* stmt = nullptr;
    int bindIdx = 1;

    if (hasPK) {
        sql =
            "INSERT INTO TypesOfLiquids "
            "(LiquidID, Name, Density, Viscosity, Color, Description, OtherPhysicalPropertiesJSON) "
            "VALUES (?, ?, ?, ?, ?, ?, ?);";
    }
    else {
        sql =
            "INSERT INTO TypesOfLiquids "
            "(Name, Density, Viscosity, Color, Description, OtherPhysicalPropertiesJSON) "
            "VALUES (?, ?, ?, ?, ?, ?);";
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        printf("SQLite error: %s\n", sqlite3_errmsg(db));
        return false;
    }

    if (hasPK) {
        sqlite3_bind_int(stmt, bindIdx++, std::stoi(liquidID));
    }
    sqlite3_bind_text(stmt, bindIdx++, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, bindIdx++, density);
    sqlite3_bind_double(stmt, bindIdx++, viscosity);
    sqlite3_bind_text(stmt, bindIdx++, color.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, bindIdx++, description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, bindIdx++, otherPhysicalPropertiesJSON.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return success;
}

bool FluidDatabase::loadLiquidType(int liquidID, std::map<std::string, std::string>& liquidData) {
     if (!validateDB()) return false;

     const char* sql =
          "SELECT LiquidID, Name, Density, Viscosity, Color, Description, OtherPhysicalPropertiesJSON "
          "FROM TypesOfLiquids WHERE LiquidID = ?;";

     sqlite3_stmt* stmt = nullptr;
     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
         printf("SQLite error: %s\n", sqlite3_errmsg(db));
         return false;
     }

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

bool FluidDatabase::loadAllLiquidTypes(std::vector<std::map<std::string, std::string>>& records) {
    if (!validateDB()) return false;
    const char* sql = "SELECT * FROM TypesOfLiquids;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> row;
        row["LiquidID"] = std::to_string(sqlite3_column_int(stmt, 0));
        row["Name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        row["Density"] = std::to_string(sqlite3_column_double(stmt, 2));
        row["Viscosity"] = std::to_string(sqlite3_column_double(stmt, 3));
        row["Color"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        row["Description"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        row["OtherPhysicalPropertiesJSON"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        records.push_back(row);
    }
    sqlite3_finalize(stmt);
    return true;
}

bool FluidDatabase::updateLiquidType(int liquidID, std::map<std::string, std::string>& liquidData) {
    if (!validateDB()) return false;

    // List of all possible columns (excluding PK)
    const char* allColumns[] = {
        "Name", "Density", "Viscosity", "Color", "Description", "OtherPhysicalPropertiesJSON"
    };
    std::string sql = "UPDATE TypesOfLiquids SET ";
    std::vector<std::string> columnsToUpdate;
    std::vector<std::string> valuesToBind;

    for (size_t i = 0; i < sizeof(allColumns) / sizeof(allColumns[0]); ++i) {
        auto it = liquidData.find(allColumns[i]);
        if (it != liquidData.end() && !it->second.empty()) {
            if (!columnsToUpdate.empty()) sql += ", ";
            sql += allColumns[i];
            sql += " = ?";
            columnsToUpdate.push_back(allColumns[i]);
            valuesToBind.push_back(it->second);
        }
    }

    if (columnsToUpdate.empty()) {
        // Nothing to update
        return false;
    }

    sql += " WHERE LiquidID = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    int bindIdx = 1;
    for (size_t i = 0; i < columnsToUpdate.size(); ++i) {
        const std::string& col = columnsToUpdate[i];
        const std::string& val = valuesToBind[i];
        if (col == "Density" || col == "Viscosity") {
            sqlite3_bind_double(stmt, bindIdx++, std::stod(val));
        }
        else {
            sqlite3_bind_text(stmt, bindIdx++, val.c_str(), -1, SQLITE_TRANSIENT);
        }
    }
    sqlite3_bind_int(stmt, bindIdx, liquidID);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return success;
}

bool FluidDatabase::saveSimulation(const int simulationID, const int configID, const std::string& dateTime,
    const std::string& resultFilePath, double duration,
    const std::string& notes, const std::string& user,
    int seed, const std::string& version,
    const std::string& otherMetadataJSON) {
    if (!validateDB()) return false;

    bool hasPK = simulationID > 0;
    const char* sql = nullptr;
    sqlite3_stmt* stmt = nullptr;
    int bindIdx = 1;

    if (hasPK) {
        sql =
            "INSERT INTO SavedSimulations "
            "(SimulationID, ConfigID, DateTime, ResultFilePath, Duration, Notes, User, Seed, Version, OtherMetadataJSON) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    }
    else {
        sql =
            "INSERT INTO SavedSimulations "
            "(ConfigID, DateTime, ResultFilePath, Duration, Notes, User, Seed, Version, OtherMetadataJSON) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        printf("SQLite error: %s\n", sqlite3_errmsg(db));
        return false;
    }

    if (hasPK) {
        sqlite3_bind_int(stmt, bindIdx++, simulationID);
    }
    sqlite3_bind_int(stmt, bindIdx++, configID);
    sqlite3_bind_text(stmt, bindIdx++, dateTime.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, bindIdx++, resultFilePath.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, bindIdx++, duration);
    sqlite3_bind_text(stmt, bindIdx++, notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, bindIdx++, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, bindIdx++, seed);
    sqlite3_bind_text(stmt, bindIdx++, version.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, bindIdx++, otherMetadataJSON.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return success;
}

bool FluidDatabase::loadSimulation(int simulationID, std::map<std::string, std::string>& simulationData) {
     if (!validateDB()) return false;

     const char* sql =
          "SELECT SimulationID, ConfigID, DateTime, ResultFilePath, Duration, Notes, User, Seed, Version, OtherMetadataJSON "
          "FROM SavedSimulations WHERE SimulationID = ?;";

     sqlite3_stmt* stmt = nullptr;
     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
         printf("SQLite error: %s\n", sqlite3_errmsg(db));
         return false;
     }

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

bool FluidDatabase::loadAllSimulations(std::vector<std::map<std::string, std::string>>& records) {
    if (!validateDB()) return false;
    const char* sql = "SELECT * FROM SavedSimulations;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> row;
        row["SimulationID"] = std::to_string(sqlite3_column_int(stmt, 0));
        row["ConfigID"] = std::to_string(sqlite3_column_int(stmt, 1));
        row["DateTime"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        row["ResultFilePath"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        row["Duration"] = std::to_string(sqlite3_column_double(stmt, 4));
        row["Notes"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        row["User"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        row["Seed"] = std::to_string(sqlite3_column_int(stmt, 7));
        row["Version"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        row["OtherMetadataJSON"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        records.push_back(row);
    }
    sqlite3_finalize(stmt);
    return true;
}

bool FluidDatabase::updateSimulation(int simulationID, std::map<std::string, std::string>& simulationData) {
    if (!validateDB()) return false;

    // List of all possible columns (excluding PK)
    const char* allColumns[] = {
        "ConfigID", "DateTime", "ResultFilePath", "Duration", "Notes",
        "User", "Seed", "Version", "OtherMetadataJSON"
    };
    std::string sql = "UPDATE SavedSimulations SET ";
    std::vector<std::string> columnsToUpdate;
    std::vector<std::string> valuesToBind;

    for (size_t i = 0; i < sizeof(allColumns) / sizeof(allColumns[0]); ++i) {
        auto it = simulationData.find(allColumns[i]);
        if (it != simulationData.end() && !it->second.empty()) {
            if (!columnsToUpdate.empty()) sql += ", ";
            sql += allColumns[i];
            sql += " = ?";
            columnsToUpdate.push_back(allColumns[i]);
            valuesToBind.push_back(it->second);
        }
    }

    if (columnsToUpdate.empty()) {
        // Nothing to update
        return false;
    }

    sql += " WHERE SimulationID = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    int bindIdx = 1;
    for (size_t i = 0; i < columnsToUpdate.size(); ++i) {
        const std::string& col = columnsToUpdate[i];
        const std::string& val = valuesToBind[i];
        if (col == "ConfigID" || col == "Seed") {
            sqlite3_bind_int(stmt, bindIdx++, std::stoi(val));
        }
        else if (col == "Duration") {
            sqlite3_bind_double(stmt, bindIdx++, std::stod(val));
        }
        else {
            sqlite3_bind_text(stmt, bindIdx++, val.c_str(), -1, SQLITE_TRANSIENT);
        }
    }
    sqlite3_bind_int(stmt, bindIdx, simulationID);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return success;
}

bool FluidDatabase::queryTable(
    const std::string& tableName,
    const std::vector<std::string>& columns,
    const std::map<std::string, std::string>& filters,
    std::vector<std::map<std::string, std::string>>& results)
{
    if (!validateDB()) return false;

    // Build SELECT statement
    std::string sql = "SELECT ";
    for (size_t i = 0; i < columns.size(); ++i) {
        if (i > 0) sql += ", ";
        sql += columns[i];
    }
    sql += " FROM " + tableName;

    // Build WHERE clause
    std::vector<std::string> filterKeys;
    for (const auto& kv : filters) {
        if (!kv.second.empty())
            filterKeys.push_back(kv.first);
    }
    if (!filterKeys.empty()) {
        sql += " WHERE ";
        for (size_t i = 0; i < filterKeys.size(); ++i) {
            if (i > 0) sql += " AND ";
            sql += filterKeys[i] + " = ?";
        }
    }
    sql += ";";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        printf("SQLite error: %s\n", sqlite3_errmsg(db));
        return false;
    }

    // Bind filter values
    int bindIdx = 1;
    for (const auto& key : filterKeys) {
        sqlite3_bind_text(stmt, bindIdx++, filters.at(key).c_str(), -1, SQLITE_TRANSIENT);
    }

    // Fetch results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> row;
        for (size_t i = 0; i < columns.size(); ++i) {
            const unsigned char* text = sqlite3_column_text(stmt, static_cast<int>(i));
            row[columns[i]] = text ? reinterpret_cast<const char*>(text) : "";
        }
        results.push_back(row);
    }
    sqlite3_finalize(stmt);
    return true;
}

bool FluidDatabase::validateDB() {
    try {
        if (!db) {
            throw std::runtime_error("No DB found to save to.");
        }
        else return true;
    }
    catch (const std::bad_exception& ex) {
        std::cout << ex.what() << std::endl;
        return false;
    }
    
}