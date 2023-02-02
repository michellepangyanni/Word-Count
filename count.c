/*
 * COMP 321 Project 2: Word Count
 *
 * This program counts the characters, words, and lines in one or more files,
 * depending on the command-line arguments.
 * 
 * <Michelle Pang, yp29>
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "csapp.h"

struct counts {
	int   char_count;
	int   word_count;
	int   line_count;
};
struct file_count{
	char *file_name;
	struct counts count;
};
struct node {
	struct file_count data;
	struct node* next;
	struct node* prev;

};
static void	
app_error_fmt(const char *fmt, ...);

static int	
do_count(char *input_files[], const int nfiles,
		    const bool char_flag, const bool word_flag,
		    const bool line_flag, const bool test_flag);
			
static void	
print_counts(FILE *fp, struct counts *cnts, const char *name,
		    const bool char_flag, const bool word_flag,
		    const bool line_flag);

/*
 * Requires:
 *   The first argument, "fmt", is a printf-style format string, and all
 *   subsequent arguments must match the types of arguments indicated in the
 *   format string.
 *
 * Effects:
 *   Prints a formatted error message to stderr using the supplied
 *   format string and arguments.  The message is prepended with
 *   the string "ERROR: " and a newline is added at the end.
 */
static void
app_error_fmt(const char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "ERROR: ");
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}
/**
 * Requires:
 *   The "head" argument must not be NULL and must point to an allocated structure of type "struct node**"
 *   The "new data" argument must not be NULL and must point to an allocated structure of type "struct file_count"
 * Effect:
 *   Given the reference to the head of a list, create a new node from the file_count structure,
 * 	 append this new node at the end.
 * 
*/
static void 
append(struct node** head, struct file_count new_data)
{	
	//allocation malloc memory for the new node 
	//while initializing a new node
	struct node* new_node = (struct node*) malloc(sizeof(struct node));
	//create a "last" node pointing to the head for later traversal
	struct node *last = *head;
	//assign data to the new node, which new_node is the last node on the linked list
	new_node -> data = new_data;
	//and make the next node as NULL
	new_node -> next = NULL;

	//If the linked list is empty, the new node becomes the head node
	if (*head == NULL)
	{
		*head = new_node;
		//Terminate the function
		return;
	}
	//If the linked list is not empty, traverse until we reach the last node
	while(last -> next != NULL)
	{
		//update the last node
		last = last -> next;
	}
	//change the prev of the new node
	new_node -> prev = last;
	//change the next of the new node
	last -> next = new_node;

	//end the function
	return;

}
/*
 * Requires:
 *   The "fp" argument must be a valid FILE pointer.
 *   The "cnts" argument must not be NULL and must point to an allocated
 *   structure of type "struct counts".
 *   The "name" argument must not be NULL and must point to an allocated
 *   string.
 *
 * Effects:
 *   Prints the character, word, and line counts for a particular file
 *   as directed by the flags.
 */
static void
print_counts(FILE *fp, struct counts *cnts, const char *name,
    const bool char_flag, const bool word_flag, const bool line_flag)
{

	if (line_flag)
		fprintf(fp, "%8d", cnts->line_count);
	if (word_flag)
		fprintf(fp, "%8d", cnts->word_count);
	if (char_flag)
		fprintf(fp, "%8d", cnts->char_count);
	fprintf(fp, " %s\n", name);
}
/**
 * Requires:
 *   The "head" argument must not be NULL and must point to an allocated structure of type "struct node*"
 * Effects:
 *   Print out the file in "ASCIIbetical" order based on the filename
*/
static void 
print_sort(struct node* head, const bool char_flag, const bool word_flag, const bool line_flag)
{
    //Declare a "current" node for iteration
    struct node* current;
    //Iterate every node in the linked list until the entire list finishes
    while(head != NULL)
    {
        //1. Initialize a "min" node with an initial reference to the head node
        struct node* min = head;
        //Assign head node to the current node
        current = head;
        //Find the minimum file name -- the name has the lowest ASCII value
        while(current != NULL)
        {	//If 
            if (strcmp(min -> data.file_name, current -> data.file_name) > 0)
            {
                //2. Assign "currrent" node to be the "min" node if it is smaller than "min" node
                min = current;
            }
            //3. Update current to continue, it is the tail when its next is NULL
            current = current -> next;
        }
		//Print information of the file with lowest "ASCIIbetical" order
        print_counts(stdout, &min -> data.count, min -> data.file_name, char_flag, word_flag, line_flag);

		//Store references to min's next and prev nodes
		struct node* prev = min -> prev;
		struct node* next = min -> next;
		//4. Remove the "min" current node from the linked list
		//Let "head" be NULL when "min" is the only node in the list
		if((min == head) && (next == NULL))
		{
			//End the sorting
			head = NULL;
		}
		//If the "min" is the head, 
		else if(min == head){
			//Let the head to be the next node
			head = next;
			head -> prev = NULL;
		}
		//If "min" is the last node(the tail)
		else if(next == NULL){
			prev -> next = NULL;
		}
		//If "min" is in the middle
		else{
			prev -> next = next;
			next -> prev = prev;
		}
		//Free "min" from the memory;
		Free(min);
    }
}


/*
 * Requires:
 *   The "input_files" argument is an array of strings with "nfiles" valid
 *   strings.
 *
 * Effects: 
 *   Prints to stdout the counts for each file in input_files in
 *   alphabetical order followed by the total counts.  The
 *   "char/word/line_flag" arguments indicate which counts should be
 *   printed on a file-by-file basis.  The total count will include all
 *   three counts regardless of the flags.  An error message is printed
 *   to stderr for each file that cannot be opened.
 *
 *   Returns 0 if every file in input_files was successfully opened and
 *   processed.  If, however, an error occurred opening or processing any
 *   of the files, a small positive integer is returned.
 *
 *   The behavior is undefined when "test_flag" is true.
 */
static int
do_count(char *input_files[], const int nfiles, const bool char_flag,
    const bool word_flag, const bool line_flag, const bool test_flag)
{
	int error = 0;
	// Create an instance of the count structs to store the totals
	struct counts countTotal = {0,0,0};
	// Initialize the head node to be NULL
	struct node* head = NULL;
	//Iterate over each file
	for (int i = 0; i < nfiles; i++){
		//open the current file to read
		FILE *cur_file = fopen(input_files[i], "r");
		//Catch the error if we cannot open the file
		if (cur_file == NULL){
			app_error_fmt("cannot open file \'%s\'", input_files[i]);
			//Set error value to 1
			error = 1;
			//Continue to the next file after reporting the eror
			continue;
		}

		// Create an instance of the file_count structure for the current file
		struct file_count currentFile_count;
		currentFile_count.file_name = input_files[i];
		currentFile_count.count.char_count = 0;
		currentFile_count.count.line_count = 0;
		currentFile_count.count.word_count = 0;
		//Testing the cannot read file error
		if (test_flag){
			 close(fileno(cur_file));
		}
		//Make prev_c initially a space
		int prev_c = 32;
		//Create the character for the while loop
		int c;
		//Iterate over each character in the file until the end of the file is reached
		while ((c = fgetc(cur_file)) != EOF){
			//Update the total and current file character count
			countTotal.char_count += 1;
			currentFile_count.count.char_count += 1;

			//Check if we have begun a new word, and update the word counts to account for the previous word. 
			//A word is detected for 1. it is not a space. 2. the previous character is a space
			if (!isspace(c) && isspace(prev_c)){
				countTotal.word_count += 1;
				currentFile_count.count.word_count += 1;
			}
			//Update line count when the current character is the new line character
			if (c == '\n'){
				countTotal.line_count += 1;
				currentFile_count.count.line_count += 1;
			}
			//Set the previous character to the current character for the next iteration
			prev_c = c;

		}
		//Catch the error when the file cannot be read
		if(!feof(cur_file)){
			app_error_fmt("cannot read file \'%s\'", input_files[i]);
			error = 1;
		}
		//Append your current file's node to the linked list
		append(&head, currentFile_count);
	}
	//Print the counts in ASCIIbetical order
	print_sort(head, char_flag, word_flag, line_flag);
	//If the flag is false, we still want to print it, but its count value is 0
	if (!char_flag){
		countTotal.char_count = 0;
	}
	if(!word_flag){
		countTotal.word_count = 0;
	}
	if(!line_flag){
		countTotal.line_count = 0;
	}
	//Print total counts
	print_counts(stdout, &countTotal, "total", true, true, true);
	return (error);
}

/*
 * Requires:
 *   Nothing.
 *
 * Effects:
 *   Parses command line arguments and invokes do_count with the appropriate
 *   arguments based on the command line.
 */
int 
main(int argc, char **argv)
{
	int c;			// Option character
	extern int optind;	// Option index

	// Abort flag: Was there an error on the command line? 
	bool abort_flag = false;

	// Option flags: Were these options on the command line?
	bool char_flag = false;
	bool line_flag = false;
	bool test_flag = false;
	bool word_flag = false;

	// Process the command line arguments.
	while ((c = getopt(argc, argv, "cltw")) != -1) {
		switch (c) {
		case 'c':
			// Count characters.
			if (char_flag) {
				// A flag can only appear once.
				abort_flag = true;
			} else {
				char_flag = true;
			}
			break;
		case 'l':
			// Count lines.
			if (line_flag) {
				// A flag can only appear once.
				abort_flag = true;
			} else {
				line_flag = true;
			}
			break;
		case 't':
			// Enable test flag.
			if (test_flag) {
				// A flag can only appear once.
				abort_flag = true;
			} else {
				test_flag = true;
			}
			break;
		case 'w':
			// Count words.
			if (word_flag) {
				// A flag can only appear once.
				abort_flag = true;
			} else {
				word_flag = true;
			}
			break;
		case '?':
			// An error character was returned by getopt().
			abort_flag = true;
			break;
		}
	}
	if (abort_flag || optind == argc) {
		/*
		 * In this case, use fprintf instead of app_error_fmt because
		 * error messages describing how to run a program typically
		 * begin with "usage: " not "ERROR: ".
		 */
		fprintf(stderr,
		    "usage: %s [-c] [-l] [-t] [-w] <input filenames>\n",
		    argv[0]);
		return (1);	// Indicate an error.
	}

	// Process the input files, and return the result.
	return (do_count(&argv[optind], argc - optind, char_flag, word_flag,
	    line_flag, test_flag));
}
