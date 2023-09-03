#ifndef PRODUCT_STREAM_H
#define PRODUCT_STREAM_H

#include "cube-state.h"
#include "perm-trie.h"
#include <vector>
#include <queue>

// A stream that gives A * B in lexicographical order, where A and B are sets of permutations
// In reality, A is a vector of PermPairs and B is a PermutationTrie
class ProductStream {
	std::vector<PermPair> A;
	PermutationTrie B;

	Permutation last;
	bool lastIsSet = false;

	using PermPair3 = std::tuple<PermPair,PermPair,PermPair>;
	std::priority_queue<PermPair3,std::vector<PermPair3>,std::greater<PermPair3>> queue;
public:
	ProductStream(const std::vector<PermPair> &A, PermutationTrie &&B);
	PermPair *getNext();
};

#endif
