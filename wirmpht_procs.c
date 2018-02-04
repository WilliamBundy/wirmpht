
#ifndef WirmphtEnabled
struct Proc_Arg
{
	char** terms;
	isize count;

	char* defaults;
};
#endif

void init_proc_arg(Proc_Arg* arg, isize count, Memory_Arena* arena)
{
	arg->terms = arena_push_array(arena, char*, count);
	arg->defaults = NULL;
	arg->count = 0;
}

#ifndef WirmphtEnabled
struct Proc_Prototype
{
	Token *start, *end;
	char** decorators;
	isize decorators_count;
	char* name;
	Proc_Arg* args;
	isize args_count;

	Proc_Prototype* next;
};
#endif

void print_proc_prototype(Proc_Prototype* p)
{
	if(p->name == NULL) return;
	//TODO(will) figure out how to prototype generated functions, maybe?
	// Gotta parse #define stuff.
	if(p->decorators_count <= 0) return;


	for(isize i = 0; i < p->decorators_count; ++i) {
		printf("%s ", p->decorators[i]);
	}
	printf("%s(", p->name);

	for(isize i = 0; i < p->args_count; ++i) {
		Proc_Arg* a = p->args + i;
		for(isize j = 0; j < a->count; ++j) {
			printf("%s", a->terms[j]);
			if(j == a->count - 1) {
				//TODO(will): there's something dumb with the way C++ handles default arguments
				// so, we don't. 
				a->defaults = NULL;
				if(a->defaults == NULL) {
					if(i != p->args_count - 1) printf(", ");
				} else {
					printf(" %s", a->defaults);
					if(i != p->args_count - 1) printf(" ");
				}
			} else {
				printf(" ");
			}
		}
	} 

	if(p->args_count != 0) {
		if(p->args[p->args_count - 1].defaults == NULL) 
			printf(");");
		else 
			printf(";");
	}

	printf("\n");


}

Proc_Prototype* find_proc_prototypes(Lexer* lex, Token* start, Memory_Arena* arena)
{
	//NOTE(will) will not find C-style "struct Type function() {"
	//TODO(will) Contains code for default arguments, which aren't generated -- delete
	Token* head = start;
	Token* next = NULL;
	Hash structhash = hash_literal("struct");
	Hash enumhash = hash_literal("enum");

	Proc_Prototype* proc_start = arena_push_struct(arena, Proc_Prototype);
	Proc_Prototype* proc_head = proc_start;

	do {
		head = parse_metaprogram_directive(head);
		if (head->kind == Token_Identifier) {
			if(head->hash != structhash && head->hash != enumhash) {
				//pattern: 
				//	mode 0 <identifiers...>
				//	mode 0 <open paren> 
				//	mode 1 <identifier, * identifier, comma...>
				//	mode 1 <close paren> 
				//	mode 2 <open brace>
				//proc = {0};
				Proc_Prototype proc = {0};
				proc.decorators = arena_push_array(arena, char*, 256);
				proc.args = arena_push_array(arena, Proc_Arg, 256);
				proc.start = head;

				Token* sub_head = head;
				int32 mode = 0;
				Token* default_args_token = NULL;
				int32 paren_depth = 0;
				Proc_Arg* arg = proc.args;
				init_proc_arg(arg, 256, arena);
				bool quit = false;
				do {
					if(sub_head->kind == Token_Semicolon) {
						//proc = {0};
						mode = -1;
						break;
					}
					sub_head = parse_metaprogram_directive(sub_head);
					switch(mode) {
						case 0:
							if (sub_head->kind == Token_Identifier) {
								char* buf = arena_push_array(arena, char, 256);
								memcpy(buf, sub_head->start, sub_head->len);
								int len = sub_head->len;
								next = sub_head->next;
								while(next->kind == Token_Asterisk) {
									buf[len++] = '*';
									next = next->next;
								}
								buf[len] = '\0';
								proc.decorators[proc.decorators_count++] = buf;
							} else if(sub_head->kind == Token_OpenParen) {
								arg = proc.args + proc.args_count++;
								init_proc_arg(arg, 256, arena);
								proc.name = proc.decorators[--proc.decorators_count];
								mode = 1;
							} else if(sub_head->kind == Token_Asterisk) {
								continue;
							}
							break;
						case 1:
							if(sub_head->kind == Token_CloseParen) {
								if(paren_depth == 0) {
									if(default_args_token != NULL) {
										char* start = default_args_token->start;
										isize len = sub_head->start - start + sub_head->len;
										char* buf = arena_push_array(arena, char, len+1);
										memcpy(buf, start, len);
										buf[len] = '\0';
										arg->defaults = buf;
										default_args_token = NULL;
									}
									mode = 2;
								} else {
									paren_depth--;
								}
							} else if(sub_head->kind == Token_Identifier) {
								if(default_args_token == NULL) {
									char* buf = arena_push_array(arena, char, 256);
									memcpy(buf, sub_head->start, sub_head->len);
									int len = sub_head->len;
									next = sub_head->next;
									while(next->kind == Token_Asterisk) {
										buf[len++] = '*';
										next = next->next;
									}
									buf[len] = '\0';
									arg->terms[arg->count++] = buf;
								}

							} else if (sub_head->kind == Token_Equals) {
								default_args_token = sub_head;
							} else if (sub_head->kind == Token_Comma) {
								if(paren_depth == 0) {
									if(default_args_token != NULL) {
										//TODO(will) code duplication
										char* start = default_args_token->start;
										isize len = sub_head->start - start + sub_head->len;
										char* buf = arena_push_array(arena, char, len+1);
										memcpy(buf, start, len);
										buf[len] = '\0';
										arg->defaults = buf;
										default_args_token = NULL;
									}
									arg = proc.args + proc.args_count++;
									init_proc_arg(arg, 256, arena);
								}
							} else if(sub_head->kind == Token_OpenParen) {
								paren_depth++;
							}
							break;
						case 2: 
							if(sub_head->kind == Token_OpenBrace) {
								int32 brace_depth = 0;
								sub_head = sub_head->next;
								do {
									if(sub_head->kind == Token_OpenBrace) {
										brace_depth++;
									} else if(sub_head->kind == Token_CloseBrace) {
										if(brace_depth == 0) {
									//		print_token(sub_head, start);
									//		printf("\n");
											quit = true;
											break;	
										}
										brace_depth--;
									}
								} while (sub_head = sub_head->next);
							}
							break;
					}
					if(quit) {
						head = sub_head;
						proc.end = head;
						*proc_head = proc;
						proc_head->next = arena_push_struct(arena, Proc_Prototype);
						lex->procedures_count++;
						proc_head = proc_head->next;
						break;
					}
					if (sub_head == NULL) break;
				} while(sub_head = sub_head->next);
			}
		}
		if (head == NULL) break;
	} while(head = head->next);
	return proc_start;
}


void odin_print_proc_prototype(Proc_Prototype* p)
{
	if(p->name == NULL) return;
	//TODO(will) figure out how to prototype generated functions, maybe?
	// Gotta parse #define stuff.
	if(p->decorators_count <= 0) return;

	printf("%s :: proc(", p->name);

	for(isize i = 0; i < p->args_count; ++i) {
		Proc_Arg* a = p->args + i;
		if(!a->terms[a->count - 1]) continue;
		printf("%s: ", a->terms[a->count - 1]);

		for(isize j = 0; j < a->count - 1; ++j)  {
			char* term = a->terms[j];


			char buf[256];
			isize t = 0, b = 0, c = 0, l = strlen(term);
			if(l == 6 && !strcmp("struct", term)) {
				continue;
			}

			if(l >= 4 && !strncmp("void", term, 4)) {
				printf("rawptr");
				if(j != a->count - 2) {
					printf(" ", buf);
				} 
				continue;	
			}
			
			for(c = 0; c < l; ++c) {
				if(term[c] == '*') {
					t++;
					buf[b++] = '^';
				}
			}

			l -= t;
			for(c = 0; c < l; ++c) {
				buf[b++] = term[c];
			}
			buf[b] = '\0';

			printf("%s", buf);
			if(j != a->count - 2) {
				printf(" ", buf);
			} 
		}

		if(i != p->args_count - 1)
			printf(", ");
	}

	printf(")");
	if(p->decorators_count == 1) {
		char* term = p->decorators[0];

		isize l = strlen(term);
		if(l == 4 && !strncmp("void", term, 4)) {
			printf(" ---;\n");
			return;
		}
	}

	{
		char* term = p->decorators[p->decorators_count-1];
		isize l = strlen(term);
		if(l == 4 && !strncmp("void", term, 4)) {
			;
		} else if(l > 4 && !strncmp("void", term, 4)) {
			printf(" -> rawptr");
		} else {
			char buf[256];
			isize t = 0, b = 0, c = 0, l = strlen(term);

			for(c = 0; c < l; ++c) {
				if(term[c] == '*') {
					t++;
					buf[b++] = '^';
				}
			}

			l -= t;
			for(c = 0; c < l; ++c) {
				buf[b++] = term[c];
			}
			buf[b] = '\0';

			printf(" -> %s", buf);
		}

		printf(" ---;\n");
	}
	return;


	
}

