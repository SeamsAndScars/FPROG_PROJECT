all: chapter_categorizer 

.outputFolder:
	mkdir -p out
	
chapter_categorizer: .outputFolder
	clang++ -std=c++17 -lstdc++ -lm chapter_categorizer.cpp chapter_categorizer.h -Wall -Wextra  out/chapter_categorizer
	./out/chapter_categorizer

