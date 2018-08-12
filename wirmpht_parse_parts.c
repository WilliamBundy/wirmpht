
void parse_number_tokens(Token* head)
{
	Token* next = head->next;
	if(head->kind == Token_Number) {
		if(next && next->kind == Token_Dot) {
			head->kind = Token_Float;
			head->len++;
			head->next = next->next;
			parse_number_tokens(head);
		} else {
			head->kind = Token_Integer;
		}
	} else if(head->kind == Token_Float) {
		if(next && next->kind == Token_Number) {
			head->len += next->len;
			head->next = next->next;
		}
	}
}

void parse_include_directive(Lexer* lex, Token* directive)
{
	wb_arenaStartTemp(tempArena);
	char* buf = wb_arenaPush(tempArena, directive->len + 1);
	memcpy(buf, directive->start, directive->len);
	buf[directive->len] = '\0';
	Token* head = wb_arenaPush(tempArena, sizeof(Token));
	Token* start = head;

	{
		Token t;
		Lexer_File f;
		f.head = buf;
		f.start = buf;
		while(lexer_get_token(NULL, &f, &t)) {
			*head = t;
			head->next = wb_arenaPush(tempArena, sizeof(Token));
			head = head->next;
		}
		head->next = NULL;
	}

	if(start->hash != hash_string("include", sizeof("include") - 1)) {
		wb_arenaEndTemp(tempArena);
		return;
	}

	head = start;
	do {
		if(head->kind == Token_String) {
			char* filename = wb_arenaPush(tempArena,  head->len + 1);
			memcpy(filename, head->start, head->len);
			filename[head->len] = '\0';
			Lexer_File* file = get_next_file(lex);
			Lexer_File* including = lex->files + directive->location.file;
			init_lexer_file(file, filename, including->filename, including->pathlen, workArena);

			if(file->start != NULL) {
				if(Metaprogram->verbose) fprintf(stderr, ">>> Adding file %s\n", filename);
				Token* new_file_head = wb_arenaPush(workArena, sizeof(Token));
				Token* new_file_start = new_file_head;
				Token* last = directive;
				Token t;
				while(lexer_get_token(lex, file, &t)) {
					*new_file_head = t;
					new_file_head->next = wb_arenaPush(workArena, sizeof(Token));
					new_file_head->prev = last;
					last = new_file_head;
					new_file_head = new_file_head->next;
				}
				Token* oldnext = directive->next;
				directive->next = new_file_start;
				new_file_start->prev = directive;
				new_file_head->next = oldnext;
				oldnext->prev = new_file_head;
			} else if (Metaprogram->verbose) {
				if(Metaprogram->verbose) fprintf(stderr, ">>> Couldn't add file %s\n", filename);
			}
			break;
		}
	} while(head = head->next);

	wb_arenaEndTemp(tempArena);
}


Token* parse_metaprogram_directive(Token* t) 
{
	if(t->kind != Token_MetaprogramDirective) return t;
	Token* head = t;

	
	Hash excludehash = hash_literal("exclude");
	Hash endhash = hash_literal("end");

	if(t->hash == excludehash) {
		do {
			if(head->kind == Token_MetaprogramDirective) {
				if(head->hash == endhash) {
					head = head->next;
					break;
				}
			}
		} while(head = head->next);
	}

	return head;
}

void parse_tokens(Lexer* lex, Token* start)
{
	Token* head = start;

	do {
		head = parse_metaprogram_directive(head);
		if(head->kind == Token_CompilerDirective && head->start[0] == 'i') {
			parse_include_directive(lex, head);
		}
	} while(head = head->next);

	head = start;

	do {
		Token* next;
		switch(head->kind) {
			case Token_DollarSign:
				head = parse_metaprogram_directive(head);
				break;
			case Token_Ampersand:
				next = head->next;
				if(next && next->kind == Token_Ampersand) {
					head->kind = Operator_LogicalAnd;
					head->len++;
					head->next = next->next;
				}
				break;
			case Token_Pipe:
				next = head->next;
				if(next && next->kind == Token_Pipe) {
					head->kind = Operator_LogicalOr;
					head->len++;
					head->next = next->next;
				}
				break;
			case Token_Equals:
				next = head->next;
				if(next && next->kind == Token_Equals) {
					head->kind = Operator_BooleanEquals;
					head->len++;
					head->next = next->next;
				}
				break;
			case Token_ExclamationMark:
				next = head->next;
				if(next && next->kind == Token_Equals) {
					head->kind = Operator_BooleanNotEquals;
					head->len++;
					head->next = next->next;
				}
				break;
			case Token_GreaterThan:
				next = head->next;
				if(next && next->kind == Token_Equals) {
					head->kind = Operator_BooleanGreaterEquals;
					head->len++;
					head->next = next->next;
				}
				break;
			case Token_LessThan:
				next = head->next;
				if(next && next->kind == Token_Equals) {
					head->kind = Operator_BooleanLessEquals;
					head->len++;
					head->next = next->next;
				}
				break;
			case Token_Number:
				parse_number_tokens(head);
				break;
			case Token_Minus:
				next = head->next;
				if(next && next->kind == Token_GreaterThan) {
					head->kind = Operator_PtrMemberAccess;
					head->len++;
					head->next = next->next;
				} else if(next && next->kind == Token_Minus) {
					head->kind = Operator_Decrement;
					head->len++;
					head->next = next->next;
				} else if(next && next->kind == Token_Number) {
					i32 prevkind = Token_Unknown;
					if(head->prev != NULL) {
						prevkind = head->prev->kind;
					}
					if(prevkind != Token_Number &&
							prevkind != Token_Integer && 
							prevkind != Token_Float && 
							prevkind != Token_Identifier ) {
						head->kind = Token_Number;
						head->len += next->len;
						head->next = next->next;
						next = head->next;
						parse_number_tokens(head);
					}
				}
				break;
			case Token_Plus:
				next = head->next;
				if(next && next->kind == Token_Plus) {
					head->kind = Operator_Increment;
					head->len++;
					head->next = next->next;
				}
			default:
				break;
		}
	} while(head = head->next);

}

bool match_pattern3(Token* head, i32 a, i32 b, i32 c)
{
	if(head->kind == a) {
		if(head->next != NULL)
		if(head->next->kind == b) {
			if(head->next->next != NULL)
			if(head->next->next->kind == c) {
				return true;
			}
		}
	}
	return false;
}
