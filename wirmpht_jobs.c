
void w_printTypedefs(Struct_Def* s_head)
{
	//Print struct typedefs
	do {
		if(s_head->name == NULL) continue;
		if(s_head->is_typedefed_struct) {
			printf("%s %s_;\n", s_head->kind == StructKind_Struct ? "struct" : "union", s_head->name);
		} else {
			printf("typedef %s %s %s;\n", s_head->kind == StructKind_Struct ? "struct" : "union", s_head->name, s_head->name);
		}


	} while(s_head = s_head->next);
}

void w_printEnumDefs(Token* start)
{
	//Print Enum defs
	Token* head = start; Hash enumhash = hash_literal("enum");
	do {
		if(head->hash == enumhash) {
			if(head->next->kind == Token_Identifier) {
				printf("typedef enum %.*s %.*s;\n", head->next->len, head->next->start,
						head->next->len, head->next->start);
			}
		}

	} while(head = head->next);

}

void w_extractExistingTypedefs(Token* start)
{
	//copy typedefs from program
	Token* head = start;
	Hash typedefhash = hash_literal("typedef");
	Hash structhash = hash_literal("struct");
	Hash unionhash = hash_literal("union");
	do {
		head = parse_metaprogram_directive(head);
		if(head->hash == typedefhash) {
			Token* tdef = head;
			Token* end = NULL;
			do {
				if(head->kind == Token_Semicolon) {
					end = head;
					break;
				}
			} while(head = head->next);
			if(end != NULL) {
				Token* subhead = tdef->next;
				bool doprint = true;
#if 0
				do {
					if(subhead->hash == structhash || 
							subhead->hash == unionhash /*||
														 subhead->kind == Token_OpenParen ||
														 subhead->kind == Token_CloseParen*/) {

						doprint = false;
						break;	
					}

					subhead = subhead->next;
				} while(subhead != end);
#endif
				if(subhead->hash == structhash || subhead->hash == unionhash) {
					doprint = false;
				} else if(tdef->next->kind == Token_CloseBrace ||
						tdef->next->kind == Token_Comma ||
						tdef->next->kind == Token_CloseParen ||
						tdef->next->kind == Token_CloseBracket) {
					doprint = false;
				}
				if(doprint) {
					subhead = tdef;
					do {
						printf("%.*s", subhead->len, subhead->start);
						if(subhead->next->kind == Token_Identifier) {
							printf(" ");
						}
						if(subhead->next->kind == Token_Semicolon) {
							printf(";\n");
						}
						subhead = subhead->next;
					} while(subhead != end);

				}
			}
		}
	} while(head = head->next);
}

MetaType* w_findUniqueTypes(MetaType* type_start, Struct_Def* structdef)
{
		// find uniquetypes
		MetaType* unique_type_start = wb_arenaPush(workArena, sizeof(MetaType));
		*unique_type_start = *type_start;
		unique_type_start->next = NULL;
		MetaType* unique_type_head = unique_type_start;
		Struct_Def* s_head = structdef;
		MetaType* type_head = type_start;
		do {
			bool eq = true;
			unique_type_head = unique_type_start;
			do {
				if(meta_type_equals(type_head, unique_type_head)) {
					eq = false;
					break;
				}
				if(unique_type_head->next == NULL) break;
				unique_type_head = unique_type_head->next;
			} while(1);
			if(eq) {
				s_head = structdef;
				do {
					if(type_head->hash == s_head->namehash) {
						//fprintf(stderr, "%s %s \n", type_head->name, s_head->name);
						eq = false; 
						break;
					}
				} while(s_head = s_head->next);
				if(!eq) continue;
				unique_type_head->next = wb_arenaPush(workArena, sizeof(MetaType));
				unique_type_head = unique_type_head->next;
				*unique_type_head = *type_head;
				unique_type_head->next = NULL;
			}
		} while(type_head = type_head->next);
		return unique_type_start;
}

void w_printMetaTypeEnum(Struct_Def* structdef, Struct_Def** all_structs, MetaType* unique_type_start, isize* num_structs)
{
	//Print Meta_Type enum
	isize meta_index_counter = 0;
	Struct_Def* s_head = structdef;
	printf("enum Wirmpht_MetaType\n{\n");

	do {
		if(s_head->name == NULL) continue;
		wb_arenaStartTemp(tempArena);
		print_struct_names(s_head, -1, 
				"Meta_Type_", strlen("Meta_Type_"), ",\n",
				all_structs, &meta_index_counter, 
				tempArena);
		wb_arenaEndTemp(tempArena);
	} while(s_head = s_head->next);


	//TODO(will) This will change the index of system types
	// when new types are added. This makes the whole meta_type
	// system less robust as code changes with regard to serializiation
	MetaType* type_start = unique_type_start;
	MetaType* type_head = type_start;
	do {
		if(type_head->name == NULL) continue;
		printf("\tMeta_Type_%s,\n", type_head->name);
		type_head->type_index = meta_index_counter;
		meta_index_counter++;
	} while(type_head = type_head->next);

	printf("};\n");
	*num_structs = meta_index_counter;

	// Print names
	s_head = structdef;
	printf("const char* Meta_Type_Names[] = {\n");

	do {
		if(s_head->name == NULL) continue;
		wb_arenaStartTemp(tempArena);
		print_struct_names(s_head, -1, 
				"\"", strlen("\""), "\",\n",
				NULL, NULL, 
				tempArena);
		wb_arenaEndTemp(tempArena);
	} while(s_head = s_head->next);
	type_start = unique_type_start;
	type_head = type_start;
	do {
		if(type_head->name == NULL) continue;
		printf("\t\"%s\",\n", type_head->name);
	} while(type_head = type_head->next);
	printf("};\n");
	/*s_head = structdef;
	  type_start = unique_type_start;
	  type_head = type_start;*/
}

void w_printStructs(Struct_Def* structdef)
{
	printf("\n");
	// Print structs
	Struct_Def* s_head = structdef;
	do {
		if(s_head->name == NULL) continue;
		print_struct(s_head, 0, 0);
		printf("\n");
	} while(s_head = s_head->next);
}

void w_printMetadata(isize num_structs, Struct_Def** all_structs)
{
	// Print metadata
	print_metaprogram_types();
	for(isize i = 0; i < num_structs; ++i) {
		print_reflection_data(all_structs[i]);
	}
	printf("const Wirmpht_StructInfo Wirmpht_All_Structs[] = {\n");
	for(isize i = 0; i < num_structs; ++i) {
		print_struct_info(all_structs[i], "\t", ",\n");
	}
	printf("};\n\n");

	print_metaprogram_get_struct_info_proc();
	printf("\n");
}

void w_printProcs(Proc_Prototype* pstart)
{
	Proc_Prototype* p = pstart;
	Hash statichash = hash_literal("static");
	Hash inlinehash = hash_literal("inline");
	do {
		bool quit = false;
		for(isize i = 0; i < p->decorators_count; ++i) {
			Hash termhash = hash_string(p->decorators[i], strlen(p->decorators[i]));
			if(termhash == statichash || termhash == inlinehash) {
				quit = true;
				break;
			}
		}
		if(quit) continue;
		print_proc_prototype(p);
	} while (p = p->next);
}

void w_printOdin(Struct_Def* structdef, Proc_Prototype* pstart)
{
	printf("\n");
	// Print structs
	Struct_Def* s_head = structdef;
	do {
		if(s_head->name == NULL) continue;
		odin_print_struct(s_head, 0, 0);
		printf("\n");
	} while(s_head = s_head->next);
	printf("\n");
	Proc_Prototype* p = pstart;
	Hash statichash = hash_literal("static");
	Hash inlinehash = hash_literal("inline");
	printf("foreign NAME {\n");
	do {
		bool quit = false;
		for(isize i = 0; i < p->decorators_count; ++i) {
			Hash termhash = hash_string(p->decorators[i], strlen(p->decorators[i]));
			if(termhash == statichash || termhash == inlinehash) {
				quit = true;
				break;
			}
		}
		if(quit) continue;
		printf("\t");
		odin_print_proc_prototype(p);
	} while (p = p->next);
	printf("}\n");
}
