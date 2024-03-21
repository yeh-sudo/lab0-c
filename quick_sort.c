#include <stdlib.h>

#include "queue.h"
#include "sort_impl.h"

struct list_head *random_pivot(struct list_head *head)
{
    int n = rand() % q_size(head);
    struct list_head *node;
    int cnt = 0;
    list_for_each (node, head) {
        if (cnt++ == n)
            break;
    }
    return node;
}

void quick_sort(void *priv,
                struct list_head *head,
                list_cmp_func_t cmp,
                bool descend)
{
    int n = q_size(head);
    int i = 0;
    int max_level = 2 * n;
    struct list_head **begin = malloc(sizeof(*begin) * max_level);
    struct list_head *result = q_new();

    begin[0] = head;

    while (i >= 0) {
        struct list_head *L = begin[i];
        struct list_head *left = q_new();
        struct list_head *right = q_new();
        if (!list_empty(L) && !list_is_singular(L)) {
            struct list_head *pivot = random_pivot(L);
            list_del_init(pivot);
            element_t *pivot_entry = list_entry(pivot, element_t, list);

            element_t *entry, *safe;
            list_for_each_entry_safe (entry, safe, L, list) {
                list_move(
                    &entry->list,
                    !(cmp(priv, pivot_entry->value, entry->value) ^ descend)
                        ? right
                        : left);
            }

            begin[i] = left;
            begin[i + 1] = q_new();
            list_add(pivot, begin[i + 1]);
            begin[i + 2] = right;
            i += 2;
        } else {
            if (list_is_singular(L))
                list_splice(L, result);
            i--;
        }
    }
    list_splice(result, head);
}
