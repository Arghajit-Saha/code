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

    newNode->num_keys = M - 1 - t;

    for (int i = 0; i < newNode->num_keys; i++) {
        newNode->keys[i] = child->keys[i + t];
    }

    if (!child->is_leaf) {
        for (int i = 0; i < M - t; i++) {
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
            insertNonFull(new_root, key);
            root = new_root;
        } else {
            insertNonFull(root, key);
        }
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
    printf("\n");

    freeTree(root);

    return 0;
}