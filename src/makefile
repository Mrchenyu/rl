CC = gcc
ratelimit:if_status.c tcrule.c test.c
	$(CC) $^ -o $@
clean:
	-rm *.o ratelimit -fr
