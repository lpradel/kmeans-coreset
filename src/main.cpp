#include <iostream>
#include <string>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Coreset.h"

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}


int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "Usage: input k output" << std::endl;
        std::cout << "3 arguments expected, got " << argc - 1 << ":" << std::endl;
        for (int i = 1; i < argc; ++i)
            std::cout << argv[i] << std::endl;
        return 1;
    }

    std::string input(argv[1]);
    int k = atoi(argv[2]);
    std::string output(argv[3]);

    std::cout << "Calculation Start: " << currentDateTime() << std::endl;

    try
    {
        Coreset coreset(input, k);
        coreset.process();
        coreset.calculateCoreset(3, 2, 3, 0.05);
        coreset.writeCoresetToFile(output);
    }
    catch(const std::invalid_argument& ia)
    {
        std::cerr << "Invalid argument exception: " << ia.what() << std::endl;
    }

    std::cout << "Calculation End: " << currentDateTime() << std::endl;

    return 0;
}
