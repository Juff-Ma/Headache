#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define DEFAULT_CELLCOUNT 1024
#define VERSION "1.0.0"

void print_help(void) 
{
    const char* const message =
        "Headache Brainf*ck interpreter version " VERSION " by Juff-Ma\n\n"
        "Usage: headache [options] [file]\n\n"
        "Options:\n"
        "-h:       Show this help.\n"
        "-c <num>: Specify cellcount.\n";

    printf(message);
}

int run(int cellcount, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {   
        printf("Error reading file %s.\n", filename);
        return -1;
    }
    if (fseek(file, 0L, SEEK_END))
    {
        fclose(file);
        puts("Error getting file size\n");
        return -2;
    }
    long fileLength = ftell(file) + 1;
    if (fseek(file, 0L, SEEK_SET))
    {
        fclose(file);
        puts("Error prepping file for reading\n");
        return -3;
    }

    int iProg = 0;
    uint8_t *prog = malloc((size_t)fileLength+1);
    memset(prog, 0, fileLength);

    fread(prog, 1, fileLength, file);
    fclose(file);
    file = NULL;
    
    int iData = 0;
    int *cells = malloc((size_t)cellcount * sizeof(int));
    memset(cells, 0, cellcount * sizeof(int));

    // execution logic

    
    while (iProg < fileLength)
    {
        const char instruction = prog[iProg++];
        switch (instruction)
        {
        case '>':
            ++iData;
            if (iData >= cellcount) iData = cellcount -1;
            break;
        case '<':
            --iData;
            if (iData < 0) iData = 0;
            break;
        case '+':
            cells[iData] = cells[iData] + 1;
            break;
        case '-':
            cells[iData] = cells[iData] - 1;
            break;
        case '.':
            putchar(cells[iData]);
            break;
        case ',':
            cells[iData] = getchar();
            break;
        case '[':
            if(cells[iData] == 0) 
            {
                int openCount = 1;
                while (openCount > 0)
                {
                    if (iProg >= fileLength - 1) break;
                    switch (prog[++iProg])
                    {
                    case '[': ++openCount; break;
                    case ']': --openCount; break;
                    default: break;
                    }
                }
            }
            break;
        case ']':
            if (cells[iData] == 0) break;
            int closedCount = 1;
            while (closedCount > 0)
            {
                if (iProg == 1) return -4;
                switch (prog[(--iProg) - 1])
                {
                case ']': ++closedCount; break;
                case '[': --closedCount; break;
                default: break;
                }
            }
            break;
        default:
            break;
        }
    }

    free(prog);
    free(cells);
    return 0;
}

int main(int argc, char const *argv[])
{
    if (argc <= 1)
    {
        print_help();
        return 1;
    }
    int cellcount = DEFAULT_CELLCOUNT;
    const char *filename;

    for (int i = 1; i < argc; ++i) 
    {
        if (argv[i][0] == '-')
        {
            if      (argv[i][1] == 'h')
            {
                print_help();
                return 0;
            }
            else if (argv[i][1] == 'c')
            {
                // check if number has been provided
                if (i+1 >= argc)
                {
                    print_help();
                    return 2;
                }
                cellcount = atoi(argv[++i]);

                // terminate if user didn't provide a filename afterwards
                if (i == argc)
                {
                    print_help();
                    return 3;
                }
            }
            else
            {
                print_help();
                return 4;
            }
            
        }
        else if (i+1 == argc)
        {
            filename = argv[i];
        }
        else 
        {
            print_help();
            return 5;
        }
    }

    return run(cellcount, filename);
}
