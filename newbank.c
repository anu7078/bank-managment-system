/***************************************************
 * Enhanced Bank Management System
 * Features:
 * - User/Admin authentication
 * - Customer management using linked list
 * - Transaction handling using stack
 * - Customer service requests using queue
 * - Branch connectivity using graph with shortest path
 * - Data persistence using file handling
 * - Enhanced UI with colors and menu systems
 ***************************************************/

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 #include <time.h>
 #include <float.h>
 #include <limits.h>
 
 // Constants for terminal colors
 #define RESET "\033[0m"
 #define BOLD "\033[1m"
 #define BLACK "\033[30m"
 #define RED "\033[31m"
 #define GREEN "\033[32m"
 #define YELLOW "\033[33m"
 #define BLUE "\033[34m"
 #define MAGENTA "\033[35m"
 #define CYAN "\033[36m"
 #define WHITE "\033[37m"
 #define BG_BLACK "\033[40m"
 #define BG_RED "\033[41m"
 #define BG_GREEN "\033[42m"
 #define BG_YELLOW "\033[43m"
 #define BG_BLUE "\033[44m"
 #define BG_MAGENTA "\033[45m"
 #define BG_CYAN "\033[46m"
 #define BG_WHITE "\033[47m"
 
 /***************************************************
  * SECTION 1: STRUCTURES DEFINITION
  ***************************************************/
 
 // User authentication structure
 struct User
 {
     char username[50];
     char password[50];
     bool isAdmin;
 };
 
 // Bank account structure
 struct Account
 {
     int accountNo;
     char name[50];
     float balance;
     char address[100];
     char phoneNumber[15];
     char email[50];
     char dateCreated[20];
     struct TransactionHistory *history;
 };
 
 // Transaction structure
 struct Transaction
 {
     int transactionId;
     int accountNo;
     char type[20]; // "deposit", "withdraw", "transfer"
     float amount;
     char date[20];
     char time[10];
 };
 
 // Transaction history node for stack
 struct TransactionNode
 {
     struct Transaction data;
     struct TransactionNode *next;
 };
 
 // Customer service request structure
 struct ServiceRequest
 {
     int requestId;
     int accountNo;
     char requestType[50];
     char description[200];
     bool isProcessed;
     char dateSubmitted[20];
     int priority; // 1-5, 5 being highest
 };
 
 // Service request node for queue
 struct RequestNode
 {
     struct ServiceRequest data;
     struct RequestNode *next;
 };
 
 // Bank branch structure
 struct Branch
 {
     int branchId;
     char branchName[50];
     char location[100];
     char managerName[50];
     char phoneNumber[15];
     int employeeCount;
 };
 
 // Graph node for branch connectivity
 struct BranchNode
 {
     struct Branch data;
     struct EdgeList *connections;
 };
 
 // Edge list for graph connections
 struct EdgeList
 {
     int branchId;
     float distance;
     struct EdgeList *next;
 };
 
 // Account node for linked list
 struct AccountNode
 {
     struct Account data;
     struct AccountNode *next;
 };
 
 // Global data structures
 struct User currentUser;
 struct AccountNode *accountList = NULL;
 struct TransactionNode *transactionStack = NULL;
 struct RequestNode *serviceQueue = NULL;
 struct RequestNode *serviceQueueRear = NULL;
 struct BranchNode branchGraph[10]; // Assuming max 10 branches
 int branchCount = 0;
 
 /***************************************************
  * SECTION 2: AUTHENTICATION FUNCTIONS
  ***************************************************/
 
 struct User users[10]; // Array to store users (admin & regular)
 int userCount = 0;
 
 // Initialize with default admin
 void initializeUsers()
 {
     strcpy(users[0].username, "admin");
     strcpy(users[0].password, "admin123");
     users[0].isAdmin = true;
 
     strcpy(users[1].username, "user");
     strcpy(users[1].password, "user123");
     users[1].isAdmin = false;
 
     userCount = 2;
 }
 
 // Authenticate user
 bool login(char *username, char *password, bool *isAdmin)
 {
     for (int i = 0; i < userCount; i++)
     {
         if (strcmp(users[i].username, username) == 0 &&
             strcmp(users[i].password, password) == 0)
         {
             *isAdmin = users[i].isAdmin;
             strcpy(currentUser.username, username);
             strcpy(currentUser.password, password);
             currentUser.isAdmin = *isAdmin;
             return true;
         }
     }
     return false;
 }
 
 // Add new user (admin only)
 bool addUser(char *username, char *password, bool isAdmin)
 {
     if (userCount >= 10)
     {
         return false; // Maximum users reached
     }
 
     // Check if username already exists
     for (int i = 0; i < userCount; i++)
     {
         if (strcmp(users[i].username, username) == 0)
         {
             return false; // Username already exists
         }
     }
 
     strcpy(users[userCount].username, username);
     strcpy(users[userCount].password, password);
     users[userCount].isAdmin = isAdmin;
     userCount++;
     return true;
 }
 
 /***************************************************
  * SECTION 3: DATA STRUCTURE OPERATIONS
  ***************************************************/
 
 // Linked List Operations
 struct AccountNode *createAccountNode(int accNo, char *name, float balance, char *address, char *phone, char *email)
 {
     struct AccountNode *newNode = (struct AccountNode *)malloc(sizeof(struct AccountNode));
     newNode->data.accountNo = accNo;
     strcpy(newNode->data.name, name);
     newNode->data.balance = balance;
     
     // New fields
     strcpy(newNode->data.address, address);
     strcpy(newNode->data.phoneNumber, phone);
     strcpy(newNode->data.email, email);
     
     // Get current date for account creation
     time_t now = time(NULL);
     struct tm *t = localtime(&now);
     sprintf(newNode->data.dateCreated, "%04d-%02d-%02d", 
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
     
     newNode->data.history = NULL;
     newNode->next = NULL;
     return newNode;
 }
 
 bool addAccount(int accNo, char *name, float balance, char *address, char *phone, char *email)
 {
     // Check if account already exists
     struct AccountNode *temp = accountList;
     while (temp != NULL)
     {
         if (temp->data.accountNo == accNo)
         {
             return false; // Account already exists
         }
         temp = temp->next;
     }
 
     struct AccountNode *newNode = createAccountNode(accNo, name, balance, address, phone, email);
 
     if (accountList == NULL)
     {
         accountList = newNode;
     }
     else
     {
         newNode->next = accountList;
         accountList = newNode;
     }
     return true;
 }
 
 struct Account *findAccount(int accNo)
 {
     struct AccountNode *temp = accountList;
     while (temp != NULL)
     {
         if (temp->data.accountNo == accNo)
         {
             return &(temp->data);
         }
         temp = temp->next;
     }
     return NULL;
 }
 
 // Stack Operations
 void pushTransaction(struct Transaction transaction)
 {
     struct TransactionNode *newNode = (struct TransactionNode *)malloc(sizeof(struct TransactionNode));
     newNode->data = transaction;
     newNode->next = transactionStack;
     transactionStack = newNode;
 }
 
 struct Transaction popTransaction()
 {
     struct Transaction transaction = {0};
     if (transactionStack == NULL)
     {
         return transaction;
     }
 
     struct TransactionNode *temp = transactionStack;
     transactionStack = transactionStack->next;
     transaction = temp->data;
     free(temp);
     return transaction;
 }
 
 // Queue Operations
 void enqueueRequest(struct ServiceRequest request)
 {
     struct RequestNode *newNode = (struct RequestNode *)malloc(sizeof(struct RequestNode));
     newNode->data = request;
     newNode->next = NULL;
 
     if (serviceQueue == NULL)
     {
         serviceQueue = newNode;
         serviceQueueRear = newNode;
     }
     else
     {
         serviceQueueRear->next = newNode;
         serviceQueueRear = newNode;
     }
 }
 
 struct ServiceRequest dequeueRequest()
 {
     struct ServiceRequest request = {0};
     if (serviceQueue == NULL)
     {
         return request;
     }
 
     struct RequestNode *temp = serviceQueue;
     serviceQueue = serviceQueue->next;
 
     if (serviceQueue == NULL)
     {
         serviceQueueRear = NULL;
     }
 
     request = temp->data;
     free(temp);
     return request;
 }
 
 // Graph Operations
 void addBranch(int branchId, char *branchName, char *location, char *managerName, char *phoneNumber, int employeeCount)
 {
     if (branchCount >= 10)
     {
         return; // Maximum branches reached
     }
 
     branchGraph[branchCount].data.branchId = branchId;
     strcpy(branchGraph[branchCount].data.branchName, branchName);
     strcpy(branchGraph[branchCount].data.location, location);
     strcpy(branchGraph[branchCount].data.managerName, managerName);
     strcpy(branchGraph[branchCount].data.phoneNumber, phoneNumber);
     branchGraph[branchCount].data.employeeCount = employeeCount;
     branchGraph[branchCount].connections = NULL;
     branchCount++;
 }
 
 int findBranchIndex(int branchId)
 {
     for (int i = 0; i < branchCount; i++)
     {
         if (branchGraph[i].data.branchId == branchId)
         {
             return i;
         }
     }
     return -1; // Not found
 }
 
 void addConnection(int sourceBranchId, int targetBranchId, float distance)
 {
     int sourceIndex = findBranchIndex(sourceBranchId);
 
     if (sourceIndex == -1)
     {
         return; // Source branch not found
     }
 
     // Create new connection
     struct EdgeList *newEdge = (struct EdgeList *)malloc(sizeof(struct EdgeList));
     newEdge->branchId = targetBranchId;
     newEdge->distance = distance;
     newEdge->next = branchGraph[sourceIndex].connections;
     branchGraph[sourceIndex].connections = newEdge;
 }
 
 // Dijkstra's Algorithm for Shortest Path
 void findShortestPath(int startBranchId, int endBranchId)
 {
     float distance[10];
     int previous[10];
     bool visited[10];
     int startIndex = findBranchIndex(startBranchId);
     int endIndex = findBranchIndex(endBranchId);
     
     if (startIndex == -1 || endIndex == -1)
     {
         printf("%sOne or both branches not found.%s\n", RED, RESET);
         return;
     }
     
     // Initialize arrays
     for (int i = 0; i < branchCount; i++)
     {
         distance[i] = FLT_MAX;
         previous[i] = -1;
         visited[i] = false;
     }
     
     distance[startIndex] = 0.0;
     
     // Dijkstra's algorithm
     for (int count = 0; count < branchCount; count++)
     {
         // Find minimum distance vertex
         float minDist = FLT_MAX;
         int minIndex = -1;
         
         for (int i = 0; i < branchCount; i++)
         {
             if (!visited[i] && distance[i] < minDist)
             {
                 minDist = distance[i];
                 minIndex = i;
             }
         }
         
         if (minIndex == -1) break; // No reachable vertices
         
         visited[minIndex] = true;
         
         // Update distances of adjacent vertices
         struct EdgeList *edge = branchGraph[minIndex].connections;
         while (edge != NULL)
         {
             int targetIndex = findBranchIndex(edge->branchId);
             if (targetIndex != -1 && !visited[targetIndex])
             {
                 float newDist = distance[minIndex] + edge->distance;
                 if (newDist < distance[targetIndex])
                 {
                     distance[targetIndex] = newDist;
                     previous[targetIndex] = minIndex;
                 }
             }
             edge = edge->next;
         }
     }
     
     // Print result
     if (distance[endIndex] == FLT_MAX)
     {
         printf("%sThere is no path from %s to %s.%s\n", 
                RED, branchGraph[startIndex].data.branchName, 
                branchGraph[endIndex].data.branchName, RESET);
         return;
     }
     
     // Reconstruct path
     printf("\n%s%sShortest Path from %s to %s:%s\n", 
            BG_GREEN, BLACK, branchGraph[startIndex].data.branchName, 
            branchGraph[endIndex].data.branchName, RESET);
     printf("%sTotal Distance: %.2f units%s\n", YELLOW, distance[endIndex], RESET);
     
     // Build path in reverse
     int path[10];
     int pathLength = 0;
     for (int at = endIndex; at != -1; at = previous[at])
     {
         path[pathLength++] = at;
     }
     
     // Print path in correct order
     printf("%sPath: %s", GREEN, RESET);
     for (int i = pathLength - 1; i >= 0; i--)
     {
         printf("%s%s%s", CYAN, branchGraph[path[i]].data.branchName, RESET);
         if (i > 0) printf(" %s→%s ", YELLOW, RESET);
     }
     printf("\n");
 }
 
 // File Handling Operations
 void saveAccountsToFile()
 {
     FILE *file = fopen("accounts.dat", "wb");
     if (file == NULL)
     {
         printf("%sError opening file for saving accounts.%s\n", RED, RESET);
         return;
     }
 
     struct AccountNode *temp = accountList;
     while (temp != NULL)
     {
         fwrite(&(temp->data), sizeof(struct Account), 1, file);
         temp = temp->next;
     }
 
     fclose(file);
 }
 
 void loadAccountsFromFile()
 {
     FILE *file = fopen("accounts.dat", "rb");
     if (file == NULL)
     {
         printf("%sNo previous account data found.%s\n", YELLOW, RESET);
         return;
     }
 
     struct Account account;
     while (fread(&account, sizeof(struct Account), 1, file) == 1)
     {
         // The addAccount function has been updated with new parameters, 
         // but we're loading existing records which have the data
         addAccount(account.accountNo, account.name, account.balance, 
                   account.address, account.phoneNumber, account.email);
     }
 
     fclose(file);
 }
 
 // Save transactions to file
 void saveTransactionsToFile()
 {
     FILE *file = fopen("transactions.dat", "wb");
     if (file == NULL)
     {
         printf("%sError opening file for saving transactions.%s\n", RED, RESET);
         return;
     }
 
     struct TransactionNode *temp = transactionStack;
     while (temp != NULL)
     {
         fwrite(&(temp->data), sizeof(struct Transaction), 1, file);
         temp = temp->next;
     }
 
     fclose(file);
 }
 
 // Load transactions from file
 void loadTransactionsFromFile()
 {
     FILE *file = fopen("transactions.dat", "rb");
     if (file == NULL)
     {
         return; // No previous transactions data
     }
 
     // Free existing transaction stack
     while (transactionStack != NULL)
     {
         struct TransactionNode *temp = transactionStack;
         transactionStack = transactionStack->next;
         free(temp);
     }
 
     // Read transactions into a temporary array
     struct Transaction transactions[100]; // Assuming max 100 transactions
     int count = 0;
     
     while (fread(&transactions[count], sizeof(struct Transaction), 1, file) == 1 && count < 100)
     {
         count++;
     }
     
     // Push transactions to stack in reverse order to maintain chronology
     for (int i = count - 1; i >= 0; i--)
     {
         pushTransaction(transactions[i]);
     }
 
     fclose(file);
 }
 
 // Save branches and connections to file
 void saveBranchesToFile()
 {
     FILE *branchFile = fopen("branches.dat", "wb");
     FILE *connectionFile = fopen("connections.dat", "wb");
     
     if (branchFile == NULL || connectionFile == NULL)
     {
         printf("%sError opening files for saving branch data.%s\n", RED, RESET);
         if (branchFile) fclose(branchFile);
         if (connectionFile) fclose(connectionFile);
         return;
     }
 
     // Save branch count
     fwrite(&branchCount, sizeof(int), 1, branchFile);
     
     // Save branches
     for (int i = 0; i < branchCount; i++)
     {
         fwrite(&branchGraph[i].data, sizeof(struct Branch), 1, branchFile);
         
         // Count connections
         int connectionCount = 0;
         struct EdgeList *edge = branchGraph[i].connections;
         while (edge != NULL)
         {
             connectionCount++;
             edge = edge->next;
         }
         
         // Save connection count for this branch
         fwrite(&connectionCount, sizeof(int), 1, connectionFile);
         
         // Save connections
         edge = branchGraph[i].connections;
         while (edge != NULL)
         {
             fwrite(&branchGraph[i].data.branchId, sizeof(int), 1, connectionFile);
             fwrite(&edge->branchId, sizeof(int), 1, connectionFile);
             fwrite(&edge->distance, sizeof(float), 1, connectionFile);
             edge = edge->next;
         }
     }
 
     fclose(branchFile);
     fclose(connectionFile);
 }
 
 // Load branches and connections from file
 void loadBranchesFromFile()
 {
     FILE *branchFile = fopen("branches.dat", "rb");
     FILE *connectionFile = fopen("connections.dat", "rb");
     
     if (branchFile == NULL || connectionFile == NULL)
     {
         if (branchFile) fclose(branchFile);
         if (connectionFile) fclose(connectionFile);
         return; // No previous branch data
     }
 
     // Clear existing branches
     for (int i = 0; i < branchCount; i++)
     {
         struct EdgeList *current = branchGraph[i].connections;
         while (current != NULL)
         {
             struct EdgeList *temp = current;
             current = current->next;
             free(temp);
         }
     }
     
     // Load branch count
     fread(&branchCount, sizeof(int), 1, branchFile);
     
     // Load branches
     for (int i = 0; i < branchCount; i++)
     {
         fread(&branchGraph[i].data, sizeof(struct Branch), 1, branchFile);
         branchGraph[i].connections = NULL;
         
         // Load connection count for this branch
         int connectionCount;
         fread(&connectionCount, sizeof(int), 1, connectionFile);
         
         // Load connections
         for (int j = 0; j < connectionCount; j++)
         {
             int sourceBranchId, targetBranchId;
             float distance;
             
             fread(&sourceBranchId, sizeof(int), 1, connectionFile);
             fread(&targetBranchId, sizeof(int), 1, connectionFile);
             fread(&distance, sizeof(float), 1, connectionFile);
             
             addConnection(sourceBranchId, targetBranchId, distance);
         }
     }
 
     fclose(branchFile);
     fclose(connectionFile);
 }
 
 void saveAllData()
 {
     saveAccountsToFile();
     saveTransactionsToFile();
     saveBranchesToFile();
     printf("%sAll data saved successfully!%s\n", GREEN, RESET);
 }
 
 void loadAllData()
 {
     loadAccountsFromFile();
     loadTransactionsFromFile();
     loadBranchesFromFile();
 }
 
 /***************************************************
  * SECTION 4: TRANSACTION FUNCTIONS
  ***************************************************/
 
 bool deposit(int accountNo, float amount)
 {
     struct Account *account = findAccount(accountNo);
     if (account == NULL)
     {
         return false;
     }
 
     account->balance += amount;
 
     // Record transaction
     struct Transaction transaction;
     transaction.accountNo = accountNo;
     transaction.amount = amount;
     strcpy(transaction.type, "deposit");
     transaction.transactionId = rand() % 10000 + 1000;
     
     // Get current date and time
     time_t now = time(NULL);
     struct tm *t = localtime(&now);
     sprintf(transaction.date, "%04d-%02d-%02d", 
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
     sprintf(transaction.time, "%02d:%02d", t->tm_hour, t->tm_min);
 
     pushTransaction(transaction);
     return true;
 }
 
 bool withdraw(int accountNo, float amount)
 {
     struct Account *account = findAccount(accountNo);
     if (account == NULL || account->balance < amount)
     {
         return false;
     }
 
     account->balance -= amount;
 
     // Record transaction
     struct Transaction transaction;
     transaction.accountNo = accountNo;
     transaction.amount = amount;
     strcpy(transaction.type, "withdraw");
     transaction.transactionId = rand() % 10000 + 1000;
     
     // Get current date and time
     time_t now = time(NULL);
     struct tm *t = localtime(&now);
     sprintf(transaction.date, "%04d-%02d-%02d", 
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
     sprintf(transaction.time, "%02d:%02d", t->tm_hour, t->tm_min);
 
     pushTransaction(transaction);
     return true;
 }
 
 bool transfer(int fromAccountNo, int toAccountNo, float amount)
 {
     struct Account *fromAccount = findAccount(fromAccountNo);
     struct Account *toAccount = findAccount(toAccountNo);
     
     if (fromAccount == NULL || toAccount == NULL || fromAccount->balance < amount)
     {
         return false;
     }
     
     fromAccount->balance -= amount;
     toAccount->balance += amount;
     
     // Record transaction for sender
     struct Transaction transaction;
     transaction.accountNo = fromAccountNo;
     transaction.amount = amount;
     sprintf(transaction.type, "transfer to %d", toAccountNo);
     transaction.transactionId = rand() % 10000 + 1000;
     
     // Get current date and time
     time_t now = time(NULL);
     struct tm *t = localtime(&now);
     sprintf(transaction.date, "%04d-%02d-%02d", 
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
     sprintf(transaction.time, "%02d:%02d", t->tm_hour, t->tm_min);
     
     pushTransaction(transaction);
     
     // Record transaction for receiver
     transaction.accountNo = toAccountNo;
     sprintf(transaction.type, "receive from %d", fromAccountNo);
     transaction.transactionId = rand() % 10000 + 1000;
     
     pushTransaction(transaction);
     
     return true;
 }
 
 void viewTransactions()
 {
     if (transactionStack == NULL)
     {
         printf("%sNo transactions to display.%s\n", YELLOW, RESET);
         return;
     }
 
     printf("\n%s%s Transaction History %s\n", BG_GREEN, BLACK, RESET);
     printf("%s%s%-5s %-8s %-15s %-10s %-12s %-8s %s\n", 
            BG_CYAN, BLACK, "ID", "Account", "Type", "Amount", "Date", "Time", RESET);
 
     struct TransactionNode *temp = transactionStack;
     while (temp != NULL)
     {
         printf("%-5d %-8d %-15s %s$%.2f%s %-12s %-8s\n",
                temp->data.transactionId,
                temp->data.accountNo,
                temp->data.type,
                GREEN,
                temp->data.amount,
                RESET,
                temp->data.date,
                temp->data.time);
         temp = temp->next;
     }
 }
 
 void viewAccountTransactions(int accountNo)
 {
     if (transactionStack == NULL)
     {
         printf("%sNo transactions to display.%s\n", YELLOW, RESET);
         return;
     }
     
     struct Account *account = findAccount(accountNo);
     if (account == NULL)
     {
         printf("%sAccount not found.%s\n", RED, RESET);
         return;
     }
 
     printf("\n%s%s Transaction History for Account %d - %s %s\n", 
            BG_GREEN, BLACK, accountNo, account->name, RESET);
     printf("%s%s%-5s %-15s %-10s %-12s %-8s %s\n", 
            BG_CYAN, BLACK, "ID", "Type", "Amount", "Date", "Time", RESET);
 
     bool found = false;
     struct TransactionNode *temp = transactionStack;
     while (temp != NULL)
     {
         if (temp->data.accountNo == accountNo)
         {
             printf("%-5d %-15s %s$%.2f%s %-12s %-8s\n",
                    temp->data.transactionId,
                    temp->data.type,
                    GREEN,
                    temp->data.amount,
                    RESET,
                    temp->data.date,
                    temp->data.time);
             found = true;
         }
         temp = temp->next;
     }
     
     if (!found)
     {
         printf("%sNo transactions found for this account.%s\n", YELLOW, RESET);
     }
 }
 
 bool undoLastTransaction()
 {
     if (transactionStack == NULL)
     {
         return false;
     }
 
     struct Transaction lastTrans = popTransaction();
     struct Account *account = findAccount(lastTrans.accountNo);
 
     if (account == NULL)
     {
         return false;
     }
 
     if (strcmp(lastTrans.type, "deposit") == 0)
     {
         account->balance -= lastTrans.amount;
     }
     else if (strcmp(lastTrans.type, "withdraw") == 0)
     {
         account->balance += lastTrans.amount;
     }
     else if (strncmp(lastTrans.type, "transfer to", 11) == 0)
     {
         // Extract to account number
         int toAccountNo;
         sscanf(lastTrans.type + 11, "%d", &toAccountNo);
         struct Account *toAccount = findAccount(toAccountNo);
         
         if (toAccount != NULL)
         {
             account->balance += lastTrans.amount;
             toAccount->balance -= lastTrans.amount;
             
             // Pop the receive transaction as well
             popTransaction();
         }
     }
     else if (strncmp(lastTrans.type, "receive from", 12) == 0)
     {
         // Extract from account number
         int fromAccountNo;
         sscanf(lastTrans.type + 12, "%d", &fromAccountNo);
         struct Account *fromAccount = findAccount(fromAccountNo);
         
         if (fromAccount != NULL)
         {
             account->balance -= lastTrans.amount;
             fromAccount->balance += lastTrans.amount;
             
             // Pop the transfer transaction as well
             popTransaction();
         }
     }
 
     printf("%sLast transaction undone: %s of $%.2f for account %d%s\n",
            GREEN, lastTrans.type, lastTrans.amount, lastTrans.accountNo, RESET);
     return true;
 }
 /***************************************************
 * SECTION 5: CUSTOMER SERVICE FUNCTIONS
 ***************************************************/

int getNextRequestId()
{
    static int nextId = 1000;
    return ++nextId;
}

bool submitServiceRequest(int accountNo, char *requestType, char *description, int priority)
{
    struct Account *account = findAccount(accountNo);
    if (account == NULL)
    {
        return false;
    }

    struct ServiceRequest request;
    request.requestId = getNextRequestId();
    request.accountNo = accountNo;
    strcpy(request.requestType, requestType);
    strcpy(request.description, description);
    request.isProcessed = false;
    request.priority = priority;
    
    // Get current date
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    sprintf(request.dateSubmitted, "%04d-%02d-%02d", 
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    
    enqueueRequest(request);
    return true;
}

void processNextRequest()
{
    if (serviceQueue == NULL)
    {
        printf("%sNo pending service requests.%s\n", YELLOW, RESET);
        return;
    }
    
    struct ServiceRequest request = dequeueRequest();
    struct Account *account = findAccount(request.accountNo);
    
    printf("\n%s%s Processing Service Request %s\n", BG_GREEN, BLACK, RESET);
    printf("%sRequest ID: %d%s\n", CYAN, request.requestId, RESET);
    printf("%sAccount: %d - %s%s\n", CYAN, request.accountNo, 
           account ? account->name : "Unknown Account", RESET);
    printf("%sType: %s%s\n", CYAN, request.requestType, RESET);
    printf("%sPriority: %d/5%s\n", CYAN, request.priority, RESET);
    printf("%sDescription: %s%s\n", CYAN, request.description, RESET);
    printf("%sDate Submitted: %s%s\n", CYAN, request.dateSubmitted, RESET);
    
    printf("\n%sRequest has been marked as processed.%s\n", GREEN, RESET);
}

void viewPendingRequests()
{
    if (serviceQueue == NULL)
    {
        printf("%sNo pending service requests.%s\n", YELLOW, RESET);
        return;
    }
    
    printf("\n%s%s Pending Service Requests %s\n", BG_GREEN, BLACK, RESET);
    printf("%s%s%-5s %-8s %-15s %-15s %-12s %s\n", 
           BG_CYAN, BLACK, "ID", "Account", "Type", "Priority", "Date", RESET);
    
    struct RequestNode *temp = serviceQueue;
    while (temp != NULL)
    {
        printf("%-5d %-8d %-15s %-15d %-12s\n",
               temp->data.requestId,
               temp->data.accountNo,
               temp->data.requestType,
               temp->data.priority,
               temp->data.dateSubmitted);
        temp = temp->next;
    }
}

// Save service requests to file
void saveRequestsToFile()
{
    FILE *file = fopen("requests.dat", "wb");
    if (file == NULL)
    {
        printf("%sError opening file for saving service requests.%s\n", RED, RESET);
        return;
    }

    struct RequestNode *temp = serviceQueue;
    while (temp != NULL)
    {
        fwrite(&(temp->data), sizeof(struct ServiceRequest), 1, file);
        temp = temp->next;
    }

    fclose(file);
}

// Load service requests from file
void loadRequestsFromFile()
{
    FILE *file = fopen("requests.dat", "rb");
    if (file == NULL)
    {
        return; // No previous requests data
    }

    // Free existing service queue
    while (serviceQueue != NULL)
    {
        struct RequestNode *temp = serviceQueue;
        serviceQueue = serviceQueue->next;
        free(temp);
    }
    serviceQueueRear = NULL;

    struct ServiceRequest request;
    while (fread(&request, sizeof(struct ServiceRequest), 1, file) == 1)
    {
        enqueueRequest(request);
    }

    fclose(file);
}

/***************************************************
 * SECTION 6: BRANCH MANAGEMENT FUNCTIONS
 ***************************************************/

void viewAllBranches()
{
    if (branchCount == 0)
    {
        printf("%sNo branches available.%s\n", YELLOW, RESET);
        return;
    }

    printf("\n%s%s Bank Branches %s\n", BG_GREEN, BLACK, RESET);
    printf("%s%s%-5s %-15s %-20s %-15s %-15s %s\n", 
           BG_CYAN, BLACK, "ID", "Name", "Location", "Manager", "Phone", RESET);

    for (int i = 0; i < branchCount; i++)
    {
        printf("%-5d %-15s %-20s %-15s %-15s\n",
               branchGraph[i].data.branchId,
               branchGraph[i].data.branchName,
               branchGraph[i].data.location,
               branchGraph[i].data.managerName,
               branchGraph[i].data.phoneNumber);
    }
}

void viewBranchConnections(int branchId)
{
    int index = findBranchIndex(branchId);
    if (index == -1)
    {
        printf("%sBranch not found.%s\n", RED, RESET);
        return;
    }

    printf("\n%s%s Connections for Branch: %s %s\n", 
           BG_GREEN, BLACK, branchGraph[index].data.branchName, RESET);
    printf("%s%s%-5s %-15s %-10s %s\n", 
           BG_CYAN, BLACK, "ID", "Name", "Distance", RESET);

    struct EdgeList *edge = branchGraph[index].connections;
    if (edge == NULL)
    {
        printf("%sNo connections for this branch.%s\n", YELLOW, RESET);
        return;
    }

    while (edge != NULL)
    {
        int targetIndex = findBranchIndex(edge->branchId);
        if (targetIndex != -1)
        {
            printf("%-5d %-15s %-10.2f\n",
                   branchGraph[targetIndex].data.branchId,
                   branchGraph[targetIndex].data.branchName,
                   edge->distance);
        }
        edge = edge->next;
    }
}

/***************************************************
 * SECTION 7: UI FUNCTIONS
 ***************************************************/

void clearScreen()
{
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void displayBanner()
{
    printf("\n%s%s =============================================== %s\n", BG_BLUE, WHITE, RESET);
    printf("%s%s                                               %s\n", BG_BLUE, WHITE, RESET);
    printf("%s%s      ENHANCED BANK MANAGEMENT SYSTEM          %s\n", BG_BLUE, WHITE, RESET);
    printf("%s%s                                               %s\n", BG_BLUE, WHITE, RESET);
    printf("%s%s =============================================== %s\n", BG_BLUE, WHITE, RESET);
    
    // Display logged-in user
    if (strlen(currentUser.username) > 0)
    {
        printf("\n%sLogged in as:%s %s%s%s (%s)%s\n", 
               CYAN, RESET, BOLD, currentUser.username, RESET,
               currentUser.isAdmin ? "Administrator" : "Regular User", RESET);
    }
}

void pauseExecution()
{
    printf("\n%sPress Enter to continue...%s", YELLOW, RESET);
    while (getchar() != '\n'); // Clear input buffer
    getchar(); // Wait for Enter
}

void displayAdminMenu()
{
    clearScreen();
    displayBanner();
    
    printf("\n%s%s ADMIN MENU %s\n", BG_CYAN, BLACK, RESET);
    printf("%s 1. Account Management %s\n", YELLOW, RESET);
    printf("%s 2. Transaction Management %s\n", YELLOW, RESET);
    printf("%s 3. Service Request Management %s\n", YELLOW, RESET);
    printf("%s 4. Branch Management %s\n", YELLOW, RESET);
    printf("%s 5. User Management %s\n", YELLOW, RESET);
    printf("%s 6. Save/Load Data %s\n", YELLOW, RESET);
    printf("%s 7. Logout %s\n", YELLOW, RESET);
    printf("%s 8. Exit %s\n", RED, RESET);
    printf("\n%sEnter your choice: %s", GREEN, RESET);
}

void displayUserMenu()
{
    clearScreen();
    displayBanner();
    
    printf("\n%s%s USER MENU %s\n", BG_CYAN, BLACK, RESET);
    printf("%s 1. View Account Details %s\n", YELLOW, RESET);
    printf("%s 2. Make a Transaction %s\n", YELLOW, RESET);
    printf("%s 3. Submit Service Request %s\n", YELLOW, RESET);
    printf("%s 4. View Branch Information %s\n", YELLOW, RESET);
    printf("%s 5. Logout %s\n", YELLOW, RESET);
    printf("%s 6. Exit %s\n", RED, RESET);
    printf("\n%sEnter your choice: %s", GREEN, RESET);
}

void displayAccountMenu()
{
    clearScreen();
    displayBanner();
    
    printf("\n%s%s ACCOUNT MANAGEMENT %s\n", BG_CYAN, BLACK, RESET);
    printf("%s 1. Create New Account %s\n", YELLOW, RESET);
    printf("%s 2. View All Accounts %s\n", YELLOW, RESET);
    printf("%s 3. Search Account by Number %s\n", YELLOW, RESET);
    printf("%s 4. Update Account Details %s\n", YELLOW, RESET);
    printf("%s 5. Delete Account %s\n", YELLOW, RESET);
    printf("%s 6. Return to Main Menu %s\n", YELLOW, RESET);
    printf("\n%sEnter your choice: %s", GREEN, RESET);
}

void displayTransactionMenu()
{
    clearScreen();
    displayBanner();
    
    printf("\n%s%s TRANSACTION MANAGEMENT %s\n", BG_CYAN, BLACK, RESET);
    printf("%s 1. Deposit %s\n", YELLOW, RESET);
    printf("%s 2. Withdraw %s\n", YELLOW, RESET);
    printf("%s 3. Transfer %s\n", YELLOW, RESET);
    printf("%s 4. View All Transactions %s\n", YELLOW, RESET);
    printf("%s 5. View Account Transactions %s\n", YELLOW, RESET);
    printf("%s 6. Undo Last Transaction %s\n", YELLOW, RESET);
    printf("%s 7. Return to Main Menu %s\n", YELLOW, RESET);
    printf("\n%sEnter your choice: %s", GREEN, RESET);
}

void displayServiceMenu()
{
    clearScreen();
    displayBanner();
    
    printf("\n%s%s SERVICE REQUEST MANAGEMENT %s\n", BG_CYAN, BLACK, RESET);
    printf("%s 1. Submit New Request %s\n", YELLOW, RESET);
    printf("%s 2. View All Pending Requests %s\n", YELLOW, RESET);
    printf("%s 3. Process Next Request %s\n", YELLOW, RESET);
    printf("%s 4. Return to Main Menu %s\n", YELLOW, RESET);
    printf("\n%sEnter your choice: %s", GREEN, RESET);
}

void displayBranchMenu()
{
    clearScreen();
    displayBanner();
    
    printf("\n%s%s BRANCH MANAGEMENT %s\n", BG_CYAN, BLACK, RESET);
    printf("%s 1. Add New Branch %s\n", YELLOW, RESET);
    printf("%s 2. View All Branches %s\n", YELLOW, RESET);
    printf("%s 3. Add Branch Connection %s\n", YELLOW, RESET);
    printf("%s 4. View Branch Connections %s\n", YELLOW, RESET);
    printf("%s 5. Find Shortest Path Between Branches %s\n", YELLOW, RESET);
    printf("%s 6. Return to Main Menu %s\n", YELLOW, RESET);
    printf("\n%sEnter your choice: %s", GREEN, RESET);
}

void displayUserManagementMenu()
{
    clearScreen();
    displayBanner();
    
    printf("\n%s%s USER MANAGEMENT %s\n", BG_CYAN, BLACK, RESET);
    printf("%s 1. Add New User %s\n", YELLOW, RESET);
    printf("%s 2. View All Users %s\n", YELLOW, RESET);
    printf("%s 3. Return to Main Menu %s\n", YELLOW, RESET);
    printf("\n%sEnter your choice: %s", GREEN, RESET);
}

void displaySaveLoadMenu()
{
    clearScreen();
    displayBanner();
    
    printf("\n%s%s DATA MANAGEMENT %s\n", BG_CYAN, BLACK, RESET);
    printf("%s 1. Save All Data %s\n", YELLOW, RESET);
    printf("%s 2. Load All Data %s\n", YELLOW, RESET);
    printf("%s 3. Return to Main Menu %s\n", YELLOW, RESET);
    printf("\n%sEnter your choice: %s", GREEN, RESET);
}

/***************************************************
 * SECTION 8: ACCOUNT MANAGEMENT FUNCTIONS
 ***************************************************/

void viewAllAccounts()
{
    if (accountList == NULL)
    {
        printf("%sNo accounts to display.%s\n", YELLOW, RESET);
        return;
    }

    printf("\n%s%s All Accounts %s\n", BG_GREEN, BLACK, RESET);
    printf("%s%s%-5s %-15s %-10s %-20s %-15s %s\n", 
           BG_CYAN, BLACK, "No", "Name", "Balance", "Address", "Phone", RESET);

    struct AccountNode *temp = accountList;
    while (temp != NULL)
    {
        printf("%-5d %-15s %s$%.2f%s %-20.20s %-15s\n",
               temp->data.accountNo,
               temp->data.name,
               GREEN,
               temp->data.balance,
               RESET,
               temp->data.address,
               temp->data.phoneNumber);
        temp = temp->next;
    }
}

void searchAccount()
{
    int accountNo;
    printf("\n%sEnter Account Number to search: %s", CYAN, RESET);
    scanf("%d", &accountNo);
    getchar(); // Clear input buffer
    
    struct Account *account = findAccount(accountNo);
    if (account == NULL)
    {
        printf("%sAccount not found.%s\n", RED, RESET);
        return;
    }
    
    printf("\n%s%s Account Details %s\n", BG_GREEN, BLACK, RESET);
    printf("%sAccount Number: %d%s\n", CYAN, account->accountNo, RESET);
    printf("%sName: %s%s\n", CYAN, account->name, RESET);
    printf("%sBalance: %s$%.2f%s\n", CYAN, GREEN, account->balance, RESET);
    printf("%sAddress: %s%s\n", CYAN, account->address, RESET);
    printf("%sPhone: %s%s\n", CYAN, account->phoneNumber, RESET);
    printf("%sEmail: %s%s\n", CYAN, account->email, RESET);
    printf("%sDate Created: %s%s\n", CYAN, account->dateCreated, RESET);
}

bool updateAccountDetails(int accountNo)
{
    struct Account *account = findAccount(accountNo);
    if (account == NULL)
    {
        return false;
    }
    
    printf("\n%s%s Update Account Details %s\n", BG_GREEN, BLACK, RESET);
    printf("%sCurrent Name: %s - Enter new name (or press Enter to keep): %s", CYAN, account->name, RESET);
    
    char input[100];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0; // Remove newline
    
    if (strlen(input) > 0)
    {
        strcpy(account->name, input);
    }
    
    printf("%sCurrent Address: %s - Enter new address (or press Enter to keep): %s", CYAN, account->address, RESET);
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    
    if (strlen(input) > 0)
    {
        strcpy(account->address, input);
    }
    
    printf("%sCurrent Phone: %s - Enter new phone (or press Enter to keep): %s", CYAN, account->phoneNumber, RESET);
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    
    if (strlen(input) > 0)
    {
        strcpy(account->phoneNumber, input);
    }
    
    printf("%sCurrent Email: %s - Enter new email (or press Enter to keep): %s", CYAN, account->email, RESET);
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    
    if (strlen(input) > 0)
    {
        strcpy(account->email, input);
    }
    
    printf("%sAccount details updated successfully!%s\n", GREEN, RESET);
    return true;
}

bool deleteAccount(int accountNo)
{
    if (accountList == NULL)
    {
        return false;
    }
    
    // If first node is the one to delete
    if (accountList->data.accountNo == accountNo)
    {
        struct AccountNode *temp = accountList;
        accountList = accountList->next;
        free(temp);
        return true;
    }
    
    // Search for the node to delete
    struct AccountNode *current = accountList;
    struct AccountNode *prev = NULL;
    
    while (current != NULL && current->data.accountNo != accountNo)
    {
        prev = current;
        current = current->next;
    }
    
    // If account not found
    if (current == NULL)
    {
        return false;
    }
    
    // Remove the node
    prev->next = current->next;
    free(current);
    return true;
}
/***************************************************
 * SECTION 9: MAIN AND MENU HANDLING FUNCTIONS
 ***************************************************/

// Handle account management menu
void handleAccountMenu()
{
    int choice;
    int accountNo;
    char name[50], address[100], phone[15], email[50];
    float initialBalance;
    
    do {
        displayAccountMenu();
        scanf("%d", &choice);
        getchar(); // Clear input buffer
        
        switch (choice) {
            case 1: // Create New Account
                printf("\n%s%s Create New Account %s\n", BG_GREEN, BLACK, RESET);
                
                printf("%sEnter Account Number: %s", CYAN, RESET);
                scanf("%d", &accountNo);
                getchar(); // Clear input buffer
                
                printf("%sEnter Name: %s", CYAN, RESET);
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0; // Remove newline
                
                printf("%sEnter Initial Balance: %s", CYAN, RESET);
                scanf("%f", &initialBalance);
                getchar(); // Clear input buffer
                
                printf("%sEnter Address: %s", CYAN, RESET);
                fgets(address, sizeof(address), stdin);
                address[strcspn(address, "\n")] = 0;
                
                printf("%sEnter Phone Number: %s", CYAN, RESET);
                fgets(phone, sizeof(phone), stdin);
                phone[strcspn(phone, "\n")] = 0;
                
                printf("%sEnter Email: %s", CYAN, RESET);
                fgets(email, sizeof(email), stdin);
                email[strcspn(email, "\n")] = 0;
                
                if (addAccount(accountNo, name, initialBalance, address, phone, email)) {
                    printf("%sAccount created successfully!%s\n", GREEN, RESET);
                } else {
                    printf("%sAccount number already exists!%s\n", RED, RESET);
                }
                pauseExecution();
                break;
                
            case 2: // View All Accounts
                viewAllAccounts();
                pauseExecution();
                break;
                
            case 3: // Search Account by Number
                searchAccount();
                pauseExecution();
                break;
                
            case 4: // Update Account Details
                printf("\n%sEnter Account Number to update: %s", CYAN, RESET);
                scanf("%d", &accountNo);
                getchar(); // Clear input buffer
                
                if (!updateAccountDetails(accountNo)) {
                    printf("%sAccount not found!%s\n", RED, RESET);
                }
                pauseExecution();
                break;
                
            case 5: // Delete Account
                printf("\n%sEnter Account Number to delete: %s", CYAN, RESET);
                scanf("%d", &accountNo);
                getchar(); // Clear input buffer
                
                if (deleteAccount(accountNo)) {
                    printf("%sAccount deleted successfully!%s\n", GREEN, RESET);
                } else {
                    printf("%sAccount not found!%s\n", RED, RESET);
                }
                pauseExecution();
                break;
                
            case 6: // Return to Main Menu
                break;
                
            default:
                printf("%sInvalid choice!%s\n", RED, RESET);
                pauseExecution();
        }
    } while (choice != 6);
}

// Handle transaction menu
void handleTransactionMenu()
{
    int choice;
    int accountNo, toAccountNo;
    float amount;
    
    do {
        displayTransactionMenu();
        scanf("%d", &choice);
        getchar(); // Clear input buffer
        
        switch (choice) {
            case 1: // Deposit
                printf("\n%s%s Deposit %s\n", BG_GREEN, BLACK, RESET);
                
                printf("%sEnter Account Number: %s", CYAN, RESET);
                scanf("%d", &accountNo);
                getchar(); // Clear input buffer
                
                printf("%sEnter Amount to Deposit: %s", CYAN, RESET);
                scanf("%f", &amount);
                getchar(); // Clear input buffer
                
                if (deposit(accountNo, amount)) {
                    printf("%sDeposit successful!%s\n", GREEN, RESET);
                } else {
                    printf("%sAccount not found!%s\n", RED, RESET);
                }
                pauseExecution();
                break;
                
            case 2: // Withdraw
                printf("\n%s%s Withdraw %s\n", BG_GREEN, BLACK, RESET);
                
                printf("%sEnter Account Number: %s", CYAN, RESET);
                scanf("%d", &accountNo);
                getchar(); // Clear input buffer
                
                printf("%sEnter Amount to Withdraw: %s", CYAN, RESET);
                scanf("%f", &amount);
                getchar(); // Clear input buffer
                
                if (withdraw(accountNo, amount)) {
                    printf("%sWithdrawal successful!%s\n", GREEN, RESET);
                } else {
                    printf("%sAccount not found or insufficient balance!%s\n", RED, RESET);
                }
                pauseExecution();
                break;
                
            case 3: // Transfer
                printf("\n%s%s Transfer %s\n", BG_GREEN, BLACK, RESET);
                
                printf("%sEnter Source Account Number: %s", CYAN, RESET);
                scanf("%d", &accountNo);
                getchar(); // Clear input buffer
                
                printf("%sEnter Destination Account Number: %s", CYAN, RESET);
                scanf("%d", &toAccountNo);
                getchar(); // Clear input buffer
                
                printf("%sEnter Amount to Transfer: %s", CYAN, RESET);
                scanf("%f", &amount);
                getchar(); // Clear input buffer
                
                if (transfer(accountNo, toAccountNo, amount)) {
                    printf("%sTransfer successful!%s\n", GREEN, RESET);
                } else {
                    printf("%sTransaction failed! Check account numbers and balance.%s\n", RED, RESET);
                }
                pauseExecution();
                break;
                
            case 4: // View All Transactions
                viewTransactions();
                pauseExecution();
                break;
                
            case 5: // View Account Transactions
                printf("\n%sEnter Account Number: %s", CYAN, RESET);
                scanf("%d", &accountNo);
                getchar(); // Clear input buffer
                
                viewAccountTransactions(accountNo);
                pauseExecution();
                break;
                
            case 6: // Undo Last Transaction
                if (undoLastTransaction()) {
                    printf("%sTransaction undone successfully!%s\n", GREEN, RESET);
                } else {
                    printf("%sNo transactions to undo or invalid transaction!%s\n", RED, RESET);
                }
                pauseExecution();
                break;
                
            case 7: // Return to Main Menu
                break;
                
            default:
                printf("%sInvalid choice!%s\n", RED, RESET);
                pauseExecution();
        }
    } while (choice != 7);
}

// Handle service request menu
void handleServiceMenu()
{
    int choice;
    int accountNo, priority;
    char requestType[50], description[200];
    
    do {
        displayServiceMenu();
        scanf("%d", &choice);
        getchar(); // Clear input buffer
        
        switch (choice) {
            case 1: // Submit New Request
                printf("\n%s%s Submit Service Request %s\n", BG_GREEN, BLACK, RESET);
                
                printf("%sEnter Account Number: %s", CYAN, RESET);
                scanf("%d", &accountNo);
                getchar(); // Clear input buffer
                
                printf("%sEnter Request Type: %s", CYAN, RESET);
                fgets(requestType, sizeof(requestType), stdin);
                requestType[strcspn(requestType, "\n")] = 0; // Remove newline
                
                printf("%sEnter Description: %s", CYAN, RESET);
                fgets(description, sizeof(description), stdin);
                description[strcspn(description, "\n")] = 0;
                
                printf("%sEnter Priority (1-5, 5 being highest): %s", CYAN, RESET);
                scanf("%d", &priority);
                getchar(); // Clear input buffer
                
                if (priority < 1 || priority > 5) {
                    printf("%sInvalid priority! Setting to default (3).%s\n", YELLOW, RESET);
                    priority = 3;
                }
                
                if (submitServiceRequest(accountNo, requestType, description, priority)) {
                    printf("%sService request submitted successfully!%s\n", GREEN, RESET);
                } else {
                    printf("%sAccount not found!%s\n", RED, RESET);
                }
                pauseExecution();
                break;
                
            case 2: // View All Pending Requests
                viewPendingRequests();
                pauseExecution();
                break;
                
            case 3: // Process Next Request
                processNextRequest();
                pauseExecution();
                break;
                
            case 4: // Return to Main Menu
                break;
                
            default:
                printf("%sInvalid choice!%s\n", RED, RESET);
                pauseExecution();
        }
    } while (choice != 4);
}

// Handle branch management menu
void handleBranchMenu()
{
    int choice;
    int branchId, targetBranchId, employeeCount;
    float distance;
    char branchName[50], location[100], managerName[50], phoneNumber[15];
    
    do {
        displayBranchMenu();
        scanf("%d", &choice);
        getchar(); // Clear input buffer
        
        switch (choice) {
            case 1: // Add New Branch
                printf("\n%s%s Add New Branch %s\n", BG_GREEN, BLACK, RESET);
                
                printf("%sEnter Branch ID: %s", CYAN, RESET);
                scanf("%d", &branchId);
                getchar(); // Clear input buffer
                
                printf("%sEnter Branch Name: %s", CYAN, RESET);
                fgets(branchName, sizeof(branchName), stdin);
                branchName[strcspn(branchName, "\n")] = 0; // Remove newline
                
                printf("%sEnter Location: %s", CYAN, RESET);
                fgets(location, sizeof(location), stdin);
                location[strcspn(location, "\n")] = 0;
                
                printf("%sEnter Manager Name: %s", CYAN, RESET);
                fgets(managerName, sizeof(managerName), stdin);
                managerName[strcspn(managerName, "\n")] = 0;
                
                printf("%sEnter Phone Number: %s", CYAN, RESET);
                fgets(phoneNumber, sizeof(phoneNumber), stdin);
                phoneNumber[strcspn(phoneNumber, "\n")] = 0;
                
                printf("%sEnter Employee Count: %s", CYAN, RESET);
                scanf("%d", &employeeCount);
                getchar(); // Clear input buffer
                
                addBranch(branchId, branchName, location, managerName, phoneNumber, employeeCount);
                printf("%sBranch added successfully!%s\n", GREEN, RESET);
                pauseExecution();
                break;
                
            case 2: // View All Branches
                viewAllBranches();
                pauseExecution();
                break;
                
            case 3: // Add Branch Connection
                printf("\n%s%s Add Branch Connection %s\n", BG_GREEN, BLACK, RESET);
                
                printf("%sEnter Source Branch ID: %s", CYAN, RESET);
                scanf("%d", &branchId);
                getchar(); // Clear input buffer
                
                printf("%sEnter Target Branch ID: %s", CYAN, RESET);
                scanf("%d", &targetBranchId);
                getchar(); // Clear input buffer
                
                printf("%sEnter Distance (in km): %s", CYAN, RESET);
                scanf("%f", &distance);
                getchar(); // Clear input buffer
                
                addConnection(branchId, targetBranchId, distance);
                printf("%sConnection added successfully!%s\n", GREEN, RESET);
                pauseExecution();
                break;
                
            case 4: // View Branch Connections
                printf("\n%sEnter Branch ID: %s", CYAN, RESET);
                scanf("%d", &branchId);
                getchar(); // Clear input buffer
                
                viewBranchConnections(branchId);
                pauseExecution();
                break;
                
            case 5: // Find Shortest Path Between Branches
                printf("\n%s%s Find Shortest Path %s\n", BG_GREEN, BLACK, RESET);
                
                printf("%sEnter Start Branch ID: %s", CYAN, RESET);
                scanf("%d", &branchId);
                getchar(); // Clear input buffer
                
                printf("%sEnter Destination Branch ID: %s", CYAN, RESET);
                scanf("%d", &targetBranchId);
                getchar(); // Clear input buffer
                
                findShortestPath(branchId, targetBranchId);
                pauseExecution();
                break;
                
            case 6: // Return to Main Menu
                break;
                
            default:
                printf("%sInvalid choice!%s\n", RED, RESET);
                pauseExecution();
        }
    } while (choice != 6);
}

// Handle user management menu
void handleUserManagementMenu()
{
    int choice;
    char username[50], password[50];
    int isAdmin;
    
    do {
        displayUserManagementMenu();
        scanf("%d", &choice);
        getchar(); // Clear input buffer
        
        switch (choice) {
            case 1: // Add New User
                printf("\n%s%s Add New User %s\n", BG_GREEN, BLACK, RESET);
                
                printf("%sEnter Username: %s", CYAN, RESET);
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = 0; // Remove newline
                
                printf("%sEnter Password: %s", CYAN, RESET);
                fgets(password, sizeof(password), stdin);
                password[strcspn(password, "\n")] = 0;
                
                printf("%sIs Admin (1 for Yes, 0 for No): %s", CYAN, RESET);
                scanf("%d", &isAdmin);
                getchar(); // Clear input buffer
                
                if (addUser(username, password, isAdmin == 1)) {
                    printf("%sUser added successfully!%s\n", GREEN, RESET);
                } else {
                    printf("%sFailed to add user! Username might exist already.%s\n", RED, RESET);
                }
                pauseExecution();
                break;
                
            case 2: // View All Users
                printf("\n%s%s All Users %s\n", BG_GREEN, BLACK, RESET);
                printf("%s%s%-20s %-15s %s\n", BG_CYAN, BLACK, "Username", "Role", RESET);
                
                for (int i = 0; i < userCount; i++) {
                    printf("%-20s %-15s\n", 
                          users[i].username, 
                          users[i].isAdmin ? "Administrator" : "Regular User");
                }
                pauseExecution();
                break;
                
            case 3: // Return to Main Menu
                break;
                
            default:
                printf("%sInvalid choice!%s\n", RED, RESET);
                pauseExecution();
        }
    } while (choice != 3);
}

// Handle save/load menu
void handleSaveLoadMenu()
{
    int choice;
    
    do {
        displaySaveLoadMenu();
        scanf("%d", &choice);
        getchar(); // Clear input buffer
        
        switch (choice) {
            case 1: // Save All Data
                saveAllData();
                pauseExecution();
                break;
                
            case 2: // Load All Data
                loadAllData();
                printf("%sAll data loaded successfully!%s\n", GREEN, RESET);
                pauseExecution();
                break;
                
            case 3: // Return to Main Menu
                break;
                
            default:
                printf("%sInvalid choice!%s\n", RED, RESET);
                pauseExecution();
        }
    } while (choice != 3);
}

// Handle user account operations (regular user)
void handleUserAccountOperations()
{
    int choice;
    int accountNo, toAccountNo;
    float amount;
    char requestType[50], description[200];
    int priority;
    
    do {
        displayUserMenu();
        scanf("%d", &choice);
        getchar(); // Clear input buffer
        
        switch (choice) {
            case 1: // View Account Details
                printf("\n%sEnter Your Account Number: %s", CYAN, RESET);
                scanf("%d", &accountNo);
                getchar(); // Clear input buffer
                
                searchAccount();
                pauseExecution();
                break;
                
            case 2: // Make a Transaction
                printf("\n%s%s Transaction Options %s\n", BG_GREEN, BLACK, RESET);
                printf("%s 1. Deposit %s\n", YELLOW, RESET);
                printf("%s 2. Withdraw %s\n", YELLOW, RESET);
                printf("%s 3. Transfer %s\n", YELLOW, RESET);
                printf("\n%sEnter choice: %s", GREEN, RESET);
                
                int transChoice;
                scanf("%d", &transChoice);
                getchar(); // Clear input buffer
                
                switch (transChoice) {
                    case 1: // Deposit
                        printf("\n%sEnter Your Account Number: %s", CYAN, RESET);
                        scanf("%d", &accountNo);
                        getchar(); // Clear input buffer
                        
                        printf("%sEnter Amount to Deposit: %s", CYAN, RESET);
                        scanf("%f", &amount);
                        getchar(); // Clear input buffer
                        
                        if (deposit(accountNo, amount)) {
                            printf("%sDeposit successful!%s\n", GREEN, RESET);
                        } else {
                            printf("%sAccount not found!%s\n", RED, RESET);
                        }
                        break;
                        
                    case 2: // Withdraw
                        printf("\n%sEnter Your Account Number: %s", CYAN, RESET);
                        scanf("%d", &accountNo);
                        getchar(); // Clear input buffer
                        
                        printf("%sEnter Amount to Withdraw: %s", CYAN, RESET);
                        scanf("%f", &amount);
                        getchar(); // Clear input buffer
                        
                        if (withdraw(accountNo, amount)) {
                            printf("%sWithdrawal successful!%s\n", GREEN, RESET);
                        } else {
                            printf("%sAccount not found or insufficient balance!%s\n", RED, RESET);
                        }
                        break;
                        
                    case 3: // Transfer
                        printf("\n%sEnter Your Account Number: %s", CYAN, RESET);
                        scanf("%d", &accountNo);
                        getchar(); // Clear input buffer
                        
                        printf("%sEnter Destination Account Number: %s", CYAN, RESET);
                        scanf("%d", &toAccountNo);
                        getchar(); // Clear input buffer
                        
                        printf("%sEnter Amount to Transfer: %s", CYAN, RESET);
                        scanf("%f", &amount);
                        getchar(); // Clear input buffer
                        
                        if (transfer(accountNo, toAccountNo, amount)) {
                            printf("%sTransfer successful!%s\n", GREEN, RESET);
                        } else {
                            printf("%sTransaction failed! Check account numbers and balance.%s\n", RED, RESET);
                        }
                        break;
                        
                    default:
                        printf("%sInvalid choice!%s\n", RED, RESET);
                }
                pauseExecution();
                break;
                
            case 3: // Submit Service Request
                printf("\n%s%s Submit Service Request %s\n", BG_GREEN, BLACK, RESET);
                
                printf("%sEnter Your Account Number: %s", CYAN, RESET);
                scanf("%d", &accountNo);
                getchar(); // Clear input buffer
                
                printf("%sEnter Request Type: %s", CYAN, RESET);
                fgets(requestType, sizeof(requestType), stdin);
                requestType[strcspn(requestType, "\n")] = 0; // Remove newline
                
                printf("%sEnter Description: %s", CYAN, RESET);
                fgets(description, sizeof(description), stdin);
                description[strcspn(description, "\n")] = 0;
                
                printf("%sEnter Priority (1-5, 5 being highest): %s", CYAN, RESET);
                scanf("%d", &priority);
                getchar(); // Clear input buffer
                
                if (priority < 1 || priority > 5) {
                    printf("%sInvalid priority! Setting to default (3).%s\n", YELLOW, RESET);
                    priority = 3;
                }
                
                if (submitServiceRequest(accountNo, requestType, description, priority)) {
                    printf("%sService request submitted successfully!%s\n", GREEN, RESET);
                } else {
                    printf("%sAccount not found!%s\n", RED, RESET);
                }
                pauseExecution();
                break;
                
            case 4: // View Branch Information
                viewAllBranches();
                
                printf("\n%sDo you want to see branch connections? (1 for Yes, 0 for No): %s", CYAN, RESET);
                int showConnections;
                scanf("%d", &showConnections);
                getchar(); // Clear input buffer
                
                if (showConnections) {
                    printf("\n%sEnter Branch ID: %s", CYAN, RESET);
                    scanf("%d", &accountNo); // Reusing accountNo variable for branch ID
                    getchar(); // Clear input buffer
                    
                    viewBranchConnections(accountNo);
                }
                pauseExecution();
                break;
                
            case 5: // Logout
                printf("%sLogging out...%s\n", YELLOW, RESET);
                // Clear current user info
                strcpy(currentUser.username, "");
                strcpy(currentUser.password, "");
                currentUser.isAdmin = false;
                break;
                
            case 6: // Exit
                printf("%sThank you for using the Bank Management System!%s\n", GREEN, RESET);
                saveAllData(); // Auto-save on exit
                exit(0);
                
            default:
                printf("%sInvalid choice!%s\n", RED, RESET);
                pauseExecution();
        }
    } while (choice != 5 && choice != 6);
}

// Handle admin operations
void handleAdminOperations()
{
    int choice;
    
    do {
        displayAdminMenu();
        scanf("%d", &choice);
        getchar(); // Clear input buffer
        
        switch (choice) {
            case 1: // Account Management
                handleAccountMenu();
                break;
                
            case 2: // Transaction Management
                handleTransactionMenu();
                break;
                
            case 3: // Service Request Management
                handleServiceMenu();
                break;
                
            case 4: // Branch Management
                handleBranchMenu();
                break;
                
            case 5: // User Management
                handleUserManagementMenu();
                break;
                
            case 6: // Save/Load Data
                handleSaveLoadMenu();
                break;
                
            case 7: // Logout
                printf("%sLogging out...%s\n", YELLOW, RESET);
                // Clear current user info
                strcpy(currentUser.username, "");
                strcpy(currentUser.password, "");
                currentUser.isAdmin = false;
                break;
                
            case 8: // Exit
                printf("%sThank you for using the Bank Management System!%s\n", GREEN, RESET);
                saveAllData(); // Auto-save on exit
                exit(0);
                
            default:
                printf("%sInvalid choice!%s\n", RED, RESET);
                pauseExecution();
        }
    } while (choice != 7 && choice != 8);
}

// Main function
int main()
{
    // Initialize the random number generator
    srand(time(NULL));
    
    // Initialize users
    initializeUsers();
    
    // Load saved data
    loadAllData();
    
    // Main program loop
    while (1) {
        clearScreen();
        displayBanner();
        
        // Login if not already logged in
        if (strlen(currentUser.username) == 0) {
            char username[50], password[50];
            bool isAdmin;
            
            printf("\n%s%s LOGIN %s\n", BG_CYAN, BLACK, RESET);
            printf("%sUsername: %s", CYAN, RESET);
            fgets(username, sizeof(username), stdin);
            username[strcspn(username, "\n")] = 0; // Remove newline
            
            printf("%sPassword: %s", CYAN, RESET);
            fgets(password, sizeof(password), stdin);
            password[strcspn(password, "\n")] = 0;
            
            if (login(username, password, &isAdmin)) {
                printf("%sLogin successful!%s\n", GREEN, RESET);
                pauseExecution();
                
                if (isAdmin) {
                    handleAdminOperations();
                } else {
                    handleUserAccountOperations();
                }
            } else {
                printf("%sInvalid username or password!%s\n", RED, RESET);
                pauseExecution();
            }
        } else {
            // User is already logged in, direct to appropriate menu
            if (currentUser.isAdmin) {
                handleAdminOperations();
            } else {
                handleUserAccountOperations();
            }
        }
    }
    
    return 0;
}