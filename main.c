//
// Created by qniu on 12/16/24.
//

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED
} MetaCommandResult;

// note: unrecognized is a bit like an exception
typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
} PrepareResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct {
    StatementType type;
} Statement;

typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

InputBuffer* new_input_buffer() {
    // allocates memory
    InputBuffer *input_buffer = malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

void print_prompt() {
    printf("db > ");
}

void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
      printf("Error reading input\n");
      exit(EXIT_FAILURE);
    }

    // printf("%d \n", bytes_read);
    // printf("%s \n", input_buffer->buffer);

    // ignore trailing new line
    input_buffer->input_length = bytes_read - 1;
    // make the last char pointing to 0
    input_buffer->buffer[bytes_read - 1] = 0;
    // printf("%s \n", input_buffer->buffer[bytes_read - 1]);
}

// // lineptr: pointer to the var that we use to point to the buffer containing the read line
// // n: pointer to the var we use to save the size of allocated buffer
// // return the number of bytes we read
// ssize_t getline(char** lineptr, size_t *n, FILE *stream) {
//     // allocates memory
// }

void close_input_buffer(InputBuffer* input_buffer) {
  free(input_buffer->buffer);
  free(input_buffer);
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED;
    }
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    // why do strncmp and strcmp here?
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement) {
    switch (statement->type) {
        case STATEMENT_INSERT:
            printf("We would do an insert there. \n");
            break;
        case STATEMENT_SELECT:
            printf("We would do a select there. \n");
            break;
    }
}

int main(int argc, char* argv[]) {
    InputBuffer* input_buffer = new_input_buffer();

    // simple REPL: read-execute-print loop
    while (true) {
        // continue starts next loop
        // 'break' breaks out of switch
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.') {
            // meta commands start with a dot
            switch (do_meta_command(input_buffer)) {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED):
                    printf("Unrecognized command: %s\n", input_buffer->buffer);
                    continue;
            }
        }

        // converts the line of input into our internal representation of a statement
        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
        }

        // eventually becomes our VM
        execute_statement(&statement);
        printf("Executed.\n");

        // if (strcmp(input_buffer->buffer, ".exit") == 0) {
        //     close_input_buffer(input_buffer);
        //     exit(EXIT_SUCCESS);
        // } else {
        //     printf("Unrecognized command '%s'. \n", input_buffer -> buffer);
        // }
    }
}
