#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display(void);
void wp_display(void);
bool wp_delete(int _no);
int wp_insert(char *wp_s, int wp_val, bool *success);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
	int args_result;
	if (args==NULL) args_result = 1;
	else sscanf(args, "%d", &args_result);
	cpu_exec(args_result);
	return 0;
}

static int cmd_info(char *args) {
	if (args==NULL) 
		printf("More subcommand needed\n");
	else if (strcmp(args, "r") == 0)
		isa_reg_display();
	else if (strcmp(args, "w") == 0)
		wp_display();
	else printf("Unkown subcommand '%s'\n", args);
	return 0;
}

static int cmd_p(char *args) {
	bool succ = true;
	if (args==NULL) {
		printf("More subcommand needed\n");
	}
	else {
		int  cmd_p_ans = expr(args, &succ);
		if (succ) printf("%d\n", cmd_p_ans);	
	}
	return 0;	
}

static int cmd_x(char *args) {
	char *arg = strtok(NULL, " ");
	if (arg==NULL) { 
		printf("More subcommand needed\n");
		return 0;
	}
	char *arg_sec = strtok(NULL, " ");
	if (arg_sec==NULL) { 
		printf("More subcommand needed\n");
		return 0;
	}
	int arg_number, arg_addr;
	bool succ = true;
   	sscanf(arg, "%d", &arg_number);
	arg_addr = expr(arg_sec, &succ);
	if (!succ) return 0;
	for (int i = 0; i < arg_number; ++i) {
		printf("0x%08x: 0x%08x %d\n", arg_addr+i*4, isa_vaddr_read(arg_addr+i*4, 4), isa_vaddr_read(arg_addr+i*4, 4));
	}
	return 0;
}

static int cmd_w(char *args) {
	char *arg = strtok(NULL, " ");
	if (arg==NULL) {
		printf("More subcommand needed\n");
		return 0;
	}
	bool succ = true;
	int cmd_w_ans = expr(arg, &succ);
	if (!succ) return 0;
	succ = 1;
	int w_no = wp_insert(arg, cmd_w_ans, &succ);
	if (!succ)
		printf("No enough space\n");
	else printf("Watchpoint %d: %s\n", w_no, arg);
	return 0;
}

static int cmd_d(char *args) {
	char *arg = strtok(NULL, " ");
	if (arg==NULL) { 
		printf("More subcommand needed\n");
		return 0;
	}
	int cmd_d_num;
   	sscanf(arg, "%d", &cmd_d_num);
	if (wp_delete(cmd_d_num))
		printf("No such watchpoint: %d\n", cmd_d_num);
	return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Executing code given numbers of line a time", cmd_si},
  { "info", "print the status of registers and watchpoint information", cmd_info},
  { "p", "print the value of given expression", cmd_p},
  { "x", "print the value in continious", cmd_x},
  { "w", "set the watchpoint", cmd_w},
  { "d", "delete the watchpoint", cmd_d},
  /* TODO: Add more commands */
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }
    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
