#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void* func(void* input);
void sigHandler(int sigNum);
int exitFlag = 0;
int runCount = 0;
pthread_mutex_t lock;

int main() {
    signal(SIGINT, sigHandler); // Handle Ctrl+C
    pthread_t threadWorker;
    char input[256]; // Allocate space for input
    pthread_mutex_init(&lock, NULL);

    printf("Multi-threaded file server simulation. Enter filenames. Press Ctrl+C to exit.\n");

    while (!exitFlag) {
        printf("Enter filename: ");
        if (scanf("%255s", input) != 1) {
            break; // Handle input failure
        }

        // Allocate memory for the filename
        char* filename = malloc(strlen(input) + 1); // geekforgeeks for malloc usage
        if (filename == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        strcpy(filename, input);

        // Create worker thread
        if (pthread_create(&threadWorker, NULL, func, filename) != 0) {
            perror("Failed to create thread");
            free(filename);
        } else {
            pthread_detach(threadWorker); // Automatically clean up finished threads - did look this up
            pthread_mutex_lock(&lock);
            runCount++;
            pthread_mutex_unlock(&lock);
        }
    }

    printf("Shutting down... Total file requests received: %d\n", runCount);
    pthread_mutex_destroy(&lock);
    return 0;
}

void* func(void* input) {
    char* filename = (char*)input;
    sleep(1); 
    printf("\nFile '%s' accessed.\n", filename);
    free(filename); // Free mem
    return NULL;
}

void sigHandler(int sigNum) {
    exitFlag = 1;
    printf("\nExiting: Total files given = %d\n", runCount);
    pthread_mutex_destroy(&lock);
    exit(0);
}

