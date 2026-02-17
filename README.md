# TheTrafficSimulator V 1.0

A multi-threaded C++ traffic simulation system that models vehicle behavior and dynamics on a road network.

## Overview

TheTrafficSimulator is a traffic simulation engine that supports multiple vehicle types (bikes, cars, trucks) with speed and acceleration dynamics. It uses multi-threading to handle concurrent vehicle movements and real-time telemetry logging.

## Features

- **Multiple Vehicle Types**: Support for bikes, cars, and trucks with different characteristics
- **Dynamic Vehicle Density**: Adjustable vehicle density that changes during simulation
- **Real-time Telemetry**: CSV-based logging of vehicle positions and speeds
- **Multi-threaded Architecture**: Concurrent vehicle movement and data logging
- **Customizable Road Properties**: Variable road lengths and vehicle parameters

## Project Structure

```
TheTrafficSimulator/
├── Makefile              # Build configuration
├── README.md             # Project documentation
├── logs/                 # Telemetry data output (CSV files)
└── src/
    └── traffic_sim.cpp   # Traffic simulation engine
```

## Building

### Prerequisites

- GCC or Clang compiler with C++11 support
- Make utility

### Build Instructions

```bash
make              # Build the traffic simulator
make clean        # Remove build artifacts
```

The compiled binary will be named `traffic_sim`.

## Running

```bash
./traffic_sim
```

The simulator will:
1. Initialize a road with vehicle instances
2. Start vehicle density adjustment thread
3. Start telemetry logging thread
4. Simulate traffic patterns in real-time
5. Generate CSV telemetry files in the `logs/` folder

## Telemetry Output

Telemetry data is logged to CSV files in the `logs/` folder with the naming convention:
```
Telemetry_<timestamp>.csv
```

Each log entry contains:
- Timestamp
- Vehicle ID
- Current position
- Current speed

## Architecture

### Classes

- **vehicle**: Base class for all vehicles with position, speed, and acceleration properties
- **car**: Concrete vehicle type inheriting from base vehicle class
- **bike**: Concrete vehicle type inheriting from base vehicle class
- **truck**: Concrete vehicle type inheriting from base vehicle class
- **road**: Manages a collection of vehicles and road properties
- **Telemetry**: Singleton class for logging vehicle telemetry data

### Threading

- **Thread 1**: Vehicle density adjustment (changes every 5 seconds)
- **Thread 2**: Telemetry logging (records data every 500ms)
- **Main Thread**: Vehicle simulation and visualization

## Configuration

Edit `src/traffic_sim.cpp` to modify:
- Road length
- Initial vehicle density
- Vehicle speed parameters
- Telemetry logging interval
- Density change intervals

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

Ganeshkumar Naik
