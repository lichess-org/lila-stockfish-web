const listeners = [];

if (!Module['listen']) Module['listen'] = data => console.log(data); // attach listener here

if (!Module['onError']) Module['onError'] = data => console.error(data); // attach handler here

Module['print'] = data => Module['listen']?.(data); // setTimeout not needed here

Module['printErr'] = data => Module['onError']?.(data);

Module['getRecommendedNnue'] = () => UTF8ToString(_getRecommendedNnue());

Module['postMessage'] = function (uci) {
  const sz = lengthBytesUTF8(uci) + 1;
  const utf8 = _malloc(sz); // deallocated in src/wasm/glue.cpp
  if (!utf8) throw new Error(`Could not allocate ${sz} bytes`);
  stringToUTF8(uci, utf8, sz);
  _uci(utf8);
};

Module['setNnueBuffer'] = function (buf /** Uint8Array */) {
  if (!buf) {
    _setNnueBuffer(0, 0);
    return;
  }
  if (buf.byteLength <= 0) throw new Error(`${buf.byteLength} bytes?`);
  const heapBuf = _malloc(buf.byteLength); // deallocated in src/wasm/glue.cpp
  if (!heapBuf) throw new Error(`Could not allocate ${buf.byteLength} bytes`);
  Module['HEAPU8'].set(buf, heapBuf);
  _setNnueBuffer(heapBuf, buf.byteLength);
};
