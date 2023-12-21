#ifndef CUBE_SOLVER_H
#define CUBE_SOLVER_H

#include <string>
#include <vector>
#include "permutation.h"
#include "cube-state.h"

std::vector<Word> solve3By3s(const std::vector<Permutation> &scrambles, int maxSolSize, bool quiet = false);

Word solve3By3(const Permutation &scramble, int maxSolSize, bool quiet = false);

#endif
