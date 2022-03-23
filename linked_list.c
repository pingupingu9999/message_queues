#include "linked_list.h"
#include <assert.h>

void List_init(ListHead* list_head) {
  list_head -> first = 0;
  list_head -> last = 0;
  list_head -> size = 0;
}

ListItem* List_find(ListHead* list_head, ListItem* list_item) {
  // linear scanning of list
  ListItem* aux = list_head->first;
  while(aux){
    if (aux == list_item)
      return list_item;
    aux = aux -> next;
  }
  return 0;
}


ListItem* List_index(ListHead* list_head, int index){
  ListItem* aux = list_head -> first;
  for(int i = 0; i < index; i++){
    if (aux != 0)
      aux = aux -> next;
    else 
      return 0;
  }
  return aux;
}

ListItem* List_insert(ListHead* list_head, ListItem* prev, ListItem* list_item) {
  if (list_item == 0) return 0;
  if (list_item -> next || list_item -> prev)
    return 0;
  
#ifdef _LIST_DEBUG_
  // we check that the element is not in the list
  ListItem* instance=List_find(list_head, list_item);
  assert(!instance);

  // we check that the previous is inthe list

  if (prev) {
    ListItem* prev_instance=List_find(list_head, prev);
    assert(prev_instance);
  }
  // we check that the previous is inthe list
#endif

  ListItem* next = prev ? prev -> next : list_head -> first;
  if (prev) {
    list_item -> prev = prev;
    prev -> next = list_item;
  }
  if (next) {
    list_item -> next = next;
    next -> prev = list_item;
  }
  if (!prev)
    list_head -> first = list_item;
  if(!next)
    list_head -> last = list_item;
  ++list_head -> size;
  return list_item;
}

ListItem* List_detach(ListHead* list_head, ListItem* list_item) {
  
#ifdef _LIST_DEBUG_
  // we check that the element is in the list
  ListItem* instance=List_find(list_head, list_item);
  assert(instance);
#endif
  if (list_item == 0)  return 0;

  ListItem* prev = list_item -> prev;
  ListItem* next = list_item -> next;
  if (prev){
    prev -> next = next;
  }
  if(next){
    next -> prev = prev;
  }
  if (list_item == list_head -> first)
    list_head -> first = next;
  if (list_item == list_head -> last)
    list_head -> last = prev;
  list_head -> size--;
  list_item -> next = list_item -> prev = 0;
  return list_item;
}

ListItem* List_pop(ListHead* list_head){
  if (list_head -> first != 0)
    return List_detach(list_head, list_head -> first);
  return 0;
}