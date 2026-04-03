#include <ctype.h>
#include <stdio.h>
#include <string.h>

struct Object {
    int numer;
    char* data;
};

int isNumber(const char* str)
{
    if (str == NULL || *str == '\0')
        return 0;

    if (*str == '-' || *str == '+')
        str++;

    int dotCount = 0;
    int spacesCount = 0;
    while (*str) {
        if (*str == '.') {
            dotCount++;
            if (dotCount > 1)
                return 0;
        } else if (*str == ' ') {
            spacesCount++;
            if ((*(str + 1) != '\0') && (*(str + 1) == '0')) {
                if ((*(str + 2) != '.') && (*(str + 1) != '\0')) {
                    return 0;
                }
            }
            if (spacesCount > 2)
                return 0;
        } else if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

void spaces(char* input, char* output)
{
    int i = 0;
    int j = 1;
    output[0] = ' ';
    while (input[i] != '\0') {
        if (input[i] == ',') {
            output[j++] = ' ';
            output[j++] = ',';
            output[j++] = ' ';
        } else {
            output[j++] = input[i];
        }
        i++;
    }
    output[j++] = ' ';
    output[j] = '\0';
}

int main()
{
    FILE* fp = fopen("input.csv", "r");
    if (!fp)
        return 1;

    char line1[1024];
    int arrLong[1024];
    struct Object arrData[1024];

    int i = 0;
    int j = 0;
    int sohr = 0;
    while (fgets(line1, 1024, fp)) {
        int j = 0;
        char line2[1024];
        spaces(line1, line2);
        char* token = strtok(line2, ",");
        while (token) {
            arrData[i].numer = j;
            arrData[i].data = strdup(token);
            token = strtok(NULL, ",");
            j++;
            i++;
            sohr = j;
        }
    }
    if (sohr == 0) {
        fclose(fp);
        return 0;
    }
    int countRow = i / sohr;
    int countCol = sohr;
    for (sohr--; sohr >= 0; sohr--) {
        arrLong[sohr] = 0;
    }
    for (i--; i >= 0; i--) {
        int l1 = arrLong[arrData[i].numer];
        int l2 = strlen(arrData[i].data);
        if (l1 < l2) {
            arrLong[arrData[i].numer] = l2;
        }
    }
    char string1[1024];
    char string2[1024];
    char string3[1024];
    sohr++;
    j = sohr;
    int t = 0;
    string1[t] = '+';
    while (arrLong[j]) {
        for (int n = 0; n <= arrLong[j]; n++) {
            t++;
            string1[t] = '=';
        }
        string1[t] = '+';
        j++;
    }
    t = 0;
    j = sohr;
    string2[t] = '+';
    while (arrLong[j]) {
        for (int n = 0; n <= arrLong[j]; n++) {
            t++;
            string2[t] = '-';
        }
        string2[t] = '+';
        j++;
    }
    t++;
    string1[t] = '\0';
    string2[t] = '\0';

    FILE* file = fopen("output.txt", "w+");
    fprintf(file, "%s\n", string1);
    t = 0;
    j = 0;
    i = 0;

    string3[t] = '|';
    t++;
    for (; i < countCol; i++) {
        if (isNumber(arrData[i].data)) {
            for (int n = 0; n < arrLong[j] - strlen(arrData[i].data); n++) {
                string3[t] = ' ';
                t++;
            }
            for (int n = 0; n < strlen(arrData[i].data); n++) {
                string3[t] = arrData[i].data[n];
                t++;
            }
        } else {
            for (int n = 0; n < arrLong[j]; n++) {
                if (arrData[i].data[n] && arrData[i].data[n] != '\n') {
                    string3[t] = arrData[i].data[n];
                } else {
                    string3[t] = ' ';
                }
                t++;
            }
        }
        j++;
        string3[t] = '|';
        t++;
    }
    string3[t] = '\0';
    fprintf(file, "%s\n", string3);

    for (int b = 1; b < countRow; b++) {
        fprintf(file, "%s\n", string2);
        char string3[1024];
        j = 0;
        t = 0;
        string3[t] = '|';
        t++;
        for (int c = 0; c < countCol; c++) {
            if (isNumber(arrData[i].data)) {
                for (int n = 0; n < arrLong[j] - strlen(arrData[i].data); n++) {
                    string3[t] = ' ';
                    t++;
                }
                for (int n = 0; n < strlen(arrData[i].data); n++) {
                    string3[t] = arrData[i].data[n];
                    t++;
                }
            } else {
                for (int n = 0; n < arrLong[j]; n++) {
                    if (arrData[i].data[n] && arrData[i].data[n] != '\n') {
                        string3[t] = arrData[i].data[n];
                    } else {
                        string3[t] = ' ';
                    }
                    t++;
                }
            }
            i++;
            j++;
            string3[t] = '|';
            t++;
        }
        string3[t] = '\0';
        fprintf(file, "%s\n", string3);
    }
    fprintf(file, "%s\n", string1);
    fclose(file);
    fclose(fp);
    return 0;
}