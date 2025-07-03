#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int32_t get_int(const uint8_t is_sign) {
  char buf[20];
  int32_t val;
  int count = 0;

  while (1) {
    if (!fgets(buf, sizeof(buf), stdin)) {
      continue;
    }
    if (is_sign) {
      if (sscanf(buf, "%d %n", &val, &count) == 1 && buf[count] == '\0') {
        return val;
      } else {
        puts("Invalid input!");
      }
    } else {
      if (sscanf(buf, "%u %n", &val, &count) == 1 && buf[count] == '\0') {
        return val;
      } else {
        puts("Invalid input!");
      }
    }
  }
}

char *get_string() {
  char buf[10];
  char *string = NULL;

  if (fgets(buf, sizeof(buf), stdin)) {
    buf[strcspn(buf, "\n")] = '\0';
    if ((string = malloc(strlen(buf) + 1))) {
      strcpy(string, buf);
    }
  }

  return string;
}
