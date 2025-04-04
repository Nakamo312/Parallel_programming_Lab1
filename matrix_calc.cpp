#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <string> 

using namespace std;
using namespace std::chrono;

void generateRandomMatrix(vector<vector<double>>& matrix) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 100.0);

    for (auto& row : matrix) {
        for (auto& elem : row) {
            elem = dis(gen);
        }
    }
}

vector<vector<double>> readMatrixBinary(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) throw runtime_error("Cannot open file: " + filename);

    size_t rows, cols;
    file.read(reinterpret_cast<char*>(&rows), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&cols), sizeof(size_t));

    vector<vector<double>> matrix(rows, vector<double>(cols));
    for (auto& row : matrix) {
        file.read(reinterpret_cast<char*>(row.data()), cols * sizeof(double));
        if (!file) throw runtime_error("Error reading matrix data");
    }

    return matrix;
}

void writeMatrixBinary(const string& filename, const vector<vector<double>>& matrix) {
    ofstream file(filename, ios::binary);
    if (!file) throw runtime_error("Cannot create file: " + filename);

    size_t rows = matrix.size();
    size_t cols = rows > 0 ? matrix[0].size() : 0;
    file.write(reinterpret_cast<const char*>(&rows), sizeof(size_t));
    file.write(reinterpret_cast<const char*>(&cols), sizeof(size_t));

    for (const auto& row : matrix) {
        file.write(reinterpret_cast<const char*>(row.data()), row.size() * sizeof(double));
    }
}

vector<vector<double>> multiplyMatrices(const vector<vector<double>>& A,
    const vector<vector<double>>& B) {
    if (A.empty() || B.empty() || A[0].size() != B.size()) {
        throw runtime_error("Matrix dimensions mismatch for multiplication");
    }

    size_t m = A.size();
    size_t n = A[0].size();
    size_t p = B[0].size();

    vector<vector<double>> C(m, vector<double>(p, 0.0));

    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < p; ++j) {
            for (size_t k = 0; k < n; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return C;
}

int main(int argc, char* argv[]) {
    if (argc != 8 && argc != 5) {
        cerr << "Usage modes:\n"
            << "1. Generate and save matrices: " << argv[0] << " generate <rows1> <cols1> <rows2> <cols2> <file1> <file2>\n"
            << "2. Multiply matrices: " << argv[0] << " multiply <matrixA> <matrixB> <output>\n";
        return 1;
    }

    try {
        string mode(argv[1]);

        if (mode == "generate" && argc == 8) {
            int rows1 = stoi(argv[2]);
            int cols1 = stoi(argv[3]);
            int rows2 = stoi(argv[4]);
            int cols2 = stoi(argv[5]);

            vector<vector<double>> A(rows1, vector<double>(cols1));
            vector<vector<double>> B(rows2, vector<double>(cols2));

            generateRandomMatrix(A);
            generateRandomMatrix(B);

            writeMatrixBinary(argv[6], A);
            writeMatrixBinary(argv[7], B);

            cout << "Matrix A (" << rows1 << "x" << cols1 << ") saved to " << argv[6] << "\n";
            cout << "Matrix B (" << rows2 << "x" << cols2 << ") saved to " << argv[7] << "\n";
        }
        else if (mode == "multiply" && argc == 5) {
            auto A = readMatrixBinary(argv[2]);
            auto B = readMatrixBinary(argv[3]);

            auto startCompute = high_resolution_clock::now();
            auto C = multiplyMatrices(A, B);
            auto endCompute = high_resolution_clock::now();

            auto duration_ms = duration_cast<milliseconds>(endCompute - startCompute).count();
            cerr << duration_ms << endl;
            cout << "Compute time: " << duration_ms << " ms\n";

            writeMatrixBinary(argv[4], C);

            size_t dataSize = A.size() * A[0].size() * sizeof(double) +
                B.size() * B[0].size() * sizeof(double) +
                C.size() * C[0].size() * sizeof(double);

            cout << "Total data size: " << dataSize / (1024 * 1024) << " MB\n";
            cout << "GFLOPS: "
                << (2.0 * A.size() * A[0].size() * B[0].size()) /
                (duration_ms * 1e6) 
                << "\n";
        }
        else {
            cerr << "Invalid arguments. Use one of the following modes:\n"
                << "1. Generate mode: " << argv[0] << " generate <rows1> <cols1> <rows2> <cols2> <file1> <file2>\n"
                << "2. Multiply mode: " << argv[0] << " multiply <matrixA> <matrixB> <output>\n";
            return 1;
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}