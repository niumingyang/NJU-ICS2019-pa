#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp() {
  if (free_==NULL)
	return NULL;
  else {
	WP* x = free_;
	free_ = x->next;
	return x;
  }
}

void free_wp(WP* wp) {
  wp->next = free_;
  free_ = wp;
}

int wp_insert(char *wp_s, int wp_val, bool *success) {
			printf("%s\n", wp_s);//test
	WP* wpt = new_wp();
	if (wpt==NULL) {
		*success = 0;
		return 0;
	}
	wpt->expr = wp_s;
	wpt->value = wp_val;
	wpt->next = head;
			printf("%s\n", wpt->expr);//test
	head = wpt;
			printf("%s\n", head->expr);//test
	return wpt->NO;
}

bool wp_delete(int _no) {
			printf("%s %d %d\n", head->expr, head->NO, head->value);//test
	if (head==NULL) return 1;
	WP* wp_h = NULL;
	if (head->NO==_no) {
		wp_h = head;
		head = head->next;
		free_wp(wp_h);
		return 0;
	}
	WP* wp_i = head;
	while ((wp_i->next)!=NULL&&(wp_i->next->NO)!=_no)
		wp_i = wp_i->next;
	if (wp_i->next==NULL) return 1;
	wp_h = wp_i->next;
	wp_i->next = wp_i->next->next;
	free_wp(wp_h);
	return 0;
}

void wp_display() {
	if (head==NULL) {
		printf("No watchpoints\n");
		return;
	}
	WP* wp_cnt = head;
	while (wp_cnt!=NULL) {
		printf("Watchpoint No.%d: %s ***** %d\n", wp_cnt->NO, wp_cnt->expr, wp_cnt->value);
		wp_cnt = wp_cnt->next;
	}
}

bool check_wp() {
			printf("%s %d %d\n", head->expr, head->NO, head->value);//test
	bool wp_suc = 1;
	if (head!=NULL) {
		WP* wp_cnt = head;
		while (wp_cnt!=NULL) {
			int wp_v = expr(wp_cnt->expr, &wp_suc);
			assert(wp_suc==1);
			if (wp_v!=wp_cnt->value) {
				printf ("Watchpoint No.%d: '%s' ***** %d-->%d\n", wp_cnt->NO, wp_cnt->expr, wp_cnt->value, wp_v);
				wp_cnt->value = wp_v;
				wp_suc = 0;	
			}
			wp_cnt = wp_cnt->next;
		}
	}
	return wp_suc;
}
