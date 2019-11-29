#include <common.h>
#include<assert.h>
void main_loop();
void hal_init();

int
main(void) {
	Log("game start!");

  hal_init();assert(0);
	main_loop();

	return 0;
}
