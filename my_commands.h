#ifndef HASH_TABLE
#define HASH_TABLE 

typedef struct {
    char* var_name;
    char* value;
} var;

unsigned long long hash(char* word);

int found(char* word, var table[]);

void init_table(var table[], int size);

void set_var(char* variable, char* value, var table[]);

void unset_var(char* variable, var table[]);

//--------------------------------------------------------

void pipe_function(char* args[]);               // |

void run_on_file_content(char* args[]);         // <

void redirect_output_to_file(char* args[]);     // >

void run_in_background(char* args[]);           // &

#endif