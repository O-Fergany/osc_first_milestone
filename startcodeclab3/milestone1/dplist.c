#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

/*
 * The real definition of struct list / struct node
 */

struct dplist_node {
    dplist_node_t *prev, *next;
    void *element;
};

struct dplist {
    dplist_node_t *head;

    void *(*element_copy)(void *src_element);

    void (*element_free)(void **element);

    int (*element_compare)(void *x, void *y);
};


dplist_t *dpl_create(
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    dplist_t *list_ptr;
    list_ptr = malloc(sizeof(struct dplist));
    list_ptr->head = NULL;
    list_ptr->element_copy = element_copy;
    list_ptr->element_free = element_free;
    list_ptr->element_compare = element_compare;
    return list_ptr;
}

void dpl_free(dplist_t **list, bool free_element) {
    if (list == NULL || *list == NULL) return;
    dplist_node_t *current_node = (*list)->head;
    dplist_node_t *next_node = NULL;
    dplist_t *target_list = *list;

    while(current_node != NULL) {
        next_node = current_node->next;
        if (free_element && current_node->element != NULL && target_list->element_free != NULL) {
            target_list->element_free(&(current_node->element));
        }
        free(current_node);
        current_node = next_node;
    }

    free(*list);
    *list = NULL;
}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {
    dplist_node_t *ref_node, *new_node;
    if (list == NULL) return NULL;

    int size = dpl_size(list);
    if (index < 0 || index > size) {
        fprintf(stderr, "index out of bound\n");
        return list;
    }

    new_node = malloc(sizeof(dplist_node_t));
    if (new_node == NULL) return list;

    if (insert_copy && list->element_copy != NULL) {
        new_node->element = list->element_copy(element);
    } else {
        new_node->element = element;
    }

    if (list->head == NULL) {
        new_node->prev = NULL;
        new_node->next = NULL;
        list->head = new_node;
    } else if (index == 0) {
        new_node->prev = NULL;
        new_node->next = list->head;
        list->head->prev = new_node;
        list->head = new_node;
    } else {
        /* Get the reference node *at* index. If index == size, it gets the last node (which is the node BEFORE the insert position). */
        ref_node = dpl_get_reference_at_index(list, index);

        if (ref_node == NULL) {
             /* This should only happen if dpl_get_reference_at_index returns NULL for a valid index, which it shouldn't. */
             fprintf(stderr, "index out of bound\n");
             free(new_node);
             return list;
        }

        if (index == size) {
            /* Insert at tail (after ref_node which is the current last node) */
            new_node->next = NULL;
            new_node->prev = ref_node;
            ref_node->next = new_node;
        } else {
            /* Insert before ref_node (in the middle) */
            new_node->prev = ref_node->prev;
            new_node->next = ref_node;
            if (ref_node->prev != NULL) {
                ref_node->prev->next = new_node;
            }
            ref_node->prev = new_node;
        }
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {
    if (list == NULL || list->head == NULL) return NULL;

    int size = dpl_size(list);
    if (index < 0 || index >= size) {
        fprintf(stderr, "index out of bound\n");
        return list;
    }

    dplist_node_t *node_to_remove = dpl_get_reference_at_index(list, index);

    if (node_to_remove == NULL) {
        /* Should not happen due to the index check above */
        return list;
    }

    if (node_to_remove->prev == NULL ) {
        list->head = node_to_remove->next;
    } else {
        node_to_remove->prev->next = node_to_remove->next;
    }

    if (node_to_remove->next != NULL) {
        node_to_remove->next->prev = node_to_remove->prev;
    }

    /* Set the new head's prev to NULL if it exists */
    if (list->head != NULL) {
        list->head->prev = NULL;
    }

    if (free_element && node_to_remove->element != NULL && list->element_free != NULL) {
        list->element_free(&(node_to_remove->element));
    }

    free(node_to_remove);
    return list;
}

int dpl_size(dplist_t *list) {
    if (list == NULL) return 0;
    int count = 0;
    dplist_node_t *runner = list->head;
    while (runner != NULL) {
        count++;
        runner = runner->next;
    }
    return count;
}

void *dpl_get_element_at_index(dplist_t *list, int index) {
    if (list == NULL) return NULL;

    int size = dpl_size(list);
    if (index < 0 || index >= size) {
        fprintf(stderr, "index out of bound\n");
        return NULL;
    }

    dplist_node_t *ref = dpl_get_reference_at_index(list, index);
    if (ref == NULL) return NULL;
    return ref->element;
}

int dpl_get_index_of_element(dplist_t *list, void *element) {
    if (list == NULL || list->element_compare == NULL) return -1;
    int idx = 0;
    dplist_node_t *current_ptr = list->head;

    while (current_ptr != NULL) {
        if (list->element_compare(current_ptr->element, element) == 0) {
            return idx;
        }
        idx++;
        current_ptr = current_ptr->next;
    }
    return -1;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    if (list == NULL || list->head == NULL) return NULL;

    int size = dpl_size(list);

    /* Enforce the new index bound rule */
    if (index < 0 || index >= size) {
        fprintf(stderr, "index out of bound\n");
        return NULL;
    }

    dplist_node_t *current_ref = list->head;
    int current_idx = 0;

    while (current_ref != NULL && current_idx < index){
        current_ref = current_ref->next;
        current_idx++;
    }

    return current_ref;
}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {
    if (list == NULL || reference == NULL) return NULL;
    dplist_node_t *iter_node = list->head;

    while (iter_node != NULL) {
        if (iter_node == reference) {
            return iter_node->element;
        }
        iter_node = iter_node->next;
    }

    return NULL;
}