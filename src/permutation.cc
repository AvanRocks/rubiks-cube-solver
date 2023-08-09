#include "permutation.h"
#include <stdexcept>
#include <iterator>
using namespace std;

Permutation::Permutation() {}

Permutation::Permutation(vector<idx> perm) : perm{perm} {
	// check if permutation is valid
	int n = perm.size();
	vector<bool> exists(n+1, false);
	for (int i = 0; i < n; i++) {
		if (!(1 <= perm[i] && perm[i] <= n)) {
			throw invalid_argument("permutation is invalid. out of bounds element.");
		} else if (exists[perm[i]])  {
			throw invalid_argument("permutation is invalid. duplicate element: " + to_string(perm[i]) + ".");
		}
		exists[perm[i]] = true;
	}
	for (size_t i = 1; i < exists.size(); i++) {
		if (!exists[i]) {
			throw invalid_argument("permutation is invalid. missing element.");
		}
	}
}

Permutation Permutation::operator*(const Permutation &other) const {
	if (perm.size() != other.perm.size()) {
		throw invalid_argument("Cannot compose permutations of different lengths. this.size(): " + to_string(perm.size()) + ", other.perm.size(): " + to_string(other.perm.size()) + ".");
	}
	vector<idx> newPerm(perm.size());
	for (size_t i = 0; i < perm.size(); i++) {
		newPerm[i] = perm[other.perm[i]-1];
	}
	return Permutation{newPerm};
}

std::ostream& operator<<(ostream& out, const Permutation &perm) {
	copy(perm.perm.begin(), perm.perm.end(), ostream_iterator<int>(out, " "));
	return out;
}

Permutation Permutation::getInverse() const {
	vector<idx> inverse(perm.size());
	for (size_t i = 0; i < perm.size(); i++) {
		inverse[perm[i]-1] = i+1;
	}
	return {inverse};
}

std::vector<idx> Permutation::getPerm() const { return perm; }
