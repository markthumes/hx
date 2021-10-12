CFLAGS=-Wall -Wextra -ggdb
LIBS=-lX11 -lXext
INC=-I. -I..
default:
	g++ -c ../hx.cpp $(CFLAGS) $(INC) -o hx.o
	g++ -c main.cpp  $(CFLAGS) $(INC) -o main.o
	g++ main.o hx.o -o a.out $(LIBS)
