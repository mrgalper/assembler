#include <stdio.h> /* printf */
#include <stdlib.h> /* malloc */
#include <string.h> /* strncpy */

#include "logger.h" /* logger API */
#include "queue.h" /* queue API  */

typedef struct {
    char *filename;
    char *msg;
    size_t line;
} log_entry_t;

struct logger{
    queue_ty *log_queue;
};

logger_t *LoggerInit(void) {
    logger_t *logger = (logger_t *)malloc(sizeof(logger_t));
    if (logger == NULL) {
        return NULL; 
    }

    logger->log_queue = QueueCreate();
    if (logger->log_queue == NULL) {
        free((void *)logger);
        return NULL; 
    }

    return logger;
}

void LoggerDestroy(logger_t *logger) {
    if (logger == NULL) {
        return; 
    }

    while (!QueueIsEmpty(logger->log_queue)) {
        log_entry_t *log_entry = (log_entry_t *)QueuePeek(logger->log_queue);
        free((void *)log_entry->filename);
        free((void *)log_entry->msg);
        free((void *)log_entry);
        QueueDequeue(logger->log_queue);
    }

    QueueDestroy(logger->log_queue);
    free((void *)logger);
}

logger_status_t AddLog(logger_t *logger, const char *filename, 
                                                const char *msg, size_t line) {
    if (logger == NULL || filename == NULL || msg == NULL) {
        return FAIL; 
    }

    log_entry_t *log_entry = (log_entry_t *)malloc(sizeof(log_entry_t));
    if (log_entry == NULL) {
        return FAIL; 
    }

    // Allocate memory for filename and copy the content
    log_entry->filename = (char *)malloc(strlen(filename) + 1);
    if (log_entry->filename == NULL) {
        free((void *)log_entry);
        return FAIL; 
    }
    strncpy(log_entry->filename, filename, strlen(filename) + 1);

    // Allocate memory for msg and copy the content
    log_entry->msg = (char *)malloc(strlen(msg) + 1);
    if (log_entry->msg == NULL) {
        free((void *)log_entry->filename);
        free((void *)log_entry);
        return FAIL; 
    }
    strncpy(log_entry->msg, msg, strlen(msg) + 1);
    log_entry->line = line;

    if (QueueEnqueue(logger->log_queue, log_entry) == 0) {
        free((void *)log_entry->filename);
        free((void *)log_entry->msg);
        free((void *)log_entry);
        return FAIL;
    }

    return SUCCESS;
}


void PrintAllLogs(logger_t *logger) {
    if (logger == NULL) {
        return;  
    }

    while (!QueueIsEmpty(logger->log_queue)) {
        log_entry_t *log_entry = (log_entry_t *)QueuePeek(logger->log_queue);
        printf("[File: %s]: Line: %zu Message: %s\n", 
                        log_entry->filename, log_entry->line, log_entry->msg);
        QueueDequeue(logger->log_queue);
        free((void *)log_entry);
    }
}
#define LOGGER_TEST
#ifdef LOGGER_TEST
int main() {
    logger_t *logger = LoggerInit();
    if (logger == NULL) {
        printf("Failed to initialize logger.\n");
        return 1;
    }

    AddLog(logger, "file1.c", "Error: Invalid input", 10);
    AddLog(logger, "file2.c", "Warning: Deprecated function", 25);
    AddLog(logger, "file3.c", "Error: Out of memory", 42);

    PrintAllLogs(logger);

    LoggerDestroy(logger);
    return 0;
}
#endif /*LOGGER_TEST*/