let binding, exps = {};

const fun = require('./browser');

const ret = fun().then(binding => {
  const {TextBuffer, Patch} = binding;
  exps.TextBuffer = TextBuffer;
  exps.Patch = Patch;
  exps.MarkerIndex = binding.MarkerIndex;
  const {findSync, findAllSync, findAndMarkAllSync, findWordsWithSubsequenceInRange, getCharacterAtPosition} = TextBuffer.prototype
  const DEFAULT_RANGE = Object.freeze({start: {row: 0, column: 0}, end: {row: Infinity, column: Infinity}})

  TextBuffer.prototype.findInRangeSync = function (pattern, range) {
    let ignoreCase = false
    let unicode = false
    if (pattern.source) {
      ignoreCase = pattern.flags.includes('i')
      unicode = pattern.unicode
      pattern = pattern.source
    }
    const result = findSync.call(this, pattern, ignoreCase, unicode, range)
    if (typeof result === 'string') {
      throw new Error(result);
    } else {
      return result
    }
  }

  TextBuffer.prototype.findSync = function (pattern, range) {
    return this.findInRangeSync(pattern, DEFAULT_RANGE)
  }

  TextBuffer.prototype.findAllInRangeSync = function (pattern, range) {
    let ignoreCase = false
    let unicode = false
    if (pattern.source) {
      ignoreCase = pattern.flags.includes('i')
      unicode = pattern.unicode
      pattern = pattern.source
    }
    const result = findAllSync.call(this, pattern, ignoreCase, unicode, range)
    if (typeof result === 'string') {
      throw new Error(result);
    } else {
      return result
    }
  }
  TextBuffer.prototype.findAllSync = function (pattern, range) {
    return this.findAllInRangeSync(pattern, DEFAULT_RANGE)
  }

  TextBuffer.prototype.findAndMarkAllInRangeSync = function (markerIndex, nextId, exclusive, pattern, range) {
    let ignoreCase = false
    let unicode = false
    if (pattern.source) {
      ignoreCase = pattern.flags.includes('i')
      unicode = pattern.unicode
      pattern = pattern.source
    }
    const result = findAndMarkAllSync.call(this, markerIndex, nextId, exclusive, pattern, ignoreCase, unicode, range)
    if (typeof result === 'string') {
      throw new Error(result);
    } else {
      return result
    }
  }

  TextBuffer.prototype.findAndMarkAllSync = function (markerIndex, nextId, exclusive, pattern) {
    return this.findAndMarkAllInRangeSync(markerIndex, nextId, exclusive, pattern, DEFAULT_RANGE)
  }

  TextBuffer.prototype.find = function (pattern) {
    return new Promise(resolve => resolve(this.findSync(pattern)))
  }

  TextBuffer.prototype.findInRange = function (pattern, range) {
    return new Promise(resolve => resolve(this.findInRangeSync(pattern, range)))
  }

  TextBuffer.prototype.findAll = function (pattern) {
    return new Promise(resolve => resolve(this.findAllSync(pattern)))
  }

  TextBuffer.prototype.findAllInRange = function (pattern, range) {
    return new Promise(resolve => resolve(this.findAllInRangeSync(pattern, range)))
  }

  TextBuffer.prototype.findWordsWithSubsequence = function (query, extraWordCharacters, maxCount) {
    const range = {start: {row: 0, column: 0}, end: this.getExtent()}
    return Promise.resolve(
      findWordsWithSubsequenceInRange.call(this, query, extraWordCharacters, range).slice(0, maxCount)
    )
  }

  TextBuffer.prototype.findWordsWithSubsequenceInRange = function (query, extraWordCharacters, maxCount, range) {
    return Promise.resolve(
      findWordsWithSubsequenceInRange.call(this, query, extraWordCharacters, range).slice(0, maxCount)
    )
  }

  TextBuffer.prototype.getCharacterAtPosition = function (position) {
    return String.fromCharCode(getCharacterAtPosition.call(this, position))
  }

  const {compose} = Patch
  const {splice} = Patch.prototype

  Patch.compose = function (patches) {
    const result = compose.call(this, patches)
    if (!result) throw new Error('Patch does not apply')
    return result
  }

  Patch.prototype.splice = Object.assign(function () {
    if (!splice.apply(this, arguments)) {
      throw new Error('Patch does not apply')
    }
  }, splice)
  return exps;
})

function normalizeEncoding(encoding) {
  return encoding.toUpperCase()
    .replace(/[^A-Z\d]/g, '')
    .replace(/^(UTF|UCS|ISO|WINDOWS|KOI8|EUC)(\w)/, '$1-$2')
    .replace(/^(ISO-8859)(\d)/, '$1-$2')
    .replace(/^(SHIFT)(\w)/, '$1_$2')
}

exps.superstring = ret
module.exports = exps;
