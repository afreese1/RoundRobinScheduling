#ifndef LIST1_H_INCLUDED
#define LIST1_H_INCLUDED
#include "PCB.h"
//Implementation of a List using a doubly linked circular list
//with a header

    typedef PCB ListInfo;


    struct NodeType
    {
      ListInfo info;
      NodeType* next;
      NodeType* prev;
    };

    typedef NodeType *NodePtr;
    typedef NodePtr List;

    //initialize the list with the header pointing at itself
     void initializeList(List *listPtr);

     //return true if the only element on the list is the header
     //otherwise return false
     bool isEmpty(const List list);

     //Remover all elements including the header from the list
     //set the List to null
     void clearList(List *listPtr);

     //Add the value to the front of the list
     void pushFront(const List list, ListInfo value);

     //Add the value to the back of the list
     void pushBack(const List list, ListInfo value);

     //Remove and return the value at the front of the list
     ListInfo popFront(const List list);

     //Remove and return the value at the back of the list
     ListInfo popBack(const List list);

     //Return true if the value appears in the list,
     //otherwise return false
     bool findInList(const List list, ListInfo value);

     //Return the value at the front of the list
     ListInfo getFront(const List list);

     //Return the value at the back of the list
     ListInfo getBack(const List list);

#endif // LIST1_H_INCLUDED
