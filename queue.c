#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#ifndef strlcpy
#define strlcpy(dst, src, sz) snprintf((dst), (sz), "%s", (src))
#endif

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_q = malloc(sizeof(struct list_head));
    if (!new_q) {
        return NULL;
    }
    INIT_LIST_HEAD(new_q);
    return new_q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        free(entry->value);
        free(entry);
    }
    free(l);
}

element_t *create_new_element(char *s)
{
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return NULL;
    new_element->value = strdup(s);
    if (!new_element->value) {
        free(new_element);
        return NULL;
    }
    INIT_LIST_HEAD(&new_element->list);
    return new_element;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_element = create_new_element(s);
    if (!new_element)
        return false;
    list_add(&new_element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_element = create_new_element(s);
    if (!new_element)
        return false;
    list_add_tail(&new_element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *node = head->next;
    list_del(node);
    element_t *remove_element = list_entry(node, element_t, list);
    strlcpy(sp, remove_element->value, bufsize);
    return remove_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *node = head->prev;
    list_del(node);
    element_t *remove_element = list_entry(node, element_t, list);
    strlcpy(sp, remove_element->value, bufsize);
    return remove_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    int len = 0;
    struct list_head *node;
    list_for_each (node, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *fast = head->next->next;
    struct list_head *slow;
    list_for_each (slow, head) {
        if (fast == head || fast->next == head)
            break;
        fast = fast->next->next;
    }
    list_del(slow);
    element_t *del_element = list_entry(slow, element_t, list);
    free(del_element->value);
    free(del_element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    element_t *entry, *safe;
    bool add_entry = false;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (&safe->list == head)
            break;
        if (!strcmp(entry->value, safe->value)) {
            add_entry = true;
            list_del(&entry->list);
            free(entry->value);
            free(entry);
        } else {
            if (add_entry) {
                list_del(&entry->list);
                free(entry->value);
                free(entry);
            }
            add_entry = false;
        }
    }
    if (add_entry) {
        list_del(&entry->list);
        free(entry->value);
        free(entry);
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *node;
    for (node = head->next; node != head && node->next != head;
         node = node->next) {
        list_move(node, node->next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        list_del_init(node);
        list_add(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *tmp_head = q_new();
    struct list_head *reverse_head = q_new();
    struct list_head *node;
    int cnt;
    for (cnt = 0, node = head->next; node != head;) {
        if (cnt == k) {
            node = node->next;
            cnt = 0;
            list_cut_position(reverse_head, head, node->prev);
            q_reverse(reverse_head);
            list_splice_tail(tmp_head, reverse_head);
            continue;
        }
        node = node->next;
        cnt++;
    }
    list_splice(tmp_head, head);
}

/**
 * if s1 == s2: return 0
 * if s1 > s2: return positive
 * if s1 < s2: return negative
 */
bool cmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2) > 0 ? true : false;
}

void mergeTwoLists(struct list_head *L1, struct list_head *L2, bool descend)
{
    if (!L1 || !L2)
        return;
    struct list_head head;
    INIT_LIST_HEAD(&head);
    while (!list_empty(L1) && !list_empty(L2)) {
        element_t *e1 = list_first_entry(L1, element_t, list);
        element_t *e2 = list_first_entry(L2, element_t, list);
        struct list_head *node =
            !(cmp(e1->value, e2->value) ^ descend) ? L1->next : L2->next;
        list_move_tail(node, &head);
    }
    list_splice_tail_init(list_empty(L1) ? L2 : L1, &head);
    list_splice(&head, L1);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *slow = head;
    struct list_head *fast = head->next;
    for (; fast != head && fast->next != head; fast = fast->next->next)
        slow = slow->next;
    struct list_head left;
    list_cut_position(&left, head, slow);
    q_sort(&left, descend);
    q_sort(head, descend);
    mergeTwoLists(head, &left, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *entry, *safe;
    queue_contex_t *first_qctx = list_entry(head->next, queue_contex_t, chain);
    bool first = true;
    int sz = q_size(first_qctx->q);
    element_t *e1, *e2;
    list_for_each_entry_safe (entry, safe, head, chain) {
        if (first) {
            first = false;
            continue;
        }
        sz += q_size(entry->q);
        LIST_HEAD(tmp_head);
        while (!list_empty(first_qctx->q) && !list_empty(entry->q)) {
            e1 = list_entry(first_qctx->q->next, element_t, list);
            e2 = list_entry(entry->q->next, element_t, list);
            struct list_head *node = !(cmp(e1->value, e2->value) ^ descend)
                                         ? first_qctx->q->next
                                         : entry->q->next;
            list_del_init(node);
            list_add_tail(node, &tmp_head);
        }
        list_splice_tail_init(
            (list_empty(first_qctx->q) ? entry->q : first_qctx->q), &tmp_head);
        list_splice(&tmp_head, first_qctx->q);
    }
    return sz;
}
