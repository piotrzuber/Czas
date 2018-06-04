#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MSIZE 10000
#define HALF_MSIZE (MSIZE / 2)

#define SUBTRACT 1
#define JUMP 2
#define CALL 3
#define RETURN 4
#define READ 5
#define WRITE 6
#define DEF 0

typedef struct instruction{
    int opcode;
    int arg1;
    int arg2;
    char *label;
} instruction_t;

typedef struct program{
    instruction_t instructions[4000];
    char label_buffer[3000];
    char *labels[1000];
    int heap[4000];
    int memory[MSIZE];
} program_t;

void Init_Memory(program_t *p){

    for(int i = 0; i <= MSIZE; i++){
        p->memory[i] = -1 - i + (MSIZE + 1) / 2;
    }
}

void Init_Heap(program_t *p){

    for (int i = 0; i < 4000; i++){
        p->heap[i] = -1;
    }
}

void Skip_Blank(){

    int c;
    c = getchar();
    while (c == '|' || isspace(c)) {
        c = getchar();
    }
    ungetc(c, stdin);
}

void Scan_Label(int c, program_t *p, char **label_buffer_it, int *hm_labels, int *i){
    int it = 0;
    bool is_label = false;
    char tmp_label[2000];
    while (isalnum(c) || c == '_') {
        tmp_label[it] = (char) c;
        it++;
        tmp_label[it] = '\0';
        c = getchar();
    }
    ungetc(c, stdin);

    for (int labels_it = 0; labels_it <= *hm_labels; labels_it++){
        if (strcmp(p->labels[labels_it], tmp_label) == 0){
            p->instructions[*i].label = p->labels[labels_it];
            is_label = true;
        }
        else {
            continue;
        }
    }
    if (is_label == false) {
        p->labels[*hm_labels] = *label_buffer_it;
        it = 0;
        while (tmp_label[it] != '\0'){
            **label_buffer_it = tmp_label[it];
            (*label_buffer_it)++;
            it++;
        }
        **label_buffer_it = '\0';
        (*label_buffer_it)++;
        p->instructions[*i].label = p->labels[*hm_labels];
        (*hm_labels)++;
        p->labels[*hm_labels] = *label_buffer_it;
    }
    Skip_Blank();
    (*i)++;
}

void Parse(program_t *p){

    int c;
    int address;
    int i = 0;
    char *label_buffer_it = p->label_buffer;
    int hm_labels = 0;

    for (int it = 0; it < 4000; it++){
        p->instructions[it].opcode = -1;
    }

    p->labels[hm_labels] = label_buffer_it;

    c = getchar();
    while (c != EOF && c != '&') {
        if (isdigit(c) || c == '-') {
            ungetc(c, stdin);
            scanf("%d", &address);
            p->instructions[i].arg1 = address;
            Skip_Blank();
            c = getchar();
            if (isdigit(c) || c == '-') {
                ungetc(c, stdin);
                scanf("%d", &address);
                p->instructions[i].arg2 = address;
                p->instructions[i].opcode = SUBTRACT;
                c = getchar();
                i++;
            }
            else if (isalpha(c) || c == '_') {
                p->instructions[i].opcode = JUMP;
                Scan_Label(c, p, &label_buffer_it, &hm_labels, &i);
                c = getchar();
            }
            else if (c == '^') {
                p->instructions[i].opcode = WRITE;
                i++;
                c = getchar();
            }
        }
        else if (isalpha(c) || c == '_') {
            p->instructions[i].opcode = CALL;
            Scan_Label(c, p, &label_buffer_it, &hm_labels, &i);
            c = getchar();
        }
        else if (c == ';'){
            p->instructions[i].opcode = RETURN;
            c = getchar();
            i++;
        }
        else if (c == '^'){
            p->instructions[i].opcode = READ;
            Skip_Blank();
            scanf("%d", &address);
            p->instructions[i].arg2 = address;
            c = getchar();
            i++;
        }
        else if (c == ':'){
            p->instructions[i].opcode = DEF;
            Skip_Blank();
            c = getchar();
            Scan_Label(c, p, &label_buffer_it, &hm_labels, &i);
            c = getchar();
        }
        else {
            c = getchar();
        }
    }
}

void Run_Program(program_t *p) {

    Init_Memory(p);
    Init_Heap(p);

    int ins_it = 0;
    int i;
    int h = 0;
    int c;

    while (p->instructions[ins_it].opcode > -1){
        switch (p->instructions[ins_it].opcode) {
            case DEF:
                ins_it++;
                break;
            case SUBTRACT:
                p->memory[p->memory[p->instructions[ins_it].arg1 + HALF_MSIZE] + HALF_MSIZE] =  \
                p->memory[p->memory[p->instructions[ins_it].arg1 + HALF_MSIZE] + HALF_MSIZE] - p->memory[p->memory[p->instructions[ins_it].arg2 + HALF_MSIZE] + HALF_MSIZE];
                ins_it++;
                break;
            case JUMP:
                if (p->memory[p->memory[p->instructions[ins_it].arg1 + HALF_MSIZE] + HALF_MSIZE] > 0){
                    i = 0;
                    while (p->instructions[i].opcode > -1){
                        if ((p->instructions[i].opcode == 0) && (p->instructions[i].label == p->instructions[ins_it].label)){
                            ins_it = i;
                            break;
                        }
                        i++;
                    }
                }
                else {
                    ins_it++;
                }
                break;
            case CALL:
                i = 0;
                if (ins_it + 1 < 4000){
                    p->heap[h] = ins_it + 1;
                    h++;
                }
                while (p->instructions[i].opcode > -1){
                    if ((p->instructions[i].opcode == 0) && (p->instructions[i].label == p->instructions[ins_it].label)) {
                        ins_it = i;
                        break;
                    }
                    i++;
                }
                break;
            case RETURN:
                if (h == 0){
                    return;
                }
                else {
                    ins_it = p->heap[--h];
                    p->heap[h] = 0;
                }
                break;
            case READ:
                c = getchar();
                p->memory[p->memory[p->instructions[ins_it].arg2 + HALF_MSIZE] + HALF_MSIZE] = c;
                ins_it++;
                break;
            case WRITE:
                if ((-HALF_MSIZE <= p->memory[p->instructions[ins_it].arg1 + HALF_MSIZE]) && (p->memory[p->instructions[ins_it].arg1 + HALF_MSIZE] < HALF_MSIZE)) {
                    putchar(p->memory[p->memory[p->instructions[ins_it].arg1 + HALF_MSIZE] + HALF_MSIZE]);
                }
                else {
                    putchar(-1 - p->instructions[ins_it].arg1);
                }
                ins_it++;
        }
    }
}

int main(void){
    program_t p;

    Parse(&p);

    Run_Program(&p);

    return 0;
}