so:
	clang++ -std=c++1z -I./include src/Open.cpp -fPIC -shared -o lib/liboverdose_open.so
	clang++ -std=c++1z -I./include src/Split.cpp -fPIC -shared -o lib/liboverdose_split.so

.PHONY: test
test:
	clang++ -std=c++1z -I./include test/main.cpp -o bin/test.out -lpthread -L./lib -Wl,-rpath ./ -loverdose_open -loverdose_split 
	./bin/test.out

.PHONY: clean
clean:
	rm bin/*.out
	rm lib/*
  


