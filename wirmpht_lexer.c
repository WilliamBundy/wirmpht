
typedef u64 Hash;

Hash hash_string(char* c, int len)
{
	Hash hash = 0;
	for(isize i = 0; i < len; ++i) {
		hash = c[i] + hash * 65599;
	}
	return hash;
}

static inline int32 dec_str_to_int(char* str, isize len)
{
	int32 result = 0;
	for(isize i = 0; i < len; ++i) {
		result = result * 10 + str[i] - '0';
	}
	return result;
}


#define hash_literal(c) hash_string(c, sizeof(c) - 1)

enum 
{
	Token_Unknown,

	Token_Semicolon,
	Token_Backslash,
	Token_OpenBrace,
	Token_CloseBrace,
	Token_OpenParen,
	Token_CloseParen,
	Token_OpenBracket,
	Token_CloseBracket,
	Token_Tilde,
	Token_ExclamationMark,

	Token_DollarSign,
	Token_Modulo,
	Token_Caret,
	Token_Ampersand,
	Token_Asterisk,
	Token_Pipe,
	Token_Dot,
	Token_Comma,
	Token_QuestionMark,
	Token_Colon,
	Token_Plus,
	Token_Minus,
	Token_Divide, 
	Token_Equals,
	Token_GreaterThan,
	Token_LessThan,
	
	Token_CompilerDirective,
	Token_MetaprogramDirective,
	Token_Char,
	Token_String,
	Token_Number,
	Token_Identifier,

	Token_Integer,
	Token_Float,

	Operator_LogicalAnd,
	Operator_LogicalOr,
	Operator_BooleanEquals,
	Operator_BooleanNotEquals,
	Operator_BooleanGreaterEquals,
	Operator_BooleanLessEquals,
	Operator_PtrMemberAccess,
	Operator_Decrement,
	Operator_Increment,

	Token_EndOfFile,
	i32_Count,
	
};

#ifndef WirmphtEnabled
struct Lexer_Location
{
	isize file;
	isize line;
	isize offset;
};

struct Token
{
	i32 kind;	
	char* start;
	isize len;
	Hash hash;

	Lexer_Location location;

	Token *prev, *next;
};
#endif

void print_token(Token* t, Token* start)
{
	isize sp = (isize) start;
	printf("%d<-[%d %d %.*s]->%d", ((isize)t->prev - sp) / sizeof(Token), ((isize)t - sp) / sizeof(Token), t->kind, t->len, t->start, ((isize)t->next - sp) / sizeof(Token));
}



#ifndef WirmphtEnabled
struct Lexer_File
{
	char* filename;
	isize pathlen;
	isize index;
	isize length;
	Lexer_Location location;
	char* start;
	char* head;

};
#endif

void init_lexer_file(Lexer_File* file, char* filename, char* prev_path, isize prev_path_len, wb_MemoryArena* arena)
{
	isize len = strlen(filename);
	char* filename_copy = wb_arenaPush(arena, len + prev_path_len+1);
	memcpy(filename_copy, prev_path, prev_path_len);
	memcpy(filename_copy + prev_path_len, filename, len);
	filename_copy[len + prev_path_len] = '\0';
	//printf(">>>>%s \n", filename_copy);
	len = strlen(filename_copy);
	isize pathlen = 0;
	isize extlen = 0;
	for(isize i = len - 1; i >= 0; --i) {
		if(filename_copy[i] == '.') {
			extlen = i;
		}
		if(filename_copy[i] == '/' || filename_copy[i] == '\\') {
			pathlen = i + 1;
			break;
		}
	}
	Hash exthash = hash_string(filename_copy + extlen, len - extlen);
	if(exthash != hash_literal(".i") && exthash != hash_literal(".c") && exthash != hash_literal(".cpp") && exthash != hash_literal(".h")) {
		if(Metaprogram->verbose) fprintf(stderr, "Skipping %s\n", filename_copy);
		file->pathlen = pathlen;
		file->filename = filename_copy;
		file->start = NULL;
		file->head = NULL;
		return;
	}
	file->pathlen = pathlen;
	file->filename = filename_copy;
	file->start = load_file(filename_copy, &file->length, arena);
	file->head = file->start;
}


#ifndef WirmphtEnabled
struct Lexer
{
	Lexer_File* main_file;

	Lexer_File* files;
	isize files_count, files_capacity;

	isize structs_count;
	isize procedures_count;
};
#endif

void init_lexer(Lexer* lex, isize file_capacity, wb_MemoryArena* arena)
{
	lex->files = wb_arenaPush(arena, sizeof(Lexer_File) * file_capacity);
	lex->files_count = 0;
	lex->files_capacity = file_capacity;
	lex->main_file = lex->files;
	lex->main_file->index = 0;

	lex->structs_count = 0;
	lex->procedures_count = 0;
}

Lexer_File* get_next_file(Lexer* lex)
{
	Lexer_File* file =  lex->files + lex->files_count++;
	file->index = lex->files_count - 1;
	return file;
}



bool is_space(char c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f';
}

bool is_number(char c)
{
	return c >= '0' &&  c <= '9'; 
}

bool is_valid_identifier(char c)
{
	return is_number(c) || ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')) || c == '_';
}


#define nextchar do { \
		if(f->head[0] == '\n') { \
			f->location.line++; \
			f->location.offset = 0; \
		} else { \
			f->location.offset++; \
		} \
		f->head++; \
} while(0) 

#define valid ((f->head[0] != '\0') && (f->head[0] != EOF))

bool lexer_get_token(Lexer* lexer, Lexer_File* f, Token* t) 
{ 
	while(valid) {
		if(is_space(f->head[0])) {
			nextchar;
		} else if(f->head[0] == '/') {
			if(f->head[1] == '*') {
				nextchar;
				nextchar;
				while(valid && !(f->head[0] == '*' &&
							f->head[1] == '/')) {
					nextchar;
				}
				nextchar;
				nextchar;
			} else if(f->head[1] == '/') {
				nextchar;
				nextchar;
				if(f->head[0] == '\\') {
					nextchar;
				}
				while(valid && (f->head[0] != '\n')) {
					nextchar;	
				}
				nextchar;
			} else {
				break;
			}
		} else {
			break;
		}
	}
	
	t->start = f->head;
	t->len = 1;
	t->kind = Token_Unknown;
	f->location.file = f->index;
	t->location = f->location;
	t->next = NULL;

	//printf("%d<%c> ", f->head[0], f->head[0]);
	bool hit = true;
	switch(f->head[0]) {
		case ';':
			t->kind = Token_Semicolon;
			break;
		case '\\':
			t->kind = Token_Backslash;
			break;
		case '{':
			t->kind = Token_OpenBrace;
			break;
		case '}':
			t->kind = Token_CloseBrace;
			break;
		case '(':
			t->kind = Token_OpenParen;
			break;
		case ')':
			t->kind = Token_CloseParen;
			break;
		case '[':
			t->kind = Token_OpenBracket;
			break;
		case ']':
			t->kind = Token_CloseBracket;
			break;
		case '~':
			t->kind = Token_Tilde;
			break;
		case '!':
			t->kind = Token_ExclamationMark;
			break;
		case '#':
			t->kind = Token_CompilerDirective;
			t->location.offset++;
			t->start++;
			nextchar;
			while(valid && (f->head[0] != '\n')) {
				if(f->head[0] == '/') {
					if(f->head[1] == '/') {
						break;
					} else if(f->head[1] == '*') {
						while(valid) {
							if(f->head[0] == '*' && f->head[1] == '/') {
								break;
							}
							nextchar;
						}
					}
				} else if(f->head[0] == '\\') {
					nextchar;
				}
				nextchar;
			}
			t->len = f->head - t->start;
			break;
		case '$': {
			char* oldhead = f->head;
			nextchar;
			while(valid) {
				if(is_space(f->head[0])) {
					nextchar;
				} else if(f->head[0] == '/') {
					if(f->head[1] == '*') {
						nextchar;
						nextchar;
						while(valid && !(f->head[0] == '*' &&
									f->head[1] == '/')) {
							nextchar;
						}
						nextchar;
						nextchar;
					} else if(f->head[1] == '/') {
						nextchar;
						nextchar;
						if(f->head[0] == '\\') {
							nextchar;
						}
						while(valid && (f->head[0] != '\n')) {
							nextchar;	
						}
						nextchar;
					} else {
						break;
					}
				} else {
					break;
				}
			}
			if(f->head[0] == '(') {
				t->kind = Token_MetaprogramDirective;
				nextchar;
				t->start = f->head;
				t->location = f->location;
				int brace_depth = 0;
				while(brace_depth >= 0) {
					nextchar;
					if(f->head[0] == '(') {
						brace_depth++;
					} else if (f->head[0] == ')') {
						brace_depth--;
					}
					if(!valid) {
						break;
					}
				}
				t->len = f->head - t->start;
			} else {
				t->kind = Token_DollarSign;
			}


		} break;
		case '%':
			t->kind = Token_Modulo;
			break;
		case '^':
			t->kind = Token_Caret;
			break;
		case '&':
			t->kind = Token_Ampersand;
			break;
		case '*':
			t->kind = Token_Asterisk;
			break;
		case '|':
			t->kind = Token_Pipe;
			break;
		case '.':
			t->kind = Token_Dot;
			break;
		case ',':
			t->kind = Token_Comma;
			break;
		case '?':
			t->kind = Token_QuestionMark;
			break;
		case ':':
			t->kind = Token_Colon;
			break;
		case '+':
			t->kind = Token_Plus;
			break;
		case '-':
			t->kind = Token_Minus;
			break;
		case '/':
			t->kind = Token_Divide;
			break;
		case '=':
			t->kind = Token_Equals;
			break;
		case '>':
			t->kind = Token_GreaterThan;
			break;
		case '<':
			t->kind = Token_LessThan;
			break;
		default:
			hit = false;
			break;
	}
	if(hit) {
		nextchar;
	} else switch(f->head[0]) {		
		case '\'':
			t->kind = Token_Char;
			t->location.offset++;
			t->start++;
			nextchar;
			while(valid) {
				nextchar;
				if(f->head[0] == '\'') {
					if(f->head[-1] != '\\') {
						break;
					} else {
						if(f->head[-2] == '\\') {
							break;
						}
					}
				}
			}
			t->len = f->head - t->start;
			nextchar;
			break;
		case '"':
			t->kind = Token_String;
			t->location.offset++;
			t->start++;
			nextchar;
			while(valid) {
				if(f->head[0] == '"') {
					char* last = f->head - 1;
					if(last[0] != '\\') {
						break;
					}
				}
				nextchar;

				/*if(f->head[0] == '\n') {
					
					fprintf(stderr, ">>> Error, encountered newline in string literal\n");
					fprintf(stderr, ">>> line %d, char %d\n", f->location.line, f->location.offset);
					fprintf(stderr, ">>> %.*s \n", f->head - t->start, t->start);
					break;
				}*/
			}
			t->len = f->head - t->start;
			nextchar;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			t->kind = Token_Number;
			while(valid && is_number(f->head[0])) {
				nextchar;
			}
			t->len = f->head - t->start;
			break;
		case '\0':
		case EOF:
			t->kind = Token_EndOfFile;
			return false;
		default:
			if(is_valid_identifier(f->head[0])) {
				t->kind = Token_Identifier;
				while(is_valid_identifier(f->head[0])) {
					nextchar;
				}
				t->len = f->head - t->start;
			} else {
				t->kind = Token_Unknown;
			}
			break;
	}
	t->hash = hash_string(t->start, t->len);

	return true;
}
#undef nextchar
#undef valid

