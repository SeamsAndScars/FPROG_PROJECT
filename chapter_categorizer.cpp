#include "chapter_categorizer.h"

using namespace std;

struct Chapter {
    int number;
    vector<string> content;
};

auto ReadFile = [](string filename) -> vector<string> {
    ifstream myfile(filename);
    vector<string> fileData;
    string line;

    while (getline(myfile, line)) {
        fileData.push_back(line);
    }

    myfile.close();
    return fileData;
};


auto tokenize = [](const string& input) -> vector<string> {
    vector<string> tokens;
    
    auto processToken = [&](const vector<char>& tokenChars) {
        if (!tokenChars.empty()) {
            tokens.emplace_back(tokenChars.data(), tokenChars.size());
        }
    };

    vector<char> currentToken;

    for (char ch : input) {
        if (isalnum(ch)) {
            // Add alphanumeric character to the current token
            currentToken.push_back(ch);
        } else {
            // Non-alphanumeric character found, process the current token if not empty
            processToken(currentToken);
            currentToken.clear();
        }
    }

    // Process the last token if not empty
    processToken(currentToken);

    return tokens;
};

auto TokenizeString = [](const std::vector<std::string>& inputStrings) -> std::vector<std::string> {
    std::vector<std::string> tokenizedWords;

    for (const auto& str : inputStrings) {
        std::vector<std::string> tokens = tokenize(str);
        for (const auto& token : tokens) {
            tokenizedWords.push_back(token);
        }
    }
    return tokenizedWords;
};

auto CleanseWords = [](const vector<string> &inputWords) -> vector<string> {
    vector<string> cleansedWords;

    for (const auto &word: inputWords) {
        string cleansedWord;
        for (char c: word) {
            if (isalpha(c)) { //only alphabetical chars allowed!
                cleansedWord += tolower(c); // Convert to lowercase
            }
        }
        if (!cleansedWord.empty()) {
            cleansedWords.push_back(cleansedWord);
        }
    }

    return cleansedWords;
};

auto FilterWords = [](const vector<string> &mainText, const vector<string> &filterList1, const vector<string> &filterList2) -> vector<string> {
    vector<string> filteredWords;
    set<string> filterSet1(filterList1.begin(), filterList1.end());
    set<string> filterSet2(filterList2.begin(), filterList2.end());

    copy_if(mainText.begin(), mainText.end(), back_inserter(filteredWords), [&](const string &word) {
        return filterSet1.find(word) != filterSet1.end() || filterSet2.find(word) != filterSet2.end();
    });

    return filteredWords;
};

auto MapFunction = [](const string &word) {
        return make_pair(word, 1);
    };

auto ReduceFunction = [](int accumulator, const pair<string, int> &wordPair) {
        return accumulator + wordPair.second;
    };

auto CountWordOccurrences = [](const vector<string> &wordList) -> map<string, int> {
    map<string, int> wordCount;

    vector<pair<string, int>> mappedResult(wordList.size());
    transform(wordList.begin(), wordList.end(), mappedResult.begin(), MapFunction);

    for (const auto &wordPair : mappedResult) {
        wordCount[wordPair.first] += wordPair.second;
    }

    return wordCount;
};

// Function to calculate the density of terms
auto CalculateTermDensity = [](const vector<string> &chapterContent, const map<string, int> &wordOccurrences) -> map<string, double> {
    size_t totalWordCount = chapterContent.size();

    auto calculateDensity = [&](const pair<string, int> &wordOccurrence) -> pair<string, double> {
        const string &word = wordOccurrence.first;
        int frequency = wordOccurrence.second;

        double density = (totalWordCount > 0) ? (static_cast<double>(frequency) / totalWordCount * 100.0) : 0.0;
        return make_pair(word, density);
    };

    map<string, double> termDensity;
    transform(wordOccurrences.begin(), wordOccurrences.end(), inserter(termDensity, termDensity.begin()), calculateDensity);

    return termDensity;
};

auto isChapterHeader = [](const std::string& line) {
    return line.find("CHAPTER ") == 0;
};

auto createChapter = [](int number) {
    return Chapter{number, {}};
};

auto appendLineToChapter = [](Chapter chapter, const std::string& line) {
    chapter.content.push_back(line);
    return chapter;
};

auto SplitIntoChapters = [](const std::vector<std::string>& bookContent) -> std::vector<Chapter> {
    int cumulativeChapterCount = 0;

    auto processLine = [&](std::vector<Chapter>& chapters, const std::string& line) {
        if (isChapterHeader(line)) {
            cumulativeChapterCount++;
            chapters.push_back(createChapter(cumulativeChapterCount));
        } else if (!chapters.empty()) {
            chapters.back() = appendLineToChapter(chapters.back(), line);
        }
    };

    std::vector<Chapter> chapters;
    std::for_each(bookContent.begin(), bookContent.end(), [&](const std::string& line) {
        processLine(chapters, line);
    });

    chapters.erase(std::remove_if(chapters.begin(), chapters.end(), [](const Chapter& chapter) {
        return chapter.content.empty();
    }), chapters.end());

    return chapters;
};

// Function to process a single chapter
string ProcessChapter(const Chapter& chapter, const vector<string>& peaceList, const vector<string>& warList) {
    vector<string> tokenizedWords = TokenizeString(chapter.content);
    vector<string> cleansedWords = CleanseWords(tokenizedWords);
    vector<string> filteredWords = FilterWords(cleansedWords, peaceList, warList);

    map<string, int> wordOccurrences = CountWordOccurrences(filteredWords);
    map<string, double> termDensity = CalculateTermDensity(filteredWords, wordOccurrences);

    size_t totalChapterWordCount = chapter.content.size();
    double warDensity = (totalChapterWordCount > 0) ? termDensity["war"] : 0.0;
    double peaceDensity = (totalChapterWordCount > 0) ? termDensity["peace"] : 0.0;

    return (warDensity > peaceDensity) ? "War-related" : "Peace-related";
}

// Function to print chapter categories
void PrintChapterCategories(const vector<Chapter>& chapters, const vector<string>& chapterCategories) {
    for (size_t i = 0; i < chapters.size(); ++i) {
        cout << "Chapter " << chapters[i].number << ": " << chapterCategories[i] << endl;
    }
}


int main() {
    vector<string> wordList = ReadFile(book);
    vector<string> peaceList = ReadFile(peace_terms);
    vector<string> warList = ReadFile(war_terms);
    
    //PRINT STUFF
    /*
    vector<string> tokenizedWords = TokenizeStringNoRegex(wordList);
    std::string fileName = "Testfile3.txt";

    std::ofstream outputFile(fileName);
    if (outputFile.is_open()) {
        // Iterate through the vector and write each string to the file
        for (const std::string& str : tokenizedWords) {
            outputFile << str << std::endl;
        }

        // Close the file stream
        outputFile.close();

        std::cout << "Vector of strings has been written to " << fileName << std::endl;
    } else {
        std::cerr << "Error opening the file: " << fileName << std::endl;
    }
    */
    //END OF PRINT

    vector<Chapter> chapters = SplitIntoChapters(wordList);

    vector<string> chapterCategories(chapters.size());
    transform(chapters.begin(), chapters.end(), chapterCategories.begin(),
              [&](const Chapter& chapter) { return ProcessChapter(chapter, peaceList, warList); });

    PrintChapterCategories(chapters, chapterCategories);

    return 0;
}