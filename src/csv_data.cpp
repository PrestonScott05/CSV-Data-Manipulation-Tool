#include "csv_data.h"
#include "utility.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <limits>
#include <algorithm>

//constructor and destructor
CSVData::CSVData(const std::string& filename, int width) : filename(filename), width(width), rows(0), cols(0), data(nullptr) {
    loadData();
}

CSVData::~CSVData() {
    for (int i = 0;  i < rows; i++) {
        delete[] data[i];
    }
    delete[] data;
}

void CSVData::loadData() {
    std::ifstream input(filename);
    if (!input.is_open()) {
        std::cout << "Failed to open " << "\"" << filename << "\"" << std::endl;
        exit(1);
    }

    cols = 0; 
    std::string line;
    // determine number of columns from the first line
    if (getline(input, line)) {
        std::istringstream ss(line);
        std::string token;
        while (getline(ss, token, ',')) {
            cols++;
        }
    }

    // reset
    input.clear(); 
    input.seekg(0); 
    rows = std::count(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>(), '\n') + 1;

    // Allocate memory for data
    data = new std::string*[rows];
    for (int i = 0; i < rows; i++) {
        data[i] = new std::string[cols];
    }

    input.clear();
    input.seekg(0);
    int row = 0;
    while (getline(input, line)) {
        std::istringstream ss(line);
        std::string val;
        int col = 0;
        while (getline(ss, val, ',')) {
            if (col < cols) { 
                data[row][col] = val;
                col++;
            }
        }
        row++;
    }

    input.close();
}


void CSVData::printIntro() {
    std::cout << "Enter a command or \"help\" for a command list:" << std::endl;
}

void CSVData::printCommandList() {
    std::cout << "command list:" << std::endl;
    std::cout << "        print" << std::endl;
    std::cout << "        cols" << std::endl;
    std::cout << "        search *|col_name val" << std::endl;
    std::cout << "        min col_name" << std::endl;
    std::cout << "        max col_name" << std::endl;
    std::cout << "        avg col_name" << std::endl;
}

void CSVData::printData() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            std::cout << std::setw(width) << data[i][j];
        }
        std::cout << std::endl;
    }
}

void CSVData::printCols() {
    for (int i = 0; i < cols; i++) {
        std::cout << data[0][i] << std::endl;
    }
}

void CSVData::printRow(int row) {
    for (int i = 0; i < cols; i++) {
        std::cout << std::setw(width) << data[row][i];
    }
    std::cout << std::endl;
}

void CSVData::printHeader() {
    for (int i = 0; i < cols; i++) {
        std::cout << std::setw(width) << data[0][i];
    }
    std::cout << std::endl;
}

bool CSVData::checkCol(std::string column)
{
    for (int i = 0; i < cols; i++) {
        if (data[0][i] == column) {
            return true;
        }
    }
    return false;
}

double CSVData::maxCol(int colindex, bool &isValid)
{
    double max = 0.0;
    isValid = false;

    for (int i = 1; i < rows; i++) {
        if (!data[i][colindex].empty() && isValidDouble(data[i][colindex])) {
            double val = std::stod(data[i][colindex]);
            if (!isValid || val > max) {
                max = val;
                isValid = true;
            }
        }
    }

    return max;
}

double CSVData::avgCol(int colindex, bool &isValid)
{
    double sum = 0.0;
    int count = 0;
    isValid = false;

    for (int i = 1; i < rows; i++) {
        if (!data[i][colindex].empty() && isValidDouble(data[i][colindex])) {
            double val = std::stod(data[i][colindex]);
            sum += val;
            count++;
            isValid = true;
        }
    }

    return (count > 0) ? (sum / count) : 0.0;
}

double CSVData::minCol(int colindex, bool &isValid)
{
    double min = 0.0;
    isValid = false;

    for (int i = 1; i < rows; i++) {
        if (!data[i][colindex].empty() && isValidDouble(data[i][colindex])) {
            double val = std::stod(data[i][colindex]);
            if (!isValid) {
                min = val; 
                isValid = true;
            } else if (val < min) {
                min = val; 
            }
        }
    }

    return min;
}

void CSVData::handleSearchCommand() {
    std::string inputLine;
    std::cout << "Enter column name and value to search for (enclose spaces within quotes): ";
    std::getline(std::cin >> std::ws, inputLine); 

    std::istringstream input(inputLine);
    std::string column, value;
    char checkQuote;

    input >> checkQuote;
    if (checkQuote == '"') {
        std::getline(input, column, '"'); // read column name up to the next quote
        input >> std::ws; //skip whitespace
    } else {
        input.putback(checkQuote);
        input >> column; // no quote means no spaces, read as usual
    }

    input >> std::ws; 
    if (input.peek() == '"') {
        input.get(); // Consume the quote
        std::getline(input, value, '"'); 
    } else {
        input >> value; // No quote means no spaces, read as usual
    }

    bool everyColumn = false;

    if (column == "*") {
        everyColumn = true;
    }
    
    if (!everyColumn) {
        int columnIndex = -1;
        for (int i = 0; i < cols; ++i) {
            if (data[0][i] == column) {
                columnIndex = i;
                break;
            }
        }

        if (columnIndex == -1) {
            std::cout << "Column not found." << std::endl;
            return;
        }

        bool found = false;
        for (int i = 1; i < rows; ++i) {
            if (data[i][columnIndex] == value) {
                if (!found) {
                    printHeader();
                    found = true;
                }
                printRow(i);
            }
        }
        if (!found) {
            std::cout << "No matching records found." << std::endl;
        }   
    } else {
        bool found = false;
        for (int i = 1; i < rows; ++i) {
            for (int j = 0; j < cols; j++) {
                if (data[i][j] == value) {
                    if (!found) {
                        printHeader();
                        found = true;
                    }
                    printRow(i);
                }
            }
        }
        if (!found) {
            std::cout << "No matching records found." << std::endl;
        }
    }
}

void CSVData::handleMaxCommand() {
    std::string inputLine;
    std::cout << "Enter column name to find maximum value (enclose spaces within quotes): ";
    std::getline(std::cin >> std::ws, inputLine); 

    std::istringstream input(inputLine);
    std::string column, value;
    char checkQuote;

    input >> checkQuote;
    if (checkQuote == '"') {
        std::getline(input, column, '"'); // read column name up to the next quote
        input >> std::ws; //skip whitespace
    } else {
        input.putback(checkQuote);
        input >> column; // no quote means no spaces, read as usual
    }

    input >> std::ws; 
    if (input.peek() == '"') {
        input.get(); // consume the quote
        std::getline(input, value, '"'); 
    } else {
        input >> value; // no quote means no spaces, read as usual
    }
    

    int columnIndex = -1;
    for (int i = 0; i < cols; ++i) {
        if (data[0][i] == column) {
            columnIndex = i;
            break;
        }
    }

    if (columnIndex == -1) {
        std::cout << "Column not found." << std::endl;
        return;
    }

    bool isValid;
    double max = maxCol(columnIndex, isValid);
    if (isValid) {
        std::cout << "The max value in column \"" << column << "\" is " << max << std::endl;
    } else {
        std::cout << "No valid numeric data found in column." << std::endl;
    }
}

void CSVData::handleAvgCommand() {
    std::string inputLine;
    std::cout << "Enter column name to find the average value (enclose spaces within quotes): ";
    std::getline(std::cin >> std::ws, inputLine); 

    std::istringstream input(inputLine);
    std::string column, value;
    char checkQuote;

    input >> checkQuote;
    if (checkQuote == '"') {
        std::getline(input, column, '"'); // read column name up to the next quote
        input >> std::ws; //skip whitespace
    } else {
        input.putback(checkQuote);
        input >> column; // no quote means no spaces, read as usual
    }

    input >> std::ws; 
    if (input.peek() == '"') {
        input.get(); // consume the quote
        std::getline(input, value, '"'); 
    } else {
        input >> value; // no quote means no spaces, read as usual
    }

    int columnIndex = -1;
    for (int i = 0; i < cols; ++i) {
        if (data[0][i] == column) {
            columnIndex = i;
            break;
        }
    }

    if (columnIndex == -1) {
        std::cout << "Column not found." << std::endl;
        return;
    }

    bool isValid;
    double avg = avgCol(columnIndex, isValid);
    if (isValid) {
        std::cout << "The average value in column \"" << column << "\" is " << avg << std::endl;
    } else {
        std::cout << "No valid numeric data found in column." << std::endl;
    }
}

void CSVData::handleMinCommand() {
    std::string inputLine;
    std::cout << "Enter column name to find minimum value (enclose spaces within quotes): ";
    std::getline(std::cin >> std::ws, inputLine); 

    std::istringstream input(inputLine);
    std::string column, value;
    char checkQuote;

    input >> checkQuote;
    if (checkQuote == '"') {
        std::getline(input, column, '"'); // read column name up to the next quote
        input >> std::ws; //skip whitespace
    } else {
        input.putback(checkQuote);
        input >> column; // no quote means no spaces, read as usual
    }

    input >> std::ws; 
    if (input.peek() == '"') {
        input.get(); // consume the quote
        std::getline(input, value, '"'); 
    } else {
        input >> value; // no quote means no spaces, read as usual
    }

    int columnIndex = -1;
    for (int i = 0; i < cols; ++i) {
        if (data[0][i] == column) {
            columnIndex = i;
            break;
        }
    }

    if (columnIndex == -1) {
        std::cout << "Column not found." << std::endl;
        return;
    }

    bool isValid;
    double min = minCol(columnIndex, isValid);
    if (isValid) {
        std::cout << "The minimum value in column \"" << column << "\" is " << min << std::endl;
    } else {
        std::cout << "No valid numeric data found in column." << std::endl;
    }
}

void CSVData::processInput() {
    while (true) {
        printIntro();
        std::string choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore newline left in the input buffer

        if (choice == "quit") {
            break;
        } else if (choice == "help") {
            printCommandList();
        } else if (choice == "print") {
            printData();
        } else if (choice == "cols") {
            printCols();
        } else if (choice == "search") {
            handleSearchCommand();
        } else if (choice == "max") {
            handleMaxCommand();
        } else if (choice == "min") {
            handleMinCommand();
        } else if (choice == "avg") {
            handleAvgCommand();
        } else {
            std::cout << "Invalid command" << std::endl;
        }
    }
}



