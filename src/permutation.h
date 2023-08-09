#ifndef PERMUTATION_H
#define PERMUTATION_H

#include <vector>
#include <iostream>

using idx = unsigned char;

class Permutation {
	std::vector<idx> perm;
public:
	Permutation();
	Permutation(std::vector<idx> perm);
	Permutation operator*(const Permutation &other) const;
	auto operator<=>(const Permutation &other) const = default;
	friend std::ostream& operator<<(std::ostream& out, const Permutation &perm);
	Permutation getInverse() const;
	std::vector<idx> getPerm() const;
};


#endif
