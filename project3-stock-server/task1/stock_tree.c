#include "stock_tree.h"

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

        sprintf(buffer, "%d %d %d\n", root->item.ID, root->item.left_stock, root->item.price);

        // Append to the buffer
        strcat(buf, buffer);

        saveTreeToFile(root->left, buf);
        saveTreeToFile(root->right, buf);
    }
}

// Initialize a stock item
void initStockItem(stockItem *item, int id, int stock, int price)
{
    item->ID = id;
    item->left_stock = stock;
    item->price = price;
    item->readcnt = 0;
    sem_init(&item->mutex, 0, 1);
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

// Delete a stock item from the binary tree
void deleteStockItem(treeNode **root, int id)
{
    if (*root == NULL)
        return;

    if (id < (*root)->item.ID)
    {
        deleteStockItem(&(*root)->left, id);
    }
    else if (id > (*root)->item.ID)
    {
        deleteStockItem(&(*root)->right, id);
    }
    else
    {
        if ((*root)->left == NULL)
        {
            treeNode *temp = *root;
            *root = (*root)->right;
            free(temp);
        }
        else if ((*root)->right == NULL)
        {
            treeNode *temp = *root;
            *root = (*root)->left;
            free(temp);
        }
        else
        {
            treeNode *temp = (*root)->right;
            while (temp->left != NULL)
            {
                temp = temp->left;
            }
            (*root)->item = temp->item;
            deleteStockItem(&(*root)->right, temp->item.ID);
        }
    }
}

// Delete the entire tree
void deleteTree(treeNode *root)
{
    if (root != NULL)
    {
        deleteTree(root->left);
        deleteTree(root->right);
        sem_destroy(&root->item.mutex);
        free(root);
    }
}
