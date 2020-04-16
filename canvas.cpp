#include "canvas.h"

#include <iostream>

using namespace std;

/**
* Append to the bottom.
*
* @param canvas: a canvas pointer
* @param patch: a patch pointer
*/
void append_to_bottom(Canvas *canvas, Patch *patch)
{
    PatchNode* p = new PatchNode;

    if (canvas->top == nullptr){

        canvas->top = p;
        canvas->top->patch = patch;
        canvas->top->above = nullptr;
        canvas->top->below = nullptr;

        canvas->bottom = canvas->top;
        return;
    }


    else{

        canvas->bottom->below = p;
        canvas->bottom->below->patch = patch;
        canvas->bottom->below->above = canvas->bottom->below;
        canvas->bottom->below->below = nullptr;
        canvas->bottom = canvas->bottom->below;
        return;
    }

}

/**
* Delete the canvas and the resources owned by it.
*
* @param canvas, a canvas pointer.
*/
void clear(Canvas *&canvas)
{
    if (canvas == nullptr)
        return;

    while (canvas->bottom != canvas->top) {
        PatchNode* current = canvas->top;
        while (current->below != canvas->bottom) {
            current = current->below;
        }

        clear(canvas->bottom->patch);
        canvas->bottom->above = nullptr;
        canvas->bottom->below = nullptr;

        delete canvas->bottom;
        canvas->bottom = current;
    }

    //Deleting canvas -> bottom
    canvas->bottom = nullptr;

    //Deleting canvas -> top
    //delete canvas->top->patch
    clear(canvas->top->patch);
    canvas->top->below = nullptr;

    delete canvas->top;
    canvas->top = nullptr;


    //Deleting canvas -> selected
    canvas->selected = nullptr;


    //Deleting canvas
    delete canvas;
    canvas = nullptr;

}

/**
* Render the canvas to a buffer without border. Selected patch will be hightlighted as '@'.
*
* @param canvas, a canvas pointer.
* @return buffer, a height x width 2d array, required by the provided print function.
*/
char **render(const Canvas *canvas)
{
    if (canvas == nullptr){
        return nullptr;
    }

    //Initialize 2D array
    char** buffer = new char* [canvas->height];
    for (int i = 0; i < canvas->height; ++i) {
        buffer[i] = new char [canvas->width];
    }

    //Initialize
    for (int j = 0; j < canvas->height; ++j) {
        for (int i = 0; i < canvas->width; ++i) {
            buffer[j][i] = ' ';
        }
    }



    PatchNode* curr_pn = canvas->top;

    while (curr_pn != nullptr) {
        Line *curr_line = curr_pn->patch->head;

        for (int i = curr_pn->patch->x; i < canvas->height && curr_line != nullptr; ++i) {
            int k = 0;
            for (int j = curr_pn->patch->y; j < canvas->width; ++j) {
                if (curr_line->data[k] == '\0'){
                    break;}

                if (buffer[i][j] == ' ' && curr_line->data[k] != ' ') {
                    if (curr_pn == canvas->selected)
                        buffer[i][j] = '@';
                    else
                        buffer[i][j] = curr_line->data[k];
                }
                ++k;
            }
            curr_line = curr_line->next;
        }
        curr_pn = curr_pn->below;
    }

    return buffer;
}

/**
* Bring the selected patch node above by one node, do nothing if not applicable.
*
* @param canvas, a canvas pointer.
*/
void bring_selected_above(Canvas *canvas)
{
    if (canvas->selected == canvas->top)
        return;

    //Initializing target
    PatchNode* tar = canvas->top;
    while (true){
        if (tar->below == canvas->selected)
            break;
        tar = tar->below;
    }


    //Initializing after
    PatchNode* aft;
    aft = (canvas->selected != canvas->bottom) ? canvas->selected->below : nullptr;


    //Initializing before
    PatchNode* bef = canvas->top;
    while (true){
        if (bef->below == tar)
            break;

        if (tar == canvas->top){
            bef = nullptr;
            break;
        }
        bef = bef->below;
    }

    if (bef != nullptr)
        bef->below = canvas->selected;

    canvas->selected->above = bef;

    tar->below = aft;

    if (aft != nullptr)
        aft->above = tar;

    tar->above = canvas->selected;

    canvas->selected->below = tar;

    if (canvas->selected == canvas->bottom){
        canvas->bottom = tar;
    }

    if (tar == canvas->top){
        canvas->top = canvas->selected;
    }

    tar = nullptr;
    bef = nullptr;
    aft = nullptr;
}

/**
* Send the selected patch node below by one node, do nothing if not applicable.
*
* @param canvas, a canvas pointer.
*/
void send_selected_below(Canvas *canvas)
{
    if (canvas->selected == canvas->bottom)
        return;

    //Initializing target
    PatchNode* tar = canvas->selected->below;

    //Initializing after
    PatchNode* aft = (tar != canvas->bottom) ? tar->below : nullptr;


    //Initializing before
    PatchNode* bef = canvas->top;
    while (true){
        if (bef->below == canvas->selected)
            break;

        if (canvas->selected == canvas->top){
            bef = nullptr;
            break;
        }
        bef = bef->below;
    }


    if (bef != nullptr)
        bef->below = tar;

    tar->above = bef;

    canvas->selected->above = tar;

    tar->below = canvas->selected;

    canvas->selected->below = aft;

    if (aft != nullptr)
        aft->above = canvas->selected;

    if (canvas->selected == canvas->top){
        canvas->top = tar;
    }

    if (tar == canvas->bottom){
        canvas->bottom = canvas->selected;
    }

    tar = nullptr;
    bef = nullptr;
    aft = nullptr;
}

/**
* Select the top patch among the patches that covers (x, y). A patch conver a position means it has an non-transparant character on it.
* Selection of out of bound position should be allowed, despite that the position is not rendered.
*
* @param canvas, a canvas pointer.
* @param x, canvas row to select.
* @param y, canvas column to select.
*/
void select_at(Canvas *canvas, int x, int y)
{

    canvas->selected = canvas->top;


    while (true) {

        Line* l = canvas->selected->patch->head;

        //Initializing sizes of the patch;
        int x_patch_size = 1;
        while (l->data[x_patch_size] != '\0'){
            x_patch_size++;
        }

        int y_patch_size = 1;
        while (l->next != nullptr){
            y_patch_size++;
            l = l->next;
        }

        l = canvas->selected->patch->head;
        //In range of patch -> search the inside
        if ((x >= canvas->selected->patch->x) && (y >= canvas->selected->patch->y) && (x < canvas->selected->patch->x + y_patch_size) && (y < canvas->selected->patch->y + x_patch_size)){
            for (int i = 0; i < x - canvas->selected->patch->x; ++i) {
                l = l->next;
            }


            if ( l->data[y - canvas->selected->patch->y] != ' '){

                break;
            }
        }



        if (canvas->selected == canvas->bottom) {
            canvas->selected = nullptr;
            break;
        }

        canvas->selected = canvas->selected->below;
    }
}


/**
* Erase all characters at (x, y) until there is no non-transparant character on (x, y).
* If a patch node contains no non-transparant character after the erasing, it will be deleted.
* Erasing of out of bound position should be allowed, despite that the position is not rendered.
*
* @param canvas, a canvas pointer.
* @param x, canvas row to erase.
* @param y, canvas column to erase.
*/
void erase_pixel_at(Canvas *canvas, int x, int y)
{
    canvas->selected = canvas->top;
    while (canvas->selected != nullptr) {

        Line* l = canvas->selected->patch->head;

        //Initializing sizes of the patch;
        int x_patch_size = 1;
        while (l->data[x_patch_size] != '\0'){
            x_patch_size++;
        }

        int y_patch_size = 1;
        while (l->next != nullptr){
            y_patch_size++;
            l = l->next;
        }
        l = canvas->selected->patch->head;


        //In range of patch -> search the inside
        if ((x >= canvas->selected->patch->x) && (y >= canvas->selected->patch->y) && (x < canvas->selected->patch->x + y_patch_size) && (y < canvas->selected->patch->y + x_patch_size)){
            for (int i = 0; i < x - canvas->selected->patch->x; ++i) {
                l = l->next;
            }

            if (l->data[y - canvas->selected->patch->y] != ' '){
                l->data[y - canvas->selected->patch->y] = ' ';

                //If Patchnode empty
                bool is_empty = true;
                l = canvas->selected->patch->head;


                while (true){
                    for (int i = 0; i < x_patch_size; ++i) {
                        if (l->data[i] != ' '){
                            is_empty = false;
                            break;
                        }
                    }
                    if (!is_empty || l->next == nullptr)
                        break;
                    l = l->next;
                }

                //If empty -> delete
                if (is_empty){
                    //Initializing bef
                    PatchNode* bef = (canvas->selected != canvas->top) ? canvas->top : nullptr;
                    while (bef != nullptr && bef->below != canvas->selected){
                        bef = bef->below;
                    }

                    //Initializing aft
                    PatchNode* aft = (canvas->selected != canvas->bottom) ? canvas->selected->below : nullptr;



                    if (bef != nullptr)
                        bef->below = aft;
                    else
                        canvas->top = aft;

                    if (aft != nullptr)
                        aft->above = bef;
                    else
                        canvas->bottom = bef;

                    clear(canvas->selected->patch);
                    canvas->selected->below = nullptr;
                    canvas->selected->above = nullptr;
                    delete canvas->selected;
                    canvas->selected = aft;
                    continue;
                }
            }

        }


        //If didnt find anything on (x,y)
        if (canvas->selected == canvas->bottom) {
            canvas->selected = nullptr;
            break;
        }

        canvas->selected = canvas->selected->below;
    }
}