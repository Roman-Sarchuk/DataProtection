#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cctype>

using namespace std;


#define OUTPUT_FILE_NAME "output.txt"


std::string readFileContent(const std::string& filePath);

void writeFileContent(const std::string& filePath, const std::string& content);

void toUpperCase(std::string& text);

void toLowerCase(std::string& text);



int main()
{
    std::cout << "Hello World!\n";
}



// --- auxiliary ---
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