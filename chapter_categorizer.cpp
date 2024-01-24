
#include "chapter_categorizer.h"

using namespace std;

struct Chapter {
    int number;
    std::vector<std::string> content;
};

auto ReadFile = [](string filename) -> vector<string> {
    ifstream myfile(filename);
    vector<string> fileData;
    string line;

    /*if(!myfile.is_open())
    {
        return nullptr;
    }*/

    while (getline(myfile, line)) {
        fileData.push_back(line);
    }

    myfile.close();
    return fileData;
};


auto TokenizeString = [](const vector<string> &inputStrings) -> vector<string> {
    vector<string> tokenizedWords;

    for (const auto &line: inputStrings) {
        regex wordRegex("\\b\\w+\\b");
        sregex_iterator iter(line.begin(), line.end(), wordRegex);
        sregex_iterator end;

        while (iter != end) {
            tokenizedWords.push_back(iter->str());
            ++iter;
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

auto CountWordOccurrences = [](const vector<string> &wordList) -> map<string, int> {
    map<string, int> wordCount;

    auto MapFunction = [](const string &word) {
        return make_pair(word, 1);
    };

    auto ReduceFunction = [](int accumulator, const pair<string, int> &wordPair) {
        return accumulator + wordPair.second;
    };

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

// Function to split the book into chapters
auto SplitIntoChapters = [](const vector<string> &bookContent) -> vector<Chapter> {
    vector<Chapter> chapters;

    Chapter currentChapter;
    bool inChapter = false;
    int cumulativeChapterCount = 0;

    for (const auto &line : bookContent) {
        if (regex_search(line, regex("^CHAPTER \\d+"))) {
            if (inChapter) {
                chapters.push_back(currentChapter);
            }

            cumulativeChapterCount++;
            currentChapter = {cumulativeChapterCount, {}};
            inChapter = true;
        } else {
            if (inChapter) {
                currentChapter.content.push_back(line);
            }
        }
    }

    if (inChapter && !currentChapter.content.empty()) {
        chapters.push_back(currentChapter);
    }

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

    vector<Chapter> chapters = SplitIntoChapters(wordList);

    vector<string> chapterCategories(chapters.size());
    transform(chapters.begin(), chapters.end(), chapterCategories.begin(),
              [&](const Chapter& chapter) { return ProcessChapter(chapter, peaceList, warList); });

    PrintChapterCategories(chapters, chapterCategories);

    return 0;
}