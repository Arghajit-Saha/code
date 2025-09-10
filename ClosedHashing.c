#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 10

typedef struct {
    int key;
    int status;
} HashTableItem;

HashTableItem hashTable[TABLE_SIZE];

void initializeHashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i].key = 0;
        hashTable[i].status = 0;
    }
}

int hashFunction(int key) {
    return key % TABLE_SIZE;
}

void insert(int key) {
    int index = hashFunction(key);
    int originalIndex = index;
    int i = 1;

    while (hashTable[index].status == 1) {
        index = (originalIndex + i) % TABLE_SIZE;
        i++;
        
        if (index == originalIndex) {
            printf("Hash table is full! Cannot insert key %d.\n", key);
            return;
        }
    }

    hashTable[index].key = key;
    hashTable[index].status = 1;
    printf("Inserted key %d at index %d.\n", key, index);
}

void display() {
    printf("\n--- Hash Table ---\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        printf("Index %d: ", i);
        if (hashTable[i].status == 1) {
            printf("[Key: %d]\n", hashTable[i].key);
        } else {
            printf("[Empty]\n");
        }
    }
    printf("------------------\n\n");
}

int main() {
    int n, key;
    
    initializeHashTable();

    printf("Enter number of keys to insert: ");
    scanf("%d", &n);

    printf("Enter %d keys:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &key);
        insert(key);
    }
    
    display();

    return 0;
}