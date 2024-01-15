#include "chapter_categorizer.h"
using namespace std;

auto ReadFile = [](std::string filename) -> std::optional<std::vector<std::string>> {
    std::ifstream myfile(filename);
    std::vector<std::string> fileData;
    std::string line;

    if(!myfile.is_open())
    {
        return std::nullopt;
    }

    while(std::getline(myfile, line))
    {
        fileData.push_back(line);
    }

    myfile.close();
    return fileData;
};


auto tokenizeString = [](string inputString){
    stringstream sstream(inputString);
    
    /* In PROGRESS

    for_each(char c){

    }
    */
};

int main()
{
    auto result = ReadFile("war_terms.txt");
    
    if (result.has_value()) {
        // File read successfully, process the lines in 'result.value()'
        const std::vector<std::string>& lines = result.value();
        for (const auto& line : lines) {
            std::cout << line << std::endl;
        }
    } else {
        // Handle the error case
        std::cerr << "Failed to read the file." << std::endl;
    }
    
}