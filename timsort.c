#include <stdint.h>
#include <stdlib.h>

#include "list.h"
#include "queue.h"
#include "sort_impl.h"

static inline size_t run_size(struct list_head *head)
{
    if (!head)
        return 0;
    if (!head->next)
        return 1;
    return (size_t) (head->next->prev);
}

struct pair {
    struct list_head *head, *next;
};

static size_t stk_size;

static struct list_head *merge(void *priv,
                               list_cmp_func_t cmp,
                               struct list_head *a,
                               struct list_head *b,
                               bool descend)
{
    struct list_head *head = NULL;
    struct list_head **tail = &head;
    struct list_head **node;

    for (node = NULL; a && b; *node = (*node)->next) {
        /* if equal, take 'a' -- important for sort stability */
        node = !(cmp(priv, list_entry(a, element_t, list)->value,
                     list_entry(b, element_t, list)->value) ^
                 descend)
                   ? &a
                   : &b;
        *tail = *node;
        tail = &(*tail)->next;
    }
    *tail = (struct list_head *) ((uintptr_t) a | (uintptr_t) b);
    return head;
}

static void build_prev_link(struct list_head *head,
                            struct list_head *tail,
                            struct list_head *list)
{
    tail->next = list;
    do {
        list->prev = tail;
        tail = list;
        list = list->next;
    } while (list);

    /* The final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

static void merge_final(void *priv,
                        list_cmp_func_t cmp,
                        struct list_head *head,
                        struct list_head *a,
                        struct list_head *b,
                        bool descend)
{
    struct list_head *tmp_head;
    struct list_head **tail = &tmp_head;
    struct list_head **node;

    for (node = NULL; a && b; *node = (*node)->next) {
        /* if equal, take 'a' -- important for sort stability */
        node = !(cmp(priv, list_entry(a, element_t, list)->value,
                     list_entry(b, element_t, list)->value) ^
                 descend)
                   ? &a
                   : &b;
        *tail = *node;
        tail = &(*tail)->next;
    }
    *tail = (struct list_head *) ((uintptr_t) a | (uintptr_t) b);

    /* Finish linking remainder of list b on to tail */
    build_prev_link(head, head, tmp_head);
}

static struct pair find_run(void *priv,
                            struct list_head *list,
                            list_cmp_func_t cmp,
                            bool descend)
{
    size_t len = 1;
    struct list_head *next = list->next, *head = list;
    struct pair result;

    if (!next) {
        result.head = head, result.next = next;
        return result;
    }

    if (cmp(priv, list_entry(list, element_t, list)->value,
            list_entry(next, element_t, list)->value) ^
        descend) {
        /* decending run, also reverse the list */
        struct list_head *prev = NULL;
        do {
            len++;
            list->next = prev;
            prev = list;
            list = next;
            next = list->next;
            head = list;
        } while (next && cmp(priv, list_entry(list, element_t, list)->value,
                             list_entry(next, element_t, list)->value) ^
                             descend);
        list->next = prev;
    } else {
        do {
            len++;
            list = next;
            next = list->next;
        } while (next && !(cmp(priv, list_entry(list, element_t, list)->value,
                               list_entry(next, element_t, list)->value) ^
                           descend));
        list->next = NULL;
    }
    head->prev = NULL;
    head->next->prev = (struct list_head *) len;
    result.head = head, result.next = next;
    return result;
}

static struct list_head *merge_at(void *priv,
                                  list_cmp_func_t cmp,
                                  struct list_head *at,
                                  bool descend)
{
    size_t len = run_size(at) + run_size(at->prev);
    struct list_head *prev = at->prev->prev;
    struct list_head *list = merge(priv, cmp, at->prev, at, descend);
    list->prev = prev;
    list->next->prev = (struct list_head *) len;
    --stk_size;
    return list;
}

static struct list_head *merge_force_collapse(void *priv,
                                              list_cmp_func_t cmp,
                                              struct list_head *tp,
                                              bool descend)
{
    while (stk_size >= 3) {
        if (run_size(tp->prev->prev) < run_size(tp)) {
            tp->prev = merge_at(priv, cmp, tp->prev, descend);
        } else {
            tp = merge_at(priv, cmp, tp, descend);
        }
    }
    return tp;
}

static struct list_head *merge_collapse(void *priv,
                                        list_cmp_func_t cmp,
                                        struct list_head *tp,
                                        bool descend)
{
    int n;
    while ((n = stk_size) >= 2) {
        if ((n >= 3 &&
             run_size(tp->prev->prev) <= run_size(tp->prev) + run_size(tp)) ||
            (n >= 4 && run_size(tp->prev->prev->prev) <=
                           run_size(tp->prev->prev) + run_size(tp->prev))) {
            if (run_size(tp->prev->prev) < run_size(tp)) {
                tp->prev = merge_at(priv, cmp, tp->prev, descend);
            } else {
                tp = merge_at(priv, cmp, tp, descend);
            }
        } else if (run_size(tp->prev) <= run_size(tp)) {
            tp = merge_at(priv, cmp, tp, descend);
        } else {
            break;
        }
    }

    return tp;
}

void timsort(void *priv,
             struct list_head *head,
             list_cmp_func_t cmp,
             bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    stk_size = 0;

    struct list_head *list = head->next, *tp = NULL;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    do {
        /* Find next run */
        struct pair result = find_run(priv, list, cmp, descend);
        result.head->prev = tp;
        tp = result.head;
        list = result.next;
        stk_size++;
        tp = merge_collapse(priv, cmp, tp, descend);
    } while (list);

    /* End of input; merge together all the runs. */
    tp = merge_force_collapse(priv, cmp, tp, descend);

    /* The final merge; rebuild prev links */
    struct list_head *stk0 = tp, *stk1 = stk0->prev;
    while (stk1 && stk1->prev)
        stk0 = stk0->prev, stk1 = stk1->prev;
    if (stk_size <= 1) {
        build_prev_link(head, head, stk0);
        return;
    }
    merge_final(priv, cmp, head, stk1, stk0, descend);
}