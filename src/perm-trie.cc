#include <algorithm>
#include <utility>
#include <stdexcept>
#include "perm-trie.h"
using namespace std;

PermutationTrie::PermutationTrie() : head{make_unique<Node>(false, nullptr)} {}

PermutationTrie::PermutationTrie(const vector<PermPair> &permPairs) : PermutationTrie() {
	for (auto pair : permPairs) {
		insert(pair);
	}
}

// if inserting an already existing node, it does nothing
void PermutationTrie::insert(const PermPair &permPair) {
	const Permutation &perm = permPair.perm;

	Node *curr = head.get();
	Node *prev = nullptr;
	for (size_t i = 0; i < perm.size(); i++) {
		// find next child
		if (curr->isLeaf) {
			// we have encountered a compressed node, decompress it by one level

			if (!prev) { // can only happen if head is a leaf
				throw runtime_error("head is a leaf node.");
			}

			// decompress the current node by one level

			auto it = find_if(prev->children.begin(), prev->children.end(), 
												[i, &perm](const auto &pair) {
													return perm[i-1] == pair.first;
												});
			// now it->second is a unique_ptr that points to curr

			idx nextIdx = curr->permPair->perm[i];

			// make children array
			pair<idx, unique_ptr<Node>> nextPair {nextIdx, move(it->second)};
			vector<pair<idx,unique_ptr<Node>>> children;
			children.emplace_back(move(nextPair));

			unique_ptr<Node> inBetween = make_unique<Node>(false, nullptr, move(children));
			it->second = move(inBetween);

			curr = it->second.get();
		} // decompresion done, now continue inserting

		auto it = find_if(curr->children.begin(), curr->children.end(), 
											[i, &perm](const auto &pair) {
												return perm[i] == pair.first;
											});
		if (it == curr->children.end()) {
			// child not found
			// in this case, always insert a leaf node (we are compressing the trie)
			unique_ptr<Node> leaf = make_unique<Node>(true, 
																								make_unique<PermPair>(permPair), 
																								vector<pair<idx,unique_ptr<Node>>>() // empty vector
																							 );
			curr->children.emplace_back(perm[i], move(leaf));
			break;
		} else {
			// child found, so continue traversing the trie
			prev = curr;
			curr = it->second.get();
		}
	} 
}

const PermPair *PermutationTrie::min(const Permutation &s, Node *start) const {
	Node *curr = start;
	while (!curr->isLeaf) {
		sort(curr->children.begin(), curr->children.end(),
					[&s](const auto &a, const auto &b) {
						return s[a.first - 1] < s[b.first - 1];
					}
				);
		curr = curr->children[0].second.get();
	}
	return curr->permPair.get();
}

const PermPair *PermutationTrie::min(const Permutation &s) const {
	if (head->children.size() == 0) return nullptr;
	else return min(s, head.get());
}

const PermPair *PermutationTrie::next(const Permutation &s, const Permutation &r) const {
	// descend to the location of permutation r, and save the visited nodes 
	Node *curr = head.get();
	vector<Node*> prevs;
	for (auto i : r) {
		if (curr->isLeaf) break;
		prevs.emplace_back(curr);
		auto it = find_if(curr->children.begin(), curr->children.end(),
				[i](const auto &pair) {
					return pair.first == i;
				});
		curr = it->second.get();
	}

	// find the next node
	for (int i = (int)prevs.size() - 1; i >= 0; i--) {
		Node *prev = prevs[i];
		sort(prev->children.begin(), prev->children.end(),
					[&s](const auto &a, const auto &b) {
						return s[a.first - 1] < s[b.first - 1];
					}
				);

		auto it = upper_bound(prev->children.begin(), prev->children.end(), make_pair(r[i], nullptr),
													[&s](const auto &a, const auto &b) {
														return s[a.first - 1] < s[b.first - 1];
													}
												 );
		if (it != prev->children.end()) {
			return min(s, it->second.get());
		}
	}
	return nullptr;
}

void PermutationTrie::dfs(Node *p, int depth, int &numNodesSaved, int &numPointerBytesSaved) const {
	if (!p) return;
	if (p->isLeaf) {
		numNodesSaved += p->permPair->perm.size() - depth;
	} else {
		numPointerBytesSaved += (48 * sizeof(unique_ptr<Node>)) - (p->children.size() * sizeof(pair<idx,unique_ptr<Node>>));
		for (const auto &pair : p->children) {
			dfs(pair.second.get(), depth+1, numNodesSaved, numPointerBytesSaved);
		}
	}
}

void PermutationTrie::printInfo() const {
	int numNodesSaved = 0;
	int numPointerBytesSaved = 0;
	dfs(head.get(), 0, numNodesSaved, numPointerBytesSaved);
	cout << "Compression saved " << numNodesSaved * sizeof(Node) / (1024 * 1024) << " MB." << endl;
	cout << "Association lists saved " << numPointerBytesSaved / (1024 * 1024) << " MB." << endl;
}
