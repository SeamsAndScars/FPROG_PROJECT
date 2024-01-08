#include "chapter_categorizer.h"
//using namespace std;


auto ReadFile = [](string filename) -> optional<vector<std::string>> {
    ifstream myfile;
    std::vector<std::string> fileData;
    std::string line;

    myfile.open(filename);

    if(myfile == NULL)
    {
        return NULL;
    }

    while(std::getline(myfile, line))
    {
        fileData.push_back(line);
    }

    myfile.close();

    return fileData;
};


int main()
{
    std::vector<string> data = ReadFile("war_terms.txt");

    for(const std::string& line : fileData)
    {
        std::cout << line << std::endl;
    }
}