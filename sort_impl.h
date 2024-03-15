#include <stdbool.h>

struct list_head;

typedef bool (*list_cmp_func_t)(void *, const char *, const char *);

void timsort(void *priv,
             struct list_head *head,
             list_cmp_func_t cmp,
             bool descend);