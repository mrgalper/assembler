#include <string.h>
#include <stdio.h> 


int main()
{
    char line[] = "macro m4asdas";
    char *ptr;
    char *pos = strtok(line, " \n\t\v\r\f");

    ptr = strtok(NULL, "\n");
    // ptr = strtok(NULL, "\n");

    printf("%s\n", ptr);

}
