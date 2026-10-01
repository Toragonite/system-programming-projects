#ifndef STOCK_TREE_H
#define STOCK_TREE_H

#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include "csapp.h"

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

/** Deletes a stock item from the binary tree by its ID.
    @param root Pointer to the root of the binary tree.
    @param id The ID of the stock item to be deleted.
    This function modifies the tree to maintain the binary search tree properties post-deletion. */
void deleteStockItem(treeNode **root, int id);

/** Recursively deletes the entire binary tree, freeing all associated resources.
    @param root The root node of the binary tree to be deleted.
    This function ensures all memory is freed and semaphores are destroyed to avoid leaks. */
void deleteTree(treeNode *root);

/**
 * @brief Mounts stock data from a file into a binary tree.
 *
 * This function reads stock data from the specified file, creates stock
 * items, and inserts them into the binary tree. The file should contain
 * stock data with each line formatted as: ID stock price.
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
 * This function performs an in-order traversal of the binary tree, writing the
 * ID, left_stock, and price of each stock item to the specified file and
 * sending the same data to the client via the connection file descriptor.
 * The function uses a buffer to format the data before writing and sending.
 *
 * @param root Pointer to the root of the binary tree.
 * @param connfd File descriptor for the output file and client connection.
 *
 * @return void
 *
 * The function does not return a value. It writes formatted data to the
 * specified file and sends it to the client. If the tree is empty, the function
 * performs no operations.
 */
void saveTreeToFile(treeNode *root, char *buf);

void saveTreeDataToFile(char *filedest, treeNode *root);

#endif // STOCK_TREE_H
