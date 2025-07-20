#pragma once
#include "sqlite3.h"
#include <string>
#include <map>
#include <iostream>

/**
 * @brief Handles all database operations for the FluidSim application.
 *
 * This class provides methods to open, close, and interact with the simulation database,
 * including saving/loading simulation parameters, liquid types, and simulation results.
 */
class FluidDatabase {
public:
    /**
     * @brief Constructs a FluidDatabase object with the given database file path.
     * @param dbPath Path to the SQLite database file.
     */
    FluidDatabase(const std::string& dbPath);

    /**
     * @brief Destructor. Closes the database connection if open.
     */
    ~FluidDatabase();

    /**
     * @brief Opens the database connection.
     * @return True if the connection was successful, false otherwise.
     */
    bool open();

    /**
     * @brief Closes the database connection.
     */
    void close();

    // --- StandardSimulationConfigs table ---

    /**
     * @brief Map of simulation parameter names and values for the SimulationConfigs table.
     *
     * The map should contain the following keys (all values as strings):
     * - "ConfigID"                : Unique identifier for the configuration (integer, as string)
     * - "Name"                    : Name of the simulation configuration
     * - "GridSize"                : Grid size (e.g., "100x100" or similar)
     * - "ParticleCount"           : Number of particles (integer, as string)
     * - "InflowParamsJSON"        : Inflow parameters in JSON format
     * - "OutflowParamsJSON"       : Outflow parameters in JSON format
     * - "Timestep"                : Simulation timestep (floating-point, as string)
     * - "MethodOfComputation"     : Description or code for the computation method
     * - "FluidID"                 : ID of the fluid type used (integer, as string)
     * - "Description"             : Description of the configuration
     * - "IsStandard"              : Whether this is a standard config (0 or 1, as string)
     * - "OtherParamsJSON"         : Additional parameters in JSON format
     *
     * @note All values must be convertible to the correct type as required by the database schema.
     */

    /**
     * @brief Saves simulation parameters to the SimulationConfigs table.
     * @param parameters Map of parameter names and values.
     * @return True if the operation was successful, false otherwise.
     */
    bool saveSimulationParameters(const std::map<std::string, std::string>& parameters);

    /**
     * @brief Loads simulation parameters from the SimulationConfigs table.
     * @param ConfigID The configuration ID to load.
     * @param parameters Map to be filled with parameter names and values.
     * @return True if the configuration was found and loaded, false otherwise.
     */
    bool loadSimulationParameters(const int ConfigID, std::map<std::string, std::string>& parameters);

    /**
     * @brief Updates simulation parameters in the SimulationConfigs table.
     * @param ConfigID The configuration ID to update.
     * @param parameters Map of parameter names and values to update.
     * @return True if the update was successful, false otherwise.
     */
    bool updateSimulationParameters(const int ConfigID, std::map<std::string, std::string>& parameters);

    // --- TypesOfLiquids table ---

    /**
     * @brief Map of liquid property names and values for the TypesOfLiquids table.
     *
     * The map should contain the following keys (all values as strings):
     * - "LiquidID"                    : Unique identifier for the liquid (integer, as string)
     * - "Name"                        : Name of the liquid
     * - "Density"                     : Density of the liquid (floating-point, as string)
     * - "Viscosity"                   : Viscosity of the liquid (floating-point, as string)
     * - "Color"                       : Color of the liquid (as a string)
     * - "Description"                 : Description of the liquid
     * - "OtherPhysicalPropertiesJSON" : Additional properties in JSON format
     *
     * @note All values must be convertible to the correct type as required by the database schema.
     */

    /**
     * @brief Saves a new liquid type to the TypesOfLiquids table.
     * @param name Name of the liquid.
     * @param density Density of the liquid.
     * @param viscosity Viscosity of the liquid.
     * @param color Color of the liquid (as a string).
     * @param description Description of the liquid.
     * @param otherPhysicalPropertiesJSON Additional properties in JSON format.
     * @return True if the operation was successful, false otherwise.
     */
    bool saveLiquidType(const std::string& name, double density, double viscosity,
                        const std::string& color, const std::string& description,
                        const std::string& otherPhysicalPropertiesJSON);

    /**
     * @brief Loads a liquid type from the TypesOfLiquids table.
     * @param liquidID The ID of the liquid to load.
     * @param liquidData Map to be filled with liquid property names and values.
     * @return True if the liquid was found and loaded, false otherwise.
     */
    bool loadLiquidType(int liquidID, std::map<std::string, std::string>& liquidData);

    /**
     * @brief Updates a liquid type in the TypesOfLiquids table.
     * @param liquidID The ID of the liquid to update.
     * @param liquidData Map of property names and values to update.
     * @return True if the update was successful, false otherwise.
     */
    bool updateLiquidType(int liquidID, std::map<std::string, std::string>& liquidData);

    // --- SavedSimulations table ---

    /**
     * @brief Map of simulation result property names and values for the SavedSimulations table.
     *
     * The map should contain the following keys (all values as strings):
     * - "SimulationID"        : Unique identifier for the simulation (integer, as string)
     * - "ConfigID"            : Configuration ID used for the simulation (integer, as string)
     * - "DateTime"            : Date and time of the simulation
     * - "ResultFilePath"      : Path to the result file
     * - "Duration"            : Duration of the simulation (floating-point, as string)
     * - "Notes"               : Additional notes
     * - "User"                : User who ran the simulation
     * - "Seed"                : Random seed used (integer, as string)
     * - "Version"             : Application version
     * - "OtherMetadataJSON"   : Additional metadata in JSON format
     *
     * @note All values must be convertible to the correct type as required by the database schema.
     */

    /**
     * @brief Saves a simulation result to the SavedSimulations table.
     * @param configID The configuration ID used for the simulation.
     * @param dateTime Date and time of the simulation.
     * @param resultFilePath Path to the result file.
     * @param duration Duration of the simulation.
     * @param notes Additional notes.
     * @param user User who ran the simulation.
     * @param seed Random seed used.
     * @param version Application version.
     * @param otherMetadataJSON Additional metadata in JSON format.
     * @return True if the operation was successful, false otherwise.
     */
    bool saveSimulation(const int configID, const std::string& dateTime,
                        const std::string& resultFilePath, double duration,
                        const std::string& notes, const std::string& user,
                        int seed, const std::string& version,
                        const std::string& otherMetadataJSON);

    /**
     * @brief Loads a simulation result from the SavedSimulations table.
     * @param simulationID The ID of the simulation to load.
     * @param simulationData Map to be filled with simulation property names and values.
     * @return True if the simulation was found and loaded, false otherwise.
     */
    bool loadSimulation(int simulationID, std::map<std::string, std::string>& simulationData);

    /**
     * @brief Updates a simulation result in the SavedSimulations table.
     * @param simulationID The ID of the simulation to update.
     * @param simulationData Map of property names and values to update.
     * @return True if the update was successful, false otherwise.
     */
    bool updateSimulation(int simulationID, std::map<std::string, std::string>& simulationData);

private:
    std::string path; ///< Path to the SQLite database file.
    sqlite3* db;      ///< SQLite database connection handle.
};