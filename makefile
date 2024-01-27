.outputFolder:
	mkdir -p out

chapter_categorizer: .outputFolder chapter_categorizer.cpp chapter_categorizer.h
	g++ -std=c++17 -g -lstdc++ -lm chapter_categorizer.cpp -Wall -Wextra -Werror -I/path/to/doctest -o out/chapter_categorizer
	./out/chapter_categorizer

clean:
	rm -rf out