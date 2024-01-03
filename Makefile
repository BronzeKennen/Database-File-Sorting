sort:
	@echo " Compile sort_main ...";
	rm -f *.db
	gcc -g -I ./include/ -L ./lib/ -Wl,-rpath,./lib/ ./examples/sort_main.c ./src/record.c ./src/sort.c ./src/merge.c ./src/chunk.c -lbf -lhp_file -o ./build/sort_main -O2

chunk:
	@echo " Compile CRINGE ...";
	rm -f *.db
	gcc -g -I ./include/ -L ./lib/ -Wl,-rpath,./lib/ ./examples/test_chunk.c ./src/record.c ./src/sort.c ./src/merge.c ./src/chunk.c -lbf -lhp_file -o ./build/test -O2


run: 
	./build/sort_main