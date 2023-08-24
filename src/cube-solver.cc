#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <set>
#include <utility>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include "cube-solver.h"
#include "cube-state.h"
#include "perm-trie.h"
#include "product-stream.h"
#include "stopwatch.h"
using namespace std;

// identity permutation
Permutation id{{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48}};

PermPair idPair {id, {}};

// given A, generate A * C where C = singleMoves
vector<PermPair> addOneMove(const vector<PermPair> &permPairs) {
	vector<PermPair> ret;
	for (auto const &[perm1, word1] : permPairs) {
		for (auto const &[perm2, word2] : singleMoves) {
			// append word1 to word2
			Word word {word2};
			for (auto const &move : word1) {
				word.emplace_back(move);
			}
			ret.emplace_back(perm1 * perm2, word);
		}
	}
	// erase duplicates
	sort(ret.begin(), ret.end());
	ret.erase(unique(ret.begin(), ret.end(), permsAreEqual), ret.end());
	return ret;
}

// generate C^0 U ... U C^N where C = singleMoves and N = maxLength
vector<PermPair> genWords(int maxLength) {
	// generate each C^k
	vector<PermPair> C0 {{id, {}}}; // identity permutation
	vector<vector<PermPair>> C = {C0};
	for (int i = 0; i < maxLength; i++) {
		C.emplace_back(addOneMove(C.back()));
		//cout << "C" << i+1 << ".size(): " << C.back().size() << endl;
	}

	// take the union of each C^k
	vector<PermPair> ret;
	for (auto vec : C) {
		ret.insert(ret.end(), vec.begin(), vec.end());
	}
	sort(ret.begin(), ret.end());
	ret.erase(unique(ret.begin(), ret.end(), permsAreEqual), ret.end());

	return ret;
}

string solve3By3(const Permutation &scramble) {
	ifstream cache("moves-cache", ios::binary);

	vector<PermPair> L2;

	if (cache.fail()) {
		// generate L2 and write L2 to file

		startStopwatch("Generating L2");

		L2 = genWords(5);
		//L2 = genWords(2);

		endStopwatch();

		ofstream newCache("moves-cache", ios::binary);

		// alert if cache write failed
		if (newCache.fail()) {
			cout << "Failed to write to cache file." << endl;
		}

		startStopwatch("Writing to cache");
		for (const auto &[perm, word] : L2) {
			vector<idx> permData {perm.getPerm()};
			idx permDataBytes = permData.size() * sizeof(idx);
			newCache.write(reinterpret_cast<const char*>(&permDataBytes), sizeof(permDataBytes));
			newCache.write(reinterpret_cast<const char*>(permData.data()), permDataBytes);

			idx wordDataBytes = word.size() * sizeof(Move);
			newCache.write(reinterpret_cast<const char*>(&wordDataBytes), sizeof(wordDataBytes));
			newCache.write(reinterpret_cast<const char*>(word.data()), wordDataBytes);
		}
		endStopwatch();

	} else {
		// read L2 from file
		startStopwatch("Reading from cache");
		while (true) {
			// read permutation
			idx permDataBytes;
			cache.read(reinterpret_cast<char*>(&permDataBytes), sizeof(permDataBytes));
			idx permDataSize = permDataBytes / sizeof(idx);
			vector<idx> permData(permDataSize, 0);
			cache.read(reinterpret_cast<char*>(permData.data()), permDataBytes);
			if (cache.eof()) {
				break;
			} else if (cache.fail()) {
				throw runtime_error("Failed to read permutation.");
			}

			// read word
			idx wordDataBytes;
			cache.read(reinterpret_cast<char*>(&wordDataBytes), sizeof(wordDataBytes));
			idx wordDataSize = wordDataBytes / sizeof(Move);
			vector<Move> wordData(wordDataSize, static_cast<Move>(0));
			cache.read(reinterpret_cast<char*>(wordData.data()), wordDataSize);
			if (cache.fail()) {
				throw runtime_error("Failed to read word.");
			}

			Permutation perm {permData};
			Word word {wordData};
			L2.emplace_back(perm, word);
		}
		endStopwatch();
	}

	cout << "L2.size(): " << L2.size() << endl;

	// Note:
	//   s = l_4 * l_3 * l_2 * l_1
	//   l_3^{-1} * l_4^{-1} * s = l_2 * l_1

	startStopwatch("Creating L1 trie");
	PermutationTrie L1 {L2};
	endStopwatch();

	startStopwatch("Creating L2L1 stream");
	ProductStream L2L1 {L2, L1};
	endStopwatch();

	startStopwatch("Creating L3 vector");
	vector<PermPair> L3;
	L3.reserve(L2.size());
	for (const auto &pair : L2) {
		Word reversedWord (pair.word.rbegin(), pair.word.rend());
		L3.emplace_back(pair.perm.getInverse(), reversedWord);
	}
	endStopwatch();

	startStopwatch("Creating L4 trie");
	PermutationTrie L4;
	for (const auto &pair : L3) {
		Word reversedWord (pair.word.rbegin(), pair.word.rend());
		L4.insert({pair.perm * scramble, reversedWord});
	}
	endStopwatch();

	startStopwatch("Creating L3L4 stream");
	ProductStream L3L4 {L3, L4};
	endStopwatch();

	const PermPair *a, *b;
	a = L2L1.getNext();
	b = L3L4.getNext();

	startStopwatch("Finding common elements");
	cout << endl;
	int cnt = 0;
	Word bestSolution;
	bool found = false;
	while (a && b) {
		const PermPair &permPairA = *a, &permPairB = *b;
		if (permPairA.perm == permPairB.perm) {
			// found a solution
			Word reversedWordB (permPairB.word.rbegin(), permPairB.word.rend());

			// scrambleWord is a word equivalent to the scramble permutation
			Word scrambleWord {permPairA.word}; 
			scrambleWord.insert(scrambleWord.end(), reversedWordB.begin(), reversedWordB.end());

			// idk
			reverse(scrambleWord.begin(), scrambleWord.end());

			//cout << scrambleWord << endl;

			Word solution = inverse(scrambleWord);
			//cout << solution << endl;

			if (!found || solution.size() < bestSolution.size()) {
				bestSolution = solution;
				found = true;
			}

			//break;
		}
		a = L2L1.getNext();
		b = L3L4.getNext();
		cnt++;
		if (cnt % 1'000'000 == 0) {
			cout << "Checked " << cnt << " permutations." << endl;
		}

		if (cnt % 10'000'000 == 0) {
			cout << "The best solution so far is " << bestSolution << endl;
		}
	}
	endStopwatch();

	/*
	PermPair *curr = L1L2.getNext();
	while (curr) {
		cout << *curr << endl;
		curr = L1L2.getNext();
	}
	*/

	//L2.printInfo();

	/*
	vector<PermPair> L2 {L1};
	vector<PermPair> L3 {L1};
	for (auto &pair : L3) {
		pair.perm = pair.perm.getInverse();
	}
	vector<PermPair> L4 {L3};
	for (auto &pair : L4) {
		pair.perm = pair.perm * scramble;
	}
	*/

	return "";
}
