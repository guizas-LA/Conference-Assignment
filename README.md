# DA-Proj1


## Project Structure
```text
proj/
├── dataset/                  # Input datasets 
├── header/                   # Header files
├── src/                      # Implementation files
├── build/                    # CMake build output
├── build/                    # CMake build output
├── CMakeLists.txt            # Build configuration
├── README.md                 # Project overview
└── .gitignore                # Git ignore rules
```

## Batch Mode

The program supports a batch mode to process a dataset without opening the interactive menu.

### Build

```bash
cmake -S . -B build
cmake --build build
```

### Run

```bash
./build/project -b <input_file> <output_file>
```

### Example:

```bash
./build/project -b dataset1.csv output_dataset1.csv
```

The batch mode writes all generated results to the single output file passed in the command line, including the risk analysis when it is enabled in the dataset control section.

Control flags are read from the input dataset:

- `GenerateAssignments`: if enabled, the assignment results are written to the output file.
- `RiskAnalysis`: if enabled, the risk analysis is appended to the same output file.

The output file is created in `dataset/output/`.
