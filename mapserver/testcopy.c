/* ========================================================================= 
 * $Id$
 * 
 * Test of msCopyMap()
 *
 * ========================================================================= */
   
#include "map.h"

void printAtts(mapObj*, const char*);

int main(int argc, char *argv[]) {

    mapObj *original_map, *clone_map;

    /* ---------------------------------------------------------------------
     * Test 1: free original before freeing clone
     * --------------------------------------------------------------------- */

    // Load map file 
    original_map = msLoadMap("tests/test.map", NULL);
    
    // Dump out some attributes
    printAtts(original_map, "Original"); 

    // Clone it
    clone_map = msNewMapObj();
    msCopyMap(clone_map, original_map);

    // Write errors
    msWriteError(stderr);
    msResetErrorList();

    // Free
    msFreeMap(original_map);

    // Dump clone's attributes
    printAtts(clone_map, "Clone");
  
    // Free clone
    msFreeMap(clone_map);

    exit(0);    
}

void printAtts(mapObj *map, const char *title) {
    printf("\n%s Attributes\n----------------------\n", title);
    printf("Map Name: %s\n", map->name);
    printf("Numlayers: %d\n", map->numlayers);
    printf("Map Fontset Filename: %s\n", map->fontset.filename);
    printf("Map Symbolset Filename: %s\n", map->symbolset.filename);
}
