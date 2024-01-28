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
        if (isalpha(ch)) {
            // Add alphanumeric character to the current token
            currentToken.push_back(tolower(ch));
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
    vector<string> tokenizedWords;

    for_each(inputStrings.begin(), inputStrings.end(), [&tokenizedWords](const auto& str) {
        vector<string> tokens = tokenize(str);
        for_each(tokens.begin(), tokens.end(), [&tokenizedWords](const auto& token) {
            tokenizedWords.push_back(token);
        });
    });
    return tokenizedWords;
};

auto FilterWords = [](const vector<string> &mainText, const vector<string> &filterList) -> vector<string> {
    vector<string> filteredWords;
    set<string> filterSet(filterList.begin(), filterList.end());

    copy_if(mainText.begin(), mainText.end(), back_inserter(filteredWords), [&](const string &word) {
        return filterSet.find(word) != filterSet.end();
    });

    return filteredWords;
};

auto MapFunction = [](const string &word) {
    return make_pair(word, 1);
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

auto CalcSum = [] (map<string, double> terms){
    vector<double> values;
    transform(terms.begin(), terms.end(), std::back_inserter(values),
                   [](const auto& pair) { return pair.second; });

    // Use std::accumulate to calculate the sum
    
    return accumulate(values.begin(), values.end(), 0.0);
};

// Function to process a single chapter
auto  ProcessChapter = [](const Chapter& chapter, const vector<string>& peaceList, const vector<string>& warList) ->string{
    vector<string> tokenizedWords = TokenizeString(chapter.content);
    vector<string> filteredWordsWar = FilterWords(tokenizedWords, warList);
    vector<string> filteredWordsPeace = FilterWords(tokenizedWords, peaceList);

    map<string, int> occurrencesWar = CountWordOccurrences(filteredWordsWar);
    map<string, int> occurrencesPeace = CountWordOccurrences(filteredWordsPeace);

    map<string, double> termDensityWar = CalculateTermDensity(tokenizedWords, occurrencesWar);
    map<string, double> termDensityPeace = CalculateTermDensity(tokenizedWords, occurrencesPeace);
    
    double densityWar = CalcSum(termDensityWar);
    double densityPeace = CalcSum(termDensityPeace);

    return (densityWar > densityPeace) ? "war-related" : "peace-related";
};

// Function to print chapter categories
auto PrintChapterCategories = [](const vector<Chapter>& chapters, const vector<string>& chapterCategories) {
    for (size_t i = 0; i < chapters.size(); ++i) {
        cout << "Chapter " << chapters[i].number << ": " << chapterCategories[i] << endl;
    }
};


int main() {
    vector<string> wordList = ReadFile(book);
    vector<string> peaceList = ReadFile(peace_terms);
    vector<string> warList = ReadFile(war_terms);
    
    vector<Chapter> chapters = SplitIntoChapters(wordList);

    vector<string> chapterCategories(chapters.size());
    transform(chapters.begin(), chapters.end(), chapterCategories.begin(),
              [&](const Chapter& chapter) { return ProcessChapter(chapter, peaceList, warList); });

    PrintChapterCategories(chapters, chapterCategories);

    //RUN TESTCASES
    doctest::Context().run();

    return 0;
}

TEST_CASE("tokenize input"){
    // ARRANGE
    std::string input = "Test string containing !some!? kind of punctuation.\"and special characters!";
    std::vector<std::string> expected = {"test", "string", "containing", "some", "kind", "of", "punctuation", "and", "special", "characters"};

    std::string input2 = "_-A/!1²2\"3§³4$5%6&7/{8[(9)]0=}?ß\\´`#'*+~ÄÖÜ;äöüµ:.><|^°END";
    std::vector<std::string> expected2 = {"a", "end"};

    // ACT
    std::vector<std::string> result = tokenize(input);
    std::vector<std::string> result2 = tokenize(input2);

    // ASSERT
    CHECK_EQ(result , expected);
    CHECK_EQ(result2 , expected2);
}

TEST_CASE("ReadFile Non-Existent File Test") {
    // ARRANGE
    const std::string nonExistentFile = "non_existent_file.txt";

    // ACT
    auto result = ReadFile(nonExistentFile);

    //ASSERT 
    CHECK(result.empty());
}

TEST_CASE("ReadFile Working File") {
    //ARRANGE
    string ExistentFile = "test_ReadFile.txt";
    vector<string> expected = {"This is some content used for a test case. Please do not delete or modify this file!"};

    // ACT
    auto result = ReadFile(ExistentFile);

    // ASSERT
    CHECK(result.size() == expected.size());
    CHECK_EQ(result, expected);
}

TEST_CASE("Filter Words Test") {
    // ARRANGE
    vector<string> mainText = {"war", "weapon", "grace", "famine", "hope", "help", "thirst", "poverty", "love", "courage", "compassion", "struggle", "hopelessness"};
    vector<string> filterList = {"hope", "poverty", "grace", "hope"};
    vector<string> expected = {"grace", "hope", "poverty"};

    vector<string> filterList2 = {"ananas", "grape", "car", "window"};
    vector<string> emptyVector = {};

    // ACT
    auto result = FilterWords(mainText, filterList);
    auto result2 = FilterWords(mainText, filterList2);
    auto result3 = FilterWords(mainText, emptyVector);

    // ASSERT
    CHECK_EQ(result, expected);
    CHECK_EQ(result2,emptyVector);
    CHECK_EQ(result3, emptyVector);
}

TEST_CASE("Count Word Occurrences Test") {
    // ARRANGE
    vector<string> wordList = {"war", "weapon", "grace", "famine", "weapon", "famine", "weapon", "famine", "hopelessness","grace" ,"hopelessness", "weapon"};
    map<string, int> expected = {{"war", 1}, {"weapon", 4}, {"grace", 2}, {"famine", 3}, {"hopelessness", 2}};

    // ACT
    auto result = CountWordOccurrences(wordList);

    // ASSERT
    CHECK_EQ(result, expected);
}

TEST_CASE("Calculate Term Density Test") {
    // ARRANGE
    vector<string> chapterContent = {"war", "weapon", "grace", "famine", "weapon", "famine", "weapon", "famine", "hopelessness","grace" ,"hopelessness", "weapon"};
    map<string, int> wordOccurrences = {{"war", 1}, {"weapon", 4}, {"grace", 2}, {"famine", 3}, {"hopelessness", 2}};
    map<string, double> expected = {{"war", 8.33333}, {"weapon", 33.33333}, {"grace", 16.6667}, {"famine", 25}, {"hopelessness", 16.6667}};

    // ACT
    auto result = CalculateTermDensity(chapterContent, wordOccurrences);

    
    // ASSERT
    for (const auto& pair : result) {
        auto it = expected.find(pair.first);

        CHECK(std::abs(pair.second - it->second) < 1e-4);
    }
    //CHECK_EQ(result, expected);
}
