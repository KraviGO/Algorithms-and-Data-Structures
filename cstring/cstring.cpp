#include "cstring.h"

size_t Strlen(const char* str) {
    size_t size = 0;
    while (*str != '\0') {
        ++size;
        ++str;
    }
    return size;
}

int Strcmp(const char* first, const char* second) {
    while (true) {
        const char& first_char = *first;
        const char& second_char = *second;
        if (first_char == '\0' && second_char == '\0') {
            return 0;
        }
        if (first_char == '\0') {
            return -1;
        }
        if (second_char == '\0') {
            return 1;
        }
        if (first_char < second_char) {
            return -1;
        }
        if (second_char < first_char) {
            return 1;
        }
        ++first;
        ++second;
    }
}

int Strncmp(const char* first, const char* second, size_t count) {
    while (count > 0) {
        const char& first_char = *first;
        const char& second_char = *second;
        if (first_char == '\0' && second_char == '\0') {
            return 0;
        }
        if (first_char == '\0') {
            return -1;
        }
        if (second_char == '\0') {
            return 1;
        }
        if (first_char < second_char) {
            return -1;
        }
        if (second_char < first_char) {
            return 1;
        }
        ++first;
        ++second;
        --count;
    }
    return 0;
}

char* Strcpy(char* dest, const char* src) {
    char* begin = dest;
    while (true) {
        const char& value = *src;
        *dest = value;
        if (value == '\0') {
            return begin;
        }
        ++dest;
        ++src;
    }
}

char* Strncpy(char* dest, const char* src, size_t count) {
    char* begin = dest;

    for (; count > 0; --count) {
        const char& value = *src;
        *dest = value;
        if (value != '\0') {
            ++src;
        }
        ++dest;
    }

    return begin;
}

char* Strcat(char* dest, const char* src) {
    char* begin = dest;

    while (*dest != '\0') {
        ++dest;
    }

    while (true) {
        const char& value = *src;
        *dest = value;
        if (value == '\0') {
            return begin;
        }
        ++dest;
        ++src;
    }
}

char* Strncat(char* dest, const char* src, size_t count) {
    char* begin = dest;

    while (*dest != '\0') {
        ++dest;
    }

    while (count > 0) {
        const char& value = *src;
        *dest = value;
        if (value == '\0' ) {
            return begin;
        }
        ++dest;
        ++src;
        --count;
    }

    *dest = '\0';
    return begin;
}

const char* Strchr(const char* str, char symbol) {
    while (true) {
        const char& value = *str;
        if (value == symbol) {
            return str;
        }
        if (value == '\0') {
            return nullptr;
        }
        ++str;
    }
}

const char* Strrchr(const char* str, char symbol) {
    const char* res = nullptr;
    while (true) {
        const char& value = *str;
        if (value == symbol) {
            res = str;
        }
        if (value == '\0') {
            return res;
        }
        ++str;
    }
}

size_t Strspn(const char* dest, const char* src) {
    const char* begin = src;
    size_t res = 0;

    for (; *dest != '\0'; ++dest) {
        bool flag = false;
        for (src = begin; *src != '\0'; ++src) {
            if (*src == *dest) {
                flag = true;
                ++res;
                break;
            }
        }
        if (!flag) {
            return res;
        }
    }

    return res;
}

size_t Strcspn(const char* dest, const char* src) {
    const char* begin = src;
    size_t res = 0;

    for (; *dest != '\0'; ++dest) {
        for (src = begin; *src != '\0'; ++src) {
            if (*src == *dest) {
                return res;
            }
        }
        ++res;
    }

    return res;
}

const char* Strpbrk(const char* dest, const char* breakset) {
    const char* begin = breakset;

    for (; *dest != '\0'; ++dest) {
        for (breakset = begin; *breakset != '\0'; ++breakset) {
            if (*dest == *breakset) {
                return dest;
            }
        }
    }

    return nullptr;
}

const char* Strstr(const char* str, const char* pattern) {
    size_t str_len = Strlen(str);
    size_t pattern_len = Strlen(pattern);

    for (int i = 0; i <= static_cast<int>(str_len) - static_cast<int>(pattern_len); ++i) {
        if (Strncmp(str, pattern, pattern_len) == 0) {
            return str;
        }
        ++str;
    }

    return nullptr;
}