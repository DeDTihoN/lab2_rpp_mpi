# MPI Floyd–Warshall

All-pairs shortest paths on a weighted graph, implemented twice — once sequentially and
once distributed with MPI — so the two can be measured against each other as the number
of processes grows.

## Layout

| Path | What it does |
|---|---|
| `generate_input.cpp` | Generates random weight matrices from a vertex count, fixed seed for reproducibility |
| `floyd_seq.cpp` | Sequential Floyd–Warshall |
| `floyd_mpi.cpp` | Distributed version — the matrix is split by rows across ranks, the pivot row is broadcast each iteration |
| `run_tests_mpi.bat` | Builds everything, generates inputs, runs both versions at several process counts, writes timing summaries |

## Building

```bash
mpic++ -O2 floyd_mpi.cpp -o floyd_mpi
g++    -O2 floyd_seq.cpp -o floyd_seq
g++    -O2 generate_input.cpp -o generate_input

./generate_input 1000 > input.txt
mpirun -np 4 ./floyd_mpi < input.txt
```

Course project on parallel and distributed computing, Taras Shevchenko National
University of Kyiv.
