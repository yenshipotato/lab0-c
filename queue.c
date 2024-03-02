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
    struct list_head *new_head = malloc(sizeof(struct list_head));
    if (new_head)
        INIT_LIST_HEAD(new_head);
    return new_head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *cur_entry, *safe_entry;
    list_for_each_entry_safe (cur_entry, safe_entry, head, list)
        q_release_element(cur_entry);
    free(head);
}
/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    if ((node->value = strdup(s)) != NULL)
        list_add(&node->list, head);
    else {
        free(node);
        return false;
    }
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    if ((node->value = strdup(s)) != NULL)
        list_add_tail(&node->list, head);
    else {
        free(node);
        return false;
    }
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *rm_node = list_first_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, rm_node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&rm_node->list);
    return rm_node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *rm_node = list_last_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, rm_node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&rm_node->list);
    return rm_node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *fast = head->next, *slow = head->next;
    while (fast->next->next != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }
    element_t *rm_node = list_entry(slow, element_t, list);
    list_del(slow);
    q_release_element(rm_node);
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    q_reverseK(head, 2);

    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *now, *next;
    list_for_each_safe (now, next, head) {
        now->next = now->prev;
        now->prev = next;
    }
    now = head->next;
    head->next = head->prev;
    head->prev = now;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head))
        return;
    int len = q_size(head), node_num;
    struct list_head *temp = head->next;
    if (len < k)
        return;

    LIST_HEAD(temp_head);
    do {
        for (node_num = 0; node_num < k && temp != head; node_num++)
            temp = temp->next;
        if (node_num == k) {
            LIST_HEAD(cut);
            list_cut_position(&cut, head, temp->prev);
            q_reverse(&cut);
            list_splice_tail_init(&cut, &temp_head);
        }
    } while (temp != head);

    list_splice_init(&temp_head, head);

    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}
static int cmp(struct list_head *a, struct list_head *b)
{
    return strcmp(list_entry(a, element_t, list)->value,
                  list_entry(b, element_t, list)->value);
}
/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *now, *safe;
    q_reverse(head);
    list_for_each_safe (now, safe, head->next) {
        if (now == head)
            break;
        if (cmp(now, now->prev) > 0) {
            element_t *rm_node = list_entry(now, element_t, list);
            list_del(now);
            q_release_element(rm_node);
        }
    }
    q_reverse(head);
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *now, *safe;
    q_reverse(head);
    list_for_each_safe (now, safe, head->next) {
        if (now == head)
            break;
        if (cmp(now, now->prev) < 0) {
            element_t *rm_node = list_entry(now, element_t, list);
            list_del(now);
            q_release_element(rm_node);
        }
    }
    q_reverse(head);
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_size(head);
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