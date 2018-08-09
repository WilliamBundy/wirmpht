
/* TODO(will) 
 * 	--	Break main into multiple functions (so we can call jobs individually)
 * 	--	Switch to wb_alloc for memory
 * 	--	Clean up enum types; use ints everywhere instead :)
 * 	--	Rewrite lexer to use wb_alloc features
 * 		--	switched from linked list to array
 * 		--	we already load files synchronously, it's fine
 * 		--	instead of dumb token combining, just parse better
 * 	-- 	maybe, you know, do some real parsing instead of the wtf here
 *
 */ 

#define $(...)

$(exclude);
#include "wirmpht_defines.h"
#define WB_ALLOC_IMPLEMENTATION
#include "wb_alloc.h"
#include "wirmpht_generated.h"
$(end);

//#include "wirmpht_memory.c"
#include "wirmpht_sort.c"

#ifndef WirmphtEnabled
struct Metaprogram_Core
{
	bool verbose;
};
#endif
Metaprogram_Core* Metaprogram;
wb_MemoryArena* baseArena;
wb_MemoryArena* workArena;
wb_MemoryArena* tempArena;


char* load_file(char* filename, isize* size_out, wb_MemoryArena* arena)
{
	char* str = NULL;
	FILE* fp = fopen(filename, "rb");
	if(fp != NULL) {
		fseek(fp, 0L, SEEK_END);
		isize size = ftell(fp);
		rewind(fp);
		str = wb_arenaPush(arena, size + 1);
		fread(str, sizeof(char), size, fp);
		str[size] = '\0';
		fclose(fp);
		if(size_out != NULL) {
			*size_out = size;
		}
	} else {
		fprintf(stderr, ">>> Could not open file %s\n", filename);
	}
	return str;
}

#include "wirmpht_lexer.c"
#include "wirmpht_parse_parts.c"
#include "wirmpht_procs.c"
#include "wirmpht_structs.c"
#include "wirmpht_meta.c"
#include "wirmpht_jobs.c"


int main(int argc, char** argv)
{
	//Core + arena initialization
	wb_MemoryInfo memInfo = wb_getMemInfo();
	baseArena = wb_arenaBootstrap(memInfo);
	workArena = wb_arenaBootstrap(memInfo);
	tempArena = wb_arenaBootstrap(memInfo);
	//TODO(will): remove the whole metaprogram stuff
	Metaprogram = wb_arenaPush(base, sizeof(Metaprogram_Core));

	//argc = 2;
	//flags
	// -p --prototypes
	// -t --typedefs
	// -s --structs
	// -m --metadata
	bool pflag, tflag, sflag, mflag, vflag, oflag;
	pflag = tflag = sflag = mflag = vflag = oflag = false;
	char* file = NULL;
	for(isize i = 0; i < argc; ++i) {
		if(argv[i][0] == '-') {
			switch(argv[i][1]) {
				case 'p':
					pflag = true;
					break;
				case 't':
					tflag = true;
					break;
				case 's':
					sflag = true;
					break;
				case 'v':
					vflag = true;
					break;
				case 'm':
					mflag = true;
					break;
				case 'o':
					oflag = true;
					break;
				default:
					fprintf(stderr, "Unknown flag: %s \n", argv[i]);
			}
		} else {
			file = argv[i];
		}
	}
	
	bool istherework = pflag || tflag || sflag || mflag || oflag;
	if(file == NULL) {
		fprintf(stderr, "No input files.\n");
		return 1;
	} else if(!istherework) {
		fprintf(stderr, 
	"Error: No work defined. Use flags:\n"
	"-p		Generate function prototypes\n"
	"-t 	Generate struct/union/enum typedefs\n"
	"-s		Copy struct definitions from source\n"
	"-m		Generate reflection/introspection data\n"
	"-o		Generate odin header\n");
		return 1;
	}

	Metaprogram->verbose = vflag;
	if(file != NULL && istherework) {
		Lexer lex;
		init_lexer(&lex, 1024, Work_Arena);
		init_lexer_file(get_next_file(&lex), file, NULL, 0, Work_Arena);
		if(lex.files[0].head == NULL) {
			fprintf(stderr, "Error: File %s was null\n", lex.files[0].filename);
		}

		Token* head = arena_push_struct(Work_Arena, Token);
		Token* start = head;
		Token* last = NULL;
		Token t;
		while(lexer_get_token(&lex, lex.main_file, &t)) {
			*head = t;
			if(head->kind == Token_EndOfFile) {
				head->next = NULL;
				break;
			}
			head->next = arena_push_struct(Work_Arena, Token);
			head->prev = last;
			last = head;
			head = head->next;
		}
		head->prev = last;

		parse_tokens(&lex, start);
		Proc_Prototype* p = find_proc_prototypes(&lex, start, Work_Arena);
		Proc_Prototype* pstart = p;
		Struct_Def* structdef = find_struct_defs(&lex, start, Work_Arena);

		if(Metaprogram->verbose) fprintf(stderr, "Found %d procedures, "
				"%d structs \n\n",
				lex.procedures_count, lex.structs_count);

		Struct_Def* s_head = structdef;
		MetaType* type_start = arena_push_struct(Work_Arena, MetaType);
		MetaType* type_head = NULL;
		do {
			if(s_head->name == NULL) continue;

			if(type_head == NULL) {
				type_head = get_types_in_struct(s_head, type_start, Work_Arena);
			} else {
				type_head = get_types_in_struct(s_head, type_head, Work_Arena); 
			}
		} while(s_head = s_head->next);
		type_head = type_start;
		s_head = structdef;

		if(tflag) {
			w_printTypedefs(s_head);
			w_printEnumDefs(start);
		}

		if(tflag) {
			w_extractExistingTypedefs(start);
		}

		isize num_structs = 0;
		MetaType* unique_type_start = w_findUniqueTypes(type_start, s_head);

		Struct_Def** all_structs = arena_push_array(Work_Arena, 
				Struct_Def*, lex.structs_count + 16);

		if(mflag) {
			w_printMetaTypeEnum(structdef, all_structs, unique_type_start, &num_structs);
		}

		if(sflag) {
			w_printStructs(s_head);
		}

		if(mflag) {
			w_printMetadata(num_structs, all_structs);
		}

		if(pflag) {
			w_printProcs(pstart);
		}

		if(oflag) {
			w_printOdin(structdef, pstart);
		}
	}

	return 0;
}
