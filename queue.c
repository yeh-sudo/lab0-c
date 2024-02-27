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
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

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
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
