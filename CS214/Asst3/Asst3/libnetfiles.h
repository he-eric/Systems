#include <stdlib.h>

int set_connection();
int time_out(int);
int netserverinit(char*);
int netopen(const char*, int);
ssize_t netread(int, void*, size_t);
char* fill_with_spaces(char*);
ssize_t netwrite(int, const void*, size_t);
int netclose(int);
