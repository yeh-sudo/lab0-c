#include "shuffle.h"


struct list_head *q_nth_node(struct list_head *head, int n)
{
    struct list_head *node = head->next;
    while (n--) {
        node = node->next;
    }
    return node;
}

void shuffle(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    int len = q_size(head);
    struct list_head *prev = NULL;
    struct list_head *old = NULL;
    struct list_head *new = head->prev;

    while (len >= 1) {
        int random = rand() % len;
        old = q_nth_node(head, random);
        prev = old->prev;
        if (old != new) {
            list_move(old, new);
            list_move(new, prev);
        }
        new = old->prev;
        len--;
    }
}