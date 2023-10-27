import { fileURLToPath } from 'node:url';
import { dirname } from 'node:path';
import { createRequire } from 'node:module';

function getCallerFile() {
  const oldPrepareStackTrace = Error.prepareStackTrace;
  Error.prepareStackTrace = (_, stack) => stack;
  const stack = new Error().stack;
  Error.prepareStackTrace = oldPrepareStackTrace;
  if (typeof stack === 'object' && stack[2]) return stack[2].getFileName();
}

globalThis.__filename = () => fileURLToPath(getCallerFile());
globalThis.__dirname = dirname;
globalThis.require = createRequire(import.meta.url);
