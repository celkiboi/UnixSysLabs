#include <stdint.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

#define PATH_BUFFER 1024
#define FOUR_GB 4294967295LL

time_t oldestTime;
int32_t largeFiles = 0;

void preorder_search(char* path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("Cannot open directory. Exiting...");
        exit(-2);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullPath[PATH_BUFFER];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);
        
        struct stat fileStat;
        if (stat(fullPath, &fileStat) == -1)
        {
            perror("Failed to get file stats");
            continue;
        }

        if (entry->d_type == DT_DIR)
        {
            preorder_search(fullPath);
        }

        if (fileStat.st_mtime < oldestTime)
            oldestTime = fileStat.st_mtime;
        if (fileStat.st_size > FOUR_GB)
            largeFiles++;
    }

    closedir(dir);
}

int32_t main(int32_t argc, char* argv[])
{
    if (argc < 2)
    {
        perror("Not enough CLI arguments. Exiting...");
        return -1;
    }

    oldestTime = time(NULL);
    chdir(argv[1]);

    char currentWorkingDirectory[PATH_BUFFER] = { '\0' };
    if (getcwd(currentWorkingDirectory, sizeof(currentWorkingDirectory)) == NULL)
    {
        perror("Cannot open the working directory. Exiting...");
        exit(-3);
    }

    preorder_search(currentWorkingDirectory);

    struct tm *tm_info = localtime(&oldestTime);
    int year = tm_info->tm_year + 1900;
    int month = tm_info->tm_mon + 1;
    int day = tm_info->tm_mday;
    printf("Large files: %i\nOldest file: %i-%i-%i\n", largeFiles, year, month, day);

    return 0;
}
