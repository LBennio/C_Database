#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#define EXIT_SUCCESS 0

typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

void print_prompt();
InputBuffer* new_input_buffer();
void read_input(InputBuffer* input);
void close_input_buffer(InputBuffer* input);
ssize_t custom_getline(char **lineptr, size_t *n, FILE*stream);

int main(int argc, char* argv[]) {
    InputBuffer* input_buffer = new_input_buffer();
    
    while(true) {
        print_prompt();

        read_input(input_buffer);

        if(strcmp(input_buffer->buffer, ".exit") == 0) {
            close_input_buffer(input_buffer);
            exit(EXIT_SUCCESS);
        } else {
            printf("unrecognized command '%s'.\n", input_buffer->buffer);
        }
    }
    
}

void print_prompt() {
    printf("db > ");
}

InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));    
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

void read_input(InputBuffer* input) {
    ssize_t bytes_read = custom_getline(&(input->buffer), &(input->buffer_length), stdin);

    if(bytes_read <= 0) {
        printf("Error reading unput\n");
        exit(EXIT_SUCCESS);
    }

    input->input_length = bytes_read-1;
    input->buffer[bytes_read -1] = 0; 
}

void close_input_buffer(InputBuffer* input) {
    free(input->buffer);
    free(input);
}

ssize_t custom_getline(char **lineptr, size_t *n, FILE *stream) {
    if (lineptr == NULL || n == NULL || stream == NULL) {
        errno = EINVAL;
        return -1;
    }

    char *buf = *lineptr;
    size_t bufsize = *n;
    size_t position = 0;
    int c;

    // If buffer is NULL, allocate initial size
    if (buf == NULL) {
        bufsize = 128;
        buf = malloc(bufsize);
        if (buf == NULL) {
            return -1;
        }
    }

    while ((c = fgetc(stream)) != EOF) {
        // Check if we need to resize the buffer
        if (position >= bufsize - 1) {
            bufsize *= 2;
            char *newbuf = realloc(buf, bufsize);
            if (newbuf == NULL) {
                free(buf);
                return -1;
            }
            buf = newbuf;
        }

        buf[position++] = (char)c;

        if (c == '\n') {
            break;
        }
    }

    // Check for EOF and if no characters were read
    if (position == 0 && c == EOF) {
        return -1;
    }

    // Null-terminate the buffer
    buf[position] = '\0';

    *lineptr = buf;
    *n = bufsize;
    return position;
}