#ifndef PERM_TRIE_H
#define PERM_TRIE_H

#include <memory>
#include <utility>
#include "permutation.h"
#include "cube-state.h"

/*
 * Stores a trie of permutations.
 * - All permutations must be of the same length.
 * - To save memory, long branches of the trie are compressed into a single leaf node.
 * - That is, if you are at a node which only leads to one permutation, 
 *   then instead of storing that entire branch, only a single node representing 
 *   that branch is stored.
 * - Thus, every node except the head node is either a leaf node or stores more 
 *   than one permutation.
 */

class PermutationTrie {
	struct Node {
		bool isLeaf;
		std::unique_ptr<PermPair> permPair;
		std::vector<std::pair<idx,std::unique_ptr<Node>>> children;
	};

	std::unique_ptr<Node> head;

	// returns the permutation r in the subtrie starting at start such that s * r 
	// is lexicographically least
	const PermPair *min(const Permutation &s, Node *start) const;

	// helper function for printInfo
	void dfs(PermutationTrie::Node *p, int depth, int &numNodesSaved, int &numPointerBytesSaved) const;
public:
	PermutationTrie();
	PermutationTrie(const std::vector<PermPair> &permPairs);
	void insert(const PermPair &permPair);

	// returns the permutation r in the trie such that s * r is lexicographically least
	const PermPair *min(const Permutation &s) const;

	// returns the permutation r' in the trie such that s * r' immediately 
	// succeeds s * r in lexicographical order
	const PermPair *next(const Permutation &s, const Permutation &r) const;

	// outputs how much memory was saved by node compression and association lists
	void printInfo() const;
};

#endif
