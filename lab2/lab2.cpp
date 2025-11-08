#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;
#define OUTPUT_FILE_NAME "output.txt"


const string alphabet = "abcdefghijklmnopqrstuvwxyz .,;-'";
const int alphabetSize = alphabet.length();


string encrypt(string text, int key) {
    for (char& c : text) {
        c = tolower((unsigned char)c);
    }

    string result = "";

    for (char c : text) {
        size_t pos = alphabet.find(c);

        if (pos != string::npos) {
            int newPos = (pos + key) % alphabetSize;
            result += alphabet[newPos];
        }
        else {
            result += c;
        }
    }

    return result;
}


string decrypt(string text, int key) {
    for (char& c : text) {
        c = tolower((unsigned char)c);
    }

    string result = "";

    for (char c : text) {
        size_t pos = alphabet.find(c);

        if (pos != string::npos) {
            int newPos = (pos - key + alphabetSize) % alphabetSize;
            result += alphabet[newPos];
        }
        else {
            result += c;
        }
    }

    return result;
}


void bruteForce(string ciphertext) {
    cout << "\n=== All possible decryption variants ===" << endl;
    cout << "Review the results and choose the one that makes sense:\n" << endl;

    for (int key = 1; key < alphabetSize; key++) {
        string decrypted = decrypt(ciphertext, key);
        cout << "Key " << key << ": \n" << decrypted << endl;
    }
}


int main() {
    int choice;

    cout << "=== Caesar Cipher ===" << endl;
    cout << "1. Encryption" << endl;
    cout << "2. Decryption (with known key)" << endl;
    cout << "3. Decryption (without key - brute force)" << endl;
    cout << "Choose an option (1, 2 or 3): ";
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        // Encoding
        string plaintextPath;
        int key;

        cout << "\nEnter the path to the plaintext file: ";
        getline(cin, plaintextPath);

        // Get the text
        ifstream file(plaintextPath);
        if (!file.is_open()) {
            cerr << "Cannot open " << plaintextPath << "!" << endl;
            return 1;
        }

        string plaintext((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();

        cout << "Enter the key (shift): ";
        cin >> key;

        string encrypted = encrypt(plaintext, key);

        cout << "\nEncrypted text: \n" << encrypted << endl;

        // Save result
        ofstream out(OUTPUT_FILE_NAME);
        if (!out.is_open()) {
            cerr << "Cannot open " << OUTPUT_FILE_NAME << " for writing!" << endl;
            return 1;
        }
        out << encrypted;
        out.close();
    }
    else if (choice == 2) {
        // Decoding
        string ciphertextPath;
        int key;

        cout << "\nEnter the path to the encrypted file: ";
        getline(cin, ciphertextPath);

        // Get the text
        ifstream file(ciphertextPath);
        if (!file.is_open()) {
            cerr << "Cannot open " << ciphertextPath << "!" << endl;
            return 1;
        }

        string ciphertext((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();

        cout << "Enter the key (shift): ";
        cin >> key;

        string decrypted = decrypt(ciphertext, key);

        cout << "\nDecrypted text: \n" << decrypted << endl;

        // Save result
        ofstream out(OUTPUT_FILE_NAME);
        if (!out.is_open()) {
            cerr << "Cannot open " << OUTPUT_FILE_NAME << " for writing!" << endl;
            return 1;
        }
        out << decrypted;
        out.close();
    }
    else if (choice == 3) {
        // Brute force
        string ciphertextPath;

        cout << "\nEnter the path to the encrypted file: ";
        getline(cin, ciphertextPath);

        // Get the text
        ifstream file(ciphertextPath);
        if (!file.is_open()) {
            cerr << "Cannot open " << ciphertextPath << "!" << endl;
            return 1;
        }

        string ciphertext((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();

        bruteForce(ciphertext);

        cout << "\nEnter the key number that gave the correct result: ";
        int correctKey;
        cin >> correctKey;

        string decrypted = decrypt(ciphertext, correctKey);

        cout << "\nFinal result: \n" << decrypted << endl;

        // Save result
        ofstream out(OUTPUT_FILE_NAME);
        if (!out.is_open()) {
            cerr << "Cannot open " << OUTPUT_FILE_NAME << " for writing!" << endl;
            return 1;
        }
        out << decrypted;
        out.close();
    }
    else {
        cout << "Invalid choice!" << endl;
    }

    return 0;
}