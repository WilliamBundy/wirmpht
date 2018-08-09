typedef struct Memory_Arena Memory_Arena;
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
typedef enum Token_Kind Token_Kind;
typedef enum Struct_Kind Struct_Kind;
typedef int32_t bool;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef float real32;
typedef double real64;
typedef real32 real;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef ptrdiff_t isize;
typedef size_t usize;
typedef uint64 Hash;
enum Wirmpht_MetaType
{
	Meta_Type_Memory_Arena,
	Meta_Type_Metaprogram_Core,
	Meta_Type_Lexer_Location,
	Meta_Type_Token,
	Meta_Type_Lexer_File,
	Meta_Type_Lexer,
	Meta_Type_Proc_Arg,
	Meta_Type_Proc_Prototype,
	Meta_Type_Struct_Def,
	Meta_Type_Struct_Anon_Member,
	Meta_Type_Struct_Member_Var,
	Meta_Type_Struct_Member,
	Meta_Type_MetaType,
	Meta_Type_Meta_Member,
	Meta_Type_uint8,
	Meta_Type_isize,
	Meta_Type_bool,
	Meta_Type_i32,
	Meta_Type_char,
	Meta_Type_Hash,
	Meta_Type_int32,
	Meta_Type_uint32,
	Meta_Type_uint64,
};
const char* Meta_Type_Names[] = {
	"Memory_Arena",
	"Metaprogram_Core",
	"Lexer_Location",
	"Token",
	"Lexer_File",
	"Lexer",
	"Proc_Arg",
	"Proc_Prototype",
	"Struct_Def",
	"Struct_Anon_Member",
	"Struct_Member_Var",
	"Struct_Member",
	"MetaType",
	"Meta_Member",
	"uint8",
	"isize",
	"bool",
	"i32",
	"char",
	"Hash",
	"i32",
	"int32",
	"uint32",
	"uint64",
};

struct Memory_Arena
{
	uint8 *data;
	isize capacity;
	isize head;
	isize temp_head;
	Memory_Arena *next;
};

struct Metaprogram_Core
{
	bool verbose;
	Memory_Arena base_arena;
	Memory_Arena work_arena;
	Memory_Arena temp_arena;
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

typedef struct Wirmpht_NullEmpty Wirmpht_NullEmpty;
struct Wirmpht_NullEmpty
{
	int nothing;
};

typedef struct Wirmpht_StructMember Wirmpht_StructMember;
struct Wirmpht_StructMember
{
	char* name;
	isize offset;
	i32 flags;
	i32 type;
	i32 pointerDepth;
};

typedef struct Wirmpht_StructInfo Wirmpht_StructInfo;
struct Wirmpht_StructInfo
{
	char* name;
	char* metaName;
	isize index;
	Wirmpht_StructMember* members;
	i32 count;
};

void init_memory_arena(Memory_Arena* arena, usize size);
uint8* arena_push(Memory_Arena* arena, isize size);
void start_temp_arena(Memory_Arena* arena);
void end_temp_arena(Memory_Arena* arena);
void clear_arena(Memory_Arena* arena);
Memory_Arena* new_memory_arena(usize size, Memory_Arena* src);
char* load_file(char* filename, isize* size_out, Memory_Arena* arena);
Hash hash_string(char* c, int len);
void print_token(Token* t, Token* start);
void init_lexer_file(Lexer_File* file, char* filename, char* prev_path, isize prev_path_len, Memory_Arena* arena);
void init_lexer(Lexer* lex, isize file_capacity, Memory_Arena* arena);
Lexer_File* get_next_file(Lexer* lex);
bool is_space(char c);
bool is_number(char c);
bool is_valid_identifier(char c);
bool lexer_get_token(Lexer* lexer, Lexer_File* f, Token* t);
void parse_number_tokens(Token* head);
void parse_include_directive(Lexer* lex, Token* directive);
Token* parse_metaprogram_directive(Token* t);
void parse_tokens(Lexer* lex, Token* start);
bool match_pattern3(Token* head, Token_Kind a, Token_Kind b, Token_Kind c);
void init_proc_arg(Proc_Arg* arg, isize count, Memory_Arena* arena);
void print_proc_prototype(Proc_Prototype* p);
Proc_Prototype* find_proc_prototypes(Lexer* lex, Token* start, Memory_Arena* arena);
void odin_print_proc_prototype(Proc_Prototype* p);
void print_indent(int32 indent);
void print_struct_names(Struct_Def* def, isize index, char* prefix, isize prefix_len, char* suffix, Struct_Def** all_structs, isize* counter, Memory_Arena* arena);
void print_struct(Struct_Def* def, bool as_member_struct, int32 indent);
Token* parse_struct_member(Lexer* lex, Struct_Def* parent, Token* start, Memory_Arena* arena);
Struct_Def* find_struct_defs(Lexer* lex, Token* start, Memory_Arena* arena);
void odin_print_struct(Struct_Def* def, bool as_member_struct, int32 indent);
bool meta_type_equals(MetaType* a, MetaType* b);
void populate_meta_type(Struct_Member* member, i32 kind, Struct_Def* parent, MetaType* meta);
MetaType* get_types_in_struct(Struct_Def* def, MetaType* head, Memory_Arena* arena);
void print_meta_member(Meta_Member* member, char* prefix, char* suffix);
void print_struct_info(Struct_Def* def, char* prefix, char* suffix);
void print_reflection_data(Struct_Def* def);
void print_metaprogram_types();
void print_metaprogram_get_struct_info_proc();
int main(int argc, char** argv);
