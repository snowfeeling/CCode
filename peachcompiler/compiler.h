#pragma once
#ifndef PEACHCOMPILER_H
#define PEACHCOMPILER_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define S_EQ(str, str2) \
    (str && str2 && strcmp(str, str2) == 0)

struct pos
{
    int line;
    int col;
    const char *filename;
};

#define NUMBER_CASE \
    case '0':       \
    case '1':       \
    case '2':       \
    case '3':       \
    case '4':       \
    case '5':       \
    case '6':       \
    case '7':       \
    case '8':       \
    case '9'

#define OPERATOR_CASE_EXCLUDING_DIVISON \
    case '+':                           \
    case '-':                           \
    case '*':                           \
    case '>':                           \
    case '<':                           \
    case '^':                           \
    case '%':                           \
    case '!':                           \
    case '=':                           \
    case '~':                           \
    case '|':                           \
    case '&':                           \
    case '(':                           \
    case '[':                           \
    case ',':                           \
    case '.':                           \
    case '?'

#define SYMBOL_CASE \
    case '{':       \
    case '}':       \
    case ':':       \
    case ';':       \
    case '#':       \
    case '\\':      \
    case ')':       \
    case ']'

enum
{
    LEXICAL_ANALYSIS_ALL_OK,
    LEXICAL_ANALYSIS_INPUT_ERROR
};

enum
{
    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_KEYWORD,
    TOKEN_TYPE_OPERATOR,
    TOKEN_TYPE_SYMBOL,
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_COMMENT,
    TOKEN_TYPE_NEWLINE
};
enum
{
    NUMBER_TYPE_NORMAL,
    NUMBER_TYPE_LONG,
    NUMBER_TYPE_FLOAT,
    NUMBER_TYPE_DOUBLE
};

struct token
{
    int type;
    int flags;
    struct pos pos;
    union
    {
        char cval;
        const char *sval;
        unsigned int inum;
        unsigned long lnum;
        unsigned long long llnum;
        void *any;
    };
    struct token_number
    {
        int type;
    } num;
    // True if there is wihtespace between the token and the next token
    // i.e. * a for operator token * would mean whitespace would be set for token "a"
    bool whitespace;

    // (5+10+20)
    const char *between_brackets;
};
struct lex_process;
typedef char (*LEX_PROCESS_NEXT_CHAR)(struct lex_process *process);
typedef char (*LEX_PROCESS_PEEK_CHAR)(struct lex_process *process);
typedef void (*LEX_PROCESS_PUSH_CHAR)(struct lex_process *process, char c);

struct lex_process_functions
{
    LEX_PROCESS_NEXT_CHAR next_char;
    LEX_PROCESS_PEEK_CHAR peek_char;
    LEX_PROCESS_PUSH_CHAR push_char;
};

struct lex_process
{
    struct pos pos;
    struct vector *token_vec;
    struct compile_process *compiler;

    int current_expression_count;
    struct buffer *parentheses_buffer;
    struct lex_process_functions *function;

    void *private;
};

enum
{
    COMPILER_FILE_COMPILED_OK,
    COMPILER_FAILED_WITH_ERRORS
};

struct scope
{
    int flags;

    // void*
    struct vector *entities;

    // The total number of bytes this scope uses. Aligned to 16 bytes.
    size_t size;

    // NULL if no parent.
    struct scope *parent;
};

enum
{
    SYMBOL_TYPE_NODE,
    SYMBOL_TYPE_NATIVE_FUNCTION,
    SYMBOL_TYPE_UNKNOWN
};

struct symbol
{
    const char *name;
    int type;
    void *data;
};

struct compile_process
{
    // the flags in regards to how the file will be compiled
    int flags;

    struct pos pos;
    struct compile_process_input_file
    {
        FILE *fp;
        const char *abs_path;
    } cfile;

    struct vector *token_vec;

    struct vector *node_vec;
    struct vector *node_tree_vec;
    FILE *ofile;

    struct
    {
        struct scope *root;
        struct scope *current;
    } scope;

    struct
    {
        // Current active symbol table. struct symbol*
        struct vector *table;

        // struct vector* multiple symbol tables stored in here..
        struct vector *tables;
    } symbols;

    // Pointer to our codegenerator.
    struct code_generator *generator;
    struct resolver_process *resolver;

    // A vector of const char* that represents include directories.
    struct vector *include_dirs;
    struct preprocessor *preprocessor;
};

enum
{
    PARSE_ALL_OK,
    PARSE_GENERAL_ERROR
};

enum
{
    VALIDATION_ALL_OK,
    VALIDATION_GENERAL_ERROR
};

enum
{
    CODEGEN_ALL_OK,
    CODEGEN_GENERAL_ERROR
};

enum
{
    NODE_TYPE_EXPRESSION,
    NODE_TYPE_EXPRESSION_PARENTHESES,
    NODE_TYPE_NUMBER,
    NODE_TYPE_IDENTIFIER,
    NODE_TYPE_STRING,
    NODE_TYPE_VARIABLE,
    NODE_TYPE_VARIABLE_LIST,
    NODE_TYPE_FUNCTION,
    NODE_TYPE_BODY,
    NODE_TYPE_STATEMENT_RETURN,
    NODE_TYPE_STATEMENT_IF,
    NODE_TYPE_STATEMENT_ELSE,
    NODE_TYPE_STATEMENT_WHILE,
    NODE_TYPE_STATEMENT_DO_WHILE,
    NODE_TYPE_STATEMENT_FOR,
    NODE_TYPE_STATEMENT_BREAK,
    NODE_TYPE_STATEMENT_CONTINUE,
    NODE_TYPE_STATEMENT_SWITCH,
    NODE_TYPE_STATEMENT_CASE,
    NODE_TYPE_STATEMENT_DEFAULT,
    NODE_TYPE_STATEMENT_GOTO,
    NODE_TYPE_UNARY,
    NODE_TYPE_TENARY,
    NODE_TYPE_LABEL,
    NODE_TYPE_STRUCT,
    NODE_TYPE_UNION,
    NODE_TYPE_BRACKET,
    NODE_TYPE_CAST,
    NODE_TYPE_BLANK
};

enum
{
    NODE_FLAG_INSIDE_EXPRESSION = 0b00000001,
    NODE_FLAG_IS_FORWARD_DECLARATION = 0b00000010,
    NODE_FLAG_HAS_VARIABLE_COMBINED = 0b00000100
};

struct node
{
    int type;
    int flags;

    struct pos pos;

    struct node_binded
    {
        // Pointer to our body node
        struct node *owner;

        // Pointer to the function this node is in.
        struct node *function;
    } binded;

    union
    {
        struct exp
        {
            struct node *left;
            struct node *right;
            const char *op;

        } exp;
    };

    union
    {
        char cval;
        const char *sval;
        unsigned int inum;
        unsigned long lnum;
        unsigned long long llnum;
    };
};
enum
{
    DATATYPE_FLAG_IS_SIGNED = 0b00000001,
    DATATYPE_FLAG_IS_STATIC = 0b00000010,
    DATATYPE_FLAG_IS_CONST = 0b00000100,
    DATATYPE_FLAG_IS_POINTER = 0b00001000,
    DATATYPE_FLAG_IS_ARRAY = 0b00010000,
    DATATYPE_FLAG_IS_EXTERN = 0b00100000,
    DATATYPE_FLAG_IS_RESTRICT = 0b01000000,
    DATATYPE_FLAG_IGNORE_TYPE_CHECKING = 0b10000000,
    DATATYPE_FLAG_IS_SECONDARY = 0b100000000,
    DATATYPE_FLAG_STRUCT_UNION_NO_NAME = 0b1000000000,
    DATATYPE_FLAG_IS_LITERAL = 0b10000000000,
};
enum
{
    DATA_TYPE_VOID,
    DATA_TYPE_CHAR,
    DATA_TYPE_SHORT,
    DATA_TYPE_INTEGER,
    DATA_TYPE_LONG,
    DATA_TYPE_FLOAT,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_STRUCT,
    DATA_TYPE_UNION,
    DATA_TYPE_UNKNOWN
};
enum
{
    DATA_TYPE_EXPECT_PRIMITIVE,
    DATA_TYPE_EXPECT_UNION,
    DATA_TYPE_EXPECT_STRUCT
};
enum
{
    DATA_SIZE_ZERO = 0,
    DATA_SIZE_BYTE = 1,
    DATA_SIZE_WORD = 2,
    DATA_SIZE_DWORD = 4,
    DATA_SIZE_DDWORD = 8
};
struct datatype
{
    int flags;
    // i.e type of long, int, float ect..
    int type;

    // i.e long int. int being the secondary.
    struct datatype *secondary;
    // long
    const char *type_str;
    // The sizeof the datatype.
    size_t size;
    int pointer_depth;

    union
    {
        struct node *struct_node;
        struct node *union_node;
    };

    struct array
    {
        struct array_brackets *brackets;
        /**
         *
         * The total array size: Equation = DATATYPE_SIZE * EACH_INDEX
         */
        size_t size;
    } array;
};
int compile_file(const char *filename, const char *out_filename, int flags);
struct compile_process *compile_process_create(const char *filename, const char *filename_out, int flags);

char compile_process_next_char(struct lex_process *lex_process);
char compile_process_peek_char(struct lex_process *lex_process);
void compile_process_push_char(struct lex_process *lex_process, char c);

void compiler_error(struct compile_process *compiler, const char *msg, ...);
void compiler_warning(struct compile_process *compiler, const char *msg, ...);
struct lex_process *lex_process_create(struct compile_process *compiler, struct lex_process_functions *functions, void *private);
void lex_process_free(struct lex_process *process);
void *lex_process_private(struct lex_process *process);
struct vector *lex_process_tokens(struct lex_process *process);
int lex(struct lex_process *process);
int parse(struct compile_process *process);

struct lex_process *tokens_build_for_string(struct compile_process *compiler, const char *str);

bool token_is_keyword(struct token *token, const char *value);
bool token_is_symbol(struct token *token, char c);

bool token_is_nl_or_comment_or_newline_seperator(struct token *token);
bool keyword_is_datatype(const char *str);
bool token_is_primitive_keyword(struct token *token);
bool datatype_is_struct_or_union_for_name(const char *name);
bool token_is_operator(struct token *token, const char *val);

struct node *node_create(struct node *_node);
void make_exp_node(struct node *left_node, struct node *right_node, const char *op);

struct node *node_pop();
struct node *node_peek();
struct node *node_peek_or_null();
void node_push(struct node *node);
void node_set_vector(struct vector *vec, struct vector *root_vec);
bool node_is_expressionable(struct node *node);
struct node *node_peek_expressionable_or_null();

#define TOTAL_OPERATOR_GROUPS 14
#define MAX_OPERATORS_IN_GROUP 12

enum
{
    ASSOCIATIVITY_LEFT_TO_RIGHT,
    ASSOCIATIVITY_RIGHT_TO_LEFT
};

struct expressionable_op_precedence_group
{
    char *operators[MAX_OPERATORS_IN_GROUP];
    int associtivity;
};

enum
{
    EXPRESSIONABLE_GENERIC_TYPE_NUMBER,
    EXPRESSIONABLE_GENERIC_TYPE_IDENTIFIER,
    EXPRESSIONABLE_GENERIC_TYPE_UNARY,
    EXPRESSIONABLE_GENERIC_TYPE_PARENTHESES,
    EXPRESSIONABLE_GENERIC_TYPE_EXPRESSION,
    EXPRESSIONABLE_GENERIC_TYPE_NON_GENERIC
};

enum
{
    EXPRESSIONABLE_IS_SINGLE,
    EXPRESSIONABLE_IS_PARENTHESES
};

// Wangss add - start
int showTokens(struct lex_process *process);
void showOneNode(struct node *node);
void showNodesFromRoot(struct vector *node_vec);

// Wangss add - end.
#endif