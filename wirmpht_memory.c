

#define arena_push_struct(arena, type) ((type*)arena_push(arena, sizeof(type)))
#define arena_push_array(arena, type, count) ((type*)arena_push(arena, sizeof(type) * count))

#ifndef WirmphtEnabled
struct Memory_Arena
{
	uint8* data;
	isize capacity, head;
	isize temp_head;
	Memory_Arena* next;
};
#endif

static inline isize mem_align_4(isize p)
{
	usize mod = p & 3;
	return (mod ? p + 4 - mod : p);
}

void init_memory_arena(Memory_Arena* arena, usize size)
{
	arena->data = (uint8*)calloc(sizeof(uint8), size);
	arena->capacity = (isize)size;
	arena->head = (isize)arena->data;
	arena->temp_head = -1;
	arena->next = NULL;
}

uint8* arena_push(Memory_Arena* arena, isize size)
{
	isize old_head = arena->head;
	isize new_head = mem_align_4(size + arena->head);
	if((new_head - (isize)arena->data) > arena->capacity) {
		Log_Error("An arena was filled");
		return NULL;
	}
	arena->head = new_head;
	return (uint8*) old_head;

}

void start_temp_arena(Memory_Arena* arena)
{
	arena->temp_head = arena->head;
}

void end_temp_arena(Memory_Arena* arena)
{
	memset((void*)arena->data, 0, arena->head - arena->temp_head);
	arena->head = arena->temp_head;
	arena->temp_head = -1;
}

void clear_arena(Memory_Arena* arena)
{
	memset(arena->data, 0, arena->head - (isize)arena->data);
	//free(arena->data);
	//arena->data = (uint8*)calloc(sizeof(uint8), arena->capacity);
//#endif

	if(arena->data == NULL) {
		Log_Error("There was an error recommitting memory");
	}
	
	arena->head = (isize)arena->data;
	arena->temp_head = -1;
}

Memory_Arena* new_memory_arena(usize size, Memory_Arena* src)
{
	Memory_Arena* arena = (Memory_Arena*)arena_push(src, sizeof(Memory_Arena));
	init_memory_arena(arena, size);
	return arena;
}

