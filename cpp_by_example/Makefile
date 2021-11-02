%.o: %.cpp
	@clang++ -std=c++11 -Wall $(@:.o=.cpp) -o $@

example1: example1.o
	@./example1.o
example2: example2.o
	@./example2.o
example3: example3.o
	@./example3.o
example4: example4.o
	@./example4.o
example5: example5.o
	@./example5.o
example6: example6.o
	@./example6.o
example7: example7.o
	@./example7.o
example8: example8.o
	@./example8.o
example9: example9.o
	@./example9.o

clean:
	@rm example*.o

@PHONY: example1 example2 example3 example4 example5 example6 example7 example8 example9 clean
