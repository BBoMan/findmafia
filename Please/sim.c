#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// FIFO queue structure
typedef struct {
    int *items;               // Dynamically allocated array
    int front;                // Index of the first element
    int rear;                 // Index of the last element
    int size;                 // Current size of the queue
    int max;                  // Maximum size of the queue
} Queue;

void initQueue(Queue *q, int frameSize) {  // Queue initialization function
    q->items = (int *)malloc(sizeof(int) * frameSize);  // Dynamically allocate memory for the queue
    q->front = -1;  // Initialize front
    q->rear = -1;   // Initialize rear
    q->size = 0;    // Initialize size
    q->max = frameSize;  // Set the maximum size
}

void addQueue(Queue *q, int value) {  // Function to add an element to the queue
    if (q->size == q->max) {  // If the queue is full
        q->front = (q->front + 1) % q->max;  // Remove the oldest element
    } else {
        q->size++;  // Increase the size of the queue
    }
    q->rear = (q->rear + 1) % q->max;  // Update rear
    q->items[q->rear] = value;  // Add new element
}

bool existQueue(Queue *q, int value) {  // Function to check if an element exists in the queue
    int i = q->front;  // Start from the front
    for (int count = 0; count < q->size; count++) {  // Iterate through the queue
        if (q->items[i] == value) {  // If the value exists
            return true;  // Return true
        }
        i = (i + 1) % q->max;  // Move to the next index
    }
    return false;  // If the value does not exist, return false
}

void addFifo(Queue *q, int value, int *hit, int *fault) {  // Function to add a page using the FIFO algorithm
    if (existQueue(q, value)) {  // If the value exists
        (*hit)++;  // Increase hit count
    } else {
        addQueue(q, value);  // Add value to the queue
        (*fault)++;  // Increase fault count
    }
}

// NRU node structure definition
struct Node {
    int pageNum;
    int R;
    int M;
    int class;
    struct Node* next;
};

// Function to create a new node, determined by read or write
struct Node* createNode(int pageNum, char* operator) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));  // Allocate memory for newNode
    if (strcmp(operator, "read\n") == 0) {  // If operator is read
        newNode->pageNum = pageNum;  // Register pageNumber
        newNode->R = 0;  // R bit = 0
        newNode->M = 0;  // M bit = 0
        newNode->class = 0;  // Therefore class = 0
        newNode->next = NULL;  // Since it's a new node, next = NULL
        return newNode;  // Return the new node
    }
    else if (strcmp(operator, "write\n") == 0) {  // If operator is write
        newNode->pageNum = pageNum;  // Register pageNumber
        newNode->R = 0;  // R bit = 0
        newNode->M = 1;  // M bit = 1
        newNode->class = 1;  // Therefore class = 1
        newNode->next = NULL;  // Since it's a new node, next = NULL
        return newNode;  // Return the new node
    }
    else {  // If operator is neither read nor write, error
        printf("operator error\n");
        free(newNode);  // Free allocated memory
        return NULL;  // Return NULL
    }
}

// Function to add a node to the linked list
void addNode(struct Node** head, int pageNum, char* operator) {
    struct Node* newNode = createNode(pageNum, operator);  // Allocate memory for newNode
    struct Node* last = *head;  // Copy head value to move to the end of the linked list
    if (last == NULL) {  // If the linked list is empty
        *head = newNode;  // Add the new node to the head
        return;  // End the addNode() function
    }
    while (last->next != NULL) {  // Move to the end of the linked list
        last = last->next;  // Move last by one node
    }
    last->next = newNode;  // Add the new node to the end of the linked list
}

// Function to delete a node
void delNode(struct Node** head, int minClass) {
    struct Node* temp = *head;  // Copy head value to delete node with class = minClass
    struct Node* prev = NULL;  // Node to store the previous node of the node to be deleted

    if (temp != NULL && temp->class == minClass) {  // If the head node itself is the node to be deleted
        *head = temp->next;  // Change the head to the next node
        temp->next = NULL;  // Disconnect the deleted node from the next node
        free(temp);  // Free the allocated memory
        return;  // End the delNode() function
    }

    while (temp != NULL && temp->class != minClass) {  // Traverse the list until the node to be deleted is found
        prev = temp;  // Copy the current node value to prev
        temp = temp->next;  // Move to the next node
    }
    prev->next = temp->next;  // Set prev's next to temp's next
    temp->next = NULL;  // Disconnect the deleted node from the next node
    free(temp);  // Free the allocated memory
    return;  // End the delNode() function
}

// Return class based on state bits
int checkClass(int R, int M) {  // Determine class based on R bit and M bit
    if(R == 0 && M == 0)  // If 00, class = 0
        return 0;
    else if(R == 0 && M == 1)  // If 01, class = 1
        return 1;
    else if(R == 1 && M == 0)  // If 10, class = 2
        return 2;
    else if(R == 1 && M == 1)  // If 11, class = 3
        return 3;
}

// Replace page based on the new pageNum
void addNru(struct Node** head, int pageNum, char* operator, int *cntNode, int frameSize, int *hit, int *fault) {
    struct Node* temp = *head;  // Copy head value to check if there is a node with the new pageNum
    struct Node* prev = NULL;  // Node to store the previous node of the node to be deleted
    struct Node* last = *head;  // Copy head value to move to the end of the linked list
    int minClass = 3;  // Initialize minClass value to 3

    if (temp != NULL && temp->next != NULL) {
        while (last->next != NULL) {  // Move to the end of the linked list
            if (last->class < minClass)  // If the node's class is less than minClass
                minClass = last->class;  // Update minClass value
            last = last->next;   
        }
        if (last->class < minClass)  // The last node's class is not reflected in minClass
            minClass = last->class;  // Reflect the last node's class in minClass

        struct Node* fLast = last;  // Remember the initial last node

        while (temp != NULL && temp->pageNum != pageNum) {  // Search for a matching node
            prev = temp;
            temp = temp->next;
        }        

        if (temp == NULL) {  // If there is no matching node
            if (*cntNode < frameSize) {  // If the number of nodes is less than frameSize
                addNode(head, pageNum, operator);  // Add a node to the linked list
                (*fault)++;  // Increase fault count by 1
                (*cntNode)++;  // Increase the number of nodes by 1
                return;  // End the addNru() function
            }
            else {  // If the number of nodes is greater than frameSize
                delNode(head, minClass);  // Delete the first node with class = minClass in the linked list
                addNode(head, pageNum, operator);  // Add a node to the linked list
                (*fault)++;  // Increase fault count by 1
                return;  // End the addNru() function
            }
        }

        if (temp == fLast) {  // If the matching node is the last node
            if (strcmp(operator, "write\n") == 0) {  // If the operator was write
                temp->R = 1;  // R = 1
                temp->M = 1;  // M = 1
                temp->class = 3;  // class = 3
            }
            else if (strcmp(operator, "read\n") == 0) {  // If the operator was read
                temp->R = 1;  // R = 1
                temp->class = checkClass(temp->R, temp->M);  // Check the class value
            }
            (*hit)++;  // Increase hit count by 1
            return;
        }

        if (prev != NULL) {
            prev->next = temp->next;  // Disconnect temp
        } else {
            *head = temp->next;  // Change head
        }

        temp->next = NULL;  // Set next to NULL
        last->next = temp;  // Move to the end
        if (strcmp(operator, "write\n") == 0) {  // If the operator was write
            temp->R = 1;  // R = 1
            temp->M = 1;  // M = 1
            temp->class = 3;  // class = 3
        }
        else if (strcmp(operator, "read\n") == 0) {  // If the operator was read
            temp->R = 1;  // R = 1
            temp->class = checkClass(temp->R, temp->M);  // Check the class value
        }
        (*hit)++;  // Increase hit count by 1
        return;
    }
    else if (temp == NULL) {  // If the linked list is empty
        addNode(head, pageNum, operator);  // Add a node to the linked list
        (*fault)++;  // Increase fault count by 1
        (*cntNode)++;  // Increase the number of nodes by 1
        return;
    }
    else if (temp->next == NULL) {  // If there is only one node in the linked list
        if (temp->pageNum == pageNum) {
            if (strcmp(operator, "write\n") == 0) {  // If the operator was write
                temp->R = 1;  // R = 1
                temp->M = 1;  // M = 1
                temp->class = 3;  // class = 3
            }
            else if (strcmp(operator, "read\n") == 0) {  // If the operator was read
                temp->R = 1;  // R = 1
                temp->class = checkClass(temp->R, temp->M);  // Check the class value
            }
            (*hit)++;  // Increase hit count by 1
            return;  
        }
        else {
            addNode(head, pageNum, operator);  // Add a node to the linked list
            (*fault)++;  // Increase fault count by 1
            (*cntNode)++;  // Increase the number of nodes by 1
            return;
        }
    }
}

int main(int argc, char *argv[]) {  // argv[1] = fifo or nru / argv[2] = number of frames
    char *operator;                 // Operator variable
    int pageNum = 0;                // Page number variable
    int frameSize = atoi(argv[2]);  // Set frame size
    int access = 0;                 // Access count
    int read = 0;                   // Read count
    int write = 0;                  // Write count
    int hit = 0;                    // Hit count
    int fault = 0;                  // Page fault count
    char line[12];                  // Buffer to store each line of input

    struct Node* head = NULL;       // Head of the linked list for NRU algorithm
    int minClass = 3;               // Minimum class value
    int cntNode = 0;                // Number of nodes

    FILE *file = fopen("access.list", "r");  // Open file
    if (strcmp(argv[1], "fifo") == 0) {   // If FIFO algorithm is selected
        Queue q;                           // Create queue
        initQueue(&q, frameSize);          // Initialize queue

        while (fgets(line, sizeof(line), file)) {  // Read file line by line
            access++;                                 // Increase access count
            pageNum = atoi(strtok(line, " "));        // Parse page number
            operator = strtok(NULL, " ");             // Parse operator
            if (strcmp(operator, "read\n") == 0)      // Handle read operator
                read++;                               // Increase read count
            else if (strcmp(operator, "write\n") == 0)// Handle write operator
                write++;                              // Increase write count
            addFifo(&q, pageNum, &hit, &fault);       // Add page using FIFO algorithm
        }
        free(q.items);  // Free queue memory
    }
    else if (strcmp(argv[1], "nru") == 0) {      // If NRU algorithm is selected
        while (fgets(line, sizeof(line), file)) {  // Read file line by line
            access++;                                 // Increase access count
            pageNum = atoi(strtok(line, " "));        // Parse page number
            operator = strtok(NULL, " ");             // Parse operator
            addNru(&head, pageNum, operator, &cntNode, frameSize, &hit, &fault);  // Add page using NRU algorithm
            if (strcmp(operator, "read\n") == 0)      // Handle read operator
                read++;                               // Increase read count
            else if (strcmp(operator, "write\n") == 0)// Handle write operator
                write++;                              // Increase write count
        }
    }
    fclose(file);  // Close file

    printf("Total number of access: %d\n", access);
    printf("Total number of read: %d\n", read);
    printf("Total number of write: %d\n", write);
    printf("Number of page hits: %d\n", hit);
    printf("Number of page faults: %d\n", fault);
    printf("Page fault rate: %d/%d = %.2f%%\n", fault, access, ((float)fault / (float)access) * 100);
    printf("frame size : %d\n", cntNode);

    return 0;
}