all:
	echo 'Please specify network to build ("make erdos" or "make barabasi")'

erdos:
	cd src && g++ ErdosRenye.cpp -std=c++11 -o erdos.out && ./erdos.out

barabasi:
	cd src && g++ BarabasiAlbert.cpp -std=c++11 -o barabasi.out && ./barabasi.out

clean:
	cd src && rm *.out