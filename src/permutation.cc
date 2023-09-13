#include "permutation.h"
#include <stdexcept>
#include <iterator>
#include <numeric>
using namespace std;

Permutation::Permutation() {}

Permutation::Permutation(vector<idx> perm) : perm{perm} {
	// check if permutation is valid
	int n = perm.size();
	if (n != 48) {
		throw invalid_argument("permutation is not of size 48.");
	}
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
	Permutation tmp(*this);
	return tmp *= other;
}

Permutation &Permutation::operator*=(const Permutation &other) {
	/*
	if (perm.size() != other.perm.size()) {
		throw invalid_argument("Cannot compose permutations of different lengths. this.size(): " + to_string(perm.size()) + ", other.perm.size(): " + to_string(other.perm.size()) + ".");
	}
	*/
	vector<idx> newPerm(perm.size());
	for (size_t i = 0; i < perm.size(); i++) {
		newPerm[i] = perm[other.perm[i]-1];
	}
	perm = newPerm;
	return *this;
}

const idx& Permutation::operator[](std::size_t idx) const { return perm[idx]; }

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

const vector<idx> &Permutation::getPerm() const { return perm; }

const size_t Permutation::size() const { return perm.size(); }

vector<idx>::const_iterator Permutation::begin() const { return perm.begin(); }
vector<idx>::const_iterator Permutation::end() const { return perm.end(); }

Permutation Permutation::getIdentity(size_t size) {
	vector<idx> id (size);
	iota(id.begin(), id.end(), 1);
	return Permutation{id};
}
