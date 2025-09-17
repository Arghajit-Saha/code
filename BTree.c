#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int M;

typedef struct TreeNode {
    int num_keys;
    int *keys;
    struct TreeNode **children;
    bool is_leaf;
} TreeNode;

void traverse(TreeNode *root);
void insertNonFull(TreeNode *node, int key);
void splitChild(TreeNode *parent, int index);
TreeNode* insert(TreeNode *root, int key);
void deleteFromNode(TreeNode *node, int key);
TreeNode* delete(TreeNode *root, int key);
void freeTree(TreeNode *node);

TreeNode *createNode(bool is_leaf) {
    TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
    
    newNode->is_leaf = is_leaf;
    newNode->num_keys = 0;
    newNode->keys = (int *)malloc((M - 1) * sizeof(int));
    newNode->children = (TreeNode **)malloc(M * sizeof(TreeNode *));

    for (int i = 0; i < M; i++) {
        newNode->children[i] = NULL;
    }
    
    return newNode;
}

void splitChild(TreeNode *parent, int index) {
    int t = (M + 1) / 2;
    TreeNode *child = parent->children[index];
    TreeNode *newNode = createNode(child->is_leaf);

    newNode->num_keys = M - t;

    for (int i = 0; i < newNode->num_keys; i++) {
        newNode->keys[i] = child->keys[i + t];
    }

    if (!child->is_leaf) {
        for (int i = 0; i < M - t + 1; i++) {
            newNode->children[i] = child->children[i + t];
        }
    }

    child->num_keys = t - 1;

    for (int i = parent->num_keys; i > index; i--) {
        parent->children[i + 1] = parent->children[i];
    }
    parent->children[index + 1] = newNode;

    for (int i = parent->num_keys - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
    }
    parent->keys[index] = child->keys[t - 1];
    parent->num_keys++;
}

void insertNonFull(TreeNode *node, int key) {
    int i = node->num_keys - 1;

    if (node->is_leaf) {
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    } else {
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++;

        if (node->children[i]->num_keys == M - 1) {
            splitChild(node, i);
            if (node->keys[i] < key) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

TreeNode* insert(TreeNode *root, int key) {
    if (root == NULL) {
        root = createNode(true);
        root->keys[0] = key;
        root->num_keys = 1;
    } else {
        if (root->num_keys == M - 1) {
            TreeNode *new_root = createNode(false);
            new_root->children[0] = root;
            splitChild(new_root, 0);
            int i = 0;
            if (new_root->keys[0] < key) {
                i++;
            }
            insertNonFull(new_root->children[i], key);
            root = new_root;
        } else {
            insertNonFull(root, key);
        }
    }
    return root;
}

int findKey(TreeNode *node, int key) {
    int idx = 0;
    while (idx < node->num_keys && node->keys[idx] < key)
        ++idx;
    return idx;
}

void removeFromLeaf(TreeNode *node, int index) {
    for (int i = index + 1; i < node->num_keys; ++i)
        node->keys[i - 1] = node->keys[i];
    node->num_keys--;
}

int getPred(TreeNode *node, int index) {
    TreeNode *curr = node->children[index];
    while (!curr->is_leaf)
        curr = curr->children[curr->num_keys];
    return curr->keys[curr->num_keys - 1];
}

int getSucc(TreeNode *node, int index) {
    TreeNode *curr = node->children[index + 1];
    while (!curr->is_leaf)
        curr = curr->children[0];
    return curr->keys[0];
}

void fill(TreeNode *node, int index) {
    int t = (M + 1) / 2;
    if (index != 0 && node->children[index - 1]->num_keys >= t) {
        TreeNode *child = node->children[index];
        TreeNode *sibling = node->children[index - 1];

        for (int i = child->num_keys - 1; i >= 0; --i)
            child->keys[i + 1] = child->keys[i];

        if (!child->is_leaf) {
            for (int i = child->num_keys; i >= 0; --i)
                child->children[i + 1] = child->children[i];
        }

        child->keys[0] = node->keys[index - 1];
        if (!child->is_leaf)
            child->children[0] = sibling->children[sibling->num_keys];

        node->keys[index - 1] = sibling->keys[sibling->num_keys - 1];
        child->num_keys++;
        sibling->num_keys--;
    } else if (index != node->num_keys && node->children[index + 1]->num_keys >= t) {
        TreeNode *child = node->children[index];
        TreeNode *sibling = node->children[index + 1];

        child->keys[child->num_keys] = node->keys[index];
        if (!child->is_leaf)
            child->children[child->num_keys + 1] = sibling->children[0];

        node->keys[index] = sibling->keys[0];

        for (int i = 1; i < sibling->num_keys; ++i)
            sibling->keys[i - 1] = sibling->keys[i];
        if (!sibling->is_leaf) {
            for (int i = 1; i <= sibling->num_keys; ++i)
                sibling->children[i - 1] = sibling->children[i];
        }
        child->num_keys++;
        sibling->num_keys--;
    } else {
        if (index != node->num_keys) {
            TreeNode *child = node->children[index];
            TreeNode *sibling = node->children[index + 1];

            child->keys[t - 1] = node->keys[index];
            for (int i = 0; i < sibling->num_keys; ++i)
                child->keys[i + t] = sibling->keys[i];

            if (!child->is_leaf) {
                for (int i = 0; i <= sibling->num_keys; ++i)
                    child->children[i + t] = sibling->children[i];
            }

            for (int i = index + 1; i < node->num_keys; ++i)
                node->keys[i - 1] = node->keys[i];
            for (int i = index + 2; i <= node->num_keys; ++i)
                node->children[i - 1] = node->children[i];
            
            child->num_keys += sibling->num_keys + 1;
            node->num_keys--;
            free(sibling->keys);
            free(sibling->children);
            free(sibling);
        } else {
            TreeNode *child = node->children[index];
            TreeNode *sibling = node->children[index - 1];

            sibling->keys[t - 1] = node->keys[index - 1];
            for (int i = 0; i < child->num_keys; ++i)
                sibling->keys[i + t] = child->keys[i];

            if (!sibling->is_leaf) {
                for (int i = 0; i <= child->num_keys; ++i)
                    sibling->children[i + t] = child->children[i];
            }

            for (int i = index; i < node->num_keys; ++i)
                node->keys[i - 1] = node->keys[i];
            for (int i = index + 1; i <= node->num_keys; ++i)
                node->children[i - 1] = node->children[i];

            sibling->num_keys += child->num_keys + 1;
            node->num_keys--;
            free(child->keys);
            free(child->children);
            free(child);
        }
    }
}

void removeFromInternalNode(TreeNode *node, int index) {
    int key = node->keys[index];
    int t = (M + 1) / 2;

    if (node->children[index]->num_keys >= t) {
        int pred = getPred(node, index);
        node->keys[index] = pred;
        deleteFromNode(node->children[index], pred);
    } else if (node->children[index + 1]->num_keys >= t) {
        int succ = getSucc(node, index);
        node->keys[index] = succ;
        deleteFromNode(node->children[index + 1], succ);
    } else {
        TreeNode *child = node->children[index];
        TreeNode *sibling = node->children[index + 1];
        
        child->keys[t - 1] = key;
        for (int i = 0; i < sibling->num_keys; ++i)
            child->keys[i + t] = sibling->keys[i];

        if (!child->is_leaf) {
            for (int i = 0; i <= sibling->num_keys; ++i)
                child->children[i + t] = sibling->children[i];
        }

        for (int i = index + 1; i < node->num_keys; ++i)
            node->keys[i - 1] = node->keys[i];
        for (int i = index + 2; i <= node->num_keys; ++i)
            node->children[i - 1] = node->children[i];
        
        child->num_keys += sibling->num_keys + 1;
        node->num_keys--;
        
        free(sibling->keys);
        free(sibling->children);
        free(sibling);

        deleteFromNode(child, key);
    }
}

void deleteFromNode(TreeNode *node, int key) {
    int t = (M + 1) / 2;
    int idx = findKey(node, key);

    if (idx < node->num_keys && node->keys[idx] == key) {
        if (node->is_leaf)
            removeFromLeaf(node, idx);
        else
            removeFromInternalNode(node, idx);
    } else {
        if (node->is_leaf) {
            printf("The key %d does not exist in the tree.\n", key);
            return;
        }

        bool flag = (idx == node->num_keys);

        if (node->children[idx]->num_keys < t)
            fill(node, idx);

        if (flag && idx > node->num_keys)
            deleteFromNode(node->children[idx - 1], key);
        else
            deleteFromNode(node->children[idx], key);
    }
}

TreeNode* delete(TreeNode *root, int key) {
    if (!root) {
        printf("The tree is empty.\n");
        return NULL;
    }

    deleteFromNode(root, key);

    if (root->num_keys == 0) {
        TreeNode *tmp = root;
        if (root->is_leaf) {
            root = NULL;
        } else {
            root = root->children[0];
        }
        free(tmp->keys);
        free(tmp->children);
        free(tmp);
    }
    return root;
}

void traverse(TreeNode *root) {
    if (root != NULL) {
        int i;
        for (i = 0; i < root->num_keys; i++) {
            if (!root->is_leaf) {
                traverse(root->children[i]);
            }
            printf("%d ", root->keys[i]);
        }
        if (!root->is_leaf) {
            traverse(root->children[i]);
        }
    }
}

void freeTree(TreeNode *node) {
    if (node == NULL) {
        return;
    }
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            freeTree(node->children[i]);
        }
    }
    free(node->keys);
    free(node->children);
    free(node);
}

int main() {
    TreeNode *root = NULL;

    printf("Enter the maximum degree (M) for the B-tree (M >= 3): ");
    scanf("%d", &M);

    if (M < 3) {
        printf("Invalid degree. M must be at least 3.\n");
        return 1;
    }

    root = insert(root, 10);
    root = insert(root, 20);
    root = insert(root, 5);
    root = insert(root, 6);
    root = insert(root, 12);
    root = insert(root, 30);
    root = insert(root, 7);
    root = insert(root, 17);

    printf("In-order traversal of the constructed B-tree: ");
    traverse(root);
    printf("\n\n");

    printf("Deleting key 6...\n");
    root = delete(root, 6);
    printf("Traversal after deleting 6: ");
    traverse(root);
    printf("\n\n");

    printf("Deleting key 12...\n");
    root = delete(root, 12);
    printf("Traversal after deleting 12: ");
    traverse(root);
    printf("\n\n");
    
    printf("Deleting key 10 (an internal node key)...\n");
    root = delete(root, 10);
    printf("Traversal after deleting 10: ");
    traverse(root);
    printf("\n\n");

    printf("Deleting a non-existent key 99...\n");
    root = delete(root, 99);
    printf("Traversal after trying to delete 99: ");
    traverse(root);
    printf("\n\n");

    freeTree(root);

    return 0;
}
