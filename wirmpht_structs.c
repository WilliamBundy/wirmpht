
enum Struct_Kind
{
	StructKind_None,
	StructKind_Struct,
	StructKind_Union,
	StructKind_Member
};

#define StructMemberCapacity (256)

#ifndef WirmphtEnabled

struct Struct_Def
{
	char* name;
	char* parentname;
	char* pre_typedef_name;
	Struct_Kind kind;

	isize meta_index;
	char* meta_type_name;
	Hash namehash;

	bool is_anon_member;
	bool is_named_child;
	bool is_typedefed_struct;

	Struct_Member* members;
	Struct_Kind* member_kinds;
	isize member_count;


	Struct_Def* next;
}; 

struct Struct_Anon_Member
{
	Struct_Def def;
	char** array_sizes;
	isize array_levels;
};

struct Struct_Member_Var
{
	Hash namehash;
	char* name;
	char** terms;
	isize count;

	MetaType* type;
	int32 asterisk_count;

	char** array_sizes;
	isize array_levels;
};

union Struct_Member
{
	Struct_Anon_Member anon_struct;
	Struct_Member_Var member_var;
};
#endif

void print_indent(int32 indent)
{
	for(isize i = 0; i < indent; ++i) {
		printf("\t");
	}
}

void print_struct_names(Struct_Def* def, isize index, char* prefix, isize prefix_len, char* suffix, Struct_Def** all_structs, isize* counter, Memory_Arena* arena)
{
	if(def->kind == StructKind_None) return;
	isize chars = 0;
	
	if(counter != NULL) {
		isize local_counter = *counter;
		Struct_Def** local_struct = all_structs + local_counter;

		local_struct[0] = def;
		def->meta_index = local_counter;
		*counter = local_counter + 1;
	}

	char* buf = arena_push_array(Work_Arena, char, 256);
	if(index == -1) {
		chars = snprintf(buf, 256, "%.*s%s", prefix_len, prefix, def->name);
	} else {
		if(def->name[0] == '\0') {
			//truly anonymous
			chars = snprintf(buf, 256, "%.*s%s%d", 
					prefix_len, prefix,
					def->kind == StructKind_Struct ?
					"struct" : "union",
					index);
			def->is_anon_member = true;
		} else {
			//has a variable name
			chars = snprintf(buf, 256, "%.*s%s",
					prefix_len, prefix,
					def->name);
			def->is_anon_member = true;
		}
	}
	chars++;
	chars++;
	if(all_structs != NULL) {
		def->meta_type_name = buf;
	}
	printf("\t%s%s", buf, suffix);
	char* new_prefix = arena_push_array(arena, char, chars + 256);
	prefix_len = chars;
	if(index == -1) {
		snprintf(new_prefix, chars, "%.*s%s_", prefix_len, prefix, def->name);
	} else {
		if(def->name[0] == '\0') {
			//truly anonymous
			snprintf(new_prefix, chars, "%.*s%s%d_", 
					prefix_len, prefix,
					def->kind == StructKind_Struct ?
					"struct" : "union",
					index);
		} else {
			//has a variable name
			snprintf(new_prefix, chars, "%.*s%s_",
					prefix_len, prefix,
					def->name);
		}
	}

	isize subcount = 0;
	for(isize i = 0; i < def->member_count; ++i) {
		if(def->member_kinds[i] != StructKind_Member) {
			Struct_Anon_Member* var = &def->members[i].anon_struct;

			if(def->name[0] != '\0') {
				if(def->is_named_child) {
					var->def.parentname = def->parentname;
				} else {
					var->def.parentname = def->name;
				}
			} else {
				var->def.parentname = def->parentname;
			}
			
			print_struct_names(&var->def, subcount++, new_prefix, chars, suffix, all_structs, counter, arena);
		}
	}
}

void print_struct(Struct_Def* def, bool as_member_struct, int32 indent)
{
	if(def->name == NULL) return;
	print_indent(indent);
	if(as_member_struct) {
		if(def->kind == StructKind_Struct) {
			printf("struct {\n");
		} else if(def->kind == StructKind_Union) {
			printf("union {\n");
		}
	} else {
		if(def->is_typedefed_struct) {
			//TODO(will): this is a hack specific to my code.
			// It works, but should be fixed
			// We need to track the struct name AND the typedef'd name
			if(def->pre_typedef_name) {
				if(def->kind == StructKind_Struct) {
					printf("typedef struct %s\n", def->pre_typedef_name);
				} else if(def->kind == StructKind_Union) {
					printf("typedef union %s\n", def->pre_typedef_name);
				}
			} else {
				if(def->kind == StructKind_Struct) {
					printf("typedef struct %s_\n", def->name);
				} else if(def->kind == StructKind_Union) {
					printf("typedef union %s_\n", def->name);
				}
			}
		} else {
			if(def->kind == StructKind_Struct) {
				printf("struct %s\n", def->name);
			} else if(def->kind == StructKind_Union) {
				printf("union %s\n", def->name);
			}
		}
		print_indent(indent);
		printf("{\n");
	}
	indent++;
	for(isize i = 0; i < def->member_count; ++i) {
		if(def->member_kinds[i] == StructKind_Member) {
			Struct_Member_Var* var = &def->members[i].member_var;
			print_indent(indent);
			for(isize a = 0; a < var->count; ++a) {
				printf("%s ", var->terms[a]);
			}
			for(isize a = 0; a < var->asterisk_count; ++a) {
				printf("*");
			}
			printf("%s", def->members[i].member_var.name);
			for(isize a = 0; a < var->array_levels; ++a) {
				printf("[%s]", var->array_sizes[a]);
			}
			printf(";\n");
		} else {
			Struct_Anon_Member* var = &def->members[i].anon_struct;
			print_struct(&var->def, true, indent);
			printf("%s", var->def.name);

			for(isize a = 0; a < var->array_levels; ++a) {
				printf("[%s]", var->array_sizes[a]);
			}

			printf(";\n");
		}	
	}
	indent--;
	print_indent(indent);
	if(as_member_struct) {
		if(def->name[0] == '\0')
			printf("}");
		else 
			printf("} ");
	} else {
		if(def->is_typedefed_struct){
			printf("} %s;\n", def->name);
		} else {
			printf("};\n");
		}
	}

}



Token* parse_struct_member(Lexer* lex, Struct_Def* parent, Token* start, Memory_Arena* arena)
{
	Hash structhash = hash_literal("struct");
	Hash unionhash = hash_literal("union");
	Token* head = start;
	Token* next = head->next;
	
	Struct_Member* member = parent->members + parent->member_count;
	Struct_Kind* kind = parent->member_kinds + parent->member_count;

	if(head->kind == Token_Identifier) {
		bool contains_brace = false;
		Token* subhead = head;
		do { 
			if(subhead->kind == Token_OpenBrace) {
				contains_brace = true;
				break;
			} else if(subhead->kind == Token_Semicolon) {
				break;
			} else if(subhead->kind == Token_Asterisk) {
				break;
			}
		} while (subhead = subhead->next);


		if(contains_brace && head->hash == structhash || head->hash == unionhash) {
			lex->structs_count++;
			if(head->hash == unionhash) {
				*kind = StructKind_Union;
			} else {
				*kind = StructKind_Struct;
			}

			do {
 				head = head->next;
			} while(head->kind != Token_OpenBrace);
			head = head->next;
			Struct_Def def = {0};
			def.members = arena_push_array(arena, Struct_Member, StructMemberCapacity);
			def.member_kinds = arena_push_array(arena, Struct_Kind, StructMemberCapacity);
			def.kind = *kind;
			do {
				head = parse_struct_member(lex, &def, head, arena);
				head = head->next;
			} while(head->kind != Token_CloseBrace);

 			head = head->next;
			Struct_Anon_Member* var = &member->anon_struct;
			if(head->kind != Token_Identifier) {
				if(head->kind == Token_Semicolon) {
					def.name = "";
					def.is_anon_member = true;
					var->def = def;
					var->array_levels = 0;
					var->array_sizes = NULL;
					parent->member_count++;


					return head;
				} else {
					if(Metaprogram->verbose) fprintf(stderr, "ERROR: wanted identifier, got %d:[%.*s] \n", 
							head->kind, head->len, head->start);
				}
			} else {
				char* buf = arena_push_array(arena, char, 256);
				memcpy(buf, head->start, head->len);
				int len = head->len;
				next = head->next;
				buf[len] = '\0';
				def.name = buf;
				def.is_named_child = true;
				def.namehash = head->hash;
			}
			head = head->next;

			bool quit = false;
			do { 
				switch(head->kind) {
					case Token_Semicolon:
						quit = true;
						break;
					case Token_Comma:
						//TODO(will) Support commas like with regular variables.
						break;
					case Token_OpenBracket: {						
						head = head->next;
						if(var->array_levels == 0) {
							var->array_sizes = arena_push_array(arena, char*, 256);
						}
						char* buf = arena_push_array(arena, char, head->len + 1);
						memcpy(buf, head->start, head->len);
						int len = head->len;
						next = head->next;
						buf[len] = '\0';
						var->array_sizes[var->array_levels++] = buf;
					} break;
					default:
						break;
				}
				if(quit) break;
			} while(head = head->next);
			var->def = def;
		} else {
			*kind = StructKind_Member;
			Struct_Member_Var* var = &member->member_var;
			var->terms = arena_push_array(arena, char*, 256);
			var->count = 0;
			var->asterisk_count = 0;
			var->name = NULL;
			var->array_sizes = NULL;
			var->array_levels = 0;
			int32 mode = 0;
			bool quit = false;
			do {
				if(head->kind == Token_Comma) {
					if(mode == 0) {
						var->namehash = head->prev->hash;
						var->name = var->terms[--var->count];
					}
					parent->member_count++;
					Struct_Kind oldkind = *kind;
					member = parent->members + parent->member_count;
					kind = parent->member_kinds + parent->member_count;
					*kind = oldkind;
					member->member_var = *var;
					var = &member->member_var;
					var->asterisk_count = 0;
					var->array_levels = 0;
					var->array_sizes = NULL;
					var->name = NULL;
					mode = 1;
				} else if(head->kind == Token_Semicolon) {
					if(mode == 0) {
						var->name = var->terms[--var->count];
					}
					break;
				} else if(head->kind == Token_Identifier) {
					char* buf = arena_push_array(arena, char, head->len + 1);
					memcpy(buf, head->start, head->len);
					int len = head->len;
					next = head->next;
					buf[len] = '\0';
					if(mode == 0) {
						var->terms[var->count++] = buf;
					} else if(mode == 1) {
						var->namehash = head->hash;
						var->name = buf;
					}
				} else if(head->kind == Token_Asterisk) {
					var->asterisk_count++;
				} else if(head->kind == Token_OpenBracket) {
					head = head->next;
					if(var->array_levels == 0) {
						var->array_sizes = arena_push_array(arena, char*, 256);
					}
					char* buf = arena_push_array(arena, char, head->len + 1);
					memcpy(buf, head->start, head->len);
					int len = head->len;
					next = head->next;
					buf[len] = '\0';
					var->array_sizes[var->array_levels++] = buf;
				}
			} while(head = head->next);
		}
	}

	parent->member_count++;
	return head;
}


Struct_Def* find_struct_defs(Lexer* lex, Token* start, Memory_Arena* arena)
{
	Token* head = start;
	Token* next = NULL;

	Hash structhash = hash_literal("struct");
	Hash unionhash = hash_literal("union");
	Hash typedefhash = hash_literal("typedef");
	Struct_Def def = {0};

	Struct_Def* def_start = arena_push_struct(arena, Struct_Def);
	Struct_Def* def_head = def_start;

	int32 brace_depth = 0;
	Token* last_open, *last_closed;
	last_open = last_closed = NULL;
	do {
		head = parse_metaprogram_directive(head);
		if(head->kind == Token_OpenBrace) {
			brace_depth++;
			last_open = head;
		} else if(head->kind == Token_CloseBrace) {
			brace_depth--;
			last_closed = head;
		}

		if(brace_depth < 0) {
			if(Metaprogram->verbose) {
				fprintf(stderr, ">>> Brace Depth went negative at: %s line %d col %d \n", lex->files[head->location.file].filename, head->location.line, head->location.offset);
				if(last_open == NULL) {
					fprintf(stderr, ">>> No previous opening brace");
				} else {
					fprintf(stderr, ">>> Last Open Brace: %s line %d col %d \n", lex->files[last_open->location.file].filename, last_open->location.line, last_open->location.offset);
					fprintf(stderr, ">>> %.*s \n", last_open->len + 32, last_open->start);
				}

				if(last_closed == NULL) {
					fprintf(stderr, ">>> No previous closing brace");
				} else {
					fprintf(stderr, ">>> Last Closing Brace: %s line %d col %d \n", lex->files[last_closed->location.file].filename, last_closed->location.line, last_closed->location.offset);
					fprintf(stderr, ">>> %.*s \n", last_open->len + 32, last_open->start - 32);

				}
			}

			brace_depth = 0;
		}

		Struct_Kind kind = StructKind_None;
		bool is_typedef_struct = false;
		Token* pre_typedef_name = NULL;
		if(head->hash == typedefhash) {
			//OKAY: we need to parse these correctly.
			//types of typedef
			//typedef struct <name> <name>;
			//typedef struct <name> { ... } <name>;
			bool has_struct = false;
			bool has_brace = false;
			Token* last_head = head;
			Token* struct_head = head;
			if(head->next->kind == Token_Identifier && head->next->hash == structhash) {
				do {
					head = head->next;
					if(head->kind == Token_Identifier && head->hash == structhash) {
						has_struct = true;
						struct_head = head;
					} else if(head->kind == Token_Identifier) {
						pre_typedef_name = head;
						//fprintf(stderr, ">>> [%.*s]\n", head->len, head->start);
					} if(head->kind == Token_OpenBrace) {
						has_brace = true;
					} else if(head->kind == Token_Asterisk || head->kind == Token_OpenParen) {
						is_typedef_struct = false;
						struct_head = last_head;
					}

					is_typedef_struct = has_struct && has_brace;
					if(is_typedef_struct) {
						//fprintf(stderr, ">>> Found typedefd struct\n");
						break;
					}

				} while(head->kind != Token_Semicolon);
			}
			if(is_typedef_struct) head = struct_head;
			else {
				do {
					head = head->next;
				} while(head->kind != Token_Semicolon);
			}
		}

		if(brace_depth > 0 || head->kind != Token_Identifier) continue;

		if(head->hash == structhash) {
			kind = StructKind_Struct;
		} else if(head->hash == unionhash) {
			kind = StructKind_Union;
		}

		if(kind != StructKind_None) {
			// if(is_typedef_struct) __debugbreak();
			memset(&def, 0, sizeof(Struct_Def));
			def.kind = kind;
			def.members = arena_push_array(arena, Struct_Member, StructMemberCapacity);
			def.member_kinds = arena_push_array(arena, Struct_Kind, StructMemberCapacity);
			Token* subhead = head->next;
			do {
				if(subhead->kind == Token_Identifier) {
					//this is the name
					char* buf = arena_push_array(arena, char, subhead->len + 1);
					memcpy(buf, subhead->start, subhead->len);
					buf[subhead->len] = '\0';
					def.name = buf;
					def.namehash = subhead->hash;
				} else if(subhead->kind == Token_OpenBrace) {
					break;
				}
			} while (subhead = subhead->next);
			subhead = subhead->next;

			do {
				subhead = parse_struct_member(lex, &def, subhead, arena);
				subhead = subhead->next;
			} while(subhead->kind != Token_CloseBrace);

			if(is_typedef_struct) {
				subhead = subhead->next;
				if(subhead->kind == Token_Identifier) {
					//this is the name
					char* buf = arena_push_array(arena, char, subhead->len + 1);
					memcpy(buf, subhead->start, subhead->len);
					buf[subhead->len] = '\0';
					def.name = buf;
					def.namehash = subhead->hash;
				} 
			}

			lex->structs_count++;

			def.is_typedefed_struct = is_typedef_struct;

			*def_head = def;
			def_head->next = arena_push_struct(arena, Struct_Def); 
			def_head = def_head->next;
		}
	} while(head = head->next);
	if(Metaprogram->verbose)fprintf(stderr, "\n");

	return def_start;
}

void odin_print_struct(Struct_Def* def, bool as_member_struct, int32 indent)
{
	if(def->name == NULL) return;
	print_indent(indent);
	if(as_member_struct) {
		if(def->kind == StructKind_Struct) {
			printf("struct {\n");
		} else if(def->kind == StructKind_Union) {
			printf("struct #raw_union {\n");
		}
	} else {
		if(def->kind == StructKind_Struct) {
			printf("%s :: struct\n", def->name);
		} else if(def->kind == StructKind_Union) {
			printf("%s :: struct #raw_union\n", def->name);
		}
		print_indent(indent);
		printf("{\n");
	}
	indent++;
	for(isize i = 0; i < def->member_count; ++i) {
		if(def->member_kinds[i] == StructKind_Member) {
			Struct_Member_Var* var = &def->members[i].member_var;
			print_indent(indent);
			printf("%s: ", def->members[i].member_var.name);

			for(isize a = 0; a < var->array_levels; ++a) {
				printf("[%s]", var->array_sizes[a]);
			}
			for(isize a = 0; a < var->asterisk_count; ++a) {
				printf("^");
			}
			for(isize a = 0; a < var->count; ++a) {
				printf("%s", var->terms[a]);
			}
			printf(",\n");
		} else {
			Struct_Anon_Member* var = &def->members[i].anon_struct;
			printf("%s: ", var->def.name);
			for(isize a = 0; a < var->array_levels; ++a) {
				printf("[%s]", var->array_sizes[a]);
			}
			print_struct(&var->def, true, indent);
			printf(",\n");
		}	
	}
	indent--;
	print_indent(indent);
	printf("}");
	if(!as_member_struct) {
		printf(";");
	}
	printf("\n");

}






