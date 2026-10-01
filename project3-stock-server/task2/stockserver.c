#include "csapp.h"
#include <stdbool.h>

#define MAX_LINE_LENGTH 100
#define SBUF_SIZE 500
#define NTHREADS 1000

void sigint_handler(int sig);
void echo_cnt(int connfd);

void command(char *cmd, int connfd);
bool stockbuy(int id, int amount);
bool stocksell(int id, int amount);
void stockexit(char *buf);

/**
 * begin
 * tree structure
 */

/** Structure representing a stock item.
    It contains identification and tracking details for inventory management,
    including mechanisms for concurrent access control. */
typedef struct stockItem
{
    int ID;         /**< Unique identifier for the stock item. */
    int left_stock; /**< Initial quantity of the stock item. */
    int price;      /**< Unit price of the stock item. */
    int readcnt;    /**< Tracks how many threads are reading this item. */
    sem_t mutex;    /**< Semaphore for mutual exclusion, typically for write operations. */
    sem_t rw_mutex; /**< Semaphore for reader-writer lock. */
} stockItem;

/** Structure for a binary tree node.
    Each node holds a stock item and pointers to left and right child nodes,
    forming a binary search tree based on item IDs. */
typedef struct treeNode
{
    stockItem item;         /**< The stock item contained in this node. */
    struct treeNode *left;  /**< Pointer to the left child node. */
    struct treeNode *right; /**< Pointer to the right child node. */
} treeNode;

/** Initializes a stockItem with specific attributes.
    @param item Pointer to the stockItem to be initialized.
    @param id Unique identifier for the stock item.
    @param stock Initial quantity of the stock item.
    @param price Unit price of the stock item. */
void initStockItem(stockItem *item, int id, int stock, int price);

/** Inserts a new stock item into the binary tree.
    @param root The current root node of the binary tree.
    @param item The stockItem to be inserted into the tree.
    @return The new root node of the binary tree after insertion. */
treeNode *insertStockItem(treeNode *root, stockItem item);

/** Finds a stock item in the binary tree by its ID.
    @param root The root node of the binary tree.
    @param id The unique identifier of the stock item to find.
    @return A pointer to the found stockItem, or NULL if not found. */
stockItem *findStockItem(treeNode *root, int id);

/** Recursively deletes the entire binary tree, freeing all associated resources.
    @param root The root node of the binary tree to be deleted.
    This function ensures all memory is freed and semaphores are destroyed to avoid leaks. */
void deleteTree(treeNode *root);

/**
 * @brief Mounts stock data from a file into a binary tree.
 *
 * @param filedest The file path of the input file containing stock data.
 * @param root Pointer to the root of the binary tree.
 *
 * @return void
 */
void mountDataFromFile(char *filedest, treeNode **root);

/**
 * @brief Saves the binary tree data to a file and sends the data to the client.
 *
 *
 * @param root Pointer to the root of the binary tree.
 * @param connfd File descriptor for the output file and client connection.
 *
 * @return void
 *
 */
void saveTreeToFile(treeNode *root, char *buf);
void saveTreeDataToFile(char *filedest, treeNode *root);

// mount stock info to tree structure from stock.txt
void mountDataFromFile(char *filedest, treeNode **root)
{
    FILE *file = fopen(filedest, "r");
    if (file == NULL)
    {
        perror("mountDataFromFile: Failed to open file");
        return;
    }

    int id, stock, price;
    while (fscanf(file, "%d %d %d\n", &id, &stock, &price) == 3)
    {
        stockItem item;
        initStockItem(&item, id, stock, price);
        *root = insertStockItem(*root, item);
    }

    fclose(file);
}

// Function to save the tree data to a file
void saveTreeDataToFile(char *filedest, treeNode *root)
{
    FILE *file = fopen(filedest, "w");
    if (file == NULL)
    {
        perror("saveTreeDataToFile: Failed to open file");
        return;
    }
    // int connfd = fileno(file);

    char buf[MAXLINE * 10] = "";

    saveTreeToFile(root, buf);

    size_t len = strlen(buf);
    if (fwrite(buf, sizeof(char), len, file) != len)
    {
        perror("saveTreeDataToFile: Failed to write to file");
    }

    fclose(file);
}

// Recursive function to perform in-order traversal and save the tree data
void saveTreeToFile(treeNode *root, char *buf)
{
    if (root != NULL)
    {
        // Buffer for formatted output
        static char buffer[64];
        // P(&(root->item.mutex));
        // if (root->item.readcnt == 0)
        // {
        //     root->item.readcnt++;
        //     P(&s);
        // }
        // V(&(root->item.mutex));

        // sprintf(buffer, "%d %d %d\n", root->item.ID, root->item.left_stock, root->item.price);
        // strcat(buf, buffer);

        // P(&(root->item.mutex));
        // if (root->item.readcnt == 1)
        // {
        //     root->item.readcnt--;
        //     V(&s);
        // }
        // V(&(root->item.mutex));

        // saveTreeToFile(root->left, buf);
        // saveTreeToFile(root->right, buf);
        saveTreeToFile(root->left, buf);

        P(&root->item.mutex);
        if (root->item.readcnt == 0)
        {
            P(&root->item.rw_mutex);
        }
        root->item.readcnt++;
        V(&root->item.mutex);

        sprintf(buffer, "%d %d %d\n", root->item.ID, root->item.left_stock, root->item.price);
        strcat(buf, buffer);

        P(&root->item.mutex);
        root->item.readcnt--;
        if (root->item.readcnt == 0)
        {
            V(&root->item.rw_mutex);
        }
        V(&root->item.mutex);

        saveTreeToFile(root->right, buf);
    }
}

/* declare root global variable */
treeNode *root;

// Initialize a stock item
void initStockItem(stockItem *item, int id, int stock, int price)
{
    item->ID = id;
    item->left_stock = stock;
    item->price = price;
    item->readcnt = 0;
    sem_init(&item->mutex, 0, 1);
    sem_init(&item->rw_mutex, 0, 1);
}

// Insert a new stock item into the binary tree
treeNode *insertStockItem(treeNode *root, stockItem item)
{
    if (root == NULL)
    {
        treeNode *newNode = (treeNode *)malloc(sizeof(treeNode));
        newNode->item = item;
        newNode->left = NULL;
        newNode->right = NULL;
        return newNode;
    }
    if (item.ID < root->item.ID)
    {
        root->left = insertStockItem(root->left, item);
    }
    else
    {
        root->right = insertStockItem(root->right, item);
    }
    return root;
}

// Find a stock item in the binary tree
stockItem *findStockItem(treeNode *root, int id)
{
    while (root != NULL)
    {
        if (id == root->item.ID)
        {
            return &root->item;
        }
        else if (id < root->item.ID)
        {
            root = root->left;
        }
        else
        {
            root = root->right;
        }
    }
    return NULL;
}

// Delete the entire tree
void deleteTree(treeNode *root)
{
    if (root != NULL)
    {
        deleteTree(root->left);
        deleteTree(root->right);
        sem_destroy(&root->item.mutex);
        sem_destroy(&root->item.rw_mutex);
        free(root);
    }
}
/**
 * end
 * tree structure
 */

/**
 * begin
 * sbuf package
 */

typedef struct
{
    int *buf;    /* Buffer array */
    int n;       /* Maximum number of slots */
    int front;   /* buf[(front+1)%n] is first item */
    int rear;    /* buf[rear%n] is last item */
    sem_t mutex; /* Protects accesses to buf */
    sem_t slots; /* Counts available slots */
    sem_t items; /* Counts available items */
} sbuf_t;

sbuf_t sbuf; /* Shared buffer of connected descriptors */

static sem_t sem, mutex;
static int byte_cnt;
int connection_n = 0;

/* Create an empty, bounded, shared FIFO buffer with n slots */
void sbuf_init(sbuf_t *sp, int n)
{
    sp->buf = Calloc(n, sizeof(int));
    sp->n = n;                  /* Buffer holds max of n items */
    sp->front = sp->rear = 0;   /* Empty buffer iff front == rear */
    Sem_init(&sp->mutex, 0, 1); /* Binary semaphore for locking */
    Sem_init(&sp->slots, 0, n); /* Initially, buf has n empty slots */
    Sem_init(&sp->items, 0, 0); /* Initially, buf has 0 items */
}

/* Clean up buffer sp */
void sbuf_deinit(sbuf_t *sp)
{
    Free(sp->buf);
}

/* Insert item onto the rear of shared buffer sp */
void sbuf_insert(sbuf_t *sp, int item)
{
    P(&sp->slots);                          /* Wait for available slot */
    P(&sp->mutex);                          /* Lock the buffer */
    sp->buf[(++sp->rear) % (sp->n)] = item; /* Insert the item */
    V(&sp->mutex);                          /* Unlock the buffer */
    V(&sp->items);                          /* Announce available item */
}

/* Remove and return the first item from buffer sp */
int sbuf_remove(sbuf_t *sp)
{
    int item;
    P(&sp->items);                           /* Wait for available item */
    P(&sp->mutex);                           /* Lock the buffer */
    item = sp->buf[(++sp->front) % (sp->n)]; /* Remove the item */
    V(&sp->mutex);                           /* Unlock the buffer */
    V(&sp->slots);                           /* Announce available slot */
    return item;                             //
}

void *thread(void *vargp)
{
    Pthread_detach(pthread_self());

    while (1)
    {
        int connfd = sbuf_remove(&sbuf);
        echo_cnt(connfd);
        Close(connfd);
    }
}

static void init_echo_cnt(void)
{
    Sem_init(&mutex, 0, 1);
    byte_cnt = 0;
}

void echo_cnt(int connfd)
{
    int n;
    char buf[MAXLINE];
    rio_t rio;
    static pthread_once_t once = PTHREAD_ONCE_INIT;

    Pthread_once(&once, init_echo_cnt);
    Rio_readinitb(&rio, connfd);
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0)
    {
        P(&mutex);

        byte_cnt += n;
        printf("thread %d received %d (%d total) bytes on fd%d\n", (int)pthread_self(), n, byte_cnt, connfd);
        fprintf(stdout, "\033[0;32m%s\033[0m", buf);

        V(&mutex);

        command(buf, connfd);
    }
    saveTreeDataToFile("./stock.txt", root);
}
/**
 * end
 * sbuf package
 */

/**
 * begin
 * this area is about stock command actions
 */

void command(char *cmd, int connfd)
{
    int id, amount;
    char response[MAXLINE];

    if (!strncmp(cmd, "show", 4))
    {
        response[0] = '\0';

        saveTreeToFile(root, response);

        Rio_writen(connfd, response, MAXLINE);
    }
    else if (!strncmp(cmd, "buy", 3))
    {
        if (sscanf(cmd, "buy %d %d\n", &id, &amount) == 2)
        {
            if (stockbuy(id, amount))
            {
                strcpy(response, "[buy] \033[0;32msuccess\033[0m\n");
            }
            else
            {
                strcpy(response, "\033[0;31mNot Enough Left Stock\033[0m\n");
            }
            Rio_writen(connfd, response, MAXLINE);
        }
    }
    else if (!strncmp(cmd, "sell", 4))
    {
        if (sscanf(cmd, "sell %d %d\n", &id, &amount) == 2)
        {
            if (stocksell(id, amount))
            {
                strcpy(response, "[sell] \033[0;32msuccess\033[0m\n");
            }
            Rio_writen(connfd, response, MAXLINE);
        }
    }
    else if (!strncmp(cmd, "exit", 4))
    {
        Rio_writen(connfd, "\033[0;30mpress enter to exit\033[0m", MAXLINE);
        stockexit(response);
        Rio_writen(connfd, response, MAXLINE);
        Close(connfd);
    }
    else
    {
        // not valid command print usage
        strcpy(response, "\033[0;31ㅡInvalid command\033[0m\n");
        Rio_writen(connfd, response, strlen(response));
    }
    return;
}

bool stockbuy(int id, int amount)
{
    stockItem *targetstock = findStockItem(root, id);
    if (targetstock != NULL)
    {
        P(&targetstock->rw_mutex);

        if (targetstock->left_stock >= amount)
        {
            targetstock->left_stock -= amount;
            V(&targetstock->rw_mutex);
            return true;
        }
        else
        {
            V(&targetstock->rw_mutex);
            return false;
        }
    }
    return false;
}

bool stocksell(int id, int amount)
{
    stockItem *targetstock = findStockItem(root, id);
    if (targetstock != NULL)
    {
        P(&targetstock->rw_mutex);
        targetstock->left_stock += amount;
        V(&targetstock->rw_mutex);
        return true;
    }
    else
    {
        stockItem newstockitem;
        int price = rand() % 20000;
        initStockItem(&newstockitem, id, amount, price);
        P(&root->item.rw_mutex);
        root = insertStockItem(root, newstockitem);
        V(&root->item.rw_mutex);
        return true;
    }
}

void stockexit(char *buf)
{
    // dump tree structure to text file w/ fd
    saveTreeDataToFile("./stock.txt", root);
    buf[0] = '\0';
}

void sigint_handler(int sig)
{
    saveTreeDataToFile("./stock.txt", root);
    deleteTree(root);
    fprintf(stdout, "terminating server...\n");
    exit(0);
}

/**
 * main function
 */

int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr; /* Enough space for any address */ // line:netp:echoserveri:sockaddrstorage
    char client_hostname[MAXLINE], client_port[MAXLINE];

    Signal(SIGINT, sigint_handler);

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    mountDataFromFile("./stock.txt", &root);

    listenfd = Open_listenfd(argv[1]);

    pthread_t tid;
    sbuf_init(&sbuf, SBUF_SIZE);

    for (int i = 0; i < NTHREADS; i++)
    {
        Pthread_create(&tid, NULL, thread, NULL);
    }
    Sem_init(&sem, 0, 1);

    while (1)
    {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        /* ...print the client's IP address and port number... */
        Getnameinfo(
            (SA *)&clientaddr,
            clientlen,
            client_hostname,
            MAXLINE,
            client_port,
            MAXLINE,
            0);
        fprintf(stdout, "\033[0;32mconnected to %s:%s\033[0m\n", client_hostname, client_port);
        P(&sem);
        sbuf_insert(&sbuf, connfd);
        V(&sem);
    }
    exit(0);
}