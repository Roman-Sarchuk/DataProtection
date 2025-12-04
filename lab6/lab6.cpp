#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cctype>
#include <random>
#include <chrono>
using namespace std;

#define OUTPUT_FILE_NAME "output.txt"

// 8 keys (by 1 byte)
vector<uint8_t> KEYS = { 15, 23, 71, 99, 201, 50, 77, 5 };

// Add Initialization Vector (IV)
const uint16_t INITIALIZATION_VECTOR = 0x1234; // Or generate randomly

std::string readFileContent(const std::string& filePath);
void writeFileContent(const std::string& filePath, const std::string& content);
void writeFileContent(const std::string& filePath, const std::vector<uint16_t>& content);
void toUpperCase(std::string& text);
void toLowerCase(std::string& text);

void printHexVector(const vector<uint16_t>& vec) {
    for (uint16_t b : vec) {
        cout << hex << setw(4) << setfill('0') << b << " ";
    }
    cout << endl;
}

bool is_hex_encrypted(const string& s) {
    for (char c : s) {
        if (!isxdigit(c) && !isspace(c)) return false;
    }
    return true;
}

uint8_t F(uint8_t half, uint8_t key) {
    return (half ^ key) + ((half << 1) | (half >> 7));
}

uint16_t feistel_encrypt_block(uint16_t block, const vector<uint8_t>& keys) {
    uint8_t L = block >> 8;
    uint8_t R = block & 0xFF;

    for (uint8_t key : keys) {
        uint8_t newL = R;
        uint8_t newR = L ^ F(R, key);
        L = newL;
        R = newR;
    }
    return (uint16_t(L) << 8) | R;
}

uint16_t feistel_decrypt_block(uint16_t block, const vector<uint8_t>& keys) {
    uint8_t L = block >> 8;
    uint8_t R = block & 0xFF;

    for (int i = keys.size() - 1; i >= 0; i--) {
        uint8_t newR = L;
        uint8_t newL = R ^ F(L, keys[i]);
        L = newL;
        R = newR;
    }
    return (uint16_t(L) << 8) | R;
}

// CBC mode encryption
vector<uint16_t> encrypt(const string& text, const vector<uint8_t>& keys) {
    vector<uint16_t> blocks;
    uint16_t previous = INITIALIZATION_VECTOR;

    for (size_t i = 0; i < text.size(); i += 2) {
        uint8_t c1 = text[i];
        uint8_t c2 = (i + 1 < text.size()) ? text[i + 1] : 0;
        uint16_t block = (uint16_t(c1) << 8) | c2;

        // XOR with previous ciphertext block (or IV for first block)
        uint16_t xored = block ^ previous;

        // Encrypt the XORed block
        uint16_t encrypted = feistel_encrypt_block(xored, keys);
        blocks.push_back(encrypted);

        // Update previous for next iteration
        previous = encrypted;
    }

    // Add IV as first block for decryption
    blocks.insert(blocks.begin(), INITIALIZATION_VECTOR);
    return blocks;
}

// CBC mode decryption
string decrypt(const vector<uint16_t>& blocks_with_iv, const vector<uint8_t>& keys) {
    if (blocks_with_iv.empty()) return "";

    string result;
    uint16_t previous = blocks_with_iv[0]; // First block is IV

    for (size_t i = 1; i < blocks_with_iv.size(); i++) {
        uint16_t encrypted = blocks_with_iv[i];

        // Decrypt the block
        uint16_t decrypted = feistel_decrypt_block(encrypted, keys);

        // XOR with previous ciphertext block (or IV for first data block)
        uint16_t xored = decrypted ^ previous;

        char c1 = xored >> 8;
        char c2 = xored & 0xFF;
        result.push_back(c1);
        if (c2 != '\0') result.push_back(c2);

        // Update previous for next iteration
        previous = encrypted;
    }
    return result;
}

int main() {
    int choice;
    cout << "=== Feistel cipher with CBC mode ===" << endl;
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
        toLowerCase(plaintext);

        cout << "\nPlaintext:\n" << plaintext << endl;

        // Encrypt
        vector<uint16_t> encrypted = encrypt(plaintext, KEYS);

        cout << "\nEncrypted blocks (hex) [First block is IV]:\n";
        printHexVector(encrypted);

        // Save result
        writeFileContent(OUTPUT_FILE_NAME, encrypted);

    }
    else if (choice == 2) {
        // --- Decrypting ---
        // Get the ciphertext
        string ciphertextPath;
        cout << "\nEnter the path to the ciphertext file: ";
        getline(cin, ciphertextPath);
        string ciphertext = readFileContent(ciphertextPath);

        // Validate hex data
        if (!is_hex_encrypted(ciphertext)) {
            cerr << "The ciphertext file does not contain valid hexadecimal data!" << endl;
            return 1;
        }

        // Parse hex data into blocks
        vector<uint16_t> blocks;
        stringstream ss(ciphertext);
        string hexblock;
        while (ss >> hexblock) {
            uint16_t value = stoi(hexblock, nullptr, 16);
            blocks.push_back(value);
        }

        cout << "\nCiphertext [Encrypted blocks (hex)]:\n";
        printHexVector(blocks);

        // Decrypt
        string decrypted = decrypt(blocks, KEYS);
        cout << "\nDecrypted text: \n" << decrypted << endl;

        // Save result
        writeFileContent(OUTPUT_FILE_NAME, decrypted);

    }
    else {
        cerr << "Invalid option!" << endl;
        return 1;
    }

    return 0;
}

// --- auxiliary ---
string readFileContent(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) throw runtime_error("Cannot open file to read: " + filePath);
    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

void writeFileContent(const string& filePath, const string& content) {
    ofstream out(filePath);
    if (!out.is_open()) throw runtime_error("Cannot open file to write: " + filePath);
    out << content;
}

void writeFileContent(const std::string& filePath, const std::vector<uint16_t>& blocks) {
    ofstream out(filePath);
    if (!out.is_open()) throw runtime_error("Cannot open file to write: " + filePath);
    for (uint16_t b : blocks) {
        out << hex << setw(4) << setfill('0') << b << " ";
    }
}

void toUpperCase(string& text) {
    for (char& c : text) {
        c = toupper((unsigned char)c);
    }
}

void toLowerCase(string& text) {
    for (char& c : text) {
        c = tolower((unsigned char)c);
    }
}