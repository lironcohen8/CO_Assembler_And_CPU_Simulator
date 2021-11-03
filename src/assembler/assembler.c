#include "asembler.h"
#include <stdio.h>
#include <stdlib.h>

#define BASE_ARR_SIZE (10)
#define ARR_SIZE_MULTIPLY (2)

char* strcpy(char* destination, const char* source);

int main(int argc, char const *argv[])
{
    char* filename = argv[1], buffer[MAX_LINE_LENGTH];
    FILE* asm_program = fopen(filename);
    int line_counter = -1;
    int label_counter = 0, curr_arr_len = BASE_ARR_SIZE;
    char* tmp_label;
    label_t* labels_list = calloc(BASE_ARR_SIZE, sizeof(label_t));


    while (fgets(buffer,MAX_LINE_LENGTH, asm_program) != NULL) {
        line_counter++;
        for (int i = 0 ; i < MAX_LABEL_LENGTH + 1; i++) {
            if (buffer[i] == ':') {
                buffer[i] = '\0';
                strcpy(tmp_label,buffer);
                label_t tmp_label = {.label = tmp_label, .op_index = line_counter--}; /* label lines don't count as instruction lines */
                if (label_counter == curr_arr_len)
                {
                    curr_arr_len *= ARR_SIZE_MULTIPLY;
                    labels_list = realloc(labels_list, curr_arr_len);
                }
                
                labels_list[label_counter++] = tmp_label;
                break;
            }
        }
    }

    labels_list = realloc(labels_list, label_counter);
}