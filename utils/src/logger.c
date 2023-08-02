#include <stdio.h> /* printf */
#include <stdlib.h> /* malloc */
#include <string.h> /* strncpy */
#include "assert.h" /* assert */

#include "logger.h" /* logger API */
#include "queue.h" /* queue API  */

typedef struct {
    char *filename;
    char *msg;
    size_t line;
} log_entry_t;

struct logger{
    queue_t*log_queue;
};

logger_t *CreateLogger(void) {
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

void DestroyLogger(logger_t *logger) {
    assert(logger!= NULL);

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
    log_entry_t *log_entry = NULL;

    assert(logger != NULL);
    assert(filename != NULL);
    assert(msg != NULL);

    log_entry = (log_entry_t *)malloc(sizeof(log_entry_t));
    if (log_entry == NULL) {
        return LG_FAIL; 
    }

    /* Allocate memory for filename and copy the content */
    log_entry->filename = (char *)malloc(strlen(filename) + 1);
    if (log_entry->filename == NULL) {
        free((void *)log_entry);
        return LG_FAIL; 
    }
    strncpy(log_entry->filename, filename, strlen(filename) + 1);

    /* Allocate memory for msg and copy the content */
    log_entry->msg = (char *)malloc(strlen(msg) + 1);
    if (log_entry->msg == NULL) {
        free((void *)log_entry->filename);
        free((void *)log_entry);
        return LG_FAIL; 
    }
    strncpy(log_entry->msg, msg, strlen(msg) + 1);
    log_entry->line = line;

    if (QueueEnqueue(logger->log_queue, log_entry) == 0) {
        free((void *)log_entry->filename);
        free((void *)log_entry->msg);
        free((void *)log_entry);
        return LG_FAIL;
    }

    return LG_SUCCESS;
}

int LoggerIsEmpty(logger_t *logger) {
    return (QueueIsEmpty(logger->log_queue));
}
void PrintAllLogs(logger_t *logger) {
    assert(logger != NULL);

    while (!QueueIsEmpty(logger->log_queue)) {
        log_entry_t *log_entry = (log_entry_t *)QueuePeek(logger->log_queue);
        printf("[File: %s]: Line: %lu Message: %s\n", 
                        log_entry->filename, log_entry->line, log_entry->msg);
        free((void *)log_entry->filename);
        free((void *)log_entry->msg);
        free((void *)log_entry);
        QueueDequeue(logger->log_queue);
    }
}
