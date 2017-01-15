require('segfault-handler').registerHandler()

const assert = require('assert')
const ReferenceBufferOffsetIndex = require('./reference-buffer-offset-index')
const randomSeed = require('random-seed')
const {BufferOffsetIndex} = require('../..')

describe('BufferOffsetIndex', () => {
  it('maps character indices to 2d points and viceversa', function () {
    this.timeout(Infinity)

    const generateSeed = randomSeed.create()
    for (var iteration = 0; iteration < 100; iteration++) {
      const referenceIndex = new ReferenceBufferOffsetIndex()
      const bufferIndex = new BufferOffsetIndex()
      let seed = generateSeed(Number.MAX_SAFE_INTEGER)
      const random = randomSeed.create(seed)
      for (let i = 0; i < 50; i++) {
        const startRow = random.intBetween(0, referenceIndex.lineLengths.length)
        const deletedLinesCount = random.intBetween(0, referenceIndex.getLineCount() - startRow + 1)
        const newLineLengths = []
        for (let j = 0; j < 10; j++) {
          newLineLengths.push(random.intBetween(1, 100))
        }

        referenceIndex.splice(startRow, deletedLinesCount, newLineLengths)
        bufferIndex.splice(startRow, deletedLinesCount, newLineLengths)

        for (var j = 0; j < 100; j++) {
          const index = random.intBetween(0, referenceIndex.getCharactersCount())
          const row = random(10) <= 1 ? Infinity : random.intBetween(0, referenceIndex.getLineCount() + 10)
          const column = random(10) <= 1 ? Infinity : random.intBetween(0, referenceIndex.getLongestColumn() + 10)
          const position = {row, column}
          assert.equal(bufferIndex.character_index_for_position(position), referenceIndex.character_index_for_position(position))
          assert.deepEqual(bufferIndex.position_for_character_index(index), referenceIndex.position_for_character_index(index))
        }
      }
    }
  })
})
