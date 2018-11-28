#define MEMORY_SIZE 5000
#define HEADER_SIZE 2
#define MASK 32767

#define malloc(x) mymalloc(x, __FILE__, __LINE__)
#define free( x ) myfree( x, __FILE__, __LINE__ )

#define GET(p) (*(unsigned short *)p)
#define PUT(p, val) (GET(p)) = (val)
#define PACK(size, alloc) (size | (alloc<<15))



//extern char my_heap[MEMORY_SIZE];

void *mymalloc(size_t, char *, int);
void *mymalloc_helper(size_t);
void myfree(void *, char*, int);
void coalesce();
void coalesce_helper(char *, char*);
void initmem();
unsigned short get_size(char* );
unsigned short get_alloc(char* );
void *split_block(char *, size_t);
void error(char *, int, char *);
void printBlocks();
