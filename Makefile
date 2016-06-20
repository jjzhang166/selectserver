.PHONY:all
all:select_server select_client

select_client:select_client.c
	gcc -o $@ $^
select_server:select_server.c
	gcc -o $@ $^
.PHONY:clean
clean:
	rm -f select_client select_server
