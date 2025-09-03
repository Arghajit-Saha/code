#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 10

// --- Hashing Functions ---

// 1. Division Method
int divisionHash(int value) {
    return value % TABLE_SIZE;
}

// 2. Mid-Square Method
int midSquareHash(int value) {
    // Square the value
    long long square = (long long)value * value;
    
    // A simple extraction for small table sizes. 
    // This works well for numbers whose squares have 3 or 4 digits.
    if (square < 100) { // For small squares, fallback to division
        return square % TABLE_SIZE;
    }
    square /= 10; // Remove the last digit (e.g., 1225 -> 122)
    return (int)(square % 10); // Return the new last digit (e.g., 122 % 10 -> 2)
}

// 3. Folding Method (Sum of parts)
int foldingHash(int value) {
    int sum = 0;
    int temp = value;
    // Partition the number into chunks of 2 digits and sum them up
    while (temp > 0) {
        sum += temp % 100; 
        temp /= 100;
    }
    return sum % TABLE_SIZE;
}


// --- Separate Chaining Components (using Linked Lists) ---

// Node structure for the linked list
struct Node {
    int data;
    struct Node *next;
};

// The hash table is an array of pointers to Node
struct Node *chain[TABLE_SIZE];

// Initialize all chains to NULL
void initChain() {
    for (int i = 0; i < TABLE_SIZE; i++)
        chain[i] = NULL;
}

// Insert a value into the hash table using separate chaining
void insertChain(int value, int (*hashFunc)(int)) {
    struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
    if (!newNode) {
        printf("Memory allocation failed!\n");
        return;
    }
    newNode->data = value;
    newNode->next = NULL;

    // Calculate hash key using the provided hash function
    int key = hashFunc(value);

    if (chain[key] == NULL) {
        chain[key] = newNode;
    } else {
        printf("Collision detected for value %d at index %d. Adding to chain.\n", value, key);
        struct Node *temp = chain[key];
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

// Display the hash table with separate chains
void printChain() {
    printf("\n--- Hash Table (Separate Chaining) ---\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        struct Node *temp = chain[i];
        printf("Index [%d]: ", i);
        while (temp) {
            printf("%d -> ", temp->data);
            temp = temp->next;
        }
        printf("NULL\n");
    }
}

// Search for a value in the hash table
int searchChain(int value, int (*hashFunc)(int)) {
    int key = hashFunc(value);
    struct Node *temp = chain[key];
    while(temp) {
        if(temp->data == value)
            return 1; // Found
        temp = temp->next;
    }
    return 0; // Not found
}


// --- Open Addressing (Linear & Quadratic Probing) Components ---

// The hash table is a simple array
int hashTable[TABLE_SIZE];

// Initialize hash table with -1 to indicate empty slots
void initHashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = -1; 
    }
}

// Insert a value using Linear Probing
void insertLinear(int value, int (*hashFunc)(int)) {
    int key = hashFunc(value);
    int index = key;

    while (hashTable[index] != -1) {
        printf("Collision detected for value %d at index %d. Probing linearly.\n", value, index);
        index = (index + 1) % TABLE_SIZE;
        if (index == key) {
            printf("Hash table is full! Cannot insert %d.\n", value);
            return;
        }
    }
    hashTable[index] = value;
    printf("Value %d inserted at index %d.\n", value, index);
}

// Insert a value using Quadratic Probing
void insertQuadratic(int value, int (*hashFunc)(int)) {
    int key = hashFunc(value);
    int index = key;
    int i = 1;

    while (hashTable[index] != -1) {
        printf("Collision detected for value %d at index %d. Probing quadratically.\n", value, index);
        index = (key + i * i) % TABLE_SIZE;
        i++;
        if (i > TABLE_SIZE) { 
            printf("Could not find a slot for %d. Table might be too full.\n", value);
            return;
        }
    }
    hashTable[index] = value;
    printf("Value %d inserted at index %d.\n", value, index);
}

// Display the hash table for open addressing
void printHashTable() {
    printf("\n--- Hash Table ---\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashTable[i] != -1)
            printf("Index [%d]: %d\n", i, hashTable[i]);
        else
            printf("Index [%d]: --\n", i);
    }
}

// Search for a value using Linear Probing
void searchLinear(int value, int (*hashFunc)(int)) {
    int key = hashFunc(value);
    int index = key;

    while (hashTable[index] != -1) {
        if (hashTable[index] == value) {
            printf("Value %d found at index %d.\n", value, index);
            return;
        }
        index = (index + 1) % TABLE_SIZE;
        if (index == key) {
            break;
        }
    }
    printf("Value %d not found.\n", value);
}

// Search for a value using Quadratic Probing
void searchQuadratic(int value, int (*hashFunc)(int)) {
    int key = hashFunc(value);
    int index = key;
    int i = 1;

    while (hashTable[index] != -1) {
        if (hashTable[index] == value) {
            printf("Value %d found at index %d.\n", value, index);
            return;
        }
        index = (key + i * i) % TABLE_SIZE;
        i++;
        if (i > TABLE_SIZE * 2) { // Added a more robust loop break
            break;
        }
    }
     printf("Value %d not found.\n", value);
}


// --- Menu Handling Functions ---

// Generic function to select a hash method
int (*selectHashFunction())(int) {
    int hashChoice;
    printf("\n--- Select a Hashing Function ---\n");
    printf("1. Division Method\n");
    printf("2. Mid-Square Method\n");
    printf("3. Folding Method\n");
    printf("Enter your choice: ");
    scanf("%d", &hashChoice);

    switch(hashChoice) {
        case 1: 
            printf("Using Division Hashing.\n");
            return divisionHash;
        case 2: 
            printf("Using Mid-Square Hashing.\n");
            return midSquareHash;
        case 3: 
            printf("Using Folding Hashing.\n");
            return foldingHash;
        default:
            printf("Invalid choice. Defaulting to Division Hashing.\n");
            return divisionHash;
    }
}


void handleLinearProbing() {
    int choice, value;
    int (*selectedHashFunc)(int) = selectHashFunction();
    initHashTable(); 

    while (1) {
        printf("\n--- Linear Probing Menu ---\n");
        printf("1. Insert\n");
        printf("2. Search\n");
        printf("3. Display Hash Table\n");
        printf("4. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter value to insert: ");
                scanf("%d", &value);
                insertLinear(value, selectedHashFunc);
                break;
            case 2:
                printf("Enter value to search: ");
                scanf("%d", &value);
                searchLinear(value, selectedHashFunc);
                break;
            case 3:
                printHashTable();
                break;
            case 4:
                return;
            default:
                printf("Invalid choice!\n");
        }
    }
}

void handleQuadraticProbing() {
    int choice, value;
    int (*selectedHashFunc)(int) = selectHashFunction();
    initHashTable();

    while (1) {
        printf("\n--- Quadratic Probing Menu ---\n");
        printf("1. Insert\n");
        printf("2. Search\n");
        printf("3. Display Hash Table\n");
        printf("4. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter value to insert: ");
                scanf("%d", &value);
                insertQuadratic(value, selectedHashFunc);
                break;
            case 2:
                printf("Enter value to search: ");
                scanf("%d", &value);
                searchQuadratic(value, selectedHashFunc);
                break;
            case 3:
                printHashTable();
                break;
            case 4:
                return;
            default:
                printf("Invalid choice!\n");
        }
    }
}

void handleSeparateChaining() {
    int choice, value;
    int (*selectedHashFunc)(int) = selectHashFunction();
    initChain();

    while (1) {
        printf("\n--- Separate Chaining Menu ---\n");
        printf("1. Insert\n");
        printf("2. Search\n");
        printf("3. Display Hash Table\n");
        printf("4. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter value to insert: ");
                scanf("%d", &value);
                insertChain(value, selectedHashFunc);
                break;
            case 2:
                printf("Enter value to search: ");
                scanf("%d", &value);
                 if(searchChain(value, selectedHashFunc))
                    printf("Value %d found in the hash table.\n", value);
                else
                    printf("Value %d not found.\n", value);
                break;
            case 3:
                printChain();
                break;
            case 4:
                return;
            default:
                printf("Invalid choice!\n");
        }
    }
}

// --- Main Program Driver ---

int main() {
    int choice;
    while (1) {
        printf("\n=============== Main Hashing Menu ===============\n");
        printf("Choose a collision resolution technique:\n");
        printf("1. Linear Probing\n");
        printf("2. Quadratic Probing\n");
        printf("3. Separate Chaining\n");
        printf("4. Exit\n");
        printf("=================================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                handleLinearProbing();
                break;
            case 2:
                handleQuadraticProbing();
                break;
            case 3:
                handleSeparateChaining();
                break;
            case 4:
                printf("Exiting...\n");
                exit(0);
            default:
                printf("Invalid choice, please try again.\n");
        }
    }
    return 0;
}

