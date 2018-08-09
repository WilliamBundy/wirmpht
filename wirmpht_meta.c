
/* Serialization "what is this thing" rules
 * 	- If something is a pointer
 * 		- if this is the only pointer in the 'class'
 * 			- assume any member named count, len, length, capacity, size 
 * 			 could be the number.
 * 		- when initializing, if something has the name
 * 			"pointer(s)_{capacity, size}
 * 			- Assume this is the size to allocate
 * 		- if something has the name
 * 			"pointer(s)_{count, length}" 
 * 			- Assume this is the number of things to (de)serialize
 * 		- if $(tag, pointername:length) is present, use that, ignore
 * 			heuristic rules
 * 	- If something is a union
 * 		- if something matches the name
 * 			"unionname_{kind,type}"
 * 			use this to index sub-structs in the union
 * 		- If a union has a base-level array that is the same size 
 * 			as the union, and no kind/type variable is present, 
 * 			(de)serialize that
 *
 */



#ifndef WirmphtEnabled
struct MetaType
{
	Hash hash;
	char* name;
	bool ispointer;
	bool isunsigned;
	bool isvolatile;
	bool isconst;
	bool isarray;
	uint32 pointer_depth;

	isize type_index;

	MetaType* next;
};
#endif

bool meta_type_equals(MetaType* a, MetaType* b)
{
	if (a == NULL) return false;
	if (b == NULL) return false;
	return a->hash == b->hash;
}

void populate_meta_type(Struct_Member* member, i32 kind, Struct_Def* parent, MetaType* meta)
{
#define defhash(v) Hash v##hash = hash_literal(#v)
	defhash(const);
	defhash(volatile);
	defhash(unsigned);
	if(kind == StructKind_Member) {
		Struct_Member_Var* var = &member->member_var;
		for(isize i = 0; i < var->count; ++i) {
			char* term = var->terms[i];

			Hash termhash = hash_string(term, strlen(term));
			if(termhash == consthash) {
				meta->isconst = true;
			} else if(termhash == volatilehash) {
				meta->isvolatile = true;
			} else if(termhash == unsignedhash) {
				meta->isunsigned = true;
			} else {
				meta->name = term;
				meta->hash = termhash;
			}
			meta->ispointer = var->asterisk_count > 0;
			meta->pointer_depth = var->asterisk_count;
			meta->isarray = var->array_levels > 0;
			var->type = meta;
	 	}
	}
}

MetaType* get_types_in_struct(Struct_Def* def, MetaType* head, wb_MemoryArena* arena)
{
	for(isize i = 0; i < def->member_count; ++i) {
		if(def->member_kinds[i] == StructKind_Member) {
			populate_meta_type(def->members + i, def->member_kinds[i], def, head);
		} else {
			get_types_in_struct(&def->members[i].anon_struct.def, head, arena);
		}
		head->next = arena_push_struct(arena, MetaType); 
		head = head->next;
	}
	return head;
}

enum Meta_Flags
{
	MetaFlag_IsPointer = Flag(0),
	MetaFlag_IsUnsigned = Flag(1),
	MetaFlag_IsVolatile = Flag(2),
	MetaFlag_IsConst = Flag(3),
	MetaFlag_IsArray = Flag(4),
};

#ifndef WirmphtEnabled
struct Meta_Member
{
	uint64 flags;
	char* type_name;
	char* access_expr;
	uint32 pointer_depth;
	char* parentname;
	char* name;
};
#endif

void print_meta_member(Meta_Member* member, char* prefix, char* suffix)
{
	char* name;
	char* parentname;
	if(member->name[0] == '\0') {
		name = "nullempty";
		parentname = "Wirmpht_NullEmpty";
	} else {
		name = member->name;
		parentname = member->parentname;
	}

	isize namelen = Min(strlen("Meta_Type_"), strlen(member->type_name));
	bool is_toplevel = false;

	for(isize i = 0; i < namelen; ++i) {
		if(member->type_name[i] != "Meta_Type_"[i]) {
			is_toplevel = true;
			break;
		}
	}

	
	char* access_expr = member->name;
	/*
	if(!is_toplevel) {
		access_expr = member->type_name;
		int count = 0;
		while(*access_expr++) {
			if(*access_expr == '_') {
				count++;
			}

			if(count == 2) {
				access_expr++;
				break;
			}
		}
	}
	*/

	printf("%s{\"%s\", offsetof(%s, %s), %d, %s%s, %d}%s",
			prefix,
			member->name,
			parentname,
			access_expr,
			member->flags, 
			is_toplevel ? "Meta_Type_" : "",
			member->type_name,
			member->pointer_depth,
			suffix);
}

void print_struct_info(Struct_Def* def, char* prefix, char* suffix) 
{
	printf("%s{\n\t\t\"%s\", \n\t\t\"%s\", \n\t\t%s, \n\t\t%s_Members, \n\t\t%d\n\t}%s", 
			prefix,
			def->name,
			def->meta_type_name,
			def->meta_type_name,
			def->meta_type_name,
			def->member_count,
			suffix);

}

void print_reflection_data(Struct_Def* def)
{
	printf("Wirmpht_StructMember %s_Members[] = {\n", def->meta_type_name);
	for(isize i = 0; i < def->member_count; ++i) {
		Meta_Member m = {
			0
		};
		if(def->name[0] != '\0')
			if(def->is_named_child)
				m.parentname = def->parentname;
			else
				m.parentname = def->name;
		else 
			m.parentname = def->parentname;
		if(def->member_kinds[i] == StructKind_Member) {
			Struct_Member_Var* var = &def->members[i].member_var;
			MetaType* meta = var->type;
			if(meta == NULL) {
				fprintf(stderr, "Meta type was null on %s::%s\n", def->name, var->name);
				continue;
			}
	 		m.name = var->name;
			m.type_name = meta->name;
			if(meta->isarray) {
				m.pointer_depth = var->array_levels;
			} else {
				m.pointer_depth = var->asterisk_count;
			}

			if(meta->ispointer) Enable_Flag(m.flags, MetaFlag_IsPointer);
			if(meta->isunsigned) Enable_Flag(m.flags, MetaFlag_IsUnsigned);
			if(meta->isvolatile) Enable_Flag(m.flags, MetaFlag_IsVolatile);
			if(meta->isconst) Enable_Flag(m.flags, MetaFlag_IsConst);
			if(meta->isarray) Enable_Flag(m.flags, MetaFlag_IsArray);
		} else {
			Struct_Anon_Member* var = &def->members[i].anon_struct;
	 		m.name = var->def.name;
			m.type_name = var->def.meta_type_name;
			m.pointer_depth = var->array_levels;
			if(var->array_levels > 0) {
				m.flags = MetaFlag_IsArray;
			}
		}
		char* suffix = i == def->member_count - 1 ? "\n" : ",\n";
		m.access_expr = m.name;
		print_meta_member(&m, "\t", suffix);
	}
	printf("};\n\n");
}

void print_metaprogram_types()
{
	
		printf(
"typedef struct Wirmpht_NullEmpty Wirmpht_NullEmpty;\n"
"struct Wirmpht_NullEmpty\n"
"{\n"
"	int nothing;\n"
"};\n\n"
"typedef struct Wirmpht_StructMember Wirmpht_StructMember;\n"
"struct Wirmpht_StructMember\n"
"{\n"
"	char* name;\n"
"	isize offset;\n"
"	i32 flags;\n"
"	i32 type;\n"
"	i32 pointerDepth;\n"
"};\n\n"
"typedef struct Wirmpht_StructInfo Wirmpht_StructInfo;\n"
"struct Wirmpht_StructInfo\n"
"{\n"
"	char* name;\n"
"	char* metaName;\n"
"	isize index;\n"
"	Wirmpht_StructMember* members;\n"
"	i32 count;\n"
"};\n\n");
	return;
}

void print_metaprogram_get_struct_info_proc()
{
	/*
		printf(R"foo(
static inline const Meta_Struct_Info* get_struct_info(Meta_Type t) 
{
	return All_Meta_Struct_Info + t;
}
)foo");
*/
	return;

}
