/*  Managing a lexicon with IDs  */

/* load a lexicon and return a lexicon ID
 *  - input = filename (char *)
 *  - output = lexicon ID (int) */
int load_lexicon(char *);
int load_lexicon_copy_lite(char *);

/* delete a lexicon
 *  - input = lexicon ID (int)
 *  - output = void */
void delete_lexicon(int);

/* get a string from a code
 *  - input = lexicon ID (int) + code (int)
 *  - output = 0 if the code is missing
 *             1 if the code is here
 *             the adress of the word string in (char **) */
int code2word(int,int,char**);

/* get a code from a string
 *  - input = lexicon ID (int) + word string (char*)
 *  - output = 0 if the word is not in the lexicon
 *             1 if the word is in the lexicon
 *             the code found in (int*) */
int word2code(int,char*,int*);

int new_lexicon();

int add_word_lexicon(int , char *, int );
void lexicon_sort_code(int);
void print_lexicon_sort_code(int , FILE *);
void fprint_code_list_word(FILE *file, int lexid, int code);
void sprint_code_list_word(char *buffer, int lexid, int code);
int size_lexicon(int);
 
