#include "sema/scope.h"
#include "common/mem.h"

typedef struct node_t *node_t;
struct node_t
{
    bool is_red;
    node_t lhs, rhs, parent;
    declaration_t declaration;
};

struct scope_t
{
    scope_t parent;
    node_t root;
};

node_t node_new(declaration_t declaration, bool is_red, node_t lhs, node_t rhs)
{
    node_t node = mem_alloc(sizeof(struct node_t));
    node->is_red = is_red;
    node->lhs = lhs;
    node->rhs = rhs;
    node->parent = NULL;
    node->declaration = declaration;
    return node;
}

scope_t scope_new(scope_t parent)
{
    scope_t scope = mem_alloc(sizeof(struct scope_t));
    scope->parent = parent;
    scope->root = NULL;
    return scope;
}

void free_node(node_t node)
{
    if (node == NULL)
        return;
    free_node(node->lhs);
    free_node(node->rhs);
    mem_free(node);
}

void scope_free(scope_t scope)
{
    free_node(scope->root);
    mem_free(scope);
}

static inline node_t rotate_left(node_t node)
{
    node_t x = node->rhs;
    node->rhs = x->lhs;
    if (x->lhs != NULL)
        x->lhs->parent = node;
    x->parent = node->parent;
    node->parent = x;
    x->lhs = node;
    x->is_red = node->is_red;
    node->is_red = true;
    return x;
}

static inline node_t rotate_right(node_t node)
{
    node_t x = node->lhs;
    node->lhs = x->rhs;
    if (x->rhs != NULL)
        x->rhs->parent = node;
    x->parent = node->parent;
    node->parent = x;
    x->rhs = node;
    x->is_red = node->is_red;
    node->is_red = true;
    return x;
}

static node_t fix_up(node_t root)
{
    if (root->rhs && root->rhs->is_red)
    {
        if (root->lhs && root->lhs->is_red)
        {
            root->is_red = true;
            root->lhs->is_red = false;
            root->rhs->is_red = false;
        }
        else
        {
            root = rotate_left(root);
        }
    }
    else if (root->lhs && root->lhs->is_red && root->lhs->lhs && root->lhs->lhs->is_red)
    {
        root = rotate_right(root);
    }
    return root;
}

static node_t insert_node(node_t root, declaration_t declaration, node_t parent)
{
    if (root == NULL)
    {
        node_t node = node_new(declaration, true, NULL, NULL);
        node->parent = parent;
        return node;
    }

    span_t decl_name = declaration_name(declaration);
    span_t root_name = declaration_name(root->declaration);
    int cmp = span_cmp(decl_name, root_name);

    if (cmp == 0)
        return root;

    if (cmp < 0)
        root->lhs = insert_node(root->lhs, declaration, root);
    else
        root->rhs = insert_node(root->rhs, declaration, root);

    return fix_up(root);
}

bool scope_add(scope_t scope, declaration_t declaration)
{
    span_t name = declaration_name(declaration);

    node_t current = scope->root;
    while (current != NULL)
    {
        int cmp = span_cmp(name, declaration_name(current->declaration));
        if (cmp == 0) return false;
        current = cmp < 0 ? current->lhs : current->rhs;
    }

    scope->root = insert_node(scope->root, declaration, NULL);
    if (scope->root)
        scope->root->is_red = false;
    return true;
}

declaration_t scope_find(scope_t scope, span_t name) 
{
    if (scope == NULL)
        return NULL;

    node_t current = scope->root;
    while (current != NULL)
    {
        int cmp = span_cmp(name, declaration_name(current->declaration));
        if (cmp == 0) return current->declaration;
        current = cmp < 0 ? current->lhs : current->rhs;
    }

    return scope_find(scope->parent, name);
}

scope_t scope_parent(scope_t scope)
{
    return scope->parent;
}
