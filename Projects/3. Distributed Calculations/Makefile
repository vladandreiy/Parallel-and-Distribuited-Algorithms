P=9
N=12

build: tema3

tema3: tema3.cpp
	mpiCC tema3.cpp -o tema3 -Wall

build_debug: tema3.c
	mpiCC tema3.cpp -o tema3 -lm -Wall -g

run: 
	mpirun --oversubscribe -np $P ./tema3 $N 0

clean:
	rm -rf tema3
