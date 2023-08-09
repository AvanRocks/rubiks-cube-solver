#include <iostream>
#include <fstream>
#include "cube-solver.h"
using namespace std;

int main() {
	solve3By3({{}});

	/*
	ofstream testWrite("test-file", ios::binary);

	int x = 7;
	testWrite.write(reinterpret_cast<const char*>(&x), sizeof(x));

	vector<int> v {1, 2};
	testWrite.write(reinterpret_cast<const char*>(v.data()), v.size() * sizeof(int));
	testWrite.close();

	ifstream testRead("test-file", ios::binary);
	int y;
	//y = testRead.get();
	testRead.read(reinterpret_cast<char*>(&y), sizeof(y));
	vector<int> w (v.size(), 0);
	testRead.read(reinterpret_cast<char*>(w.data()), w.size() * sizeof(int));

	cout << y << endl;
	for (auto e : w) {
		cout << e << ' ';
	}
	cout << endl;
	*/

	return 0;
}
