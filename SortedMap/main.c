/**
 MIT License
 
 Copyright (c) 2018 Matias Barrientos.
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include "sorted_map.h"

int cmp(const void * a, const void * b) {
    const int * A = a;
    const int * B = b;
    return *A - *B;
}

int main(int argc, const char * argv[]) {
    
    sorted_map *map = sorted_map_init (cmp, free);
    
    int *data = NULL;
    
    for (int i = 1; i <= 10; i++) {
        data = malloc (sizeof (int));
        *data = i;
        sorted_map_insert (map, data, data);
    }
    
    int key = 10;
    sorted_map_remove_key (map, &key);
    
    key = 9;
    sorted_map_remove_key(map, &key);
    
    key = 4;
    sorted_map_remove_key(map, &key);
    
    key = 3;
    sorted_map_remove_key(map, &key);
    
    //    key = 7;
    //    sorted_map_remove_key (map, &key);
    //
    //    key = 1;
    //    sorted_map_remove_key (map, &key);
    //
    //    key = 2;
    //    sorted_map_remove_key (map, &key);
    //
    //    key = 6;
    //    sorted_map_remove_key (map, &key);
    //
    //    key = 8;
    //    sorted_map_remove_key (map, &key);
    //
    //    key = 5;
    //    sorted_map_remove_key (map, &key);
    
    data = sorted_map_first (map);
    
    while (data) {
        printf ("%d\n", *data);
        data = sorted_map_next (map);
    }
    
    sorted_map_release (&map);
    
    return 0;
}
