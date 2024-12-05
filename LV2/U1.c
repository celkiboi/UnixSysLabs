#include <stdint.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int32_t main(int32_t argc, char* argv[])
{
    if (argc < 2)
    {
        perror("Not enough CLI arguments. Exiting...");
        return -1;
    }

    chdir(argv[1]);

    DIR *dir = opendir(".");
    if (dir == NULL)
    {
        perror("Cannot open directory. Exiting...");
        return -2;
    }

    int32_t directoryCount = 0;
    int32_t fileCount = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;
        if (entry->d_type == DT_DIR)
            directoryCount++;
        if (entry->d_type == DT_REG)
            fileCount++;
    }

    printf("Number of directories: %i\nNumber of files: %i", directoryCount, fileCount);

    closedir(dir);
    return 0;
}