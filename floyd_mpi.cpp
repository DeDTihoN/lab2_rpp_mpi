#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <chrono>
using namespace std;

const int INF = 1000000000;

// Function to load matrix from file (executed only by rank 0)
bool loadMatrix(const string &filename, vector<vector<int>> &matrix, int &n) {
    ifstream in(filename);
    if (!in) {
        cerr << "Cannot open file: " << filename << endl;
        return false;
    }
    in >> n;
    matrix.resize(n, vector<int>(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            in >> matrix[i][j];
    in.close();
    return true;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n;
    vector<vector<int>> fullMatrix;
    double elapsed = 0.0;

    // Process 0 loads the matrix
    if (rank == 0) {
        if (argc != 2) {
            cerr << "Usage: " << argv[0] << " <input_file>\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        string input_file = argv[1];
        if (!loadMatrix(input_file, fullMatrix, n))
            MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Broadcast matrix size to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate distribution of rows for each process
    int rows_per_proc = n / size;
    int remainder = n % size;
    int local_rows = (rank < remainder) ? rows_per_proc + 1 : rows_per_proc;
    int start_row = (rank < remainder) ? rank * local_rows
                                       : remainder * (rows_per_proc + 1) + (rank - remainder) * rows_per_proc;

    // Allocate local matrix block (local_rows x n)
    vector<int> localData(local_rows * n);

    // Prepare arrays for MPI_Scatterv
    vector<int> sendcounts;
    vector<int> displs;
    if (rank == 0) {
        sendcounts.resize(size);
        displs.resize(size);
        int offset = 0;
        for (int i = 0; i < size; i++) {
            int rows = (i < remainder) ? rows_per_proc + 1 : rows_per_proc;
            sendcounts[i] = rows * n;
            displs[i] = offset;
            offset += rows * n;
        }
    }

    // Flatten the full matrix into a 1D vector (only on rank 0)
    vector<int> flatMatrix;
    if (rank == 0) {
        flatMatrix.resize(n * n);
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                flatMatrix[i * n + j] = fullMatrix[i][j];
            }
        }
    }

    // Scatter the matrix rows to all processes
    MPI_Scatterv(rank == 0 ? flatMatrix.data() : nullptr,
                 sendcounts.data(), displs.data(), MPI_INT,
                 localData.data(), local_rows * n, MPI_INT,
                 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    auto startTime = chrono::high_resolution_clock::now();

    // Main loop: MPI-based Floyd–Warshall algorithm
    for (int k = 0; k < n; k++) {
        vector<int> k_row(n);
        // If k is in local block, copy corresponding row
        if (k >= start_row && k < start_row + local_rows) {
            int local_index = k - start_row;
            for (int j = 0; j < n; j++){
                k_row[j] = localData[local_index * n + j];
            }
        }
        // Determine root process for row k
        int root;
        if (k < remainder * (rows_per_proc + 1))
            root = k / (rows_per_proc + 1);
        else
            root = remainder + (k - remainder * (rows_per_proc + 1)) / rows_per_proc;

        // Broadcast k_row from the computed root
        MPI_Bcast(k_row.data(), n, MPI_INT, root, MPI_COMM_WORLD);

        // Update local part of matrix
        for (int i = 0; i < local_rows; i++) {
            for (int j = 0; j < n; j++) {
                int candidate = localData[i * n + k] + k_row[j];
                if (candidate < localData[i * n + j])
                    localData[i * n + j] = candidate;
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    auto endTime = chrono::high_resolution_clock::now();
    elapsed = chrono::duration<double>(endTime - startTime).count();

    // Gather the updated data from all processes to rank 0
    vector<int> result;
    if (rank == 0) {
        result.resize(n * n);
    }
    MPI_Gatherv(localData.data(), local_rows * n, MPI_INT,
                result.data(), sendcounts.data(), displs.data(), MPI_INT,
                0, MPI_COMM_WORLD);

    if (rank == 0) {
        ofstream out("floyd_mpi_result.txt");
        if (!out) {
            cerr << "Cannot open output file for MPI result." << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        out << n << "\n";
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                out << result[i * n + j] << " ";
            }
            out << "\n";
        }
        out << elapsed << "\n";
        out.close();

        ofstream summary("floyd_mpi_summary.txt");
        if (summary) {
            summary << "Результати для " << n << " вершин: MPI виконання (на "
                    << size << " процесах) зайняло " << elapsed << " секунд.\n";
            summary.close();
        }
        cout << "MPI execution time: " << elapsed << " seconds." << endl;
    }

    MPI_Finalize();
    return 0;
}
