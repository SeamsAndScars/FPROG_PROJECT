
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


auto TokenizeString(const vector<string> &inputStrings) -> vector<string> {
    vector<string> tokenizedWords;

    for (const auto &line: inputStrings) {
        istringstream iss(line);
        copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokenizedWords));
    }

    return tokenizedWords;
}

auto CleanseWords(const vector<string> &inputWords) -> vector<string> {
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
}

auto FilterWords(const vector<string> &mainText, const vector<string> &filterList1, const vector<string> &filterList2) -> vector<string> {
    vector<string> filteredWords;
    set<string> filterSet1(filterList1.begin(), filterList1.end());
    set<string> filterSet2(filterList2.begin(), filterList2.end());

    copy_if(mainText.begin(), mainText.end(), back_inserter(filteredWords), [&](const string &word) {
        return filterSet1.find(word) != filterSet1.end() || filterSet2.find(word) != filterSet2.end();
    });

    return filteredWords;
}


int main() {
    vector<string> wordList = ReadFile(book);
    vector<string> peaceList = ReadFile(peace_terms);
    vector<string> warList = ReadFile(war_terms);

    vector<string> tokenizedWords = !wordList.empty() ? TokenizeString(wordList) : vector<string>();

    vector<string> cleansedWords = CleanseWords(tokenizedWords);

    vector<string> filteredWords = FilterWords(cleansedWords, peaceList, warList);

    for_each(filteredWords.begin(), filteredWords.end(), [](const string& word) {
        cout << word << endl;
    });

    return 0;

}