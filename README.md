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

## Datasets

| File | Scenario |
|---|---|
| `dataset1.csv` | Example dataset provided in the project description. |
| `dataset2.csv` | Only primary submission domains are considered. All submissions can receive the required number of reviews, and extra reviewer capacity is available. |
| `dataset3.csv` | Same as `dataset2.csv`, with risk analysis enabled. No reviewers are risky. |
| `dataset4.csv` | Only primary submission domains are considered. All submissions receive exactly the required number of reviews, with no extra reviewer capacity. |
| `dataset5.csv` | Same as `dataset4.csv`, with risk analysis enabled. All reviewers are risky. |
| `dataset6.csv` | Only primary submission domains are considered. One submission cannot receive the minimum required number of reviews. |
| `dataset7.csv` | Only primary submission domains are considered. Multiple submissions cannot receive the required number of reviews because there are not enough eligible reviewers across several topics. |
| `dataset8.csv` | Only primary submission domains are considered. All submissions can be fully assigned, with no extra reviewer capacity available. |
| `dataset9.csv` | Same as `dataset8.csv`, with risk analysis enabled. Only reviewers 7 and 8 are risky. |
| `dataset10.csv` | Both primary and secondary submission domains are considered. All submissions can receive the required number of reviews, and extra reviewer capacity is available. |
| `dataset11.csv` | Same as `dataset10.csv`, with risk analysis enabled. No reviewers are risky. |
| `dataset12.csv` | Both primary and secondary submission domains are considered. A valid assignment requires choosing between primary and secondary domains. |
| `dataset13.csv` | Same as `dataset12.csv`, with risk analysis enabled. All reviewers are risky. |
| `dataset14.csv` | Both primary and secondary submission domains are considered, with risk analysis enabled. A valid assignment requires using both domains, and no reviewers are risky. |
