#include <iostream>
#include <string>
#include "auxiliary.h"

using namespace std;

#define OUTPUT_FILE_NAME "output.txt"

const string alphabet = "abcdefghijklmnopqrstuvwxyz .,;-'";
const int alphabetSize = alphabet.length();


string encrypt(const string& text, const string& key);

string decrypt(const string& text, const string& key);



int main()
{
    int choice;

    cout << "=== Vigenere cipher ===" << endl;
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

        // Get key
        string key;

        cout << "Enter the key (string): ";
        getline(cin, key);

		toLowerCase(key);

        // Encrypt
        string encrypted = encrypt(plaintext, key);
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

        toLowerCase(ciphertext);

        // Get key
        string key;

        cout << "Enter the key (string): ";
        getline(cin, key);

        toLowerCase(key);

        // Decrypt
        string encrypted = decrypt(ciphertext, key);
        toUpperCase(encrypted);

        cout << "\nDecrypted text: \n" << encrypted << endl;

        // Save result
        writeFileContent(OUTPUT_FILE_NAME, encrypted);
    }
    else {
        cerr << "Invalid option!" << endl;
        return 1;
	}

	return 0;
}


int getCharIndex(char c) {
    for (int i = 0; i < alphabetSize; i++) {
        if (alphabet[i] == c) {
            return i;
        }
    }
    return -1;
}


string vietaChiper(const string& text, const string& key, bool isEncrypting) {
    string result = "";
	int textCharIndex, keyCharIndex, resCharIndex;

    for (size_t textIndex = 0, keyIndex = 0; text[textIndex] != '\0'; textIndex++) {
        textCharIndex = getCharIndex(text[textIndex]);
        keyCharIndex = getCharIndex(key[keyIndex]);

        if (textCharIndex == -1 || keyCharIndex == -1) {
            result += text[textIndex];
            continue;
        }

        if (isEncrypting)
            resCharIndex = (textCharIndex + keyCharIndex) % alphabetSize;
        else
            resCharIndex = (textCharIndex - keyCharIndex + alphabetSize) % alphabetSize;

        result += alphabet[resCharIndex];

        keyIndex = (keyIndex + 1) % key.length();
	}

	return result;
}


string encrypt(const string& text, const string& key) {
	return vietaChiper(text, key, true);
}


string decrypt(const string& text, const string& key) {
    return vietaChiper(text, key, false);
}