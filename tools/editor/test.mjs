
const tests = [];

function test(name, run) {
  tests.push([name, run]);
}

test.run = function() {
  let total = tests.length, failures = 0;

  console.log('');

  for (let [name, run] of tests) {
    try {
      run();
      console.log('[PASS] ' + name);
    } catch (error) {
      console.log('[FAIL] ' + name, error);
      failures++;
    }
  }

  console.log('');
  console.log(`${total} test(s), ${failures} failure(s)`);
};

function assert(expected, actual, message) {
  if (expected != actual) {
    throw new Error(message);
  }
}

global.test = test;
global.assert = assert;
