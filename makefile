all: ticTacToeResult hiddenLoop ticTacToePropertyBasedTesting

.outputFolder:
	mkdir -p out
	
ticTacToeResult: .outputFolder
	clang++ -std=c++17 -lstdc++ -lm ticTacToeResult.cpp -Wall -Wextra -Werror -o out/ticTacToeResult
	./out/ticTacToeResult

hiddenLoop: .outputFolder
	clang++ -std=c++17 -lstdc++ -lm hiddenLoop.cpp -Wall -Wextra -Werror -o out/hiddenLoop
	./out/hiddenLoop

ticTacToePropertyBasedTesting: .outputFolder
	clang++ -std=c++17 -lstdc++ -lm ticTacToePropertyBasedTesting.cpp -Wall -Wextra -Werror -o out/ticTacToePropertyBasedTesting
	./out/ticTacToePropertyBasedTesting