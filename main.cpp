#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <ctime> // For randomization
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/FormEntry.h>
#include <cgicc/HTTPStatusHeader.h>

using namespace cgicc;
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::vector<std::string> generateRandomGrid(int rows, int cols) {
    std::vector<std::string> grid(rows);
    std::srand(std::time(0)); // Seed for random generation

    for (int i = 0; i < rows; ++i) {
        std::string row;
        for (int j = 0; j < cols; ++j) {
            char randomLetter = 'A' + (std::rand() % 26); // Random letter from A-Z
            row += randomLetter;
        }
        grid[i] = row;
    }

    return grid;
}

int countWordInGrid(const std::vector<std::string>& grid, const std::string& word) {
    int count = 0;
    std::string upperWord = word;
    std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);

    for (const auto& row : grid) {
        std::string upperRow = row;
        std::transform(upperRow.begin(), upperRow.end(), upperRow.begin(), ::toupper);
        for (size_t i = 0; i <= upperRow.length() - upperWord.length(); i++) {
            if (upperRow.substr(i, upperWord.length()) == upperWord) {
                count++;
            }
        }
    }

    for (size_t col = 0; col < grid[0].length(); col++) {
        for (size_t row = 0; row <= grid.size() - upperWord.length(); row++) {
            std::string verticalSegment;
            for (size_t k = 0; k < upperWord.length(); k++) {
                verticalSegment += grid[row + k][col];
            }
            std::transform(verticalSegment.begin(), verticalSegment.end(), verticalSegment.begin(), ::toupper);
            if (verticalSegment == upperWord) {
                count++;
            }
        }
    }

    return count;
}

int main() {
    try {
        Cgicc cgi;
        std::cout << HTTPHTMLHeader() << std::endl;

        std::string gridType = cgi("gridType");
        std::string searchWord = trim(cgi("searchWord"));

        std::vector<std::string> grid;

        if (gridType == "generate") {
            int rows = std::stoi(cgi("rows"));
            int cols = std::stoi(cgi("columns"));

            grid = generateRandomGrid(rows, cols);
        } else {
            const_file_iterator file = cgi.getFile("gridFile");
            if (file != cgi.getFiles().end()) {
                std::istringstream stream((*file).getData());
                std::string line;
                while (std::getline(stream, line)) {
                    if (!line.empty()) {
                        grid.push_back(line);
                    }
                }
            }
        }

        int wordCount = countWordInGrid(grid, searchWord);

        std::cout << "<html lang=\"en\">\n";
        std::cout << "<head>\n";
        std::cout << "<title>Pattern Match Game Result</title>\n";
        std::cout << "<style>\n";
        std::cout << "body { font-family: Arial, sans-serif; background: #f5f5f5; color: #333; text-align: center; }\n";
        std::cout << ".container { max-width: 600px; margin: 20px auto; padding: 20px; background-color white; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); }\n";
        std::cout << ".content { padding: 20px; text-align: center; }\n";
        std::cout << "pre { background: #f0f0f0; border: 1px solid #ddd; padding: 10px; border-radius: 5px; font-size: 1.25em; }\n"; 
        std::cout << "</style>\n";
        std::cout << "</head>\n";
        std::cout << "<body>\n";
        std::cout << "<div class='container'>\n";
        std::cout << "<h1>Pattern Match Game Result</h1>\n";
        std::cout << "<div class='content'>\n";
        std::cout << "<pre>\n";
        for (const auto& row : grid) {
            std::cout << row << "\n";
        }
        std::cout << "</pre>\n";
        std::cout << "<p>Occurrences of the word '" << searchWord << "': " << wordCount << "</p>\n";
        std::cout << "</div>\n";
        std::cout << "</div>\n";
        std::cout << "</body>\n";
        std::cout << "</html>\n";
    } catch (const std::exception& e) {
        std::cout << "Content-Type: text/plain\n\n";
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}

