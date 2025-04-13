# MPI Floyd–Warshall

This project implements the Floyd–Warshall algorithm for computing the shortest paths in a weighted graph using MPI (Message Passing Interface). The repository includes:

- **generate_input.cpp** – a tool to generate random square matrices (graph weight matrices) from a given number of vertices with a fixed seed for reproducibility.
- **floyd_seq.cpp** – the sequential version of the Floyd–Warshall algorithm.
- **floyd_mpi.cpp** – the MPI-based distributed version of the Floyd–Warshall algorithm.
- **run_tests_mpi.bat** – a Windows batch script to compile the project, generate test input files, run the sequential and MPI versions with different process counts, and output summary files with execution times.
  
## Project Structure

- **generate_input.cpp**  
  Usage: `generate_input.exe <nodes> <output_file>`  
  Generates an input matrix of size n×n with random weights.

- **floyd_seq.cpp**  
  Usage: `floyd_seq.exe <input_file>`  
  Computes the shortest paths using the sequential Floyd–Warshall algorithm and outputs the full result and a summary (execution time).

- **floyd_mpi.cpp**  
  Usage: `mpiexec -n <num_processes> floyd_mpi.exe <input_file>`  
  Implements the MPI version of the algorithm. The input matrix is distributed among processes; each process performs updates on its block. The results are gathered and merged on the root process, which writes the final output and a summary file.

- **run_tests_mpi.bat**  
  A Windows batch script to automate compiling, generating inputs (for various matrix sizes), executing both sequential and MPI versions (with different process counts), and displaying the summary results.

## Requirements

- A C++ compiler with MPI support (MS-MPI or MPICH).  
- MPI environment installed and properly configured (with necessary include and library paths).  
- (Optional) Python with `matplotlib` to generate graphs of the experimental results.

## Building

### Directly with g++ (example for MS-MPI):
```bash
g++ floyd_mpi.cpp -I"C:/Program Files (x86)/Microsoft SDKs/MPI/Include" -L"C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64" -lmsmpi -o floyd_mpi.exe
```

## Running the Application

1. Generate input file:
   ```bash
   generate_input.exe 500 input_500.txt
   ```
2. Run the sequential version:
   ```bash
   floyd_seq.exe input_500.txt
   ```
3. Run the MPI version (example for 2 processes):
   ```bash
   mpiexec -n 2 floyd_mpi.exe input_500.txt
   ```

The application produces output files:
- `floyd_seq_result.txt` and `floyd_seq_summary.txt` (for the sequential version),
- `floyd_mpi_result.txt` and `floyd_mpi_summary.txt` (for the MPI version).

## Experimental Results

Below are sample execution times for different matrix sizes and MPI process counts:

| Matrix Size   | Sequential (s) | MPI (2 processes) (s) | MPI (4 processes) (s) | MPI (8 processes) (s) |
|---------------|----------------|-----------------------|-----------------------|-----------------------|
| 500 x 500     | 2.31182        | 0.685043              | 0.417316              | 0.442266              |
| 1000 x 1000   | 20.5362        | 5.89239               | 3.40618               | 2.65747               |
| 1500 x 1500   | 62.9051        | 19.8407               | 15.8278               | 10.0239               |
