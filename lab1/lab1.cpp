#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cctype>
#include <xlnt/xlnt.hpp>

#define INPUT_FILE_NAME "VT00.txt"
#define OUTPUT_FILE_NAME "occurrence.xlsx"

using namespace std;

// Функція для перевірки, чи символ є буквою або знаком пунктуації (для шифру)
bool isValidChar(char c) {
    return isalpha((unsigned char)c) || c == '.' || c == ',' || c == ';' || c == ':' || c == '-' || c == '\'';
}

// Функція для завантаження частот символів у Excel
void loadFreqToExcel(xlnt::worksheet& ws, int letterCol, int freqCol, const unordered_map<char, int>& freq) {
    int row = 1;
    for (const auto& pair : freq) {
        ws.cell(letterCol, row).value(string(1, pair.first));
        ws.cell(freqCol, row).value(pair.second);
        row++;
    }
}

// Функція для завантаження частот n-грам у Excel
void loadFreqToExcel(xlnt::worksheet& ws, int letterCol, int freqCol, const unordered_map<string, int>& freq) {
    int row = 1;
    for (const auto& pair : freq) {
        ws.cell(letterCol, row).value(pair.first);
        ws.cell(freqCol, row).value(pair.second);
        row++;
    }
}

// Функція для знаходження топ-N n-грам
vector<pair<string, int>> getTopNGrams(const unordered_map<string, int>& ngramFreq, int topN) {
    vector<pair<string, int>> ngrams(ngramFreq.begin(), ngramFreq.end());

    // Сортування за частотою (спадання)
    sort(ngrams.begin(), ngrams.end(),
        [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second;
        });

    // Повертаємо топ-N елементів
    if (ngrams.size() > topN) {
        ngrams.resize(topN);
    }

    return ngrams;
}

// Функція для виведення топ-N n-грам у консоль
void printTopNGrams(const vector<pair<string, int>>& topNGrams, const string& title) {
    cout << title << ":" << endl;
    for (size_t i = 0; i < topNGrams.size(); ++i) {
        cout << "[" << topNGrams[i].first << "]: " << topNGrams[i].second;
        if (i < topNGrams.size() - 1) cout << ", ";
        if ((i + 1) % 5 == 0) cout << endl;
    }
    cout << endl << endl;
}

int main() {
    // Отримуємо робочий зошит
    xlnt::workbook wb;
    try {
        wb.load(OUTPUT_FILE_NAME);
    }
    catch (const std::exception&) {
        cerr << "Cannot open " << OUTPUT_FILE_NAME << "! Creating new file." << endl;
        wb = xlnt::workbook();
    }
    xlnt::worksheet ws = wb.active_sheet();

    // Отримуємо текст
    ifstream file(INPUT_FILE_NAME);
    if (!file.is_open()) {
        cerr << "Cannot open " << INPUT_FILE_NAME << "!" << endl;
        return 1;
    }

    string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    // Перетворюємо на нижній регістр
    for (char& c : text) {
        c = tolower((unsigned char)c);
    }

    cout << "=== AUTOMATIC CHARACTER AND N-GRAM ANALYSIS ===" << endl << endl;

    // === АНАЛІЗ СИМВОЛІВ ===
    unordered_map<char, int> charFreq;
    for (char c : text) {
        if (isValidChar(c) || c == ' ') {
            charFreq[c]++;
        }
    }

    // Сортуємо символи за частотою
    vector<pair<char, int>> sortedChars(charFreq.begin(), charFreq.end());
    sort(sortedChars.begin(), sortedChars.end(),
        [](const pair<char, int>& a, const pair<char, int>& b) {
            return a.second > b.second;
        });

    cout << "CHARACTER FREQUENCIES:" << endl;
    for (size_t i = 0; i < sortedChars.size(); ++i) {
        if (sortedChars[i].first == ' ') {
            cout << "[space]: " << sortedChars[i].second;
        }
        else {
            cout << "[" << sortedChars[i].first << "]: " << sortedChars[i].second;
        }
        if (i < sortedChars.size() - 1) cout << ", ";
        if ((i + 1) % 8 == 0) cout << endl;
    }
    cout << endl << endl;

    // Завантажуємо частоти символів у Excel
    loadFreqToExcel(ws, 1, 2, charFreq);

    // === РОЗБІР ТЕКСТУ НА СЛОВА ===
    vector<string> words;
    string currentWord;

    for (char c : text) {
        if (isValidChar(c)) {
            currentWord += c;
        }
        else if (c == ' ' && !currentWord.empty()) {
            words.push_back(currentWord);
            currentWord.clear();
        }
    }
    // Додаємо останнє слово, якщо воно є
    if (!currentWord.empty()) {
        words.push_back(currentWord);
    }

    cout << "Total words found: " << words.size() << endl << endl;

    // === АНАЛІЗ СЛІВ РІЗНОЇ ДОВЖИНИ ===
    unordered_map<string, int> bigramWords;   // слова з 2 символів
    unordered_map<string, int> trigramWords;  // слова з 3 символів  
    unordered_map<string, int> fourgramWords; // слова з 4 символів
    unordered_map<string, int> otherWords;    // слова іншої довжини

    for (const auto& word : words) {
        int length = word.length();

        if (length == 2) {
            bigramWords[word]++;
        }
        else if (length == 3) {
            trigramWords[word]++;
        }
        else if (length == 4) {
            fourgramWords[word]++;
        }
        else {
            otherWords[word]++;
        }
    }

    // === ВИВЕДЕННЯ РЕЗУЛЬТАТІВ ===

    // Біграми (слова з 2 символів)
    vector<pair<string, int>> topBigrams = getTopNGrams(bigramWords, 20);
    printTopNGrams(topBigrams, "TOP 20 BIGRAMS (2-character words)");
    loadFreqToExcel(ws, 3, 4, unordered_map<string, int>(topBigrams.begin(), topBigrams.end()));

    // Триграми (слова з 3 символів)
    vector<pair<string, int>> topTrigrams = getTopNGrams(trigramWords, 20);
    printTopNGrams(topTrigrams, "TOP 20 TRIGRAMS (3-character words)");
    loadFreqToExcel(ws, 5, 6, unordered_map<string, int>(topTrigrams.begin(), topTrigrams.end()));

    // Чотириграми (слова з 4 символів)
    vector<pair<string, int>> topFourgrams = getTopNGrams(fourgramWords, 20);
    printTopNGrams(topFourgrams, "TOP 20 FOURGRAMS (4-character words)");
    loadFreqToExcel(ws, 7, 8, unordered_map<string, int>(topFourgrams.begin(), topFourgrams.end()));

    // Загальна статистика по словах
    cout << "WORD LENGTH STATISTICS:" << endl;
    cout << "2-character words: " << bigramWords.size() << " unique, " <<
        accumulate(bigramWords.begin(), bigramWords.end(), 0,
            [](int sum, const auto& pair) { return sum + pair.second; }) << " total" << endl;
    cout << "3-character words: " << trigramWords.size() << " unique, " <<
        accumulate(trigramWords.begin(), trigramWords.end(), 0,
            [](int sum, const auto& pair) { return sum + pair.second; }) << " total" << endl;
    cout << "4-character words: " << fourgramWords.size() << " unique, " <<
        accumulate(fourgramWords.begin(), fourgramWords.end(), 0,
            [](int sum, const auto& pair) { return sum + pair.second; }) << " total" << endl;
    cout << "Other words: " << otherWords.size() << " unique, " <<
        accumulate(otherWords.begin(), otherWords.end(), 0,
            [](int sum, const auto& pair) { return sum + pair.second; }) << " total" << endl;
    cout << endl;

    // Топ-20 всіх слів
    unordered_map<string, int> allWordFreq;
    for (const auto& word : words) {
        allWordFreq[word]++;
    }

    vector<pair<string, int>> topAllWords = getTopNGrams(allWordFreq, 20);
    cout << "TOP 20 ALL WORDS:" << endl;
    for (size_t i = 0; i < topAllWords.size(); ++i) {
        cout << "[" << topAllWords[i].first << "]: " << topAllWords[i].second;
        if (i < topAllWords.size() - 1) cout << ", ";
        if ((i + 1) % 5 == 0) cout << endl;
    }
    cout << endl;

    // Зберігаємо результати
    try {
        wb.save(OUTPUT_FILE_NAME);
        cout << "Results successfully saved to " << OUTPUT_FILE_NAME << endl;
    }
    catch (const std::exception&) {
        cerr << "Cannot save " << OUTPUT_FILE_NAME << "!" << endl;
        return 1;
    }

    return 0;
}