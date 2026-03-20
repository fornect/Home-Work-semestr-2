#include <stdio.h>
#include <string.h>

struct Object {
    int numer;
    char* data;
};

int main()
{
    FILE* fp = fopen("input.csv", "r");
    if (!fp)
        return 1;

    char line[1024];
    int arrLong[1024];
    struct Object arrData[1024];

    int i = 0;
    int j = 0;
    int sohr = 0;
    while (fgets(line, 1024, fp)) {
        int j = 0;
        char* token = strtok(line, ",");
        while (token) {
            arrData[i].numer = j;
            arrData[i].data = strdup(token);
            token = strtok(NULL, ",");
            j++;
            i++;
            sohr = j;
        }
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
    while (arrLong[j]) {
        string1[t] = '+';
        for (int n = 0; n <= arrLong[j] + 2; n++) {
            t++;
            string1[t] = '=';
        }
        string1[t] = '+';
        j++;
    }
    t = 0;
    j = sohr;
    while (arrLong[j]) {
        string2[t] = '+';
        for (int n = 0; n <= arrLong[j] + 2; n++) {
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

    t = 0;
    j = 0;
    i = 0;
    fprintf(file, "%s\n", string1);
    for (; i < countCol; i++) {
        string3[t] = '|';
        t++;
        string3[t] = ' ';
        t++;
        for (int n = 0; n <= arrLong[j]; n++) {
            if (arrData[i].data[n] && arrData[i].data[n] != '\n') {
                string3[t] = arrData[i].data[n];
            } else {
                string3[t] = ' ';
            }
            t++;
        }
        j++;
        string3[t] = '|';
    }
    t++;
    string3[t] = '\0';
    fprintf(file, "%s\n", string3);
    fprintf(file, "%s\n", string1);

    for (int b = 1; b < countRow; b++) {
        char string3[1024];
        j = 0;
        t = 0;
        for (int c = 0; c < countCol; c++) {
            string3[t] = '|';
            t++;
            string3[t] = ' ';
            t++;

            for (int n = 0; n <= arrLong[j]; n++) {
                if (arrData[i].data[n] && arrData[i].data[n] != '\n') {
                    string3[t] = arrData[i].data[n];
                } else {
                    string3[t] = ' ';
                }
                t++;
            }
            i++;
            j++;
            string3[t] = '|';
        }
        t++;
        string3[t] = '\0';
        fprintf(file, "%s\n", string3);
        fprintf(file, "%s\n", string2);
    }
    fclose(file);
    fclose(fp);
    return 0;
}