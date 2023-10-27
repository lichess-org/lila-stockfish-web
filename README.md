## lila-stockfish-web
Multiple stockfish wasms for use in lichess.org web analysis

## Building
```
./build.py --flags='-O0 -g3 -sSAFE_HEAP' <target(s)>
```
Include `--node` for nodejs instead of web. `--flags` for em++ compile and link. Available
targets are `clean`, `all`, `sf-nnue-40`, `sf-nnue-60`, `linrock-nnue-7`, `linrock-nnue-12`,
and `fsf-hce`.

`./build.py` downloads all stockfish sources to the `./fishes` folder. It then applies git diffs
from the `./patches` folder. Edit the Stockfish sources freely. But to share them here, update
one of the patch files.

```
# Example: Update `sf-nnue-60.patch` with your source changes: 

  cd fishes/sf-nnue-60
  git diff > ../../patches/sf-nnue-60.patch
```

## Sources


This maps targets to their ancestor repo/commit:

### sf-nnue-60
- repo: https://github.com/official-stockfish/Stockfish
- commit: [0024133](https://github.com/official-stockfish/Stockfish/commit/0024133)
- nnue: [nn-0000000000a0.nnue](https://tests.stockfishchess.org/nns?network_name=nn-0000000000a0)

### sf-nnue-40
- repo: https://github.com/official-stockfish/Stockfish
- commit: [68e1e9b](https://github.com/official-stockfish/Stockfish/commit/68e1e9b)
- tag: sf_16
- nnue: [nn-5af11540bbfe.nnue](https://tests.stockfishchess.org/nns?network_name=nn-5af11540bbfe)

### linrock-nnue-12
- repo: https://github.com/linrock/Stockfish
- commit: [be1b8e0](https://github.com/linrock/Stockfish/commit/be1b8e0)
- nnue: [nn-4fd273888b72.nnue](https://tests.stockfishchess.org/nns?network_name=nn-ecb35f70ff2a)

### linrock-nnue-7
- repo: https://github.com/linrock/Stockfish
- commit: [c97f5cb](https://github.com/linrock/Stockfish/commit/c97f5cb)
- nnue: [nn-ecb35f70ff2a.nnue](https://tests.stockfishchess.org/nns?network_name=nn-ecb35f70ff2a)

### fsf-hce
- repo: https://github.com/fairy-stockfish/Fairy-Stockfish
- commit: [a621470](https://github.com/fairy-stockfish/Fairy-Stockfish/commit/a621470)