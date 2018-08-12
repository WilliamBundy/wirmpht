typedef struct Metaprogram_Core Metaprogram_Core;
typedef struct Lexer_Location Lexer_Location;
typedef struct Token Token;
typedef struct Lexer_File Lexer_File;
typedef struct Lexer Lexer;
typedef struct Proc_Arg Proc_Arg;
typedef struct Proc_Prototype Proc_Prototype;
typedef struct Struct_Def Struct_Def;
typedef struct Struct_Anon_Member Struct_Anon_Member;
typedef struct Struct_Member_Var Struct_Member_Var;
typedef union Struct_Member Struct_Member;
typedef struct MetaType MetaType;
typedef struct Meta_Member Meta_Member;
typedef enum Meta_Flags Meta_Flags;
typedef u64 Hash;

struct Metaprogram_Core
{
	bool verbose;
};

struct Lexer_Location
{
	isize file;
	isize line;
	isize offset;
};

struct Token
{
	i32 kind;
	char *start;
	isize len;
	Hash hash;
	Lexer_Location location;
	Token *prev;
	Token *next;
};

struct Lexer_File
{
	char *filename;
	isize pathlen;
	isize index;
	isize length;
	Lexer_Location location;
	char *start;
	char *head;
};

struct Lexer
{
	Lexer_File *main_file;
	Lexer_File *files;
	isize files_count;
	isize files_capacity;
	isize structs_count;
	isize procedures_count;
};

struct Proc_Arg
{
	char **terms;
	isize count;
	char *defaults;
};

struct Proc_Prototype
{
	Token *start;
	Token *end;
	char **decorators;
	isize decorators_count;
	char *name;
	Proc_Arg *args;
	isize args_count;
	Proc_Prototype *next;
};

struct Struct_Def
{
	char *name;
	char *parentname;
	char *pre_typedef_name;
	i32 kind;
	isize meta_index;
	char *meta_type_name;
	Hash namehash;
	bool is_anon_member;
	bool is_named_child;
	bool is_typedefed_struct;
	Struct_Member *members;
	i32 *member_kinds;
	isize member_count;
	Struct_Def *next;
};

struct Struct_Anon_Member
{
	Struct_Def def;
	char **array_sizes;
	isize array_levels;
};

struct Struct_Member_Var
{
	Hash namehash;
	char *name;
	char **terms;
	isize count;
	MetaType *type;
	int32 asterisk_count;
	char **array_sizes;
	isize array_levels;
};

union Struct_Member
{
	Struct_Anon_Member anon_struct;
	Struct_Member_Var member_var;
};

struct MetaType
{
	Hash hash;
	char *name;
	bool ispointer;
	bool isunsigned;
	bool isvolatile;
	bool isconst;
	bool isarray;
	uint32 pointer_depth;
	isize type_index;
	MetaType *next;
};

struct Meta_Member
{
	uint64 flags;
	char *type_name;
	char *access_expr;
	uint32 pointer_depth;
	char *parentname;
	char *name;
};

char* load_file(char* filename, isize* size_out, wb_MemoryArena* arena);
Hash hash_string(char* c, int len);
void print_token(Token* t, Token* start);
void init_lexer_file(Lexer_File* file, char* filename, char* prev_path, isize prev_path_len, wb_MemoryArena* arena);
void init_lexer(Lexer* lex, isize file_capacity, wb_MemoryArena* arena);
Lexer_File* get_next_file(Lexer* lex);
bool is_space(char c);
bool is_number(char c);
bool is_valid_identifier(char c);
bool lexer_get_token(Lexer* lexer, Lexer_File* f, Token* t);
void parse_number_tokens(Token* head);
void parse_include_directive(Lexer* lex, Token* directive);
Token* parse_metaprogram_directive(Token* t);
void parse_tokens(Lexer* lex, Token* start);
bool match_pattern3(Token* head, i32 a, i32 b, i32 c);
void init_proc_arg(Proc_Arg* arg, isize count, wb_MemoryArena* arena);
void print_proc_prototype(Proc_Prototype* p);
Proc_Prototype* find_proc_prototypes(Lexer* lex, Token* start, wb_MemoryArena* arena);
void odin_print_proc_prototype(Proc_Prototype* p);
void print_indent(int32 indent);
void print_struct_names(Struct_Def* def, isize index, char* prefix, isize prefix_len, char* suffix, Struct_Def** all_structs, isize* counter, wb_MemoryArena* arena);
void print_struct(Struct_Def* def, bool as_member_struct, int32 indent);
Token* parse_struct_member(Lexer* lex, Struct_Def* parent, Token* start, wb_MemoryArena* arena);
Struct_Def* find_struct_defs(Lexer* lex, Token* start, wb_MemoryArena* arena);
void odin_print_struct(Struct_Def* def, bool as_member_struct, int32 indent);
bool meta_type_equals(MetaType* a, MetaType* b);
void populate_meta_type(Struct_Member* member, i32 kind, Struct_Def* parent, MetaType* meta);
MetaType* get_types_in_struct(Struct_Def* def, MetaType* head, wb_MemoryArena* arena);
void print_meta_member(Meta_Member* member, char* prefix, char* suffix);
void print_struct_info(Struct_Def* def, char* prefix, char* suffix);
void print_reflection_data(Struct_Def* def);
void print_metaprogram_types();
void print_metaprogram_get_struct_info_proc();
void w_printTypedefs(Struct_Def* s_head);
void w_printEnumDefs(Token* start);
void w_extractExistingTypedefs(Token* start);
MetaType* w_findUniqueTypes(MetaType* type_start, Struct_Def* structdef);
void w_printMetaTypeEnum(Struct_Def* structdef, Struct_Def** all_structs, MetaType* unique_type_start, isize* num_structs);
void w_printStructs(Struct_Def* structdef);
void w_printMetadata(isize num_structs, Struct_Def** all_structs);
void w_printProcs(Proc_Prototype* pstart);
void w_printOdin(Struct_Def* structdef, Proc_Prototype* pstart);
int main(int argc, char** argv);
