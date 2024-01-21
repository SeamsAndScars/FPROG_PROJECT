
#include "chapter_categorizer.h"

using namespace std;

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

auto CountWordOccurrences = [](const std::vector<std::string>& wordList) -> map<string, int> {
    // Map to store word occurrences
    std::map<std::string, int> wordCount;

    // Map function: Convert each word to a pair (word, 1)
    auto MapFunction = [](const std::string& word) {
        return std::make_pair(word, 1);
    };

    // Reduce function: Sum up the values for each key (word)
    auto ReduceFunction = [](int accumulator, const std::pair<std::string, int>& wordPair) {
        return accumulator + wordPair.second;
    };

    // Map phase: Apply map function to each element in the wordList
    std::vector<std::pair<std::string, int>> mappedResult;
    std::transform(wordList.begin(), wordList.end(), std::back_inserter(mappedResult), MapFunction);

    // Reduce phase: Use accumulate with reduce function to count occurrences
    for (const auto& wordPair : mappedResult) {
        wordCount[wordPair.first] += wordPair.second;
    }

    return wordCount;
};

// Function to calculate the density of terms
auto CalculateTermDensity = [](const vector<string>& wordList, const map<string, int>& wordOccurrences) -> map<string, double> {
    map<string, double> termDensity;

    // Function to calculate the average distance for a specific word
    auto calculateAverageDistance = [&](const string& word) -> double {
        // Find positions of the word in the wordList
        vector<int> positions;
        for (size_t i = 0; i < wordList.size(); ++i) {
            if (wordList[i] == word) {
                positions.push_back(i);
            }
        }

        // Calculate relative distances and average
        double averageDistance = 0.0;
        if (positions.size() > 1) {
            adjacent_difference(positions.begin(), positions.end(), positions.begin() + 1, minus<>());
            averageDistance = accumulate(positions.begin() + 1, positions.end(), 0.0) / (positions.size() - 1);
        }

        return averageDistance;
    };

    // Use transform to calculate average distances for each word
    transform(wordOccurrences.begin(), wordOccurrences.end(), inserter(termDensity, termDensity.begin()), [&](const auto& wordOccurrence) {
        const string& word = wordOccurrence.first;
        return make_pair(word, calculateAverageDistance(word));
    });

    return termDensity;
};



int main() {
    vector<string> wordList = ReadFile(book);
    vector<string> peaceList = ReadFile(peace_terms);
    vector<string> warList = ReadFile(war_terms);

    vector<string> tokenizedWords = !wordList.empty() ? TokenizeString(wordList) : vector<string>();

    vector<string> cleansedWords = CleanseWords(tokenizedWords);

    vector<string> filteredWords = FilterWords(cleansedWords, peaceList, warList);
    
    map<string, int> wordOccurrences = CountWordOccurrences(filteredWords);

    map<string, double> termDensity = CalculateTermDensity(filteredWords, wordOccurrences);

    // Print the results of wordOccurences function
    for (const auto& entry : termDensity) {
        std::cout << entry.first << ": " << entry.second << " average distance\n";
    }

   

    return 0;

}