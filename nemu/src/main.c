#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "monitor/expr.h"

int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

		/*use some examples to test expression evaluation*/
		/*FILE *pf;
		bool succ = 1;
		pf = fopen("/home/nmy/ics2019/nemu/tools/gen-expr/input", "r");
		if (pf==NULL) {
			printf("open failed\n");
			return 0;
		}
		int result = 0, cnt_error = 0;
		char test_expr[70000];
		for (int i = 0; i < 100; ++i) {
			fscanf(pf, "%d ", &result);
			fgets(test_expr, 70000, pf);
			test_expr[strlen(test_expr)-1] = '\0';
			int cmd_p_ans = expr(test_expr, &succ);
			if (result != cmd_p_ans||!succ) {
				printf("Expression %d: error\nExpected %d, get %d\nExpression: %s\n", i, result, cmd_p_ans, test_expr);
			    cnt_error++;
			}
			//else printf("Expression %d: no error\nExpected %d, get %d\n", i, result, cmd_p_ans);//some details
		}
		fclose(pf);
		if (!cnt_error) printf("Expression evaluation tests passed successfully\n");
		else printf("%d Errors\n", cnt_error);*/

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
