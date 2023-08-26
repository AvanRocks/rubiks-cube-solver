#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include "cube-solver.h"
using namespace std;

static Permutation id = Permutation::getIdentity(48);
int cnt = 0;
//random_device rd;
//mt19937 rng(rd());
mt19937 rng;

// go through all scrambles in random order
void genScrambles(size_t size, Word &word) {
	if (word.size() == size) {
		Permutation expectedSolution = getPermutation(word);
		Permutation scramble = expectedSolution.getInverse();
		cout << "Scramble: " << word << endl;
		Word solutionWord = solve3By3(scramble, size, false);
		Permutation solution;
		if (solutionWord.size() == 0) {
			solution = id;
		} else {
			solution = getPermutation(solutionWord);
		}
		if (scramble * solution != id) {
			throw runtime_error("bruh");
		}
		cnt++;
		if (cnt % 1'000 == 0) {
			cout << "Checked " << cnt << " scrambles." << endl;
		}
	} else {
		vector<idx> vec(18);
		iota(vec.begin(), vec.end(), 0);
		shuffle(vec.begin(), vec.end(), rng);
		for (idx i = 0; i < 18; i++) {
			word.emplace_back((Move)vec[i]);
			genScrambles(size, word);
			word.pop_back();
		}
	}
}

void testAllScrambles(int size) {
	Word word;
	genScrambles(size, word);
}

int main() {

	testAllScrambles(20);

	/*
	Permutation scramble = moveListToPermPair({{"U2","L'","D","L","U'","L'","U2","D'","R'","U","F","L'","U'","D","F","R","F2","L2","B2","U"}}).perm.getInverse();
	solve3By3(scramble, 20);
	*/

	return 0;
}
