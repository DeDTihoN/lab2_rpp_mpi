#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

const int INF = 1000000000;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <nodes> <output_file>\n";
        return 1;
    }
    int n = atoi(argv[1]);
    string filename = argv[2];

    // Fixed seed for reproducibility
    srand(42);

    ofstream out(filename);
    if (!out) {
        cerr << "Cannot open file " << filename << " for writing." << endl;
        return 1;
    }

    // First line: matrix size
    out << n << "\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j)
                out << 0 << " ";
            else {
                int weight = (rand() % 2000) + 1;
                out << weight << " ";
            }
        }
        out << "\n";
    }
    out.close();
    return 0;
}
