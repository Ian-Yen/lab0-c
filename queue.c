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
    if (!head || list_empty(head))
        return false;

    struct list_head *h = head->next, *t = head->prev;
    while (h != t && t->next != h) {
        h = h->next;
        t = t->prev;
    }

    q_del_element(t);
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

void mergeTwoLists(struct list_head *L1, struct list_head *L2, bool descend)
{
    element_t *e1 = list_entry(L1->next, element_t, list),
              *e2 = list_entry(L2->next, element_t, list), *tmp;

    while (&e1->list != L1 && &e2->list != L2) {
        if (((strcmp(e1->value, e2->value)) > 0) ^ descend) {
            tmp = list_entry(e2->list.next, element_t, list);
            list_move(&e2->list, e1->list.prev);
            e2 = tmp;
        } else
            e1 = list_entry(e1->list.next, element_t, list);
    }

    list_splice_tail(L2, L1);

    return;
}


/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *mid, *h = head->next, *t = head->prev;
    while (h != t && t->next != h) {
        h = h->next;
        t = t->prev;
    }
    mid = t;

    LIST_HEAD(tmp);
    list_cut_position(&tmp, mid, head->prev);

    q_sort(head, descend);
    q_sort(&tmp, descend);

    mergeTwoLists(head, &tmp, descend);

    return;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    const char *smallest = NULL;
    int cnt = 0;
    element_t *ele = NULL;
    for (struct list_head *pos = (head)->prev, *n = head->prev->prev;
         pos->prev != head; pos = n, n = pos->prev) {
        ele = list_entry(pos, element_t, list);
        if (!smallest || strcmp(smallest, ele->value) >= 0) {
            cnt++;
            smallest = ele->value;
            continue;
        }
        list_del(&ele->list);
    }
    return cnt;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    const char *biggest = NULL;
    int cnt = 0;
    element_t *ele;
    for (struct list_head *pos = (head)->prev, *n = head->prev->prev;
         pos != head; pos = n, n = pos->prev) {
        ele = list_entry(pos, element_t, list);
        if (!biggest || strcmp(biggest, ele->value) <= 0) {
            cnt++;
            biggest = ele->value;
            continue;
        }
        q_del_element(&ele->list);
    }
    return q_size(head);
}

void q_del_contex(queue_contex_t *node)
{
    list_del(&node->chain);
    free(node);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    if (list_is_singular(head)) {
        return list_entry(head->next, queue_contex_t, chain)->size;
    }
    queue_contex_t *merged_list = list_entry(head->next, queue_contex_t, chain);
    struct list_head *node = NULL, *safe = NULL;
    list_for_each_safe (node, safe, head) {
        if (node == head->next) {
            continue;
        }
        queue_contex_t *temp = list_entry(node, queue_contex_t, chain);
        list_splice_init(temp->q, merged_list->q);
    }
    q_sort(merged_list->q, descend);
    return merged_list->size;
}
