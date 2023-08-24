#include <iostream>
#include "cube-solver.h"
#include "perm-trie.h"
using namespace std;

int main() {
	//Permutation scramble = singleMoves[0].perm * singleMoves[1].perm * singleMoves[2].perm * singleMoves[3].perm;

	//PermPair scramblePair = moveListToPermPair({"F"});
	//PermPair scramblePair = moveListToPermPair({"F", "R", "U'"});
	//Permutation &scramble = scramblePair.perm;

	Permutation scramble {{43,44,41,20,47,11,28,9,24,13,17,42,36,40,37,25,6,21,1,29,7,19,10,3,35,39,22,18,34,33,31,48,16,15,30,2,23,32,26,46,8,4,27,12,45,14,5,38}};

	solve3By3(scramble);

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

	/*
	 * PermutationTrie testing
	vector<PermPair> permPairs {
		{{{1,2,3,4,5}}, {}},
		{{{1,2,3,5,4}}, {}},
		{{{1,2,4,3,5}}, {}},
		{{{1,2,5,3,4}}, {}},
		{{{1,3,4,5,2}}, {}},
		{{{2,4,1,3,5}}, {}},
		{{{4,1,3,2,5}}, {}},
		{{{4,1,3,5,2}}, {}},
		{{{5,1,2,3,4}}, {}}
	};
	PermutationTrie trie {permPairs};
	Permutation s {{3,1,4,2,5}};
	for (const PermPair *permPair = trie.min(s); permPair != nullptr; permPair = trie.next(s, permPair->perm)) {
		cout << permPair->perm << endl;
	}
	*/

	return 0;
}
