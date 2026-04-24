#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Airport {
    char code[5];
    char name[256];
} Airport;

typedef struct Node {
    Airport airport;
    int height;
    struct Node* left;
    struct Node* right;
} Node;

int height(Node* n)
{
    if (n) {
        return n->height;
    } else {
        return 0;
    }
}

int getBalance(Node* n)
{
    if (n) {
        return height(n->left) - height(n->right);
    } else {
        return 0;
    }
}

Node* createNode(Airport airport)
{
    Node* node = (Node*)malloc(sizeof(Node));
    node->airport = airport;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

Node* rotateRight(Node* y)
{
    Node* x = y->left;
    Node* T2 = x->right;

    x->right = y;
    y->left = T2;

    int leftHeightY, rightHeightY;
    leftHeightY = height(y->left);
    rightHeightY = height(y->right);
    if (leftHeightY > rightHeightY) {
        y->height = leftHeightY + 1;
    } else {
        y->height = rightHeightY + 1;
    }

    int leftHeightX, rightHeightX;
    leftHeightX = height(x->left);
    rightHeightX = height(x->right);
    if (leftHeightX > rightHeightX) {
        x->height = leftHeightX + 1;
    } else {
        x->height = rightHeightX + 1;
    }

    return x;
}

Node* rotateLeft(Node* x)
{
    Node* y = x->right;
    Node* T2 = y->left;

    y->left = x;
    x->right = T2;

    int leftHeightX, rightHeightX;
    leftHeightX = height(x->left);
    rightHeightX = height(x->right);
    if (leftHeightX > rightHeightX) {
        x->height = leftHeightX + 1;
    } else {
        x->height = rightHeightX + 1;
    }

    int leftHeightY, rightHeightY;
    leftHeightY = height(y->left);
    rightHeightY = height(y->right);
    if (leftHeightY > rightHeightY) {
        y->height = leftHeightY + 1;
    } else {
        y->height = rightHeightY + 1;
    }

    return y;
}

Node* insert(Node* node, Airport airport)
{
    if (!node) {
        return createNode(airport);
    }

    int cmp = strcmp(airport.code, node->airport.code);

    if (cmp < 0) {
        node->left = insert(node->left, airport);
    } else if (cmp > 0) {
        node->right = insert(node->right, airport);
    } else {
        return node;
    }

    int leftHeight, rightHeight;
    leftHeight = height(node->left);
    rightHeight = height(node->right);
    if (leftHeight > rightHeight) {
        node->height = leftHeight + 1;
    } else {
        node->height = rightHeight + 1;
    }

    int balance = getBalance(node);

    if (balance > 1 && node->left) {
        int cmpLeft = strcmp(airport.code, node->left->airport.code);
        if (cmpLeft < 0) {
            return rotateRight(node);
        }
    }

    if (balance < -1 && node->right) {
        int cmpRight = strcmp(airport.code, node->right->airport.code);
        if (cmpRight > 0) {
            return rotateLeft(node);
        }
    }

    if (balance > 1 && node->left) {
        int cmpLeft = strcmp(airport.code, node->left->airport.code);
        if (cmpLeft > 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
    }

    if (balance < -1 && node->right) {
        int cmpRight = strcmp(airport.code, node->right->airport.code);
        if (cmpRight < 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
    }

    return node;
}

Node* minValueNode(Node* node)
{
    Node* current = node;
    while (current->left) {
        current = current->left;
    }
    return current;
}

Node* deleteNode(Node* root, char* code)
{
    if (!root) {
        return root;
    }

    int cmp = strcmp(code, root->airport.code);

    if (cmp < 0) {
        root->left = deleteNode(root->left, code);
    } else if (cmp > 0) {
        root->right = deleteNode(root->right, code);
    } else {
        if (!root->left || !root->right) {
            Node* temp;
            if (root->left) {
                temp = root->left;
            } else {
                temp = root->right;
            }
            if (!temp) {
                temp = root;
                root = NULL;
            } else {
                *root = *temp;
            }
            free(temp);
        } else {
            Node* temp = minValueNode(root->right);
            root->airport = temp->airport;
            root->right = deleteNode(root->right, temp->airport.code);
        }
    }

    if (!root) {
        return root;
    }

    int leftHeight, rightHeight;
    leftHeight = height(root->left);
    rightHeight = height(root->right);
    if (leftHeight > rightHeight) {
        root->height = leftHeight + 1;
    } else {
        root->height = rightHeight + 1;
    }

    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0) {
        return rotateRight(root);
    }

    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    if (balance < -1 && getBalance(root->right) <= 0) {
        return rotateLeft(root);
    }

    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

Node* search(Node* root, char* code)
{
    if (!root || strcmp(root->airport.code, code) == 0) {
        return root;
    }
    if (strcmp(code, root->airport.code) < 0) {
        return search(root->left, code);
    }
    return search(root->right, code);
}

int countNodes(Node* root)
{
    if (!root) {
        return 0;
    }
    return 1 + countNodes(root->left) + countNodes(root->right);
}

void saveToFile(Node* root, FILE* file)
{
    if (root) {
        saveToFile(root->left, file);
        fprintf(file, "%s:%s\n", root->airport.code, root->airport.name);
        saveToFile(root->right, file);
    }
}

int saveTreeToFile(Node* root, const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Ошибка: Не удалось открыть файл %s для записи!\n", filename);
        return 0;
    }

    saveToFile(root, file);
    fclose(file);
    return 1;
}

void freeTree(Node* root)
{
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

void trimNewline(char* str)
{
    char* newline = strchr(str, '\n');
    if (newline) {
        *newline = '\0';
    }
}

void toUpperCase(char* str)
{
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int loadAirports(const char* filename, Node** root)
{
    FILE* file = fopen(filename, "r");
    if (!file) {
        return -1;
    }

    char line[512];
    int count = 0;

    while (fgets(line, sizeof(line), file)) {
        trimNewline(line);

        char* colon = strchr(line, ':');
        if (colon) {
            Airport airport;
            *colon = '\0';
            strcpy(airport.code, line);
            strcpy(airport.name, colon + 1);
            *root = insert(*root, airport);
            count++;
        }
    }

    fclose(file);
    return count;
}

int main(int argc, char* argv[])
{
    Node* root = NULL;
    char filename[256];

    if (argc < 2) {
        printf("Использование: %s <файл_аэропортов>\n", argv[0]);
        return 1;
    }

    strcpy(filename, argv[1]);

    int loadCount = loadAirports(filename, &root);
    if (loadCount < 0) {
        printf("Ошибка: Не удалось открыть файл %s\n", filename);
        return 1;
    }

    printf("Загружено %d аэропортов. Система готова к работе.\n", loadCount);

    char command[32];
    char line[512];

    while (1) {
        printf("\n> ");
        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }

        trimNewline(line);

        if (strlen(line) == 0) {
            continue;
        }

        sscanf(line, "%s", command);

        if (strcmp(command, "quit") == 0) {
            break;
        } else if (strcmp(command, "save") == 0) {
            if (saveTreeToFile(root, filename)) {
                int total = countNodes(root);
                printf("База сохранена: %d аэропортов.\n", total);
            } else {
                printf("Ошибка при сохранении базы.\n");
            }
        } else if (strcmp(command, "find") == 0) {
            char code[5];
            if (sscanf(line, "%*s %s", code) != 1) {
                printf("Использование: find <код>\n");
                continue;
            }
            toUpperCase(code);

            Node* found = search(root, code);
            if (found) {
                printf("%s → %s\n", found->airport.code, found->airport.name);
            } else {
                printf("Аэропорт с кодом '%s' не найден в базе.\n", code);
            }
        } else if (strcmp(command, "add") == 0) {
            char* colon = strchr(line, ':');
            if (!colon) {
                printf("Использование: add <код>:<название>\n");
                continue;
            }

            char code[5];
            char name[256];

            sscanf(line, "%*s %4s", code);
            toUpperCase(code);

            strcpy(name, colon + 1);

            Node* existing = search(root, code);
            if (existing) {
                printf("Аэропорт с кодом '%s' уже существует!\n", code);
                continue;
            }

            Airport newAirport;
            strcpy(newAirport.code, code);
            strcpy(newAirport.name, name);

            root = insert(root, newAirport);
            printf("Аэропорт '%s' добавлен в базу.\n", code);
        } else if (strcmp(command, "delete") == 0) {
            char code[5];
            if (sscanf(line, "%*s %s", code) != 1) {
                printf("Использование: delete <код>\n");
                continue;
            }
            toUpperCase(code);

            Node* existing = search(root, code);
            if (!existing) {
                printf("Аэропорт с кодом '%s' не найден в базе.\n", code);
                continue;
            }

            root = deleteNode(root, code);
            printf("Аэропорт '%s' удалён из базы.\n", code);
        } else {
            printf("Неизвестная команда. Доступные команды:\n");
            printf("  find <код> - найти аэропорт по IATA коду\n");
            printf("  add <код>:<название> - добавить новый аэропорт\n");
            printf("  delete <код> - удалить аэропорт\n");
            printf("  save - сохранить базу в файл\n");
            printf("  quit - завершить работу\n");
        }
    }

    freeTree(root);
    return 0;
}