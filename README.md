# Rubik's cube solver

Implements the 4-List algorithm from [this blog post](https://www.stylewarning.com/posts/brute-force-rubiks-cube/) for solving Rubik's cubes.

In princple, this algorithm can solve a more general puzzle than Rubik's cubes.
Given a set of permutations $M$ and a target permutation $s$ such $s \in M^k$ for some $k \in \mathbb{N}$, the 4-List can find permutations $m_1, ..., m_k$ such that $s = m_1 \circ \dots \circ m_k$.
Note that we assume here that all permutations are of the same length.

## Build and Run

```
make
./solve
```

The user interface for this program isn't great.
You have to edit `src/main.cc` and then compile and run to solve a different scramble.
I got bored of this project before I got around to implementing a nice interface. 
Anyway, it still works, you just gotta edit the code.

There's some comments in `main.cc` to help you test the code.
