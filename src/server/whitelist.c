#include <cut.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <whitelist.h>

char** whitelist = NULL;

void whitelist_setup(char* path)
{
    FILE* f = fopen(path, "rb");

    if (f == NULL) {
        perror(__func__);
        fprintf(stderr, "Whitelist not found, allowing all addresses\n");
        return;
    }

    push(&whitelist, NULL);
    int idx = 0;
    char c;

    while (c = fgetc(f), c != EOF) {
        if (c == '\n' || c == '\r') {
            push(&whitelist, NULL);
            idx++;
        } else {
            push(&whitelist[idx], c);
        }
    }

    fclose(f);
}

bool in_whitelist(char* ip)
{
    if (whitelist == NULL) // No whitelist, allow all
        return true;

    foreach (addr, whitelist) {
        if (*addr == NULL)
            continue;

        if (strncmp(ip, *addr, len(*addr)) == 0)
            return true;
    }

    return false;
}
