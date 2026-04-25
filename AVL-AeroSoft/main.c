#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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