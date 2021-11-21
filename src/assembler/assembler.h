#include <stdint.h>
#include "../reg.h"

#define MAX_LINE_LENGTH (500)
#define MAX_LABEL_LENGTH (50)

typedef struct {
    char * label;
    int cmd_index;
} label_t;
