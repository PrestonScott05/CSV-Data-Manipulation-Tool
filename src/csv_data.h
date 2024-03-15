#pragma once
#include <string>
#include <iostream>

class CSVData {
public:
    CSVData(const std::string &filename, int width);
    ~CSVData();

    void processInput();
private:
    std::string **data;
    int width;
    int rows;
    int cols;
    std::string filename;

    void loadData();
    void printIntro();
    void printCommandList();
    void printData();
    void printCols();
    void printRow(int row);
    void printHeader();

    //command functions
    bool checkCol(std::string column);
    double maxCol(int colindex, bool &isValid);
    double avgCol(int colindex, bool &isValid);
    double minCol(int colindex, bool &isValid);
    void handleSearchCommand();
    void handleMaxCommand();
    void handleAvgCommand();
    void handleMinCommand();
    
};
