## lila-stockfish-web
Multiple stockfish wasms for use in lichess.org web analysis

## Building
```
# Example: Make debug builds for node with SAFE_HEAP

  ./build.py --flags='-O0 -g3 -sSAFE_HEAP' --node all
```
Omit `--node` for default web builds. omit `--flags` to use default em++ flags (-O3 -DNDEBUG --closure=1)
Available targets are `clean`, `all`, `sf16-40`, `sf16-linrock-7`, and `fsf14`.

`./build.py` downloads sources to the `./fishes` folder. It then applies diffs from the `./patches` folder. 
Finally, it builds the targets in the `./builds` folder. Edit the Stockfish sources freely. But to share
them here, you must update the patch file.

```
# Example: Update `sf16-linrock-7.patch` with your source changes: 

  cd fishes/sf16-linrock-7
  git diff > ../../patches/sf16-linrock-7.patch
```

## Sources

This maps targets to their ancestor repo/commit:

### sf16-40
- repo: https://github.com/official-stockfish/Stockfish
- commit: [68e1e9b](https://github.com/official-stockfish/Stockfish/commit/68e1e9b)
- tag: sf_16
- nnue: [nn-5af11540bbfe.nnue](https://tests.stockfishchess.org/nns?network_name=nn-5af11540bbfe)

### sf16-linrock-7
- repo: https://github.com/linrock/Stockfish
- commit: [c97f5cb](https://github.com/linrock/Stockfish/commit/c97f5cb)
- nnue: [nn-ecb35f70ff2a.nnue](https://tests.stockfishchess.org/nns?network_name=nn-ecb35f70ff2a)

### fsf14
- repo: https://github.com/fairy-stockfish/Fairy-Stockfish
- commit: [a621470](https://github.com/fairy-stockfish/Fairy-Stockfish/commit/a621470)