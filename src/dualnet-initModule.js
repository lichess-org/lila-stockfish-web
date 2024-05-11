const listeners = [];

if (!Module['listen']) Module['listen'] = data => console.log(data); // attach listener here

if (!Module['onError']) Module['onError'] = data => console.error(data); // attach handler here

Module['print'] = data => Module['listen']?.(data);

Module['printErr'] = data => Module['onError']?.(data);

Module['getRecommendedNnue'] = (index /** number */) => UTF8ToString(_getRecommendedNnue(index));

Module['uci'] = function (uci) {
  const sz = lengthBytesUTF8(uci) + 1;
  const utf8 = _malloc(sz); // deallocated in src/wasm/dualnet-glue.cpp
  if (!utf8) throw new Error(`Could not allocate ${sz} bytes`);
  stringToUTF8(uci, utf8, sz);
  _uci(utf8);
};

Module['setNnueBuffer'] = function (buf /** Uint8Array */, index /** number */) {
  if (!buf) throw new Error('buf is null');
  if (buf.byteLength <= 0) throw new Error(`${buf.byteLength} bytes?`);
  const heapBuf = _malloc(buf.byteLength); // deallocated in src/wasm/dualnet-glue.cpp
  if (!heapBuf) throw new Error(`could not allocate ${buf.byteLength} bytes`);
  Module['HEAPU8'].set(buf, heapBuf);
  _setNnueBuffer(heapBuf, buf.byteLength, index);
};
