all: chapter_categorizer

.outputFolder:
	mkdir -p out

chapter_categorizer: .outputFolder chapter_categorizer.o
	clang++ -std=c++17 -lstdc++ -lm out/chapter_categorizer.o -Wall -Wextra -o out/chapter_categorizer
	./out/chapter_categorizer

chapter_categorizer.o: chapter_categorizer.cpp chapter_categorizer.h
	clang++ -std=c++17 -lstdc++ -lm -c chapter_categorizer.cpp -o out/chapter_categorizer.o

clean:
	rm -rf out
