#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <set>
#include <utility>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <cmath>
#include "cube-solver.h"
#include "cube-state.h"
#include "perm-trie.h"
#include "product-stream.h"
#include "stopwatch.h"
using namespace std;

// identity permutation
Permutation id {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48}};
int bruh;

PermPair idPair {id, {}};

// given A, generate A * C where C = singleMoves
vector<PermPair> addOneMove(const vector<PermPair> &permPairs) {
	vector<PermPair> ret;
	for (auto const &[perm1, word1] : permPairs) {
		for (auto const &[perm2, word2] : singleMoves) {
			// append word1 to word2
			Word word {word2};
			word.insert(word.end(), word1.begin(), word1.end());

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

vector<PermPair> generateOrReadL2(int size, bool quiet) {
	string cacheName = "moves-cache-" + to_string(size);
	ifstream cache(cacheName, ios::binary);

	vector<PermPair> L2;

	if (cache.fail()) {
		// generate L2 and write L2 to file

		if (!quiet) startStopwatch("Generating L2");
		L2 = genWords(size);
		if (!quiet) endStopwatch();

		ofstream newCache(cacheName, ios::binary);

		// alert if cache write failed
		if (newCache.fail()) {
			cout << "Failed to write to cache file." << endl;
		}

		if (!quiet) startStopwatch("Writing to cache");
		for (const auto &[perm, word] : L2) {
			vector<idx> permData {perm.getPerm()};
			idx permDataBytes = permData.size() * sizeof(idx);
			newCache.write(reinterpret_cast<const char*>(&permDataBytes), sizeof(permDataBytes));
			newCache.write(reinterpret_cast<const char*>(permData.data()), permDataBytes);

			idx wordDataBytes = word.size() * sizeof(Move);
			newCache.write(reinterpret_cast<const char*>(&wordDataBytes), sizeof(wordDataBytes));
			newCache.write(reinterpret_cast<const char*>(word.data()), wordDataBytes);
		}
		if (!quiet) endStopwatch();

	} else {
		// read L2 from file
		if (!quiet) startStopwatch("Reading from cache");
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
		if (!quiet) endStopwatch();
	}

	if (!quiet) cout << "L2.size(): " << L2.size() << endl;

	return L2;
}

pair<ProductStream, ProductStream>
initProductStreams(const vector<PermPair> &L2, 
									 const Permutation &scramble, 
									 bool quiet) 
{
	// Note:
	//   s = l_4 * l_3 * l_2 * l_1
	//   l_3^{-1} * l_4^{-1} * s = l_2 * l_1

	if (!quiet) startStopwatch("Creating L1 trie");
	PermutationTrie L1 {L2};
	if (!quiet) endStopwatch();

	if (!quiet) startStopwatch("Creating L2L1 stream");
	ProductStream L2L1 {L2, move(L1)};
	if (!quiet) endStopwatch();

	if (!quiet) startStopwatch("Creating L3 vector");
	vector<PermPair> L3;
	L3.reserve(L2.size());
	for (const auto &pair : L2) {
		Word reversedWord (pair.word.rbegin(), pair.word.rend());
		L3.emplace_back(pair.perm.getInverse(), reversedWord);
	}
	if (!quiet) endStopwatch();

	if (!quiet) startStopwatch("Creating L4 trie");
	PermutationTrie L4;
	for (const auto &pair : L3) {
		L4.insert({pair.perm * scramble, pair.word});
	}
	if (!quiet) endStopwatch();

	if (!quiet) startStopwatch("Creating L3L4 stream");
	ProductStream L3L4 {move(L3), move(L4)};
	if (!quiet) endStopwatch();

	return {move(L2L1), move(L3L4)};
}

Word findCommon(const Permutation &scramble, 
							 ProductStream &L2L1,
							 ProductStream &L3L4,
							 bool quiet) 
{
	const PermPair *a = L2L1.getNext();
	const PermPair *b = L3L4.getNext();

	const PermPair *prevA = nullptr;
	const PermPair *prevB = nullptr;

	if (!quiet) {
		startStopwatch("Finding common elements");
		cout << endl;
	}
	int cnt = 0;
	Word bestSolution;
	int found = 0;
	auto start = chrono::high_resolution_clock::now();

	while (a && b) {
		prevA = a; prevB = b;
		const PermPair &permPairA = *a, &permPairB = *b;
		if (permPairA.perm == permPairB.perm) {
			// found a solution

			// scrambleWord is a word equivalent to the scramble permutation
			Word scrambleWord {permPairA.word}; 
			scrambleWord.insert(scrambleWord.end(), permPairB.word.rbegin(), permPairB.word.rend());

			Word solution = inverse(scrambleWord);

			if (!found || solution.size() < bestSolution.size()) {
				bestSolution = solution;
				found++;
				if (found == 1) {
					if (!quiet) cout << "Found first solution: " << solution << endl;
				}
			}

			a = L2L1.getNext();
			b = L3L4.getNext();
			if (a->perm == prevA->perm) cout << "the same a" << endl;
			if (b->perm == prevB->perm) cout << "the same b" << endl;

			//break;
		} else if (permPairA.perm < permPairB.perm) {
			a = L2L1.getNext();
			if (a->perm == prevA->perm) cout << "the same a" << endl;
		} else {
			b = L3L4.getNext();
			if (b->perm == prevB->perm) cout << "the same b" << endl;
		}

		cnt++;
		const int NUM_PERMS = 1'000'000;
		if (cnt % NUM_PERMS == 0) {
			auto end = chrono::high_resolution_clock::now();
			double time = (chrono::duration_cast<chrono::milliseconds>(end - start).count()) / 1000.0;
			double permsPerSec = NUM_PERMS / time;
			if (!quiet) {
				cout << "Checked " << cnt << " permutations (" << permsPerSec 
						 << " permutations per sec for " << time << " seconds)" << endl;
			}
			start = chrono::high_resolution_clock::now();
		}

		if (cnt % 10'000'000 == 0) {
			if (!quiet) cout << "The best solution so far is " << bestSolution << endl;
		}

	}
	if (!quiet) endStopwatch();

	if (found) {
		return bestSolution;
	} else {
		throw invalid_argument("No solution found.");
	}
}

// uses the same L2 to save time
vector<Word> solve3By3s(const vector<Permutation> &scrambles, int maxSolSize, bool quiet) {
	// cache L2 to use for every solve
	const vector<PermPair> L2 {generateOrReadL2(ceil(maxSolSize/4.0), quiet)};

	vector<Word> solutions;
	for (const auto &scramble : scrambles) {
		auto [L2L1, L3L4] {initProductStreams(L2, scramble, quiet)};
		solutions.emplace_back(findCommon(scramble, L2L1, L3L4, quiet));
	}

	return solutions;
}

Word solve3By3(const Permutation &scramble, int maxSolSize, bool quiet) {
	vector<Word> solutions = solve3By3s({scramble}, maxSolSize, quiet);
	if (solutions.size() != 1) {
		throw runtime_error("bruh");
	} else {
		return solutions[0];
	}
}
