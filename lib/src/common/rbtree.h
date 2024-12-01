#ifndef _RBTREE_H_
#define _RBTREE_H_
#if __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <common/mem.h>

#define BLACK (true)
#define RED   (false)
typedef int (*compare_func)(const void*, const void*);
typedef int rbtree_status;

struct rbnode
{
    struct rbnode* left;
    struct rbnode* right;
    struct rbnode* parent;
    bool color;
};

struct rbtree
{
    struct rbnode* root;
    size_t element_size;
    size_t length;
    compare_func cmp;
};

void           rbtree_ctor    (struct rbtree* tree, size_t element_size, compare_func cmp);
void           rbtree_dtor    (struct rbtree* tree);
size_t         rbtree_length  (struct rbtree* tree);
void*          rbtree_lookup  (struct rbtree* tree, void* element);
rbtree_status  rbtree_insert  (struct rbtree* tree, void* element);
void           rbtree_delete  (struct rbtree* tree, void* element);

static inline
bool rbnode_color(struct rbnode* node)
{
    return node == NULL ? BLACK : node->color;
}

static inline
void* rbnode_get_data_address(struct rbnode* node)
{
    return &node[1];
}

static
struct rbnode* rbnode_new(size_t element_size, void* element_address)
{
    struct rbnode* node = (struct rbnode*)mem_alloc(sizeof(struct rbnode) + element_size);
    if (node == NULL)
        return NULL;

    node->color = RED;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    void* target = rbnode_get_data_address(node);
    memcpy(target, element_address, element_size);
    return node;
}

static
void rbnode_free(struct rbnode* node)
{
    if (node != NULL)
    {
        rbnode_free(node->left);
        rbnode_free(node->right);
        mem_free(node);
    }
}

static inline
struct rbnode* rbnode_grandparent(struct rbnode* node)
{
    return node->parent->parent;
}

static inline
struct rbnode* rbnode_sibling(struct rbnode* node)
{
    return node == node->parent->left ? node->parent->right : node->parent->left;
}

static inline
struct rbnode* rbnode_uncle(struct rbnode* node)
{
    return rbnode_sibling(node->parent);
}

static inline
struct rbnode* rbnode_maximum(struct rbnode* node)
{
    while (node->right != NULL)
        node = node->right;
    return node;
}

static inline
struct rbnode* rbnode_lookup(struct rbtree* tree, void* element, compare_func cmp)
{
    struct rbnode* node = tree->root;
    while (node != NULL)
    {
        int result = cmp(element, rbnode_get_data_address(node));
        if (result == 0)
            return node;
        else if (result < 0)
            node = node->left;
        else
            node = node->right;
    }
    return node;
}

static inline
void replace_node(struct rbtree* tree, struct rbnode* old_node, struct rbnode* new_node)
{
    if (old_node->parent == NULL)
        tree->root = new_node;
    else if (old_node == old_node->parent->left)
        old_node->parent->left = new_node;
    else
        old_node->parent->right = new_node;

    if (new_node != NULL)
        new_node->parent = old_node->parent;
}

static inline
void rotate_left(struct rbtree* tree, struct rbnode* node)
{
    struct rbnode* right = node->right;
    replace_node(tree, node, right);
    node->right = right->left;
    if (right->left != NULL)
        right->left->parent = node;

    right->left = node;
    node->parent = right;
}

static inline
void rotate_right(struct rbtree* tree, struct rbnode* node)
{
    struct rbnode* left = node->left;
    replace_node(tree, node, left);
    node->left = left->right;
    if (left->right != NULL)
        left->right->parent = node;

    left->right = node;
    node->parent = left;
}

void rbtree_ctor(struct rbtree* tree, size_t element_size, compare_func cmp)
{
    tree->root = NULL;
    tree->element_size = element_size;
    tree->length = 0;
    tree->cmp = cmp;
}

void rbtree_dtor(struct rbtree* tree)
{
    rbnode_free(tree->root);
    tree->root = NULL;
    tree->element_size = 0;
    tree->length = 0;
    tree->cmp = NULL;
}

size_t rbtree_length(struct rbtree* tree)
{
    return tree->length;
}

static
void rbtree_insert_and_balance(struct rbtree* tree, struct rbnode* node)
{
    if (node->parent == NULL)
        node->color = BLACK;
    else if (rbnode_color(node->parent) == BLACK)
        return;
    else if (rbnode_color(rbnode_uncle(node)) == RED)
    {
        node->parent->color = BLACK;
        rbnode_uncle(node)->color = BLACK;
        rbnode_grandparent(node)->color = RED;
        rbtree_insert_and_balance(tree, rbnode_grandparent(node));
    }
    else
    {
        if (node == node->parent->right && node->parent == rbnode_grandparent(node)->left)
        {
            rotate_left(tree, node->parent);
            node = node->left;
        }
        else if (node == node->parent->left && node->parent == rbnode_grandparent(node)->right)
        {
            rotate_right(tree, node->parent);
            node = node->right;
        }

        node->parent->color = BLACK;
        rbnode_grandparent(node)->color = RED;
        if (node == node->parent->left && node->parent == rbnode_grandparent(node)->left)
            rotate_right(tree, rbnode_grandparent(node));
        else
            rotate_left(tree, rbnode_grandparent(node));
    }
}

rbtree_status rbtree_insert(struct rbtree* tree, void* element)
{
    if (rbnode_lookup(tree, element, tree->cmp) != NULL)
        return EXIT_SUCCESS;

    struct rbnode* inserted_node = rbnode_new(tree->element_size, element);
    if (inserted_node == NULL)
        return ENOMEM;
    if (tree->root == NULL)
        tree->root = inserted_node;
    else
    {
        struct rbnode* n = tree->root;
        while (true)
        {
            void* target = rbnode_get_data_address(n);
            int result = tree->cmp(element, target);
            if (result < 0)
            {
                if (n->left == NULL)
                {
                    n->left = inserted_node;
                    break;
                }
                else
                    n = n->left;
            }
            else
            {
                if (n->right == NULL)
                {
                    n->right = inserted_node;
                    break;
                }
                else
                    n = n->right;
            }
        }
        inserted_node->parent = n;
    }
    rbtree_insert_and_balance(tree, inserted_node);
    tree->length++;
    return EXIT_SUCCESS;
}

void* rbtree_lookup(struct rbtree* tree, void* element)
{
    struct rbnode* node = rbnode_lookup(tree, element, tree->cmp);
    return node == NULL ? NULL : rbnode_get_data_address(node);
}

static void rbtree_delete_and_balance(struct rbtree* tree, struct rbnode* node)
{
    if (node->parent == NULL)
        return;

    if (rbnode_color(rbnode_sibling(node)) == RED)
    {
        node->parent->color = RED;
        rbnode_sibling(node)->color = BLACK;
        if (node == node->parent->left)
            rotate_left(tree, node->parent);
        else
            rotate_right(tree, node->parent);
    }

    if (rbnode_color(node->parent) == BLACK &&
        rbnode_color(rbnode_sibling(node)) == BLACK &&
        rbnode_color(rbnode_sibling(node)->left) == BLACK &&
        rbnode_color(rbnode_sibling(node)->right) == BLACK)
    {
        rbnode_sibling(node)->color = RED;
        rbtree_delete_and_balance(tree, node->parent);
    }
    else if (rbnode_color(node->parent) == RED &&
        rbnode_color(rbnode_sibling(node)) == BLACK &&
        rbnode_color(rbnode_sibling(node)->left) == BLACK &&
        rbnode_color(rbnode_sibling(node)->right) == BLACK)
    {
        rbnode_sibling(node)->color = RED;
        node->parent->color = BLACK;
    }
    else
    {
        if (node == node->parent->left &&
            rbnode_color(rbnode_sibling(node)) == BLACK &&
            rbnode_color(rbnode_sibling(node)->left) == RED &&
            rbnode_color(rbnode_sibling(node)->right) == BLACK)
        {
            rbnode_sibling(node)->color = RED;
            rbnode_sibling(node)->left->color = BLACK;
            rotate_right(tree, rbnode_sibling(node));
        }
        else if (node == node->parent->right &&
                rbnode_color(rbnode_sibling(node)) == BLACK &&
                rbnode_color(rbnode_sibling(node)->right) == RED &&
                rbnode_color(rbnode_sibling(node)->left) == BLACK)
        {
            rbnode_sibling(node)->color = RED;
            rbnode_sibling(node)->right->color = BLACK;
            rotate_left(tree, rbnode_sibling(node));
        }

        rbnode_sibling(node)->color = rbnode_color(node->parent);
        node->parent->color = BLACK;
        if (node == node->parent->left)
        {
            rbnode_sibling(node)->right->color = BLACK;
            rotate_left(tree, node->parent);
        }
        else
        {
            rbnode_sibling(node)->left->color = BLACK;
            rotate_right(tree, node->parent);
        }
    }
}

void rbtree_delete(struct rbtree* tree, void* element)
{
    struct rbnode* node = rbnode_lookup(tree, element, tree->cmp);
    if (node == NULL)
        return;
    if (node->left != NULL && node->right != NULL)
    {
        struct rbnode* pred = rbnode_maximum(node->left);
        memcpy(rbnode_get_data_address(node), rbnode_get_data_address(pred), tree->element_size);
        node = pred;
    }
    struct rbnode* child = node->right == NULL ? node->left : node->right;
    if (rbnode_color(node) == BLACK)
    {
        node->color = rbnode_color(child);
        rbtree_delete_and_balance(tree, node);
    }
    replace_node(tree, node, child);
    if (node->parent == NULL && child != NULL)
        child->color = BLACK;
    mem_free(node);

    tree->length--;
}

#if __cplusplus
}
#endif
#endif