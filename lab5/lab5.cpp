#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include "auxiliary.h"

using namespace std;

#define OUTPUT_FILE_NAME "output.txt"
#define KEY_FILE_NAME "key.txt"

const string alphabet = "abcdefghijklmnopqrstuvwxyz .,;-'";
const int ALPHABET_SIZE = alphabet.length();

vector<vector<int>> parseKeyMatrix(const string& keyText, int& n);
string encrypt(const string& text, const vector<vector<int>>& key);
string decrypt(const string& text, const vector<vector<int>>& key);

class HillCipher {
private:
    std::vector<std::vector<int>> keyMatrix;
    std::vector<std::vector<int>> inverseKeyMatrix;
    int matrixSize;
    int modValue;

    // Extended Euclidean algorithm for finding modular inverse
    int modInverse(int a, int m) {
        a = a % m;
        for (int x = 1; x < m; x++) {
            if ((a * x) % m == 1) {
                return x;
            }
        }
        throw std::runtime_error("Inverse element does not exist");
    }

    // Calculate matrix determinant
    int determinant(const std::vector<std::vector<int>>& matrix, int n) {
        if (n == 1) {
            return matrix[0][0];
        }
        if (n == 2) {
            return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
        }

        int det = 0;
        for (int p = 0; p < n; p++) {
            std::vector<std::vector<int>> submatrix(n - 1, std::vector<int>(n - 1));
            for (int i = 1; i < n; i++) {
                int col = 0;
                for (int j = 0; j < n; j++) {
                    if (j == p) continue;
                    submatrix[i - 1][col] = matrix[i][j];
                    col++;
                }
            }
            det += (p % 2 == 0 ? 1 : -1) * matrix[0][p] * determinant(submatrix, n - 1);
        }
        return det;
    }

    // Matrix transposition
    std::vector<std::vector<int>> transpose(const std::vector<std::vector<int>>& matrix) {
        int n = matrix.size();
        std::vector<std::vector<int>> result(n, std::vector<int>(n));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                result[j][i] = matrix[i][j];
            }
        }
        return result;
    }

    // Calculate adjugate matrix
    std::vector<std::vector<int>> adjugate(const std::vector<std::vector<int>>& matrix) {
        int n = matrix.size();
        std::vector<std::vector<int>> adj(n, std::vector<int>(n));

        if (n == 1) {
            adj[0][0] = 1;
            return adj;
        }

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                std::vector<std::vector<int>> submatrix(n - 1, std::vector<int>(n - 1));
                for (int x = 0; x < n; x++) {
                    for (int y = 0; y < n; y++) {
                        if (x != i && y != j) {
                            int subX = x < i ? x : x - 1;
                            int subY = y < j ? y : y - 1;
                            submatrix[subX][subY] = matrix[x][y];
                        }
                    }
                }
                adj[j][i] = ((i + j) % 2 == 0 ? 1 : -1) * determinant(submatrix, n - 1);
            }
        }
        return adj;
    }

    // Calculate inverse matrix modulo
    void calculateInverseMatrix() {
        int n = matrixSize;
        int det = determinant(keyMatrix, n);
        det = det % modValue;
        if (det < 0) det += modValue;

        // Check if determinant is coprime with modValue
        if (det == 0 || gcd(det, modValue) != 1) {
            throw std::runtime_error("Key matrix is not invertible for this alphabet");
        }

        int detInverse = modInverse(det, modValue);

        std::vector<std::vector<int>> adj = adjugate(keyMatrix);

        inverseKeyMatrix.resize(n, std::vector<int>(n));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                inverseKeyMatrix[i][j] = (adj[i][j] * detInverse) % modValue;
                if (inverseKeyMatrix[i][j] < 0) {
                    inverseKeyMatrix[i][j] += modValue;
                }
            }
        }
    }

    // GCD for invertibility check
    int gcd(int a, int b) {
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

    // Convert text to numerical vector (skip characters not in alphabet)
    std::vector<int> textToVector(const std::string& text, std::vector<int>& validIndices) {
        std::vector<int> result;
        validIndices.clear();

        for (size_t i = 0; i < text.length(); i++) {
            char c = text[i];
            size_t pos = alphabet.find(c);
            if (pos != std::string::npos) {
                result.push_back(static_cast<int>(pos));
                validIndices.push_back(static_cast<int>(i));
            }
        }
        return result;
    }

    // Matrix-vector multiplication
    std::vector<int> multiplyMatrixVector(const std::vector<std::vector<int>>& matrix,
        const std::vector<int>& vector) {
        int n = matrix.size();
        std::vector<int> result(n, 0);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                result[i] += matrix[i][j] * vector[j];
            }
            result[i] = result[i] % modValue;
            if (result[i] < 0) result[i] += modValue;
        }
        return result;
    }

public:
    HillCipher(const std::vector<std::vector<int>>& key, int mod = ALPHABET_SIZE)
        : keyMatrix(key), matrixSize(key.size()), modValue(mod) {
        calculateInverseMatrix();
    }

    // Encryption with preservation of non-alphabet characters
    std::string encrypt(const std::string& plaintext) {
        std::vector<int> validIndices;
        std::vector<int> textVector = textToVector(plaintext, validIndices);

        // If no characters to encrypt
        if (textVector.empty()) {
            return plaintext;
        }

        // Add padding if needed
        size_t originalSize = textVector.size();
        while (textVector.size() % matrixSize != 0) {
            textVector.push_back(0); // 'a' as padding
        }

        std::vector<int> encryptedVector;

        for (size_t i = 0; i < textVector.size(); i += matrixSize) {
            std::vector<int> block(textVector.begin() + i,
                textVector.begin() + i + matrixSize);
            std::vector<int> encryptedBlock = multiplyMatrixVector(keyMatrix, block);
            encryptedVector.insert(encryptedVector.end(),
                encryptedBlock.begin(), encryptedBlock.end());
        }

        // Remove padding from encrypted vector
        if (encryptedVector.size() > originalSize) {
            encryptedVector.resize(originalSize);
        }

        // Reconstruct text with position preservation
        std::string result = plaintext;
        size_t encryptedIndex = 0;

        for (size_t i = 0; i < validIndices.size() && encryptedIndex < encryptedVector.size(); i++) {
            if (encryptedIndex < encryptedVector.size()) {
                size_t pos = static_cast<size_t>(validIndices[i]);
                if (pos < result.length()) {
                    result[pos] = alphabet[encryptedVector[encryptedIndex]];
                    encryptedIndex++;
                }
            }
        }

        return result;
    }

    // Decryption with preservation of non-alphabet characters
    std::string decrypt(const std::string& ciphertext) {
        std::vector<int> validIndices;
        std::vector<int> textVector = textToVector(ciphertext, validIndices);

        // If no characters to decrypt
        if (textVector.empty()) {
            return ciphertext;
        }

        // Add padding if needed for decryption
        size_t originalSize = textVector.size();
        while (textVector.size() % matrixSize != 0) {
            textVector.push_back(0); // 'a' as padding
        }

        std::vector<int> decryptedVector;

        for (size_t i = 0; i < textVector.size(); i += matrixSize) {
            std::vector<int> block(textVector.begin() + i,
                textVector.begin() + i + matrixSize);
            std::vector<int> decryptedBlock = multiplyMatrixVector(inverseKeyMatrix, block);
            decryptedVector.insert(decryptedVector.end(),
                decryptedBlock.begin(), decryptedBlock.end());
        }

        // Remove padding from decrypted vector
        if (decryptedVector.size() > originalSize) {
            decryptedVector.resize(originalSize);
        }

        // Reconstruct text with position preservation
        std::string result = ciphertext;
        size_t decryptedIndex = 0;

        for (size_t i = 0; i < validIndices.size() && decryptedIndex < decryptedVector.size(); i++) {
            if (decryptedIndex < decryptedVector.size()) {
                size_t pos = static_cast<size_t>(validIndices[i]);
                if (pos < result.length()) {
                    result[pos] = alphabet[decryptedVector[decryptedIndex]];
                    decryptedIndex++;
                }
            }
        }

        return result;
    }
};

// Function to normalize key matrix (mod ALPHABET_SIZE)
vector<vector<int>> normalizeKeyMatrix(const vector<vector<int>>& keyMatrix) {
    vector<vector<int>> normalized = keyMatrix;
    for (auto& row : normalized) {
        for (auto& val : row) {
            val = val % ALPHABET_SIZE;
            if (val < 0) val += ALPHABET_SIZE;
        }
    }
    return normalized;
}

vector<vector<int>> parseKeyMatrix(const string& keyText, int& n) {
    vector<vector<int>> keyMatrix;
    stringstream ss(keyText);
    string line;
    int rowCount = 0;
    int expectedCols = -1;

    while (getline(ss, line)) {
        if (line.empty()) continue;
        stringstream lineStream(line);
        vector<int> row;
        string token;

        while (lineStream >> token) {
            try {
                int value = stoi(token);
                row.push_back(value);
            }
            catch (...) {
                throw runtime_error("Invalid value in key matrix: '" + token + "'");
            }
        }

        if (expectedCols == -1) expectedCols = row.size();
        else if (row.size() != expectedCols)
            throw runtime_error("Non-square key matrix: inconsistent number of columns");

        keyMatrix.push_back(row);
        rowCount++;
    }

    if (rowCount != expectedCols)
        throw runtime_error("Non-square key matrix: number of rows != number of columns");

    n = rowCount;

    // Normalize key matrix
    return normalizeKeyMatrix(keyMatrix);
}

string encrypt(const string& text, const vector<vector<int>>& key) {
    try {
        HillCipher cipher(key, ALPHABET_SIZE);
        return cipher.encrypt(text);
    }
    catch (const exception& e) {
        cerr << "Encryption error: " << e.what() << endl;
        return text; // Return original text in case of error
    }
}

string decrypt(const string& text, const vector<vector<int>>& key) {
    try {
        HillCipher cipher(key, ALPHABET_SIZE);
        return cipher.decrypt(text);
    }
    catch (const exception& e) {
        cerr << "Decryption error: " << e.what() << endl;
        return text; // Return original text in case of error
    }
}

int main()
{
    int n;
    int choice;

    cout << "=+=+= Hill cipher =+=+=" << endl;

    // --- --- --- KEY MATRIX --- --- ---
    cout << "--- Key matrix ---" << endl;
    cout << "Getting key from file '" << KEY_FILE_NAME << "'...\n" << endl;

    // Check if the key file is empty
    string keyText = readFileContent(KEY_FILE_NAME);

    if (keyText == "") {
        // If the key file is empty, fill it
        cout << "Key file '" << KEY_FILE_NAME << "' is empty!\n" << endl;

        cout << "Enter the Matrix key size N: ";
        cin >> n;
        cin.ignore();

        cout << "1. Fill randomly" << endl;
        cout << "2. Manual input" << endl;
        cout << "Choose an option (1 or 2): ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            int r;
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    r = rand() % ALPHABET_SIZE;
                    keyText += to_string(r) + "\t";
                }
                keyText += "\n";
            }
            cout << "\nGenerated key matrix:\n" << keyText << endl;
        }
        else if (choice == 2) {
            int v;
            cout << "Enter the key matrix values:" << endl;
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    cout << "K[" << i << "][" << j << "]: ";
                    cin >> v;
                    cin.ignore();
                    keyText += to_string(v) + "\t";
                }
                keyText += "\n";
            }
            cout << "\nEntered key matrix:\n" << keyText << endl;
        }
        else {
            cerr << "Invalid option!" << endl;
            return 1;
        }

        writeFileContent(KEY_FILE_NAME, keyText);
        cout << "Key matrix saved to file '" << KEY_FILE_NAME << "'." << endl << endl;
    }

    // Display key matrix
    cout << KEY_FILE_NAME << ":\n" << keyText << endl;

    // Parse key matrix
    vector<vector<int>> keyMatrix;

    try {
        keyMatrix = parseKeyMatrix(keyText, n);
    }
    catch (const runtime_error& e) {
        cerr << "Error parsing key matrix: " << e.what() << endl;
        return 1;
    }

    // Display parsed matrix
    cout << "Parsed key matrix:" << endl;
    for (auto& row : keyMatrix) {
        for (auto val : row) {
            cout << val << "\t";
        }
        cout << endl;
    }
    cout << "--- --- ------ ---\n" << endl;

    // --- --- --- CIPHER --- --- ---
    cout << "--- Cipher ---" << endl;
    cout << "1. Encryption" << endl;
    cout << "2. Decryption" << endl;
    cout << "Choose an option (1 or 2): ";
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        // --- Encrypting ---
        // Get the plaintext
        string plaintextPath;

        cout << "\nEnter the path to the plaintext file: ";
        getline(cin, plaintextPath);

        string plaintext = readFileContent(plaintextPath);

        cout << "\nPlaintext read from file:\n" << plaintext << endl << endl;

        toLowerCase(plaintext);

        // Encrypt
        string encrypted = encrypt(plaintext, keyMatrix);
        toUpperCase(encrypted);

        cout << "\nEncrypted text: \n" << encrypted << endl;

        // Save result
        writeFileContent(OUTPUT_FILE_NAME, encrypted);
    }
    else if (choice == 2) {
        // --- Decrypting ---
        // Get the plaintext
        string ciphertextPath;

        cout << "\nEnter the path to the ciphertext file: ";
        getline(cin, ciphertextPath);

        string ciphertext = readFileContent(ciphertextPath);

        cout << "\nCiphertext read from file:\n" << ciphertext << endl << endl;

        toLowerCase(ciphertext);

        // Decrypt
        string decrypted = decrypt(ciphertext, keyMatrix);
        toUpperCase(decrypted);

        cout << "\nDecrypted text: \n" << decrypted << endl;

        // Save result
        writeFileContent(OUTPUT_FILE_NAME, decrypted);
    }
    else {
        cerr << "Invalid option!" << endl;
        return 1;
    }
    cout << "--- ------- ---" << endl;

    return 0;
}