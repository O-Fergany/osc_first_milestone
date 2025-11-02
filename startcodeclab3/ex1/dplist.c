/**
 * \author Jeroen Van Aken, Bert Lagaisse, Ludo Bruynseels
 * they did the template, implementation by yours truly, Omar Dardiri
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "dplist.h"



/*
 * The real definition of struct list / struct node
 */
struct dplist_node {
    dplist_node_t *prev, *next;
    element_t element;
};

struct dplist {
    dplist_node_t *head;
    // more fields will be added later
};

dplist_t *dpl_create() {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    list->head = NULL;
  return list;
}

void dpl_free(dplist_t **list) {

    //TODO: add your code here
    if (list==NULL || *list==NULL) return;

    dplist_node_t *current = (*list)->head;
    dplist_node_t *next_node = NULL;

    while(current !=NULL){
        next_node = current->next; // save for a rainy day
        free(current);   // free my man kanye
        current = next_node; // the rainy day came
    }
    free(*list); //free his sister kanyee
    *list = NULL;
    //Do extensive testing with valgrind.ay ay captain, only if it doesnt do what i want it to do tho.

}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/


dplist_t *dpl_insert_at_index(dplist_t *list, element_t element, int index) {// didnt make , dont touch it, no smart comments
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));

    list_node->element = element;
    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index) {



    //TODO: add your code here
    if (list == NULL){
         fprintf(stderr,"!!!!ERROR::list doesnt exist, WHY\n");
         return NULL;}

    dplist_node_t *node_to_remove = dpl_get_reference_at_index(list, index);
    if (node_to_remove == NULL) return list; // from nothing and to nothing, poetry-- spongebob-2009
    if (node_to_remove->prev == NULL ){
    list->head = node_to_remove->next;}
    else{ node_to_remove->prev->next = node_to_remove->next;}// works?no? if error CHECK HERE
    if (node_to_remove->next != NULL) node_to_remove->next->prev = node_to_remove->prev;// same as 2 lines above, might be scuffed , recheclk
    if (list->head != NULL) list->head->prev = NULL;
    free(node_to_remove);// fix the leak, sleep with dry pants = good sleep
    return list;
}

int dpl_size(dplist_t *list) {

    //TODO: add your code here
    if (list == NULL) return 0; // hello darkness my old friend
    int size = 0;
    dplist_node_t *current = list->head;
    while (current != NULL) {
        size++;
        current = current->next;}
    return size;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    //int count = 0 ; ?? cant tell if i added this and commented it or if it was in the template , not used ignore
    dplist_node_t *dummy = NULL;

    //TODO: add your code here
    if (list == NULL || list->head == NULL) return NULL; // if nothing then nothing
    int size = dpl_size(list);
    dplist_node_t *current = list->head;
    int current_index = 0;
    if(index >= size || index < 0){
        fprintf(stderr,"!!!!ERROR::index out of bounds\n");
        return NULL;
    }
    while (current != NULL && current_index < index){
        current = current->next;
        current_index++;
    }
    dummy = current;
    return dummy;
}

element_t dpl_get_element_at_index(dplist_t *list, int index) {

    //TODO: add your code here
    if (list == NULL) return '\e';// i shall account for the lost souls
    dplist_node_t *ref = dpl_get_reference_at_index(list, index);
    if (ref == NULL) return '\e';
    return ref->element;
}

int dpl_get_index_of_element(dplist_t *list, element_t element) {

    //TODO: add your code here
    if (list == NULL) return -1;// running out of quotes ,ooooooooo might cause an index out of bound issue with the above implementation, if problem read this , DO NOT FORGET AND LOOK FOR THE ERROR FOR x HRS
    int current_index = 0;
    dplist_node_t *current = list->head;
    while (current != NULL) {
        if (current->element == element) {
            return current_index;
        }
        current_index++;
        current = current->next;
    }
    return -1;
}



