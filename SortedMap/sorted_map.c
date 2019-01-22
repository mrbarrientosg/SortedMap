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

#include "sorted_map.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef enum rotations rotations;

typedef struct tree_node tree_node;

enum rotations {
    heavyLeft = 2,
    left = 1,
    zero = 0,
    right = -1,
    heavyRight = -2,
    nil
};

struct tree_node {
    const void *value;
    const void *key;
    tree_node *left;
    tree_node *right;
    tree_node *parent;
    long height;
};

struct sorted_map {
    tree_node *root;
    tree_node *current;
    long count;
    sorted_map_compare_callback compare;
    sorted_map_release_object_callback release;
};

static tree_node *
tree_node_init (const void *key, const void *data) {
    tree_node *new = (tree_node *) malloc (sizeof (tree_node));

    assert (new != NULL);

    new->key = key;
    new->value = data;
    new->parent = new->left = new->right = NULL;
    new->height = 0;
    
    return new;
}

static inline long
max (long l, long r) {
    return l > r ? l : r;
}

static inline long
tree_node_height (tree_node *node) {
    if (node == NULL)
        return 0;
    
    return node->height;
}

static inline tree_node *
minimum (tree_node *node) {
    while (node->left != NULL) {
        node = node->left;
    }
    
    return node;
}

static void
tree_node_fix_height (tree_node *node) {
    if (node == NULL)
        return;
    
    long hr = 0, hl = 0;

    if (node->left != NULL) hl = tree_node_height (node->left) + 1;

    if (node->right != NULL) hr = tree_node_height (node->right) + 1;
    
    node->height = max (hl, hr);
}

static rotations
tree_node_balance_factor (tree_node *node) {
    long hl = (node->left != NULL) ? tree_node_height (node->left) : -1;
    long hr = (node->right != NULL) ? tree_node_height (node->right) : -1;
    
    long diff = hl - hr;
    
    switch (diff) {
        case -2: return heavyRight;
        case -1: return right;
        case 0: return zero;
        case 1: return left;
        case 2: return heavyLeft;
        default: return nil;
    }
}

static void
_sorted_map_release (tree_node *node) {
    if (node == NULL)
        return;
    
    _sorted_map_release (node->left);
    _sorted_map_release (node->right);
    
    free (node);
}

static tree_node *
rotate_left_left (tree_node *n5){
    tree_node *n4 = n5->left;
    
    n4->parent = n5->parent;
    
    tree_node *parentN4 = n4->parent;
    
    if (parentN4 != NULL){
        if (parentN4->left == n5) {
            parentN4->left = n4;
        } else {
            parentN4->right = n4;
        }
    }
    
    n5->left = n4->right;
    
    if (n4->right != NULL)
        n4->right->parent = n5;
    
    n4->right = n5;
    n5->parent = n4;
    
    tree_node_fix_height (n5);
    tree_node_fix_height (n4);
    
    return n4;
}

static tree_node *
rotate_left_right (tree_node *n5){
    tree_node *n3 = n5->left;
    tree_node *n4 = n3->right;
    
    n3->right = n4->left;
    
    if(n4->left != NULL)
        n4->left->parent = n3;
    
    n5->left = n4;
    n4->parent = n5;
    
    n4->left = n3;
    n3->parent = n4;
    
    tree_node_fix_height (n3);
    tree_node_fix_height (n4);
    tree_node_fix_height (n5);
    
    rotate_left_left (n5);
    
    return n4;
}

static tree_node *
rotate_right_right (tree_node *n3){
    tree_node *n4 = n3->right;
    
    n4->parent = n3->parent;
    
    tree_node *parentN4 = n4->parent;
    
    if (parentN4 != NULL) {
        if (parentN4->left == n3) {
            parentN4->left = n4;
        } else {
            parentN4->right = n4;
        }
    }
    
    n3->right = n4->left;
    
    if (n4->left != NULL)
        n4->left->parent = n3;
    
    n4->left = n3;
    n3->parent = n4;
    
    tree_node_fix_height (n3);
    tree_node_fix_height (n4);
    
    return n4;
}

static tree_node *
rotate_right_left(tree_node *n3){
    tree_node *n5 = n3->right;
    tree_node *n4 = n5->left;
    
    n5->left = n4->right;
   
    if(n4->right != NULL)
        n4->right->parent = n5;
    
    n3->right = n4;
    n4->parent = n3;
    
    n4->right = n5;
    n5->parent = n4;
    
    tree_node_fix_height (n5);
    tree_node_fix_height (n4);
    tree_node_fix_height (n3);
    
    rotate_right_right (n3);
    
    return n4;
}

static void
sorted_map_balance (sorted_map *map, tree_node *node) {
    
    while (node != NULL) {
        tree_node_fix_height (node);
        
        if (tree_node_balance_factor (node) == heavyLeft && tree_node_balance_factor (node->left) >= zero) {
            node = rotate_left_left (node);
        } else if (tree_node_balance_factor (node) == heavyRight && tree_node_balance_factor (node->right) <= zero) {
            node = rotate_right_right (node);
        } else if (tree_node_balance_factor (node) == heavyLeft && tree_node_balance_factor (node->left) == right) {
            node = rotate_left_right (node);
        } else if (tree_node_balance_factor (node) == heavyRight && tree_node_balance_factor (node->right) == left) {
            node = rotate_right_left (node);
        }
        
        if (node->parent == NULL)
            map->root = node;
        
        node = node->parent;
    }
}

static void
tree_node_remove (sorted_map *map, tree_node *node) {
    if (node == NULL)
        return;
    
    if (node->left != NULL && node->right == NULL) {
        node->key = node->left->key;
        node->value = node->left->value;

        tree_node_remove (map, node->left);

        tree_node_fix_height (node);

    } else if (node->left == NULL && node->right != NULL) {
        node->key = node->right->key;
        node->value = node->right->value;
        
        tree_node_remove (map, node->right);
        
        tree_node_fix_height (node);

    } else if (node->left != NULL && node->right != NULL) {
        tree_node *min = node->right;
        
        min = minimum(min);
        
        node->key = min->key;
        node->value = min->value;
        
        tree_node *parent = min->parent;
        
        tree_node_remove (map, min);
        
        sorted_map_balance (map, parent);
        
    } else {
        if (node->parent != NULL) {
            if (node->parent->left == node) {
                node->parent->left = NULL;
            } else {
                node->parent->right = NULL;
            }
            
            sorted_map_balance (map, node->parent);
            
        } else if (node == map->root) {
            map->root = NULL;
        }
        
        map->count -= 1;
        
        free (node);
        
        node = NULL;
    }
    
}

sorted_map *
sorted_map_init (sorted_map_compare_callback compare,
                 sorted_map_release_object_callback release) {
    sorted_map *new = (sorted_map *) malloc( sizeof (sorted_map));
    
    assert (new != NULL);

    new->root = NULL;
    new->current = NULL;
    new->count = 0;
    new->compare = compare;
    new->release = release;
    
    return new;
}

void
sorted_map_insert (sorted_map *map, const void *key, const void *value) {
    assert (map != NULL);
    
    tree_node *aux = map->root;
    tree_node *parent = NULL;
    
    while (aux != NULL) {
        parent = aux;
        
        if (map->compare (key, aux->key) == 0) return;
        
        if (map->compare (key, aux->key) < 0) {
            aux = aux->left;
        } else {
            aux = aux->right;
        }
    }
    
    tree_node *new = tree_node_init (key, value);
    
    new->parent = parent;
    
    if (parent == NULL) {
        map->root = new;
    } else if (map->compare (key, parent->key) < 0) {
        parent->left = new;
    } else {
        parent->right = new;
    }
    
    map->count += 1;
    
    sorted_map_balance (map, new);
}

void *
sorted_map_remove_key (sorted_map *map, const void *key) {
    assert (map != NULL);

    if (map->root == NULL)
        return NULL;
    
    tree_node *aux = map->root;
    
    while (aux != NULL && map->compare (key, aux->key) != 0) {
        if (map->compare (key, aux->key) < 0) {
            aux = aux->left;
        } else {
            aux = aux->right;
        }
    }
    
    if (aux == NULL)
        return NULL;
    
    void *value = (void *) aux->value;
    
    if (map->release != NULL) {
        map->release (value);
        value = NULL;
    }
    
    tree_node_remove (map, aux);
    
    return value;
}

long
sorted_map_size (sorted_map *map) {
    assert (map != NULL);

    if (map->root == NULL)
        return 0;
    
    return map->count;
}

int
sorted_map_empty (sorted_map *map) {
    assert (map != NULL);
    
    return map->count == 0;
}

void *
sorted_map_search_key (sorted_map *map, const void *key) {
    assert (map != NULL);

    if (map->root == NULL)
        return NULL;
   
    tree_node *aux = map->root;
    
    while (aux != NULL &&  map->compare (key, aux->key) != 0) {
        if (map->compare (key, aux->key) < 0) {
            aux = aux->left;
        } else {
            aux = aux->right;
        }
    }
    
    if (aux == NULL)
        return NULL;
    
    map->current = aux;
    
    return (void *) aux->value;
}

static void
_sorted_map_upper_bound (sorted_map *map, tree_node *root, const void *key) {
    if (root == NULL)
        return;
    
    if (map->compare (root->key, key) >= 0) {
        map->current = root;
        _sorted_map_upper_bound( map, root->left, key);
    } else {
        _sorted_map_upper_bound (map, root->right, key);
    }
}

void *
sorted_map_upper_bound (sorted_map *map, const void *key) {
    assert (map != NULL);
    
    if (map->root == NULL)
        return NULL;
    
    map->current = NULL;
    
    _sorted_map_upper_bound (map, map->root, key);
    
    return (map->current != NULL) ? (void * )map->current->value : NULL;
}

void *
sorted_map_first (sorted_map *map) {
    assert (map != NULL);

    if (map->root == NULL)
        return NULL;

    map->current = minimum(map->root);
    
    if (map->current == NULL)
        return NULL;
    
    return (void *) map->current->value;
}

void *
sorted_map_next (sorted_map *map) {
    assert (map != NULL);

    if (map->root == NULL || map->current == NULL)
        return NULL;
    
    if (map->current->right != NULL) {

        map->current = minimum (map->current->right);
        
        if (map->current == NULL) return NULL;
        
        return (void *) map->current->value;
    }
    
    tree_node *aux = map->current->parent;
    
    while (aux != NULL && map->current == aux->right) {
        map->current = aux;
        aux = aux->parent;
    }
    
    map->current = aux;
    
    if (map->current == NULL)
        return NULL;
    
    return (void *) map->current->value;
}

void
sorted_map_release (sorted_map **map) {
    assert (map != NULL);
    assert (*map != NULL);
    
    _sorted_map_release ((*map)->root);

    free (*map);
    *map = NULL;
}
