#include "nemu.h"
#include <sys/types.h>
#include <regex.h>
#include <ctype.h>

/*The order is important, do not change it*/
/*TK_XXX/100 stands for priority*/
enum {
  TK_NOTYPE = 256, TK_NUM, TK_H, TK_REG, TK_LB, TK_RB,
  TK_NEG = 300, TK_DREF, 
  TK_MUL = 400, TK_DIV, 
  TK_PLUS = 500, TK_MIN, 
  TK_EQ = 600, TK_NEQ,
  TK_AND = 700
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {
  {" +", TK_NOTYPE},                // spaces
  {"\\-", TK_NEG},                  // negtive
  {"\\*", TK_DREF},                 // structure dereference
  {"\\+", TK_PLUS},                 // plus
  {"\\*", TK_MUL},                  // multiply
  {"\\-", TK_MIN},                  // minus
  {"\\/", TK_DIV},                  // divide
  {"\\$([a-z]|[A-Z])+", TK_REG},    // register
  {"==", TK_EQ},                    // equal
  {"!=", TK_NEQ},                   // not equal
  {"&&", TK_AND},                   // logic and
  {"\\0\\x([0-9]|[a-f])+", TK_H},   // hexadecimal number
  {"[0-9]+", TK_NUM},               // number
  {"\\(", TK_LB},                   // left-bracket
  {"\\)", TK_RB},                   // right-bracket
  {"\\u", TK_NOTYPE}                // unsigned integer
};
/*The order is important, do not change it*/

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )
uint32_t isa_reg_str2val(const char *s, bool *success);

static regex_t re[NR_REGEX] = {};
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

static Token tokens[65550] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;
static bool expr_errorsign = false;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
		if (rules[i].token_type==TK_NEG && nr_token>=1 && (tokens[nr_token-1].type==TK_NUM||tokens[nr_token-1].type==TK_RB))
			i = 5; 
		if (rules[i].token_type==TK_DREF && nr_token>=1 && (tokens[nr_token-1].type==TK_NUM||tokens[nr_token-1].type==TK_RB))
			i = 4; 
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
        switch (rules[i].token_type) {
			case TK_NOTYPE: break;
			case TK_EQ:   tokens[nr_token++].type = TK_EQ;    break;
			case TK_NEQ:  tokens[nr_token++].type = TK_NEQ;    break;
			case TK_PLUS: tokens[nr_token++].type = TK_PLUS;  break;
			case TK_MIN:  tokens[nr_token++].type = TK_MIN;   break;
			case TK_MUL:  tokens[nr_token++].type = TK_MUL;   break;
			case TK_DIV:  tokens[nr_token++].type = TK_DIV;   break;
			case TK_LB:   tokens[nr_token++].type = TK_LB;    break;
			case TK_RB:   tokens[nr_token++].type = TK_RB;    break;
			case TK_NEG:  tokens[nr_token++].type = TK_NEG;   break;
			case TK_DREF: tokens[nr_token++].type = TK_DREF;   break;
			case TK_AND:  tokens[nr_token++].type = TK_AND;   break;
			case TK_REG: {
				bool REG_suc = 1;
				strncpy(tokens[nr_token].str, substr_start, substr_len);
				tokens[nr_token].str[substr_len] = '\0';
				for (int i = 1; i <= strlen(tokens[nr_token].str+1); ++i) 
					tokens[nr_token].str[i] = tolower(tokens[nr_token].str[i]);
				uint32_t reg_res = isa_reg_str2val(tokens[nr_token].str+1, &REG_suc);
				if (REG_suc==0) {
					printf("No such register: %s\n", tokens[nr_token].str);
					return false;
				}
				sprintf(tokens[nr_token].str, "%u", reg_res);
				tokens[nr_token++].type = TK_REG;
				break;
			}
			case TK_H: {
				strncpy(tokens[nr_token].str, substr_start, substr_len);
				tokens[nr_token].str[substr_len] = '\0';
				uint32_t hnum;
				sscanf(tokens[nr_token].str+2, "%x", &hnum);	
				sprintf(tokens[nr_token].str, "%u", hnum);
				tokens[nr_token++].type = TK_H;
			}
			case TK_NUM: {
				if (substr_len>32) {
				  printf("Overflow: given number is too big\n");
				  return false;
				}
				strncpy(tokens[nr_token].str, substr_start, substr_len);
				tokens[nr_token].str[substr_len] = '\0';
				tokens[nr_token++].type = TK_NUM;
				break;
			}
            default: {
				return false;
			}
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

bool check_parentheses(int m, int n) {
  if (tokens[m].type != TK_LB || tokens[n].type != TK_RB)
	return false;
  int bracket_cnt = 0;
  bool check_sign = 0;
  for (int i = m; i <= n; ++i) {
	switch (tokens[i].type) {
	  case TK_LB: ++bracket_cnt; break;
	  case TK_RB: --bracket_cnt; break;
	  default: break;
	}
	if (bracket_cnt < 0) {
	  printf("Illegal expression: brackets are not matched\n");
	  expr_errorsign = true;
	  return false;
	}
	if (bracket_cnt == 0&&i < n) check_sign = 1;
  } 
  if (bracket_cnt) {
	  printf("Illegal expression: brackets are not matched\n");
	  expr_errorsign = true;
	  return 0;	
  }
  if (check_sign) return 0;
  else return 1;
}

int main_optr(int m, int n){
  int bracket_cnt = 0, crt_optr = TK_NOTYPE, crt_opnum = 0;
  bool is_want;
  for (int i = m; i <= n; ++i) {
	is_want = 0;
	switch (tokens[i].type) {
	  case TK_LB: ++bracket_cnt; break;
	  case TK_RB: --bracket_cnt; break;
	  case TK_PLUS: case TK_MIN: case TK_MUL: case TK_DIV: case TK_NEG: is_want = 1; break;
	  default: break;
	}
   	if (crt_optr != TK_NEG) {
      if (bracket_cnt ==  0 && is_want && (tokens[i].type>=crt_optr || tokens[i].type/100==crt_optr/100)) {
	    crt_optr = tokens[i].type;
	    crt_opnum = i;
      }
	}
	else {
      if (bracket_cnt ==  0 && is_want && tokens[i].type>crt_optr) {
	    crt_optr = tokens[i].type;
	    crt_opnum = i;
	  }
	}
  }	
  return crt_opnum;
}

uint32_t eval(int p, int q) {
  if (expr_errorsign) return 0;
  if (p > q) {
	if (tokens[p].type == TK_NEG||tokens[p].type == TK_DREF) return 0;
	printf("illegal expression\n");
	expr_errorsign = true;
	return 0;
  }
  else if (p == q) {
    if (tokens[p].type != TK_NUM&&tokens[p].type != TK_H&&tokens[p].type != TK_REG) {
	  printf("Illegal expression: tokens[%d] is not a number\n", p);
      expr_errorsign = true;
	  return 0;	  
	}
	int tk_num;
	sscanf(tokens[p].str, "%d", &tk_num);
	return tk_num;
  }
  else if (check_parentheses(p, q) == true)
    return eval(p + 1, q - 1);
  else {
    int op = main_optr(p, q);
    uint32_t val1 = eval(p, op - 1);
    uint32_t val2 = eval(op + 1, q);
	if (expr_errorsign) return 0;
    switch (tokens[op].type) {
      case TK_PLUS: return val1 + val2;
	  case TK_MIN: case TK_NEG: return val1 - val2; 
      case TK_MUL: return val1 * val2;
	  case TK_EQ: return val1 == val2;
	  case TK_NEQ: return val1 != val2;
	  case TK_AND: return val1 && val2;
	  case TK_DREF: return isa_vaddr_read(val2, 4);
      case TK_DIV: if (val2==0) {
				     printf("Overflow: some divisor is zero\n");
					 expr_errorsign = true;
					 return 0;
				   }
				   return val1 / val2;
      default: {
	    expr_errorsign = true;
		printf("Illegal expression: tokens[%d] is not a valid operator\n", op);
	    return 0;	
      }
    }
  }
}

uint32_t expr(char *e, bool *success) {
  expr_errorsign = false;
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  uint32_t expr_ans = eval(0, nr_token-1);
  if (expr_ans==0 && expr_errorsign) {
	*success = false;
	return 0;
  }
  return expr_ans;
}
