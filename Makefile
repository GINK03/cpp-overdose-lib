overdose.so: Split.cpp
	mkdir lib || true
	mkdir bin || true
	clang++ -std=c++1z Split.cpp -fPIC -shared -o lib/liboverdose_split.so
	clang++ -std=c++1z Open.cpp -fPIC -shared -o lib/liboverdose_open.so
	clang++ -std=c++1z main.cpp -o bin/a.out -Wl,-rpath,./lib -L./lib -loverdose_split -loverdose_open


.PHONY: clean
clean:
	rm bin/a.out lib/*.so

.PHONY: install
install:
	cp *.hpp /usr/include/
	cp lib/*.so /usr/lib
