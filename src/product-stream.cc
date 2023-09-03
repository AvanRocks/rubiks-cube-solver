#include "product-stream.h"
#include "stopwatch.h"
using namespace std;

ProductStream::ProductStream(const vector<PermPair> &A,
														 PermutationTrie &&Btmp)
	: A{A}, B{move(Btmp)} 
{
	// init priority queue
	// for each a in A, add a * B.min(a) to the queue

	for (const auto &pairA : A) {
		const PermPair *pairPtrB = B.min(pairA.perm);
		if (pairPtrB) {
			const PermPair &pairB = *pairPtrB;
			Word abWord {pairB.word};
			abWord.insert(abWord.end(), pairA.word.begin(), pairA.word.end());
			const PermPair ab {pairA.perm * pairB.perm, abWord};
			queue.emplace(ab, pairA, pairB);
		} else {
			throw runtime_error("sus");
		}
	}
}

PermPair *ProductStream::getNext() {
	if (queue.empty()) {
		return nullptr;
	} else {
		PermPair a,b,ab;
		tie(ab,a,b) = queue.top();
		queue.pop();

		const PermPair *nextBPtr = B.next(a.perm, b.perm);
		if (nextBPtr) {
			const PermPair &nextB = *nextBPtr;
			Word newABWord {nextB.word};
			newABWord.insert(newABWord.end(), a.word.begin(), a.word.end());
			const PermPair newAB {a.perm * nextB.perm, newABWord};
			queue.emplace(newAB, a, nextB);
		}

		return new PermPair{ab};
	}
}
