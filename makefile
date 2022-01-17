main.o : 
	gcc main.c -lwiringPi

clean : 
	rm -f *.o *.out
