#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "main.h"


int main(int argc, char **argv)
{

    char *buffer;
    size_t buffer_size;
  
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <source.cmm>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    buffer_size = init_buffer(argv[1], &buffer);

    exit(EXIT_SUCCESS);
}



size_t init_buffer(const char *filename, char **buffer)
{
 
    size_t buffer_size, tmp;
    FILE *source_fp;

    struct stat fileinfo;

    if (stat(filename, &fileinfo) == -1)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    buffer_size = fileinfo.st_size;
    *buffer = (char *)malloc(buffer_size * sizeof(char));
    if (!*buffer)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    source_fp = fopen(filename, "rb");
    if (!source_fp)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    tmp = fread(*buffer, 1, buffer_size, source_fp);
    fclose(source_fp);

    if (tmp < buffer_size)
    {
        fprintf(stderr, "%s\n", "Error reading file");
        exit(EXIT_FAILURE);
    }

    return buffer_size;
}
