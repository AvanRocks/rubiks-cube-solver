#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include "cube-solver.h"
using namespace std;

static Permutation id = Permutation::getIdentity(48);
int cnt = 0;
random_device rd;
mt19937 rng(rd());

// go through all scrambles in random order and test them
void genScrambles(size_t size, Word &word) {
	if (word.size() == size) {
		Permutation expectedSolution = getPermutation(word);
		Permutation scramble = expectedSolution.getInverse();
		//cout << "Scramble: " << word << endl;
		Word solutionWord = solve3By3(scramble, size, true);
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

	/*
	 * Uncomment to test all scrambles of size 4
	testAllScrambles(4);
	*/

	// Edit the move list to change the scramble.
	// Keep in mind that the longer the move list, the longer the program 
	// will take to run.
	auto scramble = moveListToPermPair({"U2", "L'", "D", "L", "U'"});

	// solutions will be output to stdout
	Permutation inverseScramble = scramble.perm.getInverse();
	solve3By3(inverseScramble, 20);

	return 0;
}
