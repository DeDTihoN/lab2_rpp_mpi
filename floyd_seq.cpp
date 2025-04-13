#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
using namespace std;

const int INF = 1000000000;

int main(int argc, char* argv[]) {
    if(argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }
    string filename = argv[1];
    ifstream in(filename);
    if (!in) {
        cerr << "Cannot open file " << filename << endl;
        return 1;
    }

    int n;
    in >> n;
    // Read the matrix into a 2D vector
    vector<vector<int>> dist(n, vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            in >> dist[i][j];
    in.close();

    auto start = chrono::high_resolution_clock::now();

    // Sequential Floyd–Warshall algorithm
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                if (dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];

    auto end = chrono::high_resolution_clock::now();
    double elapsed = chrono::duration<double>(end - start).count();

    // Write the full matrix result to file
    ofstream out("floyd_seq_result.txt");
    if(!out) {
        cerr << "Cannot open output file for sequential result." << endl;
        return 1;
    }
    out << n << "\n";
    for (int i = 0; i < n; ++i){
        for (int j = 0; j < n; ++j)
            out << dist[i][j] << " ";
        out << "\n";
    }
    out << elapsed << "\n";
    out.close();

    // Write a summary file with execution time
    ofstream summary("floyd_seq_summary.txt");
    if (!summary) {
        cerr << "Cannot open summary file." << endl;
        return 1;
    }
    summary << "Результати для " << n << " вершин: Послідовне виконання зайняло "
            << elapsed << " секунд.\n";
    summary.close();

    return 0;
}
