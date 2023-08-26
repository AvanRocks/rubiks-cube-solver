#include <iterator>
#include <map>
#include <stdexcept>
#include <algorithm>
#include "cube-state.h"
using namespace std;

// compare the perm fields of two PermPairs 
bool permsAreEqual(const PermPair &lhs, const PermPair &rhs) {
	return lhs.perm == rhs.perm;
}

// PermPair output
ostream& operator<<(ostream& out, const PermPair &pair) {
	out << pair.perm << endl << pair.word;
	return out;
}

map<string, Move> stringToMove {
	{"F", F},
	{"R", R},
	{"U", U},
	{"B", B},
	{"L", L},
	{"D", D},
	{"F'", FP},
	{"R'", RP},
	{"U'", UP},
	{"B'", BP},
	{"L'", LP},
	{"D'", DP},
	{"F2", F2},
	{"R2", R2},
	{"U2", U2},
	{"B2", B2},
	{"L2", L2},
	{"D2", D2}
};

map<Move, string> moveToString {
	{F, "F"},
	{R, "R"}, 
	{U, "U"}, 
	{B, "B"}, 
	{L, "L"}, 
	{D, "D"}, 
	{FP, "F'"},
	{RP, "R'"},
	{UP, "U'"},
	{BP, "B'"},
	{LP, "L'"},
	{DP, "D'"},
	{F2, "F2"},
	{R2, "R2"},
	{U2, "U2"},
	{B2, "B2"},
	{L2, "L2"},
	{D2, "D2"}
};

Permutation getPermutation(Move move) {
	for (const auto &pair : singleMoves) {
		if (pair.word[0] == move) return pair.perm;
	}
	throw invalid_argument("Unrecognized move provided.");
}

Permutation getPermutation(Word word) {
	if (!word.size()) {
		throw invalid_argument("Expected non-empty word.");
	}

	Permutation perm {getPermutation(word[0])};
	for (size_t i = 1; i < word.size(); i++) {
		perm = getPermutation(word[i]) * perm;
	}

	return perm;
}

PermPair moveListToPermPair(vector<string> moves) {
	Word word;
	for (string s : moves) {
		if (!stringToMove.count(s)) {
			throw runtime_error("Illegal move list provided.");
		}
		word.emplace_back(stringToMove[s]);
	}
	
	if (!word.size()) {
		throw invalid_argument("Expected non-empty move list.");
	}
	Permutation perm {getPermutation(word)};
	
	return {perm, word};
}

// Word output
ostream& operator<<(ostream& out, const Word &word) {
	for (size_t i = 0; i < word.size(); i++) {
		const Move &move = word[i];
		if (!moveToString.count(move)) {
			throw runtime_error("Unrecognized move.");
		}
		cout << moveToString[move];
		if (i + 1 != word.size()) {
			cout << ' ';
		}
	}
	return out;
}

map<Move, Move> inverseMove {
	{F, FP},
	{R, RP}, 
	{U, UP}, 
	{B, BP}, 
	{L, LP}, 
	{D, DP}, 
	{FP, F},
	{RP, R},
	{UP, U},
	{BP, B},
	{LP, L},
	{DP, D},
	{F2, F2},
	{R2, R2},
	{U2, U2},
	{B2, B2},
	{L2, L2},
	{D2, D2}
};

Word inverse(Word word) {
	reverse(word.begin(), word.end());
	for (Move &move : word) {
		if (!inverseMove.count(move)) {
			throw runtime_error("Unrecognized move.");
		}
		move = inverseMove[move];
	}
	return word;
}
