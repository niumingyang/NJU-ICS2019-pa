include nemu/Makefile.git

default:
	@echo "Please run 'make' under any subprojects to compile."
 
clean:
	-$(MAKE) -C nemu clean
	-$(MAKE) -C nexus-am clean
	-$(MAKE) -C nanos-lite clean
	-$(MAKE) -C navy-apps clean

submit: clean
	git gc
	STUID=$(STUID) STUNAME=$(STUNAME) bash -c "$$(curl -s http://ics.nju.edu.cn/people/yanyanjiang/teach/submit.sh)"

count:
	find /home/nmy/ics2019/nemu/ -name "*[.h|.c]" |xargs cat|wc -l
	find /home/nmy/ics2019/nemu/ -name "*[.h|.c]" |xargs cat|grep -v ^$$|wc -l

.PHONY: default clean submit
