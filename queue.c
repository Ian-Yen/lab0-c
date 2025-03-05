#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new = malloc(sizeof(struct list_head));
    if (!new)
        return NULL;

    INIT_LIST_HEAD(new);

    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head) {
        return;
    }

    element_t *pos = NULL, *n;

    list_for_each_entry_safe (pos, n, head, list) {
        q_release_element(pos);
    }

    free(head);
}

bool q_insert(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *ele = malloc(sizeof(element_t));
    if (!ele)
        return false;

    ele->value = strdup(s);

    if (!ele->value) {
        free(ele);
        return false;
    }

    list_add(&ele->list, head);

    return true;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    return q_insert(head, s);
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    return q_insert(head->prev, s);
}

element_t *q_remove(struct list_head *head, char *sp, size_t bufsize)
{
    element_t *ele = list_entry(head, element_t, list);

    list_del(&ele->list);
    if (sp) {
        strncpy(sp, ele->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return ele;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    return q_remove(head->next, sp, bufsize);
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    return q_remove(head->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return -1;

    struct list_head *pos;
    size_t count = 0;

    list_for_each (pos, head) {
        count++;
    }
    return count;
}

void q_del_element(struct list_head *head)
{
    list_del(head);
    q_release_element(list_entry(head, element_t, list));
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head)
        return false;
    if (head == head->next)
        return false;
    bool move = false;

    struct list_head *pos, *safe, *tmp = head;
    list_for_each_safe (pos, safe, head) {
        move = !move;
        if (move) {
            tmp = tmp->next;
        }
    }
    if (tmp == NULL || tmp->next == tmp) {
        return true;
    }

    q_del_element(tmp);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || head->next == head)
        return false;

    element_t *pos = NULL, *n, *prev;
    bool del = false;
    list_for_each_entry_safe (pos, n, head, list) {
        if (pos->list.prev == head)
            continue;
        prev = list_entry(pos->list.prev, element_t, list);
        if (strcmp(pos->value, prev->value) == 0) {
            q_del_element(&(prev->list));
            del = true;
        } else if (del) {
            q_del_element(&(prev->list));
            del = false;
        }
    }
    if (del) {
        q_del_element(head->prev);
    }

    return true;
}

static inline void list_replace(struct list_head *old, struct list_head *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
}

static inline void list_swap(struct list_head *entry1, struct list_head *entry2)
{
    struct list_head *pos = entry2->prev;

    list_del(entry2);
    list_replace(entry1, entry2);
    if (pos == entry1)
        pos = entry2;
    list_add(entry1, pos);
}

void q_reverseK(struct list_head *head, int k);

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    int k = 2;
    q_reverseK(head, k);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *pos = NULL, *n;
    list_for_each_safe (pos, n, head) {
        pos->next = pos->prev;
        pos->prev = n;
    }
    n = head->next;
    head->next = head->prev;
    head->prev = n;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head))
        return;

    struct list_head *l = head, *r = head->next;
    for (;; l = r->prev) {
        for (int i = 0; i < k; i++, r = r->next)
            if (r == head)
                return;

        LIST_HEAD(tmp);
        list_cut_position(&tmp, l, r->prev);
        q_reverse(&tmp);
        list_splice(&tmp, l);
    }
}
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
