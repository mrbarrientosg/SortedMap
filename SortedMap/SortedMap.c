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

#include "SortedMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef enum Rotations Rotations;

typedef struct TreeNode TreeNode ;

enum Rotations {
    heavyLeft = 2,
    left = 1,
    zero = 0,
    right = -1,
    heavyRight = -2,
    nil
};

struct TreeNode {
    const void * value;
    const void * key;
    TreeNode  * left;
    TreeNode  * right;
    TreeNode  * parent;
    long height;
};

struct SortedMap {
    TreeNode  * root;
    TreeNode  * current;
    SortedMapCompareCallBack compare;
    long count;
};

static TreeNode  * createNode(const void * key, const void * data) {
    TreeNode  * new = (TreeNode *)malloc(sizeof(TreeNode));

    assert(new != NULL);

    new->key = key;
    new->value = data;
    new->parent = new->left = new->right = NULL;
    new->height = 0;
    return new;
}

static inline long max(long l, long r) {
    return l > r ? l : r;
}

static inline long heightNode(TreeNode  * node) {
    if (node == NULL) return 0;
    return node->height;
}

static inline TreeNode * minimum(TreeNode * node){
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

static void fixHeightNode(TreeNode  * node) {
    if (node == NULL) return;
    
    long hr = 0, hl = 0;

    if (node->left != NULL) hl = heightNode(node->left) + 1;

    if (node->right != NULL) hr = heightNode(node->right) + 1;
    
    node->height = max(hl, hr);
}

static Rotations balanceFactor(TreeNode  * node) {
    long hl = (node->left != NULL) ? heightNode(node->left) : -1;
    long hr = (node->right != NULL) ? heightNode(node->right) : -1;
    
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

static void deleteAllTree(TreeNode  * node) {
    if (node == NULL) return;
    
    deleteAllTree(node->left);
    deleteAllTree(node->right);
    
    free(node);
}

static TreeNode * rotateLeftLeft(TreeNode * n5){
    TreeNode * n4 = n5->left;
    
    n4->parent = n5->parent;
    
    TreeNode * parentN4 = n4->parent;
    
    if (parentN4 != NULL){
        if (parentN4->left == n5) {
            parentN4->left = n4;
        } else {
            parentN4->right = n4;
        }
    }
    
    n5->left = n4->right;
    
    if (n4->right != NULL) n4->right->parent = n5;
    
    n4->right = n5;
    n5->parent = n4;
    
    fixHeightNode(n5);
    fixHeightNode(n4);
    
    return n4;
}

static TreeNode * rotateLeftRight(TreeNode * n5){
    TreeNode * n3 = n5->left;
    TreeNode * n4 = n3->right;
    
    n3->right = n4->left;
    
    if(n4->left != NULL) n4->left->parent = n3;
    
    n5->left = n4;
    n4->parent = n5;
    
    n4->left = n3;
    n3->parent = n4;
    
    fixHeightNode(n3);
    fixHeightNode(n4);
    fixHeightNode(n5);
    
    rotateLeftLeft(n5);
    
    return n4;
}

static TreeNode * rotateRightRight(TreeNode * n3){
    TreeNode* n4 = n3->right;
    
    n4->parent = n3->parent;
    
    TreeNode * parentN4 = n4->parent;
    
    if (parentN4 != NULL) {
        if (parentN4->left == n3) {
            parentN4->left = n4;
        } else {
            parentN4->right = n4;
        }
    }
    
    n3->right = n4->left;
    
    if (n4->left != NULL) n4->left->parent = n3;
    
    n4->left = n3;
    n3->parent = n4;
    
    fixHeightNode(n3);
    fixHeightNode(n4);
    
    return n4;
}

static TreeNode * rotateRightLeft(TreeNode * n3){
    TreeNode* n5 = n3->right;
    TreeNode* n4 = n5->left;
    
    n5->left = n4->right;
   
    if(n4->right != NULL) n4->right->parent = n5;
    
    n3->right = n4;
    n4->parent = n3;
    
    n4->right = n5;
    n5->parent = n4;
    
    fixHeightNode(n5);
    fixHeightNode(n4);
    fixHeightNode(n3);
    
    rotateRightRight(n3);
    
    return n4;
}

static void balanceMap(SortedMap * map, TreeNode  * node) {
    
    while (node != NULL) {
        fixHeightNode(node);
        
        if (balanceFactor(node) == heavyLeft && balanceFactor(node->left) >= zero) {
            node = rotateLeftLeft(node);
        } else if (balanceFactor(node) == heavyRight && balanceFactor(node->right) <= zero) {
            node = rotateRightRight(node);
        } else if (balanceFactor(node) == heavyLeft && balanceFactor(node->left) == right) {
            node = rotateLeftRight(node);
        } else if (balanceFactor(node) == heavyRight && balanceFactor(node->right) == left) {
            node = rotateRightLeft(node);
        }
        
        if (node->parent == NULL) map->root = node;
        
        node = node->parent;
    }
}

static void deleteNode(SortedMap * map, TreeNode  * node) {
    if (node == NULL) return;
    
    if (node->left != NULL && node->right == NULL) {
        node->key = node->left->key;
        node->value = node->left->value;

        deleteNode(map, node->left);

        fixHeightNode(node);

    } else if (node->left == NULL && node->right != NULL) {
        node->key = node->right->key;
        node->value = node->right->value;
        
        deleteNode(map, node->right);
        
        fixHeightNode(node);

    } else if (node->left != NULL && node->right != NULL) {
        TreeNode * min = node->right;
        
        min = minimum(min);
        
        node->key = min->key;
        node->value = min->value;
        
        TreeNode  * parent = min->parent;
        
        deleteNode(map, min);
        
        balanceMap(map, parent);
        
    } else {
        if (node->parent != NULL) {
            if (node->parent->left == node) {
                node->parent->left = NULL;
            } else {
                node->parent->right = NULL;
            }
            
            balanceMap(map, node->parent);
            
        } else if (node == map->root) {
            map->root = NULL;
        }
        
        map->count -= 1;
        
        free(node);
        
        node = NULL;
    }
    
}

SortedMap * createSortedMap(SortedMapCompareCallBack compare) {
    SortedMap * new = (SortedMap *)malloc(sizeof(SortedMap));
    
    assert(new != NULL);

    new->root = NULL;
    new->current = NULL;
    new->count = 0;
    new->compare = compare;
    return new;
}

void insertSortedMap(SortedMap * map, const void * key, const void * value) {
    assert(map != NULL);
    
    TreeNode  * aux = map->root;
    TreeNode  * parent = NULL;
    
    while (aux != NULL) {
        parent = aux;
        
        if (map->compare(key, aux->key) == 0) return;
        
        if (map->compare(key, aux->key) < 0) {
            aux = aux->left;
        } else {
            aux = aux->right;
        }
    }
    
    TreeNode  * new = createNode(key, value);
    
    new->parent = parent;
    
    if (parent == NULL) {
        map->root = new;
    } else if (map->compare(key, parent->key) < 0) {
        parent->left = new;
    } else {
        parent->right = new;
    }
    
    map->count += 1;
    
    balanceMap(map, new);
}

void * eraseKeySortedMap(SortedMap * map, const void * key) {
    assert(map != NULL);

    if (map->root == NULL) return NULL;
    
    TreeNode  * aux = map->root;
    
    while (aux != NULL && map->compare(key, aux->key) != 0) {
        if (map->compare(key, aux->key) < 0) {
            aux = aux->left;
        } else {
            aux = aux->right;
        }
    }
    
    if (aux == NULL) return NULL;
    
    void * value = (void *)aux->value;
    
    deleteNode(map, aux);
    
    return value;
}

long sortedMapCount(SortedMap * map) {
    assert(map != NULL);

    if (map->root == NULL) return 0;
    
    return map->count;
}

int emptySortedMap(SortedMap * map) {
    assert(map != NULL);
    
    return map->count == 0;
}

void * searchSortedMap(SortedMap * map, const void * key) {
    assert(map != NULL);

    if (map->root == NULL) return NULL;
   
    TreeNode  * aux = map->root;
    
    while (aux != NULL &&  map->compare(key, aux->key) != 0) {
        if (map->compare(key, aux->key) < 0) {
            aux = aux->left;
        } else {
            aux = aux->right;
        }
    }
    
    if (aux == NULL) return NULL;
    
    map->current = aux;
    
    return (void *)aux->value;
}

static void _upperBound(SortedMap * map, TreeNode  * root, const void * key){
    if (root == NULL) return;
    
    if (map->compare(root->key, key) >= 0) {
        map->current = root;
        _upperBound(map, root->left, key);
    } else {
        _upperBound(map, root->right, key);
    }
}

void * upperBoundSortedMap(SortedMap * map, const void * key) {
    assert(map != NULL);
    
    if (map->root == NULL) return NULL;
    
    map->current = NULL;
    
    _upperBound(map, map->root, key);
    
    return (map->current != NULL) ? (void *)map->current->value : NULL;
}

void * firstSortedMap(SortedMap * map) {
    assert(map != NULL);

    if (map->root == NULL) return NULL;

    map->current = minimum(map->root);
    
    if (map->current == NULL) return NULL;
    
    return (void *)map->current->value;
}

void * nextSortedMap(SortedMap * map) {
    assert(map != NULL);

    if (map->root == NULL || map->current == NULL) return NULL;
    
    if (map->current->right != NULL) {

        map->current = minimum(map->current->right);
        
        if (map->current == NULL) return NULL;
        
        return (void *)map->current->value;
    }
    
    TreeNode  * aux = map->current->parent;
    
    while (aux != NULL && map->current == aux->right) {
        map->current = aux;
        aux = aux->parent;
    }
    
    map->current = aux;
    
    if (map->current == NULL) return NULL;
    
    return (void *)map->current->value;
}

void removeAllSortedMap(SortedMap * map) {
    deleteAllTree(map->root);
    map->root = NULL;
    map->count = 0;
}
