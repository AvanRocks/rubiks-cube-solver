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
	Permutation &operator*=(const Permutation &other);
	const idx& operator[](std::size_t idx) const;
	auto operator<=>(const Permutation &other) const = default;
	friend std::ostream& operator<<(std::ostream& out, const Permutation &perm);
	Permutation getInverse() const;
	const std::vector<idx> &getPerm() const;
	const size_t size() const;
	std::vector<idx>::const_iterator begin() const;
	std::vector<idx>::const_iterator end() const;
	static Permutation getIdentity(size_t size);
};


#endif
