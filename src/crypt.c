#include <stdio.h>
#include "sha256.h"


#define MAX_STRING_LENGTH 4608
#define LOWER(c)            ((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)            ((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))

/*
 * Returns an initial-capped string.
 */
char *capitalize(const char *str)
{
    static char strcap[MAX_STRING_LENGTH];
    int i;

    for (i = 0; str[i] != '\0'; i++)
        strcap[i] = LOWER(str[i]);
    strcap[i] = '\0';
    strcap[0] = UPPER(strcap[0]);
    return strcap;
}

int main(int argc, char **argv)
{
  if (argc != 3)
  {
    printf("Usage: %s username password\n", argv[0]);
    printf("  Create a hash for the provided player name and password\n");
    return -1;
  }
  printf("Crypt hash: %s\n", sha256_crypt_with_salt(argv[2], capitalize(argv[1])));
}
