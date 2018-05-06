#include <iostream>
#include <fstream>
#include "TokenStream.h"
#include "Parser.h"
#include "AstPrettyPrinter.h"
#include "Semantics.h"

std::string load_text_from_file(std::string filepath) {
    std::ifstream stream(filepath);
    std::string str((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    return str;
}

int main(int argc, char **argv) {
    // argv[1] will contain the file to read
    if (argc < 2) {
        printf("Missing filename in args.\n");
        return 1;
    }

    // Get file using argument as filepath
    std::string file_contents = load_text_from_file(argv[1]);
    // If the string is empty, the file didn't read properly (or it's empty)
    if (file_contents.empty()) {
        printf("Error reading file: %s\n", argv[1]);
        return 1;
    }

    TokenStream stream;
    stream.lex(file_contents);

    if(stream.errors.size() != 0) {
        for(Token token : stream.tokens) {
            printf("type: %-18s line: %-4u col: %-3u offset: %-5u raw: \"%s\"\n",
                token_type_names[(int)token.type],
                token.line,
                token.column,
                token.offset,
                token.raw.c_str()
            );
        }

        for(Error error : stream.errors) {
            printf("Error type: %-4d "
                "type: %-18s line: %-4u col: %-3u offset: %-5u raw: \"%s\"\n",
                (int)error.error_type,
                token_type_names[(int)error.token.type],
                error.token.line,
                error.token.column,
                error.token.offset,
                error.token.raw.c_str()
            );
        }
    }

    Parser parser;
    Ast ast = parser.parse(stream.tokens);

    for(Error error : parser.errors) {
        printf("Error type: %-4d "
            "type: %-18s line: %-4d col: %-3d offset: %-5d raw: \"%s\"\n",
            (int)error.error_type,
            token_type_names[(int)error.token.type],
            error.token.line,
            error.token.column,
            error.token.offset,
            error.token.raw.c_str()
        );
    }

    syntax_highlight_print(file_contents, stream);
    pretty_print_ast(ast);


    Semantics sem;
    sem.pass1(ast);

    return 0;
}
