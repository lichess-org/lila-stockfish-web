declare module 'lila-stockfish-web' {
  interface StockfishWeb {
    postMessage(uci: string): void;

    listen: (data: string) => void; // attach listener here
    
    setNnueBuffer(data: Uint8Array, index?: number): void;
    // index argument is used to select between big and small nnue if the target is a dual nnue build
    // 0 for big, 1 for small. single nnue wasms should ignore this parameter

    getRecommendedNnue(index?: number): string; // returns a bare filename, 0 for big, 1 for small
    // index argument is 0 for big, 1 for small. single nnue wasms should ignore this parameter

    onError: (msg: string) => void; // attach error handler here
  }
  export default StockfishWeb;
}
