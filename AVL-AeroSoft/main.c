#include "tree.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

typedef enum CommandResult {
    CommandContinue,
    CommandQuit
} CommandResult;

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
    printf("  add <код> <название> - добавить новый аэропорт\n");
    printf("  delete <код> - удалить аэропорт\n");
    printf("  save - сохранить базу в файл\n");
    printf("  quit - завершить работу\n");
}

static void handleFind(const AvlTree* tree, char* arguments)
{
    char code[IATA_CODE_CAPACITY];
    if (!avlTreeNormalizeCode(arguments, code)) {
        printf("Использование: find <трёхбуквенный код>\n");
        return;
    }

    const char* name = avlTreeFind(tree, code);
    if (name) {
        printf("%s → %s\n", code, name);
    } else {
        printf("Аэропорт с кодом '%s' не найден в базе.\n", code);
    }
}

static void handleAdd(AvlTree* tree, char* arguments)
{
    char* name = arguments;
    while (*name && !isspace((unsigned char)*name)) {
        ++name;
    }
    if (*name == '\0') {
        printf("Использование: add <код> <название>\n");
        return;
    }

    *name++ = '\0';
    name = trimWhitespace(name);

    char code[IATA_CODE_CAPACITY];
    if (!avlTreeNormalizeCode(arguments, code)) {
        printf("Ошибка: IATA-код должен состоять из трёх латинских букв.\n");
        return;
    }
    if (*name == '\0') {
        printf("Ошибка: название аэропорта не может быть пустым.\n");
        return;
    }
    if (strlen(name) >= AIRPORT_NAME_CAPACITY) {
        printf("Ошибка: название аэропорта слишком длинное.\n");
        return;
    }

    AvlTreeResult result = avlTreeInsert(tree, code, name);
    if (result == AvlTreeChanged) {
        printf("Аэропорт '%s' добавлен в базу.\n", code);
    } else if (result == AvlTreeNotChanged) {
        printf("Аэропорт с кодом '%s' уже существует!\n", code);
    } else {
        printf("Ошибка: не удалось добавить аэропорт.\n");
    }
}

static void handleDelete(AvlTree* tree, char* arguments)
{
    char code[IATA_CODE_CAPACITY];
    if (!avlTreeNormalizeCode(arguments, code)) {
        printf("Использование: delete <трёхбуквенный код>\n");
        return;
    }

    AvlTreeResult result = avlTreeDelete(tree, code);
    if (result == AvlTreeChanged) {
        printf("Аэропорт '%s' удалён из базы.\n", code);
    } else {
        printf("Аэропорт с кодом '%s' не найден в базе.\n", code);
    }
}

static void handleSave(const AvlTree* tree, const char* filename, const char* arguments)
{
    if (*arguments != '\0') {
        printf("Использование: save\n");
        return;
    }

    if (avlTreeSave(tree, filename)) {
        printf("База сохранена: %zu аэропортов.\n", avlTreeSize(tree));
    } else {
        printf("Ошибка при сохранении базы. Исходный файл не изменён.\n");
    }
}

static CommandResult executeCommand(AvlTree* tree, const char* filename, char* line)
{
    char* command = trimWhitespace(line);
    if (*command == '\0') {
        return CommandContinue;
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
        if (*arguments == '\0') {
            return CommandQuit;
        }
        printf("Использование: quit\n");
    } else if (strcmp(command, "save") == 0) {
        handleSave(tree, filename, arguments);
    } else if (strcmp(command, "find") == 0) {
        handleFind(tree, arguments);
    } else if (strcmp(command, "add") == 0) {
        handleAdd(tree, arguments);
    } else if (strcmp(command, "delete") == 0) {
        handleDelete(tree, arguments);
    } else {
        printHelp();
    }
    return CommandContinue;
}

static void runCommandLoop(AvlTree* tree, const char* filename)
{
    enum { InputCapacity = 512 };
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
        if (executeCommand(tree, filename, line) == CommandQuit) {
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Использование: %s <файл_аэропортов>\n", argv[0]);
        return 1;
    }

    AvlTree* tree = avlTreeCreate();
    if (!tree) {
        fprintf(stderr, "Ошибка: не удалось создать АВЛ-дерево.\n");
        return 1;
    }

    int loadCount = avlTreeLoad(tree, argv[1]);
    if (loadCount < 0) {
        fprintf(stderr, "Ошибка: не удалось корректно загрузить файл %s\n", argv[1]);
        avlTreeDestroy(tree);
        return 1;
    }

    printf("Загружено %d аэропортов. Система готова к работе.\n", loadCount);
    runCommandLoop(tree, argv[1]);
    avlTreeDestroy(tree);
    return 0;
}
