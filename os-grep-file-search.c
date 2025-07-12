#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>

pthread_mutex_t counterMutex = PTHREAD_MUTEX_INITIALIZER;
#define MAX_PATH_LENGTH 1024

typedef struct
{
    const char *filePath;
    int pipefd[2];
} ThreadData;

int counter = 0;
int match_count = 0;
void exploreDirectory(const char *path);
void* processFile(void* data);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    exploreDirectory(argv[1]);

    printf("Total number of files: %d\n", counter);
    printf("Total number of Matches: %d\n", match_count);

    return EXIT_SUCCESS;
}

void exploreDirectory(const char *path)
{
    pthread_t thread;
    pthread_mutex_t threadMutex;
    pthread_mutex_init(&threadMutex, NULL);

    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    int localCounter = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        struct stat fileStat;
        char fullPath[MAX_PATH_LENGTH];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        if (stat(fullPath, &fileStat) < 0)
        {
            perror("stat");
            exit(EXIT_FAILURE);
        }

        if (S_ISDIR(fileStat.st_mode))
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                exploreDirectory(fullPath);
            }
        }
        else
        {
            localCounter++;

            int pipefd[2];
            if (pipe(pipefd) == -1)
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
            ThreadData threadData = { fullPath, {pipefd[0], pipefd[1]} };

            if (pthread_create(&thread, NULL, processFile, (void*)&threadData) != 0)
            {
                perror("Error creating thread");
            }

            if (pthread_join(thread, NULL) != 0)
            {
                perror("Error joining thread");
            }

            close(pipefd[1]);

            char buffer[1024];
            ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1);
            buffer[bytesRead] = '\0';

            if (bytesRead > 0)
            {
                write(STDOUT_FILENO, buffer, bytesRead);
            }
            close(pipefd[0]);
            
        }
    }

    closedir(dir);

    pthread_mutex_lock(&counterMutex);
    counter += localCounter;
    pthread_mutex_unlock(&counterMutex);
}

void* processFile(void* data)
{
    ThreadData* threadData = (ThreadData*)data;

    const char* filePath = threadData->filePath;
    int local_match_count = 0;

    FILE* file = fopen(filePath, "r");
    char* word = "ipsum";
    int line_number = 0;

    if (file == NULL)
    {
        perror("Error opening file");
        return NULL;
    }

    char line[1024];
    char resultBuffer[1024];
    char result[1024];
    int resultBufferIndex = 0;
    memset(resultBuffer, 0, sizeof(resultBuffer));

    while (fgets(line, sizeof(line), file))
    {
        line_number++;
        char* found = strstr(line, word);

        if (found != NULL)
        {
            local_match_count++;
            int char_number = found - line + 1;
            snprintf(result,
                sizeof(result),
                "%s:%d:%d\n", filePath, line_number, char_number);
            strcat(resultBuffer, result);
        }
    }

    pthread_mutex_lock(&counterMutex);
    match_count += local_match_count;
    pthread_mutex_unlock(&counterMutex);
    resultBufferIndex = strlen(resultBuffer);
    write(threadData->pipefd[1], resultBuffer, resultBufferIndex);

    fclose(file);

    return NULL;
}
