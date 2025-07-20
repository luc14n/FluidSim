-- Types of Liquids
CREATE TABLE IF NOT EXISTS TypesOfLiquids (
    LiquidID INTEGER PRIMARY KEY,
    Name TEXT NOT NULL,
    Density REAL,
    Viscosity REAL,
    Color TEXT,
    Description TEXT,
    OtherPhysicalPropertiesJSON TEXT -- JSON package
);

-- Simulation Configurations
CREATE TABLE IF NOT EXISTS SimulationConfigs (
    ConfigID INTEGER PRIMARY KEY,
    Name TEXT NOT NULL,
    GridSize TEXT,
    ParticleCount INTEGER,
    InflowParamsJSON TEXT,   -- JSON package
    OutflowParamsJSON TEXT,  -- JSON package
    Timestep REAL,
    MethodOfComputation TEXT,
    FluidID INTEGER,
    Description TEXT,
    IsStandard INTEGER DEFAULT 0  -- 0 = not standard, 1 = standard,
    OtherParamsJSON TEXT,    -- JSON package
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
    OtherMetadataJSON TEXT,  -- JSON package
    FOREIGN KEY (ConfigID) REFERENCES StandardSimulationConfigs(ConfigID)
);