#include <iostream>
#include <fstream>
#include <string>
#define OUTPUT_FILE_NAME "output.txt"

using namespace std;

// const char alphabet[] = "abcdefghijklmnopqrstuvwxyz .,;-'";
char originAlphabet[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',' ','.',',',';','-','\'','\0'};
char cryptoAlphabet[] = {'m','t','u','f','v','w','z',' ','.','q','c','d','e','b','j','h','r','i','k','y','x','l','n','a',',','g','\'','o','p','-','s',';','\0' };
const int alphabetLength = sizeof(originAlphabet) / sizeof(originAlphabet[0]);


string encrypt(string text) {
    string result = "";
    
    for (char c : text) {
        bool found = false;
        for (int i = 0; originAlphabet[i] != '\0'; i++) {
            if (c == originAlphabet[i]) {
                result += cryptoAlphabet[i];
                found = true;
                break;
            }
        }
        if (!found) {
            result += c;
        }
    }

    return result;
}


string decrypt(string text, bool isStrict = false, bool highLighSpaces = false, bool isUpperView = true) {
    string result = "";

    for (char c : text) {
        bool found = false, exist = false;
        for (int i = 0; cryptoAlphabet[i] != '\0'; i++) {
            if (c == cryptoAlphabet[i]) {
                result += originAlphabet[i];
                found = true;
                break;
            }
        }
        if (!found) {
            if (highLighSpaces && c == ' ')
            {
				result += '_'; // highlight spaces
				continue;
            }
            if (isStrict) {
                for (int i = 0; originAlphabet[i] != '\0'; i++) {
                    if (c == originAlphabet[i]) {
                        exist = true;
                        break;
                    }
                }
                if (exist) {
                    result += '?'; // unknown character
                    continue;
                }
            }
            if (isUpperView)
                result += toupper((unsigned char)c);
            else
				result += c;
        }
    }

    return result;
}


int main()
{
    int choice;

    cout << "=== Direct substitution cipher ===" << endl;
    cout << "1. Encryption" << endl;
    cout << "2. Decryption" << endl;
    cout << "Choose an option (1 or 2): ";
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        // Encoding
        string plaintextPath;

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

        for (char& c : plaintext) {
            c = tolower((unsigned char)c);
        }

        string encrypted = encrypt(plaintext);

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
        // Decrypting
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

        for (char& c : ciphertext) {
            c = tolower((unsigned char)c);
        }

        string decrypted = decrypt(ciphertext);

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
    else {
        cout << "Invalid choice!" << endl;
    }

    return 0;
}
