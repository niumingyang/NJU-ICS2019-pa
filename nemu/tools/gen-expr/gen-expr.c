#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// this should be enough
static char buf[65400];
static int crt_loc = 0;
static bool of_sign = 0;
uint32_t choose (uint32_t n) {
	return rand()%n;
}
static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static int expr_test(int location) {
  sprintf(code_buf, code_format, buf+location);
  FILE *fp = fopen("/tmp/.code.c", "w");
  assert(fp != NULL);
  fputs(code_buf, fp);
  fclose(fp); 
  int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
  if (ret != 0) return 0;
  fp = popen("/tmp/.expr", "r");
  assert(fp != NULL);
  int result;
  fscanf(fp, "%d", &result);
  pclose(fp);
  return result;
}	

static void gen_num(){
  bool num_sign = 1;
  if (crt_loc>=4) {
    for (int i = crt_loc; i >= crt_loc-4; i--) {
	  if (!isdigit(buf[i])) {
	    num_sign = 0;
	    break;
	  }
    }
  if (num_sign) return;
  }
  sprintf(buf+crt_loc, "%d", choose(1<<20));
  crt_loc += strlen(buf+crt_loc);
}

static void gen(char gen_arg){
  if (choose(2)) gen(' '); 
  *(buf+crt_loc) = gen_arg;
  *(buf+crt_loc+1) = '\0';
  crt_loc++;
}

static void gen_rand_op(){
  if (choose(2)) gen(' ');
  switch(choose(4)) {
	case 0: strcpy(buf+crt_loc, "+\0"); crt_loc++; break;
	case 1: strcpy(buf+crt_loc, "-\0"); crt_loc++; break;
	case 2: strcpy(buf+crt_loc, "*\0"); crt_loc++; break;
	case 3: strcpy(buf+crt_loc, "/\0"); crt_loc++; break;
	default: assert(0);
  }
  if (choose(2)) gen(' ');
}

static inline void gen_rand_expr() {
  if (crt_loc>=65350||of_sign) return;
  switch (choose(3)) {
    case 0: gen_num(); break;
    case 1: gen('('); gen_rand_expr(); if (of_sign) return; gen(')'); break;
    default: gen_rand_expr(); 
			 if (of_sign) return;
			 gen_rand_op();
			 int ptr_now = crt_loc;
			 gen('('); gen_rand_expr(); gen(')');
			 if (buf[ptr_now-1]=='/'&&expr_test(ptr_now)==0)
				 of_sign = 1;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
	crt_loc = 0;
	buf[0] = '\0';
    gen_rand_expr();
	if (of_sign) {
	  i--;
	  of_sign = 0;
	  continue;
	}

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
