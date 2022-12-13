#include "auto-wrap.h"
#include "text.h"
#include "text-buffer.h"
#include "text-diff.h"
#include "marker-index.h"
#include <emscripten/bind.h>
#include <sstream>
#include <iomanip>

using std::string;
using std::u16string;

static TextBuffer *construct(const std::wstring &text) {
  return new TextBuffer(u16string(text.begin(), text.end()));
}

static emscripten::val find_sync(TextBuffer &buffer, std::wstring js_pattern, bool ignore_case, bool unicode, Range range) {
  u16string pattern(js_pattern.begin(), js_pattern.end());
  u16string error_message;
  Regex regex(pattern, &error_message, ignore_case, unicode);
  if (!error_message.empty()) {
    return emscripten::val(string(error_message.begin(), error_message.end()));
  }

  auto result = buffer.find(regex, range);
  if (result) {
    return emscripten::val(*result);
  }

  return emscripten::val::null();
}

static emscripten::val find_all_sync(TextBuffer &buffer, std::wstring js_pattern, bool ignore_case, bool unicode, Range range) {
  u16string pattern(js_pattern.begin(), js_pattern.end());
  u16string error_message;
  Regex regex(pattern, &error_message, ignore_case, unicode);
  if (!error_message.empty()) {
    return emscripten::val(string(error_message.begin(), error_message.end()));
  }

  return em_transmit(buffer.find_all(regex, range));
}

static emscripten::val find_and_mark_all_sync(TextBuffer &buffer, MarkerIndex &index, unsigned next_id,
                                              bool exclusive, std::wstring js_pattern, bool ignore_case, bool unicode,
                                              Range range) {
  u16string pattern(js_pattern.begin(), js_pattern.end());
  u16string error_message;
  Regex regex(pattern, &error_message, ignore_case, unicode);
  if (!error_message.empty()) {
    return emscripten::val(string(error_message.begin(), error_message.end()));
  }

  return emscripten::val(buffer.find_and_mark_all(index, next_id, exclusive, regex, range));
}

static emscripten::val base_text_digest(TextBuffer &buffer) {
  std::stringstream stream;
  stream <<
    std::setfill('0') <<
    std::setw(16) <<
    std::hex <<
    buffer.base_text().digest();
  return emscripten::val(stream.str());
}

static emscripten::val serialize_changes(TextBuffer &text_buffer) {
  static std::vector<uint8_t> output;
  output.clear();
  Serializer serializer(output);
  text_buffer.serialize_changes(serializer);
  return emscripten::val(emscripten::typed_memory_view(output.size(), output.data()));
}

static emscripten::val deserialize_changes(TextBuffer &text_buffer, std::string data) {
  static std::vector<uint8_t> input;
  input.assign(data.c_str(), data.c_str() + data.size());
  Deserializer deserializer(input);
  text_buffer.deserialize_changes(deserializer);
  return emscripten::val::null();
}

static emscripten::val line_ending_for_row(TextBuffer &buffer, uint32_t row) {
  auto line_ending = buffer.line_ending_for_row(row);
  if (line_ending) {
    string result;
    for (const uint16_t *character = line_ending; *character != 0; character++) {
      result += (char)*character;
    }
    return emscripten::val(result);
  }
  return emscripten::val::undefined();
}

static uint32_t character_index_for_position(TextBuffer &buffer, Point position) {
  return buffer.clip_position(position).offset;
}

static uint32_t get_line_count(TextBuffer &buffer) {
  return buffer.extent().row + 1;
}

static Point position_for_character_index(TextBuffer &buffer, long index) {
  return index < 0 ?
    Point{0, 0} :
    buffer.position_for_offset(static_cast<uint32_t>(index));
}

static Patch load_from_text(TextBuffer &buffer, std::u16string file_contents) {
  auto snapshot = buffer.create_snapshot();
  auto contents = new Text(file_contents);
  auto patch = text_diff(snapshot->base_text(), file_contents);

  Patch inverted_changes = buffer.get_inverted_changes(snapshot);
  delete snapshot;

  if (inverted_changes.get_change_count() > 0) {
    inverted_changes.combine(patch);
    patch = std::move(inverted_changes);
  }

  bool has_changed = patch.get_change_count() > 0;;
  if (has_changed) {
    buffer.reset(file_contents);
  } else {
    buffer.flush_changes();
  }
  return patch;
}

EMSCRIPTEN_BINDINGS(TextBuffer) {
  emscripten::class_<TextBuffer>("TextBuffer")
    .constructor<>()
    .constructor(construct, emscripten::allow_raw_pointers())
    .function("getText", WRAP(&TextBuffer::text))
    .function("setText", WRAP_OVERLOAD(&TextBuffer::set_text, void (TextBuffer::*)(u16string &&)))
    .function("getCharacterAtPosition", WRAP(&TextBuffer::character_at))
    .function("getTextInRange", WRAP(&TextBuffer::text_in_range))
    .function("setTextInRange", WRAP_OVERLOAD(&TextBuffer::set_text_in_range, void (TextBuffer::*)(Range, u16string &&)))
    .function("getLength", &TextBuffer::size)
    .function("getExtent", &TextBuffer::extent)
    .function("getLineCount", get_line_count)
    .function("hasAstral", &TextBuffer::has_astral)
    .function("reset", WRAP(&TextBuffer::reset))
    .function("lineLengthForRow", WRAP(&TextBuffer::line_length_for_row))
    .function("lineEndingForRow", line_ending_for_row)
    .function("lineForRow", WRAP(&TextBuffer::line_for_row))
    .function("characterIndexForPosition", character_index_for_position)
    .function("positionForCharacterIndex", position_for_character_index)
    .function("isModified", WRAP_OVERLOAD(&TextBuffer::is_modified, bool (TextBuffer::*)() const))
    .function("findSync", find_sync)
    .function("findAllSync", find_all_sync)
    .function("findAndMarkAllSync", find_and_mark_all_sync)
    .function("baseTextDigest", base_text_digest)
    .function("serializeChanges", serialize_changes)
    .function("deserializeChanges", deserialize_changes)
    .function("loadFromText", load_from_text)
    .function("findWordsWithSubsequenceInRange", WRAP(&TextBuffer::find_words_with_subsequence_in_range));

  emscripten::value_object<TextBuffer::SubsequenceMatch>("SubsequenceMatch")
    .field("word", WRAP_FIELD(TextBuffer::SubsequenceMatch, word))
    .field("positions", WRAP_FIELD(TextBuffer::SubsequenceMatch, positions))
    .field("matchIndices", WRAP_FIELD(TextBuffer::SubsequenceMatch, match_indices))
    .field("score", WRAP_FIELD(TextBuffer::SubsequenceMatch, score));
}
