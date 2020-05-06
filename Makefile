all:
	export CPLUS_INCLUDE_PATH=/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7 && cd src && g++ main.cpp -std=c++11 -lpython2.7 -o main.out && ./main.out

run:
	cd src && ./main.out

clean:
	cd src && rm *.out