#include <stdio.h>

int init_monitor(int, char *[]);
void ui_mainloop(int);
void expr_test();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);
  expr_test();
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
