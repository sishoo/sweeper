#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    unsigned int num;
    unsigned int* lines;
    char** contents;
} Comments;


Comments sweep_file(const char* path) {

    /*
    basically all this function does is:
        - write the contents of a file into a buffer
        - predict the amount of comments in the file (right now its just 25% of the original file)
        - predict the size of each comment (right now its just 90 chars)
        - puts the start and end indexes into buffers
        - allocate a buffer with the exact size to hold all the comments
        - allocate a buffer to hold all the lines those comments start on
        - goes through the file contents from each comment start to end and write them to a comment buffer
        - adds the comment buffer to commentS buffer 
                                            ^ notice the "s"
    */


    FILE* file_ptr = fopen(path, "r");
    if (!file_ptr) {
        fprintf(stderr, "Opening the file failed. Check if the file you input actually exists");
        abort();
    }

    fseek(file_ptr, 0, SEEK_END);
    const unsigned long file_len = ftell(file_ptr);
    rewind(file_ptr);

    char* file_contents = (char*)malloc((1 + file_len) * sizeof(char));
    if (!file_contents) {
        fprintf(stderr, "Allocating buffer failed. Does your file have a length?");
        abort();
    }

    fread(file_contents, 1, file_len, file_ptr);
    fclose(file_ptr);
    file_contents[file_len] = '\0';

    unsigned int predicted_num_comments = (file_len * 0.25) + 1;
    unsigned int predicted_size_comments_bytes = 90;

    unsigned int* comments_starts = (unsigned int*)malloc((size_t)predicted_num_comments * predicted_size_comments_bytes);
    unsigned int* comments_ends = (unsigned int*)malloc((size_t)predicted_num_comments * predicted_size_comments_bytes);
    unsigned int *const original_start = comments_starts;
    unsigned int *const original_end = comments_ends;

    unsigned int num_comments = 0;
    unsigned int num_lines = 0;
    unsigned int comment_started = 0;
    for (int i = 0; i < file_len; i++) {
        unsigned int is_comment = file_contents[i] == '/' && file_contents[i + 1] == '/';
        unsigned int is_mass_comment = file_contents[i] == '/' && file_contents[i + 1] == '*';
        unsigned int is_mass_comment_end = file_contents[i] == '*' && file_contents[i + 1] == '/';
        unsigned int is_newline = file_contents[i] == '\n';

        if (is_comment || is_mass_comment) {
            num_comments++;
            *comments_starts = i;
            comments_starts++;
            comment_started = 1;
        } else if (is_newline && comment_started) {
            num_lines++;
            *comments_ends = i;
            comments_ends++;
            comment_started = 0;
        } else if (is_mass_comment_end) {
            *comments_ends = i;
            comments_ends++;
            comment_started = 0;
        } else if (is_newline) {
            num_lines++;
        }
    }

    char** comments_buffer = malloc((1 + num_comments) * sizeof(char*));
    char** comments_buffer_start = comments_buffer;
    if (!comments_buffer) {
        fprintf(stderr, "Bro there aren't even comments in your file");
        abort();
    }
    comments_buffer[num_comments] = '\0';

    unsigned int* lines_buffer = (int*)malloc((1 + num_lines) * sizeof(int));
    if (!lines_buffer) {
        fprintf(stderr, "Bro does your file even have lines?");
        abort();
    }
    lines_buffer[num_lines] = '\0';


    for (unsigned int* i = original_start; i < comments_starts; i++) {
        for (unsigned int* j = original_end; i < comments_ends; j++) {
            unsigned int start = *i;
            unsigned int end = *j;
            char* comment = (char*)malloc((end - start + 1) * sizeof(char));
            char* comment_start = comment;
            for (int k = start; k < end; k++) {
                *comment = file_contents[k];
                comment++;
            }   
            *comment = '\0';
            *comments_buffer = comment_start;
            comments_buffer++;
        }   
    }

    // old solution
    /*
    unsigned int current_comment = 0;
    unsigned int line = 0;
    unsigned int line_index = 0;
    for (int i = 0; i < file_len; i++) {
        int is_comment = file_contents[i] == '/' && file_contents[i + 1] == '/';
        int is_mass_comment = file_contents[i] == '/' && file_contents[i + 1] == '*';

        // if these are true
        // i represents the first slash
        if (is_comment) {
            // j starts off as the start of the comment contents NOT the slashes
            // then we seek j until the end of the comment
            int j = i + 2; 
            while (j < file_len && file_contents[j] != '\n') {
                j++;
            }
            line++;
            
            char* comment = (char*)malloc((1 + j) * sizeof(char));
            // k is the start of the comment contents
            for (int k = i + 2; k <= j; k++) {
                // index into the comments buffer with the position relative to the beginning of the comment
                comment[k - i] = file_contents[k];
            }
            comment[j] = '\0';
            comments_buffer[current_comment] = comment;
            current_comment++;
            lines_buffer[line_index] = line;
            line_index++;
            i = j;
        } else if (is_mass_comment) {
            // j is the same as above
            int j = i + 2;
            while (j < (file_len - 1) && file_contents[j] != '*' && file_contents[j + 1] != '/') {
                if (file_contents[j] == '\n') {
                    line++;
                }
                j++;
            }
            if (file_contents[j + 2] == '\n') {
                line++;
            }
            j--;
            char* comment = (char*)malloc((1 + j) * sizeof(char));
            for (int k = i; k <= j; k++) {
                comment[k - i] = file_contents[k];
            }
            comment[j] = '\0';
            comments_buffer[current_comment] = comment;
            current_comment++;
            lines_buffer[line_index] = line;
            line_index++;
            i = j;
        } else if (file_contents[i] == '\n') {
            line++;
        }
    } 
    */
    
    Comments comments;
    comments.num = num_comments;
    comments.contents = comments_buffer;
    comments.lines = lines_buffer;
    free(file_contents);
    free(comments_starts);
    free(comments_ends);
    return comments;
}

// void sweep_path(const char* path) {
//     unsigned int is_file = 0;
//     size_t path_len = sizeof(path) / sizeof(char);  
//     for (int i = path_len; path_len > 0; i--) {
//         if (path[i] == ".") {
//             is_file = 1;
//         }
//     }
//     if (is_file) {
//         sweep_file(path);
//     } else {
//         sweep_dir(path);
//     }
// }

int main(int argc, char* argv[]) {

    // if (argc < 2) {
    //     fputs("Input file or directory to sweep! Use 'sweeper list' to list commands.\n", stderr);
    //     arbort();
    // }

    // if (strcmp(argv[1], "list") == 0) {
    //     printf("==========================\n");
    //     printf("Recognized Terms:\n");
    //     printf("'//NOTE: blah blah'\n");
    //     printf("'//TODO: add/remove something'\n");
    //     printf("==========================\n");
    //     printf("Commands:\n");   
    //     printf("'sweeper list' lists the commands\n");
    //     printf("'sweeper sweep --file [your file]' sweeps a file\n");
    //     printf("'sweeper sweep --dir [your dir]' sweeps a dir\n");
    //     printf("'sweeper sweep --path [your path]' sweeps a path\n");
    //     printf("'sweeper commit' commits the documentation with no associated project\n");
    //     printf("'sweeper commit --name [name of project]' commits the documentation to a project\n");
    //     printf("==========================\n");
    // } else if (strcmp(argv[1], "sweep") == 0) {
    //     if (strcmp(argv[2], "--file") == 0) {
    //         printf("Sweeping your file...\n");
    //         comments = sweep_file(argv[3]);
    //     } else if (strcmp(argv[2], "--dir") == 0) {
    //         printf("Sweeping your directory...\n");
    //         comments = sweep_dir(argv[3]);
    //     } else if (strcmp(argv[2], "--path") == 0) {
    //         printf("Sweeping your path...\n");
    //         comments = sweep_path(argv[3]);
    //     } else {
    //         fprintf(stderr, "Can't recognize argument %s, try using '--file' or '--dir'", argv[2]);
    //         abort();
    //     }
    // } else {
    //     fprintf(stderr, "Can't recognize command %s\n", argv[1]);
    //     abort();
    // }
    
    Comments comments = sweep_file("C:\\Users\\PCema\\vs code projects\\sweeper\\test.txt");

    printf("NUM: %i\n", comments.num);

    for (int i = 0; i < comments.num; i++) {
        printf("%i) %s\n", comments.lines[i], *comments.contents[i]);
    }
    
    for (int i = 0; i < comments.num; i++) {
        free(comments.contents[i]);
    }
    free(comments.contents);
    free(comments.lines);
    return 0;
}