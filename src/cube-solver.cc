#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <set>
#include <utility>
#include <stdexcept>
#include <algorithm>
#include "cube-solver.h"
using namespace std;

enum Move : char {
	F, R, U, B, L, D,
	FP, RP, UP, BP, LP, DP,
	F2, R2, U2, B2, L2, D2
};

using Word = vector<Move>;

std::ostream& operator<<(std::ostream& out, const Word &word) {
	copy(word.begin(), word.end(), ostream_iterator<int>(out, " "));
	return out;
}

struct PermPair {
	Permutation perm;
	Word word;
	auto operator<=>(const PermPair &other) const = default;
};

// compare the perm fields of two PermPairs 
bool permsAreEqual(const PermPair &lhs, const PermPair &rhs) {
	return lhs.perm == rhs.perm;
}

std::ostream& operator<<(std::ostream& out, const PermPair &pair) {
	out << pair.perm << endl << pair.word;
	return out;
}

// identity permutation
Permutation id { {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48} };

// stickers are numbered from top-left to bottom-right, skipping
// center stickers, on each face, in row-major order, with the 
// faces being ordered as top, front, left, back, right, down.
const vector<PermPair> singleMoves {
	{{{1,2,3,4,5,24,21,19,14,12,9,15,10,16,13,11,17,18,41,20,42,22,23,43,25,26,27,28,29,30,31,32,6,34,35,7,37,8,39,40,38,36,33,44,45,46,47,48}},
		{F}},
	{{{1,2,11,4,13,6,7,16,9,10,43,12,45,14,15,48,17,18,19,20,21,22,23,24,8,26,27,5,29,3,31,32,38,36,33,39,34,40,37,35,41,42,30,44,28,46,47,25}},
		{R}},
	{{{6,4,1,7,2,8,5,3,33,34,35,12,13,14,15,16,9,10,11,20,21,22,23,24,17,18,19,28,29,30,31,32,25,26,27,36,37,38,39,40,41,42,43,44,45,46,47,48}},
		{U}},
	{{{35,37,40,4,5,6,7,8,9,10,11,12,13,14,15,16,3,18,19,2,21,1,23,24,30,28,25,31,26,32,29,27,33,34,48,36,47,38,39,46,41,42,43,44,45,17,20,22}},
		{B}},
	{{{32,2,3,29,5,27,7,8,1,10,11,4,13,6,15,16,22,20,17,23,18,24,21,19,25,26,46,28,44,30,31,41,33,34,35,36,37,38,39,40,9,42,43,12,45,14,47,48}},
		{L}},
	{{{1,2,3,4,5,6,7,8,9,10,11,12,13,22,23,24,17,18,19,20,21,30,31,32,25,26,27,28,29,38,39,40,33,34,35,36,37,14,15,16,46,44,41,47,42,48,45,43}},
		{D}},
	{{{1,2,3,4,5,33,36,38,11,13,16,10,15,9,12,14,17,18,8,20,7,22,23,6,25,26,27,28,29,30,31,32,43,34,35,42,37,41,39,40,19,21,24,44,45,46,47,48}},
		{FP}},
	{{{1,2,30,4,28,6,7,25,9,10,3,12,5,14,15,8,17,18,19,20,21,22,23,24,48,26,27,45,29,43,31,32,35,37,40,34,39,33,36,38,41,42,11,44,13,46,47,16}},
		{RP}},
	{{{3,5,8,2,7,1,4,6,17,18,19,12,13,14,15,16,25,26,27,20,21,22,23,24,33,34,35,28,29,30,31,32,9,10,11,36,37,38,39,40,41,42,43,44,45,46,47,48}},
		{UP}},
	{{{22,20,17,4,5,6,7,8,9,10,11,12,13,14,15,16,46,18,19,47,21,48,23,24,27,29,32,26,31,25,28,30,33,34,1,36,2,38,39,3,41,42,43,44,45,40,37,35}},
		{BP}},
	{{{9,2,3,12,5,14,7,8,41,10,11,44,13,46,15,16,19,21,24,18,23,17,20,22,25,26,6,28,4,30,31,1,33,34,35,36,37,38,39,40,32,42,43,29,45,27,47,48}},
		{LP}},
	{{{1,2,3,4,5,6,7,8,9,10,11,12,13,38,39,40,17,18,19,20,21,14,15,16,25,26,27,28,29,22,23,24,33,34,35,36,37,30,31,32,43,45,48,42,47,41,44,46}},
		{DP}},
		{{{1,2,3,4,5,43,42,41,16,15,14,13,12,11,10,9,17,18,38,20,36,22,23,33,25,26,27,28,29,30,31,32,24,34,35,21,37,19,39,40,8,7,6,44,45,46,47,48}},
		{F2}},
	{{{1,2,43,4,45,6,7,48,9,10,30,12,28,14,15,25,17,18,19,20,21,22,23,24,16,26,27,13,29,11,31,32,40,39,38,37,36,35,34,33,41,42,3,44,5,46,47,8}},
		{R2}},
	{{{8,7,6,5,4,3,2,1,25,26,27,12,13,14,15,16,33,34,35,20,21,22,23,24,9,10,11,28,29,30,31,32,17,18,19,36,37,38,39,40,41,42,43,44,45,46,47,48}},
		{U2}},
	{{{48,47,46,4,5,6,7,8,9,10,11,12,13,14,15,16,40,18,19,37,21,35,23,24,32,31,30,29,28,27,26,25,33,34,22,36,20,38,39,17,41,42,43,44,45,3,2,1}},
		{B2}},
	{{{41,2,3,44,5,46,7,8,32,10,11,29,13,27,15,16,24,23,22,21,20,19,18,17,25,26,14,28,12,30,31,9,33,34,35,36,37,38,39,40,1,42,43,4,45,6,47,48}},
		{L2}},
	{{{1,2,3,4,5,6,7,8,9,10,11,12,13,30,31,32,17,18,19,20,21,38,39,40,25,26,27,28,29,14,15,16,33,34,35,36,37,22,23,24,48,47,46,45,44,43,42,41}},
		{D2}},
};

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

// C.size(): 1,888,833 WRONG
// C.size(): 621,649 CORRECT :)
// generate C^0 U ... U C^N where C = singleMoves and N = maxLength
vector<PermPair> genWords(int maxLength) {
	// generate each C^k
	vector<PermPair> C0 {{id, {}}}; // identity permutation
	vector<vector<PermPair>> C = {C0};
	for (int i = 0; i < maxLength; i++) {
		C.emplace_back(addOneMove(C.back()));
		cout << "C" << i+1 << ".size(): " << C.back().size() << '\n';
	}

	// take the union of each C^k
	vector<PermPair> ret;
	for (auto vec : C) {
		ret.insert(ret.end(), vec.begin(), vec.end());
	}
	sort(ret.begin(), ret.end());
	ret.erase(unique(ret.begin(), ret.end(), permsAreEqual), ret.end());
	cout << "C.size(): " << ret.size() << '\n';

	return ret;
}

const idx DELIMITER = 0;

string solve3By3(const Permutation &scramble) {
	ifstream cache("moves-cache", ios::binary);

	vector<PermPair> L1;

	if (cache.fail()) {
		// generate L1 and write L1 to file
		L1 = genWords(5);

		ofstream newCache("moves-cache", ios::binary);

		// alert if cache write failed
		if (newCache.fail()) {
			cout << "failed to write to cache file" << endl;
		}

		cout << "Writing to cache... ";
		for (const auto &[perm, word] : L1) {
			vector<idx> permData {perm.getPerm()};
			idx permDataBytes = permData.size() * sizeof(idx);
			newCache.write(reinterpret_cast<const char*>(&permDataBytes), sizeof(permDataBytes));
			newCache.write(reinterpret_cast<const char*>(permData.data()), permDataBytes);

			idx wordDataBytes = word.size() * sizeof(Move);
			newCache.write(reinterpret_cast<const char*>(&wordDataBytes), sizeof(wordDataBytes));
			newCache.write(reinterpret_cast<const char*>(word.data()), wordDataBytes);
		}
		cout << "DONE" << endl;

	} else {
		// read L1 from file
		cout << "Reading from cache... " << endl;

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
				throw runtime_error("failed to read permutation.");
			}

			// read word
			idx wordDataBytes;
			cache.read(reinterpret_cast<char*>(&wordDataBytes), sizeof(wordDataBytes));
			idx wordDataSize = wordDataBytes / sizeof(Move);
			vector<Move> wordData(wordDataSize, static_cast<Move>(0));
			cache.read(reinterpret_cast<char*>(wordData.data()), wordDataSize);
			if (cache.fail()) {
				throw runtime_error("failed to read word.");
			}

			Permutation perm {permData};
			Word word {wordData};
			L1.emplace_back(perm, word);
		}

		cout << "DONE" << endl;
		cout << "L1.size(): " << L1.size() << endl;
	}
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
