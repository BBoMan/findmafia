sumfact.o : sumfact.c
	gcc -c sumfact.c

sumfact : sumfact.o
	gcc sumfact.o -o sumfact

launch.o : launch.c
	gcc -c launch.c

launch : launch.o
	gcc launch.o -o launch

clean:
	rm -f launch launch.o sumfact sumfact.o