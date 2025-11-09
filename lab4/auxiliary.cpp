#include <fstream>
#include <string>
#include <stdexcept>
#include "auxiliary.h"

using namespace std;


string readFileContent(const string& filePath) {
    ifstream file(filePath);

    if (!file.is_open())
		throw runtime_error("Cannot open file to read: " + filePath);

    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}


void writeFileContent(const string& filePath, const string& content) {
    ofstream out(filePath);
    
    if (!out.is_open())
        throw runtime_error("Cannot open file to write: " + filePath);

    out << content;
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
