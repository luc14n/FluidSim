-- Types of Liquids
CREATE TABLE IF NOT EXISTS TypesOfLiquids (
    LiquidID INTEGER PRIMARY KEY,
    Name TEXT NOT NULL,
    Density REAL,
    Viscosity REAL,
    Color TEXT,
    Description TEXT,
    OtherPhysicalProperties TEXT
);

-- Standard Simulation Configurations
CREATE TABLE IF NOT EXISTS StandardSimulationConfigs (
    ConfigID INTEGER PRIMARY KEY,
    Name TEXT NOT NULL,
    GridSize TEXT,
    ParticleCount INTEGER,
    InflowParams TEXT,
    OutflowParams TEXT,
    Timestep REAL,
    MethodOfComputation TEXT,
    FluidID INTEGER,
    Description TEXT,
    OtherParams TEXT,
    FOREIGN KEY (FluidID) REFERENCES TypesOfLiquids(LiquidID)
);

-- Saved Simulations
CREATE TABLE IF NOT EXISTS SavedSimulations (
    SimulationID INTEGER PRIMARY KEY,
    ConfigID INTEGER,
    DateTime TEXT,
    ResultFilePath TEXT,
    Duration REAL,
    Notes TEXT,
    User TEXT,
    Seed INTEGER,
    Version TEXT,
    OtherMetadata TEXT,
    FOREIGN KEY (ConfigID) REFERENCES StandardSimulationConfigs(ConfigID)
);