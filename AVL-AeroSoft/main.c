#include "tree.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static char* trimWhitespace(char* text)
{
    while (isspace((unsigned char)*text)) {
        ++text;
    }

    char* end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1])) {
        --end;
    }
    *end = '\0';
    return text;
}

static void discardInputRemainder(void)
{
    int character;
    do {
        character = getchar();
    } while (character != '\n' && character != EOF);
}

static int readInputLine(char* line, int capacity)
{
    if (!fgets(line, capacity, stdin)) {
        return 0;
    }

    char* newline = strchr(line, '\n');
    if (newline) {
        *newline = '\0';
        return 1;
    }
    if (!feof(stdin)) {
        discardInputRemainder();
        return -1;
    }
    return 1;
}

static void printHelp(void)
{
    printf("Неизвестная команда. Доступные команды:\n");
    printf("  find <код> - найти аэропорт по IATA коду\n");
    printf("  add <код>:<название> - добавить новый аэропорт\n");
    printf("  delete <код> - удалить аэропорт\n");
    printf("  save - сохранить базу в файл\n");
    printf("  quit - завершить работу\n");
}

int main(int argc, char* argv[])
{
    enum { InputCapacity = 512 };

    if (argc != 2) {
        printf("Использование: %s <файл_аэропортов>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    Node* root = NULL;
    int loadCount = loadAirports(filename, &root);
    if (loadCount < 0) {
        fprintf(stderr, "Ошибка: не удалось корректно загрузить файл %s\n", filename);
        freeTree(root);
        return 1;
    }

    printf("Загружено %d аэропортов. Система готова к работе.\n", loadCount);

    char line[InputCapacity];
    for (;;) {
        printf("\n> ");
        int readResult = readInputLine(line, InputCapacity);
        if (readResult == 0) {
            break;
        }
        if (readResult < 0) {
            printf("Ошибка: команда слишком длинная.\n");
            continue;
        }

        char* command = trimWhitespace(line);
        if (*command == '\0') {
            continue;
        }

        char* arguments = command;
        while (*arguments && !isspace((unsigned char)*arguments)) {
            ++arguments;
        }
        if (*arguments) {
            *arguments++ = '\0';
            arguments = trimWhitespace(arguments);
        }

        if (strcmp(command, "quit") == 0) {
            if (*arguments != '\0') {
                printf("Использование: quit\n");
                continue;
            }
            break;
        }

        if (strcmp(command, "save") == 0) {
            if (*arguments != '\0') {
                printf("Использование: save\n");
                continue;
            }
            if (saveTreeToFile(root, filename)) {
                printf("База сохранена: %zu аэропортов.\n", countNodes(root));
            } else {
                printf("Ошибка при сохранении базы. Исходный файл не изменён.\n");
            }
            continue;
        }

        if (strcmp(command, "find") == 0) {
            char code[IATA_CODE_CAPACITY];
            if (!normalizeIataCode(arguments, code)) {
                printf("Использование: find <трёхбуквенный код>\n");
                continue;
            }

            const Node* found = searchAirport(root, code);
            if (found) {
                printf("%s → %s\n", found->airport.code, found->airport.name);
            } else {
                printf("Аэропорт с кодом '%s' не найден в базе.\n", code);
            }
            continue;
        }

        if (strcmp(command, "add") == 0) {
            char* colon = strchr(arguments, ':');
            if (!colon) {
                printf("Использование: add <код>:<название>\n");
                continue;
            }

            *colon = '\0';
            char* codeText = trimWhitespace(arguments);
            char* name = trimWhitespace(colon + 1);

            Airport airport = { 0 };
            if (!normalizeIataCode(codeText, airport.code)) {
                printf("Ошибка: IATA-код должен состоять из трёх латинских букв.\n");
                continue;
            }
            if (*name == '\0') {
                printf("Ошибка: название аэропорта не может быть пустым.\n");
                continue;
            }
            if (strlen(name) >= sizeof(airport.name)) {
                printf("Ошибка: название аэропорта слишком длинное.\n");
                continue;
            }
            snprintf(airport.name, sizeof(airport.name), "%s", name);

            int inserted = insertAirport(&root, &airport);
            if (inserted > 0) {
                printf("Аэропорт '%s' добавлен в базу.\n", airport.code);
            } else if (inserted == 0) {
                printf("Аэропорт с кодом '%s' уже существует!\n", airport.code);
            } else {
                printf("Ошибка: не удалось выделить память для аэропорта.\n");
            }
            continue;
        }

        if (strcmp(command, "delete") == 0) {
            char code[IATA_CODE_CAPACITY];
            if (!normalizeIataCode(arguments, code)) {
                printf("Использование: delete <трёхбуквенный код>\n");
                continue;
            }

            if (deleteAirport(&root, code)) {
                printf("Аэропорт '%s' удалён из базы.\n", code);
            } else {
                printf("Аэропорт с кодом '%s' не найден в базе.\n", code);
            }
            continue;
        }

        printHelp();
    }

    freeTree(root);
    return 0;
}
