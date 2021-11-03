#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>

#define BASE_ARR_SIZE (10)
#define ARR_SIZE_MULTIPLY (2)

char* strcpy(char* destination, const char* source);

int main(int argc, char const *argv[])
{
    char const* filename = argv[1];
    char buffer[MAX_LINE_LENGTH];
    FILE* asm_program = fopen(filename, "r");
    int line_counter = -1;
    int label_counter = 0, curr_arr_len = BASE_ARR_SIZE;
    label_t* labels_list = (label_t*)calloc(BASE_ARR_SIZE, sizeof(label_t));
    char* tmp_label_str;

    while (fgets(buffer,MAX_LINE_LENGTH, asm_program) != NULL) {
        line_counter++;
        for (int i = 0 ; i < MAX_LABEL_LENGTH + 1; i++) {
            if (buffer[i] == ':') {
                buffer[i] = '\0';
                tmp_label_str = (char *) malloc(MAX_LABEL_LENGTH * sizeof(char));
                strcpy(tmp_label_str, buffer);
                label_t tmp_label = {.label = tmp_label_str, .op_index = line_counter--}; /* label lines don't count as instruction lines */
                if (label_counter == curr_arr_len)
                {
                    curr_arr_len *= ARR_SIZE_MULTIPLY;
                    labels_list = (label_t*)realloc(labels_list, curr_arr_len * sizeof(label_t));
                }
                
                labels_list[label_counter++] = tmp_label;
                break;
            }
        }
    }

    labels_list = (label_t*)realloc(labels_list, label_counter * sizeof(label_t));
    printf("hi");
}