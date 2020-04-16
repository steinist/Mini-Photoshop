#include "patch.h"

#include <cstring>
#include <iostream>
using namespace std;

/**
* Append a new line to the tail.
*
* @param patch: a patch pointer
* @param line: a line pointer
*/
void append_to_bottom(Patch *patch, Line *line)
{

    if (patch->head == nullptr){
        patch->head = line;
    }

    if (patch->tail == nullptr){
        patch->tail = line;
    }

    else{
        patch->tail->next = line;
        patch->tail = patch->tail->next;
    }



}

/**
* Delete the patch and the resources owned by it.
*
* @param patch, a patch pointer.
*/
void clear(Patch *&patch)
{
    while (patch->tail != patch->head) {
        Line* current = patch->head;
        while (current->next != patch->tail) {
            current = current->next;
        }


        delete patch->tail;
        patch->tail = current;
    }
    patch->tail = nullptr;
    delete patch->head;
    patch->head = nullptr;


    delete patch;
    patch = nullptr;
}

/**
* Reverse the linked list of lines.
*
* @param patch, a patch pointer.
*/
void reverse(Patch *patch)
{
    Line *current = patch->tail;
    while (current != patch->head) {
        Line* prev = patch->head;
        while (prev->next != current) {
            prev = prev->next;
        }

        current->next = prev;
        current = prev;
    }
    patch->head->next = nullptr;
    Line* temp = patch->head;
    patch->head = patch->tail;
    patch->tail = temp;
}