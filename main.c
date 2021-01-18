#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void indent(int n, FILE *outfile);
void usage();
void parse_args(int count, char *args[], int *condense, char **infile, char **outfile);
void standard(FILE *outfile, FILE *infile);


int main(int argc, char *argv[]) {
    errno_t error_in, error_out;
    int condense = 0;
    char *input_file_name, *output_file_name;
    FILE *infile;
    FILE *outfile;

    parse_args(argc, argv, &condense, &input_file_name, &output_file_name);

    if ((error_in = fopen_s(&infile, input_file_name, "r")) != 0 ) {
        printf("Error opening file %s for reading, error %d", input_file_name, error_in);
    }

    if (!output_file_name) {
        outfile = stdout;
    } else {
        if ((error_out = fopen_s(&outfile, output_file_name, "w")) != 0 ) {
            printf("Error opening file %s for writing, error %d", output_file_name, error_out);
        }
    }

    if (!condense) {
        standard(outfile, infile);
        return 0;
    }

    char c = fgetc(infile), next;
    int indentation_level = 1;
    int cursor_address_counter = 0, address_value_counter = 0;
    fprintf(outfile, "#include <stdio.h>\n");
    fprintf(outfile, "int main() {\n");
    fprintf(outfile, "\tchar memory[30000], *cursor = memory;\n");

    while (c  != EOF) {
        next = fgetc(infile);
        switch (c) {
            case '>':
                if (next != '>' && next != '<') {
                    indent(indentation_level, outfile);
                    fprintf(outfile, "cursor += %d;\n", cursor_address_counter + 1);
                    cursor_address_counter = 0;
                } else {
                    ++cursor_address_counter;
                } break;
            case '<':
                if (next != '>' && next != '<') {
                    indent(indentation_level, outfile);
                    fprintf(outfile, "cursor += %d;\n", cursor_address_counter - 1);
                    cursor_address_counter = 0;
                } else {
                    --cursor_address_counter;
                } break;
            case '+':
                if (next != '+' && next != '-') {
                    indent(indentation_level, outfile);
                    fprintf(outfile, "*cursor += %d;\n", address_value_counter + 1);
                    address_value_counter = 0;
                } else {
                    ++address_value_counter;
                } break;
            case '-':
                if (next != '+' && next != '-') {
                    indent(indentation_level, outfile);
                    fprintf(outfile, "*cursor += %d;\n", address_value_counter - 1);
                    address_value_counter = 0;
                } else {
                    --address_value_counter;
                } break;
            case '[':
                indent(indentation_level, outfile);
                indentation_level++;
                fprintf(outfile, "while (*cursor) {\n"); break;
            case ']':
                indentation_level--;
                indent(indentation_level, outfile);
                fprintf(outfile, "}\n"); break;
            case ',':
                indent(indentation_level, outfile);
                fprintf(outfile, "*cursor = getchar();\n"); break;
            case '.':
                indent(indentation_level, outfile);
                fprintf(outfile, "putchar(*cursor);\n"); break;
            default:
                break;
        }
        c = next;
    }
    if (indentation_level != 1) {
        fprintf(stderr, "Error with indentation, likely missing a [ or ]\n");
    }
    else {
        indent(indentation_level, outfile);
    }
    fprintf(outfile, "return 0;\n}");
}


void indent(int n, FILE *outfile) {
    for (int i = 0; i < n; ++i) {
        fprintf(outfile, "\t");
    }
}


void usage(char *argv[]) {
    printf("Brainfuck to c transpiler\n\n");
    printf("Usage : %s [-c] INPUT_FILE [OUTPUT_FILE]\n", argv[0]);
    printf("\tIf output file is not supplied, program will output to stdout");
}


void parse_args(int count, char *args[], int *condense, char **infile, char **outfile) {
    if (count < 2) {
        usage(args);
        exit(-1);
    }
    if (count == 2)  {
        if (strcmp(args[1], "-c") == 0) {
            usage(args);
            exit(-1);
        } else {
            *infile = args[1];
            *outfile = args[2];
        }
        return;
    }
    if (strcmp(args[1], "-c") == 0) {
        ++*condense;
        *infile = args[2];
        *outfile = args[3];
    } else {
        *infile = args[1];
        *outfile = args[2];
    }
}


void standard(FILE *outfile, FILE *infile) {
    char c;
    int indentation_level = 1;
    fprintf(outfile, "#include <stdio.h>\n");
    fprintf(outfile, "int main() {\n");
    fprintf(outfile, "\tchar memory[30000], *cursor = memory;\n");

    while ((c = fgetc(infile)) != EOF) {
        switch (c) {
            case '>':
                indent(indentation_level, outfile);
                fprintf(outfile, "cursor++;\n"); break;
            case '<':
                indent(indentation_level, outfile);
                fprintf(outfile, "cursor--;\n"); break;
            case '+':
                indent(indentation_level, outfile);
                fprintf(outfile, "++*cursor;\n"); break;
            case '-':
                indent(indentation_level, outfile);
                fprintf(outfile, "--*cursor;\n"); break;
            case '[':
                indent(indentation_level, outfile);
                indentation_level++;
                fprintf(outfile, "while (*cursor) {\n"); break;
            case ']':
                indentation_level--;
                indent(indentation_level, outfile);
                fprintf(outfile, "}\n"); break;
            case ',':
                indent(indentation_level, outfile);
                fprintf(outfile, "*cursor = getchar();\n"); break;
            case '.':
                indent(indentation_level, outfile);
                fprintf(outfile, "putchar(*cursor);\n"); break;
            default:
                break;
        }
    }
    if (indentation_level != 1) {
        fprintf(stderr, "Error with indentation, likely missing a [ or ]\n");
    }
    else {
        indent(indentation_level, outfile);
    }
    fprintf(outfile, "return 0;\n}");
}
