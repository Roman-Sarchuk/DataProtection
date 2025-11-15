#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <Eigen/Dense>
#include "auxiliary.h"

using namespace Eigen;
using namespace std;

#define OUTPUT_FILE_NAME "output.txt"
#define KEY_FILE_NAME "key.txt"

const string alphabet = "abcdefghijklmnopqrstuvwxyz .,;-'";
const int alphabetSize = alphabet.length();


vector<vector<int>> parseKeyMatrix(const string& keyText, int& n);

string encrypt(const string& text, const vector<vector<int>>& key);

string decrypt(const string& text, const vector<vector<int>>& key);


int main()
{
    int n;
    int choice;

    cout << "=+=+= Hila cipher =+=+=" << endl;

    // KEY MATRIX
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
                    r = rand() % alphabetSize;
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

    cout << "--- Chipher ---" << endl;
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
        string encrypted = decrypt(ciphertext, keyMatrix);
        toUpperCase(encrypted);

        cout << "\nDecrypted text: \n" << encrypted << endl;

        // Save result
        writeFileContent(OUTPUT_FILE_NAME, encrypted);
    }
    else {
        cerr << "Invalid option!" << endl;
        return 1;
    }
    cout << "--- ------- ---" << endl;

    return 0;
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
    return keyMatrix;
}



int mod(int a, int b) {
    return (a % b + b) % b;
}


int gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b);
}


int modInverse(int a, int m) { 
    a = mod(a, m); 
    for (int x = 1; x < m; ++x) 
        if (mod(a * x, m) == 1) 
            return x; 
    return -1; 
}


void modMatrix(MatrixXi& mat, int modVal) {
    for (int i = 0; i < mat.rows(); ++i)
        for (int j = 0; j < mat.cols(); ++j)
            mat(i, j) = mod(mat(i, j), modVal);
}


bool isInAlphabet(char c) {
    return alphabet.find(tolower(c)) != string::npos;
}

int getAlphabetIndex(char c) {
    for (int i = 0; i < alphabetSize; i++) {
        if (alphabet[i] == tolower(c))
            return i;
	}
    return getAlphabetIndex('x');
}


vector<int> textToNumbers(const string& text) {
    /*
        add char idx in alphabet to nums
        or if char not in alphabet add -1
    */
    vector<int> nums;
    for (char c : text) {
        if (isInAlphabet(c)) {
            nums.push_back(getAlphabetIndex(c));
        }
        else {
            nums.push_back(getAlphabetIndex('X'));
        }
    }
    return nums;
}


string encrypt(const string& text, const vector<vector<int>>& key) {
    // init key matrix
    int n = key.size();

    MatrixXi K(n, n);

    cout << "Key matrix:" << endl;
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            K(i, j) = mod(key[i][j], alphabetSize);
            cout << K(i, j) << "\t";
        }
        cout << endl;
    }

    // transform text to numbers
    vector<int> nums = textToNumbers(text);

    while (nums.size() % n != 0)
        nums.push_back(getAlphabetIndex('X'));

    // encrypt
    string result = "";
    for (size_t i = 0; i < nums.size(); i += n) {
		// form vector
        VectorXi P(n);
        for (int j = 0; j < n; j++)
            P(j) = nums[i + j];

        // calc encrypted vector
        VectorXi C = (K * P).unaryExpr([](int x) { return mod(x, alphabetSize); });

        // add letters
        for (int j = 0; j < n; j++) {
            result += alphabet[C(j)];
        }
    }

    return result;
}


string decrypt(const string& text, const vector<vector<int>>& key) {
    // init key matrix
    int n = key.size();

    MatrixXi K_int(n, n);

    cout << "Key matrix:" << endl;
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            K_int(i, j) = mod(key[i][j], alphabetSize);
            cout << K_int(i, j) << "\t";
        }
        cout << endl;
    }

    // convert to double
    MatrixXd K = K_int.cast<double>();

    // calc determinant and its modular inverse
    int det = static_cast<int>(K.determinant() + 0, 5);
    det = mod(det, alphabetSize);

    int detInv = modInverse(det, alphabetSize);
    if (detInv == -1) {
        throw runtime_error("Matrix determinant has no modular inverse! Key is not invertible.");
    }

    // compute inverse matrix
    MatrixXd Kinv_d = K.inverse();
    MatrixXi adjK = ((Kinv_d / det).array().round()).matrix().cast<int>();

    cout << "Inverted key matrix:" << endl;
    MatrixXi Kinv(n, n);
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j) {
            Kinv(i, j) = mod(adjK(i, j) * detInv, alphabetSize);
            cout << Kinv(i, j) << "\t";
        }
        cout << endl;
    }

    // transform text to numbers
    vector<int> nums = textToNumbers(text);

    while (nums.size() % n != 0)
        nums.push_back(getAlphabetIndex('X'));

    // encrypt
    string result = "";
    for (size_t i = 0; i < nums.size(); i += n) {
        // form vector
        VectorXi C(n);
        for (int j = 0; j < n; j++) {
            C(j) = nums[i + j];

            // calc decrypted vector
            VectorXi P = (Kinv * C).unaryExpr([](int x) { return mod(x, alphabetSize); });

            // add letters
            for (int j = 0; j < n; j++) {
                result += alphabet[P(j)];
            }
        }
    }

    return result;
}
