#ifndef PRODUCT_STREAM_H
#define PRODUCT_STREAM_H

#include "cube-state.h"
#include "perm-trie.h"
#include <vector>
#include <set>
#include <memory>

// A stream that gives A * B in lexicographical order, where A and B are sets of permutations
// In reality, A is a vector of PermPairs and B is a PermutationTrie
class ProductStream {
	std::vector<PermPair> A;
	PermutationTrie B;

	Permutation last;
	bool lastIsSet = false;

	std::set<std::tuple<PermPair,PermPair,PermPair>, 
		// order by permutation ab
					 decltype(
					   [](const std::tuple<PermPair,PermPair,PermPair> &a, 
							  const std::tuple<PermPair,PermPair,PermPair> &b)
							{ return get<0>(a).perm < get<0>(b).perm; }
						)> set;
public:
	ProductStream(const std::vector<PermPair> &A, PermutationTrie &&B);
	PermPair *getNext();
};

#endif
