/*
 * echoserveri.c - An iterative echo server
 */
/* $begin echoserverimain */
#include "csapp.h"
#include "stock_tree.h"
#include <stdbool.h>

#define MAX_LINE_LENGTH 100

void handle_sigint(int sig);
void echo(int connfd);
void command(char *cmd, int connfd);
bool stockbuy(int id, int amount);
bool stocksell(int id, int amount);
void stockexit(char *buf);
// void save

treeNode *root;

/**
 * begin
 * this area is about pool structure
 */
typedef struct
{                                /* Represents a pool of connected descriptors */
    int maxfd;                   /* Largest descriptor in read_set */
    fd_set read_set;             /* Set of all active descriptors */
    fd_set ready_set;            /* Subset of descriptors ready for reading  */
    int nready;                  /* Number of ready descriptors from select */
    int maxi;                    /* High water index into client array */
    int clientfd[FD_SETSIZE];    /* Set of active descriptors */
    rio_t clientrio[FD_SETSIZE]; /* Set of active read buffers */
} pool;

static pool client_pool;

int byte_cnt = 0;

void handle_sigint(int sig)
{
    saveTreeDataToFile("./stock.txt", root);
    deleteTree(root);
    fprintf(stdout, "terminating server...\n");
    exit(0);
}

/**
 * @brief Initializes the pool of connected clients.
 *
 * This function initializes the pool structure, setting the maximum descriptor
 * and the select read set. Initially, there are no connected descriptors
 * except for the listening socket.
 *
 * @param listenfd The file descriptor of the listening socket.
 * @param p A pointer to the pool structure to be initialized.
 */
void init_pool(int listenfd, pool *p)
{
    /* Initially, there are no connected descriptors */
    int i;
    p->maxi = -1;
    for (i = 0; i < FD_SETSIZE; i++)
    {
        p->clientfd[i] = -1;

        /* Initailly, listenfd is only member of select read set */
        p->maxfd = listenfd;
        FD_ZERO(&p->read_set);
        FD_SET(listenfd, &p->read_set);
    }
}

/**
 * @brief Adds a new client to the pool.
 *
 * This function adds a new connected descriptor to the pool and updates the
 * select read set, the maximum descriptor, and the high water mark.
 *
 * @param connfd The file descriptor of the connected client.
 * @param p A pointer to the pool structure where the client will be added.
 */
void add_client(int connfd, pool *p)
{
    int i;
    p->nready--;
    for (i = 0; i < FD_SETSIZE; i++)
    {
        if (p->clientfd[i] < 0)
        {
            /* Add connected descriptor to the pool */
            p->clientfd[i] = connfd;
            Rio_readinitb(&p->clientrio[i], connfd);

            /* Add the descriptor to descriptor set */
            FD_SET(connfd, &p->read_set);

            /* Update max descriptor and pool high water mark */
            if (connfd > p->maxfd)
            {
                p->maxfd = connfd;
            }
            if (i > p->maxi)
            {
                p->maxi = i;
            }
            break;
        }
    }
    if (i == FD_SETSIZE)
    {
        app_error("add_client error : Too many clients");
    }
}

/**
 * @brief Checks the clients for readable data and handles it.
 *
 * This function iterates over the pool of connected clients and checks if any
 * of them have data ready to be read. If data is ready, it reads a text line,
 * echoes it back to the client, and updates the byte count. If EOF is detected,
 * the client descriptor is removed from the pool.
 *
 * @param p A pointer to the pool structure to be checked.
 */
void check_client(pool *p)
{
    int i, connfd, n;
    char buf[MAXLINE];
    rio_t rio;

    for (i = 0; (i <= p->maxi) && (p->nready > 0); i++)
    {
        connfd = p->clientfd[i];
        rio = p->clientrio[i];

        /* If the descriptor is ready, echo a text line from it */
        if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set)))
        {
            p->nready--;
            if ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0)
            {
                byte_cnt += n;
                printf("Server recieved %d (%d total) bytes on fd %d\n", n, byte_cnt, connfd);
                printf("%s\n", buf);

                command(buf, connfd);
                memset(buf, 0, MAXLINE);
            }

            /* EOF detected, remove desciptor from pool */
            else
            {
                Close(connfd);
                FD_CLR(connfd, &p->read_set);
                p->clientfd[i] = -1;
            }
        }
    }
}
/**
 * end
 * this area is about pool structure
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
        // have to send MAXLINE length
        //  response[0] = '\0';
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
        // response[0] = '\0';
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
        // response[0] = '\0';
    }
    else if (!strncmp(cmd, "exit", 4))
    {
        Rio_writen(connfd, "\033[0;30mpress enter to exit\033[0m", MAXLINE);
        stockexit(response);
        Rio_writen(connfd, response, MAXLINE);
        Close(connfd);
        FD_CLR(connfd, &(client_pool.read_set)); // may occur error
    }
    else
    {
        // not valid command print usage
        strcpy(response, "\033[0;31mInvalid command\033[0m\n");
        Rio_writen(connfd, response, strlen(response));
    }
    return;
}

bool stockbuy(int id, int amount)
{
    stockItem *targetstock = findStockItem(root, id);
    if (targetstock != NULL)
    {
        if (targetstock->left_stock >= amount)
        {
            targetstock->left_stock -= amount;
            return true;
        }
        else
        {
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
        targetstock->left_stock += amount;
        return true;
    }
    else
    {
        stockItem newstockitem;
        int price = rand() % 20000;
        initStockItem(&newstockitem, id, amount, price);
        insertStockItem(root, newstockitem);
        return true;
    }
    return false;
}

void stockexit(char *buf)
{
    // dump tree structure to text file w/ fd
    saveTreeDataToFile("./stock.txt", root);
    buf[0] = '\0';
}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr; /* Enough space for any address */ // line:netp:echoserveri:sockaddrstorage
    char client_hostname[MAXLINE], client_port[MAXLINE];

    // int id, inventory, price;
    // rio_t rio;

    // static pool pool;

    Signal(SIGINT, handle_sigint);

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    listenfd = Open_listenfd(argv[1]);
    init_pool(listenfd, &client_pool);

    mountDataFromFile("./stock.txt", &root);
    // saveTreeToFile(root, STDOUT_FILENO, buf);

    while (1)
    {
        /* Wait for listening/connected descritor(s) to become ready */
        client_pool.ready_set = client_pool.read_set;
        client_pool.nready = Select(client_pool.maxfd + 1, &client_pool.ready_set, NULL, NULL, NULL);

        /* If listening descriptor ready, add new client to pool */
        if (FD_ISSET(listenfd, &client_pool.ready_set))
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

            add_client(connfd, &client_pool);
        }

        check_client(&client_pool);
    }
    exit(0);
}
/* $end echoserverimain */