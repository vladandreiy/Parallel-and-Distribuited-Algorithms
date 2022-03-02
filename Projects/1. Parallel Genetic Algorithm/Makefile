# APD - Tema 1
# Octombrie 2021
all: build
build:
	@echo "Building..."
	@gcc -o tema1_par tema1_par.c genetic_algorithm.c -lm -Wall -O0 -g3 -DDEBUG -lpthread
	@echo "Done"

build_debug:
	@echo "Building debug..."
	@gcc -o tema1_par tema1_par.c genetic_algorithm.c -lm -Wall -O0 -g3 -DDEBUG -lpthread
	@echo "Done"

run: build
	@./tema1_par

run_debug: build_debug
	@./tema1_par inputs/in0 10 1

clean:
	@echo "Cleaning..."
	@rm -rf tema1_par
	@echo "Done"
