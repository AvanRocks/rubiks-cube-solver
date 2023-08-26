#include "product-stream.h"
using namespace std;

ProductStream::ProductStream(const vector<PermPair> &A,
														 PermutationTrie &&Btmp)
	: A{A}, B{move(Btmp)} 
{
	// init set
	// for each a in A, add a * B.min(a) to the queue
	for (const auto &pairA : A) {
		const PermPair *pairPtrB = B.min(pairA.perm);
		if (pairPtrB) {
			const PermPair &pairB = *pairPtrB;
			Word abWord {pairB.word};
			abWord.insert(abWord.end(), pairA.word.begin(), pairA.word.end());
			const PermPair ab {pairA.perm * pairB.perm, abWord};
			set.insert({ab, pairA, pairB});
		}
	}
}

PermPair *ProductStream::getNext() {
	if (set.empty()) {
		return nullptr;
	} else {
		PermPair a,b,ab;
		tie(ab,a,b) = *set.begin();
		set.erase(set.begin());

		const PermPair *nextBPtr = B.next(a.perm, b.perm);
		if (nextBPtr) {
			const PermPair &nextB = *nextBPtr;
			Word newABWord {nextB.word};
			newABWord.insert(newABWord.end(), a.word.begin(), a.word.end());
			const PermPair newAB {a.perm * nextB.perm, newABWord};
			set.insert({newAB, a, nextB});
		}

		return new PermPair{ab};
	}
}
