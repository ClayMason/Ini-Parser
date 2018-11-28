test: main.cpp Config.cpp Config.h
	g++ -g -Wall -Wextra -Werror main.cpp Config.cpp -o test.o -std=c++1z

clang: main.cpp Config.cpp Config.h
	clang++ -g -Wall -Wextra -Werror main.cpp Config.cpp -o test.o -std=c++1z

gcc: main.cpp Config.cpp Config.h
	gcc -g -Wall -Wextra -Werror main.cpp Config.cpp -o test.o -std=c++1z

verbose: main.cpp Config.cpp Config.h
	g++ -g -Wall -Wextra -Werror main.cpp Config.cpp -o test.o -std=c++1z -v