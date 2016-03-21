#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int prefix(const char *str, const char *pre) {
  return strncmp(pre, str, strlen(pre)) == 0;
}

int main(void) {
  size_t bufsize = 128;
  char *controller_message = (char *)malloc(bufsize * sizeof(char));

  for (;;) {
    getline(&controller_message, &bufsize, stdin);

    if (prefix(controller_message, "aei")) {
      printf("protocol-version 1\n");
      printf("id name bot_net\n");
      printf("id author brendanator\n");
      printf("id version 1\n");
      printf("aeiok\n");
    } else if (prefix(controller_message, "isready")) {
      printf("readyok\n");
    } else if (prefix(controller_message, "newgame")) {

    } else if (prefix(controller_message, "setposition")) {
      // <side> <position>
    } else if (prefix(controller_message, "setoption")) {
      // name <id> [value <x>]
    } else if (prefix(controller_message, "makemove")) {
      // <move> 
    } else if (prefix(controller_message, "go")) {
      // [ponder]
    } else if (prefix(controller_message, "stop")) {
       // 
    } else if (prefix(controller_message, "quit")) {
      exit(EXIT_SUCCESS);
    } else {
      exit(EXIT_FAILURE);
    }
  }
}
