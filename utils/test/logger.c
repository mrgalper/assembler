#include "stdio.h" /* printf*/

#include "logger.h" /* logger API*/

int main() {
    logger_t *logger = CreateLogger();
    if (logger == NULL) {
        printf("Failed to initialize logger.\n");
        return 1;
    }

    AddLog(logger, "file1.c", "Error: Invalid input", 10);
    AddLog(logger, "file2.c", "Warning: Deprecated function", 25);
    AddLog(logger, "file3.c", "Error: Out of memory", 42);

    PrintAllLogs(logger);

    DestroyLogger(logger);
    return 0;
}