build: 
	gcc connect4.c -o connect4

test: build 
	./connect4

.PHONY: exec 
exec: build
