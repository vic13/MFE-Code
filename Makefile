all:
	cd src && g++ main.cpp -std=c++11 -o main.out && ./main.out

run:
	cd src && ./main.out

clean:
	cd src && rm *.out