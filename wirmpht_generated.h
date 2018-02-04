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
typedef enum Token_Kind Token_Kind;
typedef enum Struct_Kind Struct_Kind;
typedef enum Meta_Flags Meta_Flags;
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
	Meta_Type_Token_Kind,
	Meta_Type_char,
	Meta_Type_Hash,
	Meta_Type_Struct_Kind,
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
	"Token_Kind",
	"char",
	"Hash",
	"Struct_Kind",
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
	Token_Kind kind;
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
	Struct_Kind kind;
	isize meta_index;
	char *meta_type_name;
	Hash namehash;
	bool is_anon_member;
	bool is_named_child;
	bool is_typedefed_struct;
	Struct_Member *members;
	Struct_Kind *member_kinds;
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

Wirmpht_StructMember Meta_Type_Memory_Arena_Members[] = {
	{"data", offsetof(Memory_Arena, data), 1, Meta_Type_uint8, 1},
	{"capacity", offsetof(Memory_Arena, capacity), 0, Meta_Type_isize, 0},
	{"head", offsetof(Memory_Arena, head), 0, Meta_Type_isize, 0},
	{"temp_head", offsetof(Memory_Arena, temp_head), 0, Meta_Type_isize, 0},
	{"next", offsetof(Memory_Arena, next), 1, Meta_Type_Memory_Arena, 1}
};

Wirmpht_StructMember Meta_Type_Metaprogram_Core_Members[] = {
	{"verbose", offsetof(Metaprogram_Core, verbose), 0, Meta_Type_bool, 0},
	{"base_arena", offsetof(Metaprogram_Core, base_arena), 0, Meta_Type_Memory_Arena, 0},
	{"work_arena", offsetof(Metaprogram_Core, work_arena), 0, Meta_Type_Memory_Arena, 0},
	{"temp_arena", offsetof(Metaprogram_Core, temp_arena), 0, Meta_Type_Memory_Arena, 0}
};

Wirmpht_StructMember Meta_Type_Lexer_Location_Members[] = {
	{"file", offsetof(Lexer_Location, file), 0, Meta_Type_isize, 0},
	{"line", offsetof(Lexer_Location, line), 0, Meta_Type_isize, 0},
	{"offset", offsetof(Lexer_Location, offset), 0, Meta_Type_isize, 0}
};

Wirmpht_StructMember Meta_Type_Token_Members[] = {
	{"kind", offsetof(Token, kind), 0, Meta_Type_Token_Kind, 0},
	{"start", offsetof(Token, start), 1, Meta_Type_char, 1},
	{"len", offsetof(Token, len), 0, Meta_Type_isize, 0},
	{"hash", offsetof(Token, hash), 0, Meta_Type_Hash, 0},
	{"location", offsetof(Token, location), 0, Meta_Type_Lexer_Location, 0},
	{"prev", offsetof(Token, prev), 1, Meta_Type_Token, 1},
	{"next", offsetof(Token, next), 1, Meta_Type_Token, 1}
};

Wirmpht_StructMember Meta_Type_Lexer_File_Members[] = {
	{"filename", offsetof(Lexer_File, filename), 1, Meta_Type_char, 1},
	{"pathlen", offsetof(Lexer_File, pathlen), 0, Meta_Type_isize, 0},
	{"index", offsetof(Lexer_File, index), 0, Meta_Type_isize, 0},
	{"length", offsetof(Lexer_File, length), 0, Meta_Type_isize, 0},
	{"location", offsetof(Lexer_File, location), 0, Meta_Type_Lexer_Location, 0},
	{"start", offsetof(Lexer_File, start), 1, Meta_Type_char, 1},
	{"head", offsetof(Lexer_File, head), 1, Meta_Type_char, 1}
};

Wirmpht_StructMember Meta_Type_Lexer_Members[] = {
	{"main_file", offsetof(Lexer, main_file), 1, Meta_Type_Lexer_File, 1},
	{"files", offsetof(Lexer, files), 1, Meta_Type_Lexer_File, 1},
	{"files_count", offsetof(Lexer, files_count), 0, Meta_Type_isize, 0},
	{"files_capacity", offsetof(Lexer, files_capacity), 0, Meta_Type_isize, 0},
	{"structs_count", offsetof(Lexer, structs_count), 0, Meta_Type_isize, 0},
	{"procedures_count", offsetof(Lexer, procedures_count), 0, Meta_Type_isize, 0}
};

Wirmpht_StructMember Meta_Type_Proc_Arg_Members[] = {
	{"terms", offsetof(Proc_Arg, terms), 1, Meta_Type_char, 2},
	{"count", offsetof(Proc_Arg, count), 0, Meta_Type_isize, 0},
	{"defaults", offsetof(Proc_Arg, defaults), 1, Meta_Type_char, 1}
};

Wirmpht_StructMember Meta_Type_Proc_Prototype_Members[] = {
	{"start", offsetof(Proc_Prototype, start), 1, Meta_Type_Token, 1},
	{"end", offsetof(Proc_Prototype, end), 1, Meta_Type_Token, 1},
	{"decorators", offsetof(Proc_Prototype, decorators), 1, Meta_Type_char, 2},
	{"decorators_count", offsetof(Proc_Prototype, decorators_count), 0, Meta_Type_isize, 0},
	{"name", offsetof(Proc_Prototype, name), 1, Meta_Type_char, 1},
	{"args", offsetof(Proc_Prototype, args), 1, Meta_Type_Proc_Arg, 1},
	{"args_count", offsetof(Proc_Prototype, args_count), 0, Meta_Type_isize, 0},
	{"next", offsetof(Proc_Prototype, next), 1, Meta_Type_Proc_Prototype, 1}
};

Wirmpht_StructMember Meta_Type_Struct_Def_Members[] = {
	{"name", offsetof(Struct_Def, name), 1, Meta_Type_char, 1},
	{"parentname", offsetof(Struct_Def, parentname), 1, Meta_Type_char, 1},
	{"pre_typedef_name", offsetof(Struct_Def, pre_typedef_name), 1, Meta_Type_char, 1},
	{"kind", offsetof(Struct_Def, kind), 0, Meta_Type_Struct_Kind, 0},
	{"meta_index", offsetof(Struct_Def, meta_index), 0, Meta_Type_isize, 0},
	{"meta_type_name", offsetof(Struct_Def, meta_type_name), 1, Meta_Type_char, 1},
	{"namehash", offsetof(Struct_Def, namehash), 0, Meta_Type_Hash, 0},
	{"is_anon_member", offsetof(Struct_Def, is_anon_member), 0, Meta_Type_bool, 0},
	{"is_named_child", offsetof(Struct_Def, is_named_child), 0, Meta_Type_bool, 0},
	{"is_typedefed_struct", offsetof(Struct_Def, is_typedefed_struct), 0, Meta_Type_bool, 0},
	{"members", offsetof(Struct_Def, members), 1, Meta_Type_Struct_Member, 1},
	{"member_kinds", offsetof(Struct_Def, member_kinds), 1, Meta_Type_Struct_Kind, 1},
	{"member_count", offsetof(Struct_Def, member_count), 0, Meta_Type_isize, 0},
	{"next", offsetof(Struct_Def, next), 1, Meta_Type_Struct_Def, 1}
};

Wirmpht_StructMember Meta_Type_Struct_Anon_Member_Members[] = {
	{"def", offsetof(Struct_Anon_Member, def), 0, Meta_Type_Struct_Def, 0},
	{"array_sizes", offsetof(Struct_Anon_Member, array_sizes), 1, Meta_Type_char, 2},
	{"array_levels", offsetof(Struct_Anon_Member, array_levels), 0, Meta_Type_isize, 0}
};

Wirmpht_StructMember Meta_Type_Struct_Member_Var_Members[] = {
	{"namehash", offsetof(Struct_Member_Var, namehash), 0, Meta_Type_Hash, 0},
	{"name", offsetof(Struct_Member_Var, name), 1, Meta_Type_char, 1},
	{"terms", offsetof(Struct_Member_Var, terms), 1, Meta_Type_char, 2},
	{"count", offsetof(Struct_Member_Var, count), 0, Meta_Type_isize, 0},
	{"type", offsetof(Struct_Member_Var, type), 1, Meta_Type_MetaType, 1},
	{"asterisk_count", offsetof(Struct_Member_Var, asterisk_count), 0, Meta_Type_int32, 0},
	{"array_sizes", offsetof(Struct_Member_Var, array_sizes), 1, Meta_Type_char, 2},
	{"array_levels", offsetof(Struct_Member_Var, array_levels), 0, Meta_Type_isize, 0}
};

Wirmpht_StructMember Meta_Type_Struct_Member_Members[] = {
	{"anon_struct", offsetof(Struct_Member, anon_struct), 0, Meta_Type_Struct_Anon_Member, 0},
	{"member_var", offsetof(Struct_Member, member_var), 0, Meta_Type_Struct_Member_Var, 0}
};

Wirmpht_StructMember Meta_Type_MetaType_Members[] = {
	{"hash", offsetof(MetaType, hash), 0, Meta_Type_Hash, 0},
	{"name", offsetof(MetaType, name), 1, Meta_Type_char, 1},
	{"ispointer", offsetof(MetaType, ispointer), 0, Meta_Type_bool, 0},
	{"isunsigned", offsetof(MetaType, isunsigned), 0, Meta_Type_bool, 0},
	{"isvolatile", offsetof(MetaType, isvolatile), 0, Meta_Type_bool, 0},
	{"isconst", offsetof(MetaType, isconst), 0, Meta_Type_bool, 0},
	{"isarray", offsetof(MetaType, isarray), 0, Meta_Type_bool, 0},
	{"pointer_depth", offsetof(MetaType, pointer_depth), 0, Meta_Type_uint32, 0},
	{"type_index", offsetof(MetaType, type_index), 0, Meta_Type_isize, 0},
	{"next", offsetof(MetaType, next), 1, Meta_Type_MetaType, 1}
};

Wirmpht_StructMember Meta_Type_Meta_Member_Members[] = {
	{"flags", offsetof(Meta_Member, flags), 0, Meta_Type_uint64, 0},
	{"type_name", offsetof(Meta_Member, type_name), 1, Meta_Type_char, 1},
	{"access_expr", offsetof(Meta_Member, access_expr), 1, Meta_Type_char, 1},
	{"pointer_depth", offsetof(Meta_Member, pointer_depth), 0, Meta_Type_uint32, 0},
	{"parentname", offsetof(Meta_Member, parentname), 1, Meta_Type_char, 1},
	{"name", offsetof(Meta_Member, name), 1, Meta_Type_char, 1}
};

const Wirmpht_StructInfo Wirmpht_All_Structs[] = {
	{
		"Memory_Arena", 
		"Meta_Type_Memory_Arena", 
		Meta_Type_Memory_Arena, 
		Meta_Type_Memory_Arena_Members, 
		5
	},
	{
		"Metaprogram_Core", 
		"Meta_Type_Metaprogram_Core", 
		Meta_Type_Metaprogram_Core, 
		Meta_Type_Metaprogram_Core_Members, 
		4
	},
	{
		"Lexer_Location", 
		"Meta_Type_Lexer_Location", 
		Meta_Type_Lexer_Location, 
		Meta_Type_Lexer_Location_Members, 
		3
	},
	{
		"Token", 
		"Meta_Type_Token", 
		Meta_Type_Token, 
		Meta_Type_Token_Members, 
		7
	},
	{
		"Lexer_File", 
		"Meta_Type_Lexer_File", 
		Meta_Type_Lexer_File, 
		Meta_Type_Lexer_File_Members, 
		7
	},
	{
		"Lexer", 
		"Meta_Type_Lexer", 
		Meta_Type_Lexer, 
		Meta_Type_Lexer_Members, 
		6
	},
	{
		"Proc_Arg", 
		"Meta_Type_Proc_Arg", 
		Meta_Type_Proc_Arg, 
		Meta_Type_Proc_Arg_Members, 
		3
	},
	{
		"Proc_Prototype", 
		"Meta_Type_Proc_Prototype", 
		Meta_Type_Proc_Prototype, 
		Meta_Type_Proc_Prototype_Members, 
		8
	},
	{
		"Struct_Def", 
		"Meta_Type_Struct_Def", 
		Meta_Type_Struct_Def, 
		Meta_Type_Struct_Def_Members, 
		14
	},
	{
		"Struct_Anon_Member", 
		"Meta_Type_Struct_Anon_Member", 
		Meta_Type_Struct_Anon_Member, 
		Meta_Type_Struct_Anon_Member_Members, 
		3
	},
	{
		"Struct_Member_Var", 
		"Meta_Type_Struct_Member_Var", 
		Meta_Type_Struct_Member_Var, 
		Meta_Type_Struct_Member_Var_Members, 
		8
	},
	{
		"Struct_Member", 
		"Meta_Type_Struct_Member", 
		Meta_Type_Struct_Member, 
		Meta_Type_Struct_Member_Members, 
		2
	},
	{
		"MetaType", 
		"Meta_Type_MetaType", 
		Meta_Type_MetaType, 
		Meta_Type_MetaType_Members, 
		10
	},
	{
		"Meta_Member", 
		"Meta_Type_Meta_Member", 
		Meta_Type_Meta_Member, 
		Meta_Type_Meta_Member_Members, 
		6
	},
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
void populate_meta_type(Struct_Member* member, Struct_Kind kind, Struct_Def* parent, MetaType* meta);
MetaType* get_types_in_struct(Struct_Def* def, MetaType* head, Memory_Arena* arena);
void print_meta_member(Meta_Member* member, char* prefix, char* suffix);
void print_struct_info(Struct_Def* def, char* prefix, char* suffix);
void print_reflection_data(Struct_Def* def);
void print_metaprogram_types();
void print_metaprogram_get_struct_info_proc();
int main(int argc, char** argv);
