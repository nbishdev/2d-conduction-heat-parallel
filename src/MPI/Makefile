OBJS	=	arg_check.o grid.o main.o update.o
SOURCE	=	arg_check.c grid.c main.c update.c
HEADER	=	header.h
OUT		=	heat
CC		=	mpicc
FLAGS	=	-g -c
LOG		=	-mpe=mpilog
TRACE	=	-mpe=mpitrace

#	-g flag enables debugging mode
#	-c flag generates object code for separate files
#	-mpe=mpilog flag enables .clog2 logfile
#	-mpe=mpitrace flag enables MPI functions tracing

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) -lm

#	create/compile the individual files >>separately<< 
arg_check.o: arg_check.c
	$(CC) $(FLAGS) arg_check.c

grid.o: grid.c
	$(CC) $(FLAGS) grid.c

main.o: main.c
	$(CC) $(FLAGS) main.c
	
update.o: update.c
	$(CC) $(FLAGS) update.c

#	clean house
clean:
	rm -f $(OBJS) $(OUT) *.clog2 *.dat

#	do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
