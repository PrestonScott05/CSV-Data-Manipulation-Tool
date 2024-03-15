#include "csv_data.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./program database.csv #col_width" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    int width = std::stoi(argv[2]);

    CSVData csvData(filename, width);
    csvData.processInput();

    return 0;
}