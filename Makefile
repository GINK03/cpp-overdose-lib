so:
	clang++ -std=c++1z -I./include src/Open.cpp -fPIC -shared -o lib/liboverdose_open.so
	clang++ -std=c++1z -I./include src/Split.cpp -fPIC -shared -o lib/liboverdose_split.so
	clang++ -std=c++1z -I./include example.cpp -o a.out -lpthread -L./lib -Wl,-rpath ./ -loverdose_open -loverdose_split 

.PHONY: test
test:
	clang++ -std=c++1z -I./include src/Open.cpp -fPIC -shared -o lib/liboverdose_open.so
	clang++ -std=c++1z -I./include src/Split.cpp -fPIC -shared -o lib/liboverdose_split.so
	clang++ -std=c++1z -I./include test/main.cpp -o bin/test.out -lpthread -L./lib -Wl,-rpath ./ -loverdose_open -loverdose_split 
	mv lib/* /usr/lib64
	./bin/test.out

.PHONY: clean
clean:
	rm bin/*.out || echo ""
	rm lib/* || echo ""
	rm a.out || echo ""
  


