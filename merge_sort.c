#include <stdlib.h>

#include "list.h"
#include "queue.h"
#include "sort_impl.h"

void merge(void *priv,
           struct list_head *L1,
           struct list_head *L2,
           list_cmp_func_t cmp,
           bool descend)
{
    if (!L1 || !L2)
        return;
    struct list_head head;
    INIT_LIST_HEAD(&head);
    while (!list_empty(L1) && !list_empty(L2)) {
        element_t *e1 = list_first_entry(L1, element_t, list);
        element_t *e2 = list_first_entry(L2, element_t, list);
        struct list_head *node =
            !(cmp(priv, e1->value, e2->value) ^ descend) ? L1->next : L2->next;
        list_move_tail(node, &head);
    }
    list_splice_tail_init(list_empty(L1) ? L2 : L1, &head);
    list_splice(&head, L1);
}

void merge_sort(void *priv,
                struct list_head *head,
                list_cmp_func_t cmp,
                bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *slow = head;
    struct list_head *fast = head->next;
    for (; fast != head && fast->next != head; fast = fast->next->next)
        slow = slow->next;
    struct list_head left;
    list_cut_position(&left, head, slow);
    merge_sort(priv, &left, cmp, descend);
    merge_sort(priv, head, cmp, descend);
    merge(priv, head, &left, cmp, descend);
}