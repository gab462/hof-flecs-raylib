#pragma once

#include <stdbool.h>

void whitelist_setup(char* path);
bool in_whitelist(char* ip);
