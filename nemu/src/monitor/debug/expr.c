#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_PLUS, TK_MIN, TK_MUL, TK_DIV, TK_NUM, TK_LB, TK_RB

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},     // spaces
  {"\\+", TK_PLUS},      // plus
  {"\\*", TK_MUL},       // multiply
  {"\\-", TK_MIN},       // minus
  {"\\/", TK_DIV},       // divide
  {"==", TK_EQ},         // equal
  {"[0-9]+", TK_NUM},    // number
  {"\\(", TK_LB},        // left-bracket
  {"\\)", TK_RB}         // right-bracket
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
			case TK_NOTYPE: break;
			case TK_EQ: {
				tokens[nr_token++].type = TK_EQ;
			    break;
			}
			case TK_PLUS: {
				tokens[nr_token++].type = TK_PLUS;
				break;
			}
			case TK_MIN: {
				tokens[nr_token++].type = TK_MIN;
				break;
			}	
			case TK_MUL: {
				tokens[nr_token++].type = TK_MUL;
				break;
			}
			case TK_DIV: {
				tokens[nr_token++].type = TK_DIV;
				break;
			}
			case TK_NUM: {
				if (substr_len>32)
					assert(0);
				tokens[nr_token].type = TK_NUM;
				strncpy(tokens[nr_token].str, substr_start, substr_len);
				nr_token++;
				break;
			}
			case TK_LB: {
				tokens[nr_token++].type = TK_LB;
				break;
			}
			case TK_RB: {
				tokens[nr_token++].type = TK_RB;
				break;
			}
            default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
