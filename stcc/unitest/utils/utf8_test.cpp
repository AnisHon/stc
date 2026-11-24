#include "stcc/utils/utf8.h"

#include <array>
#include <limits>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

namespace {

namespace utf8 = stcc::utils::utf8;
using namespace std::string_view_literals;

constexpr const char *end_of(std::string_view bytes) noexcept {
    return bytes.data() + bytes.size();
}

struct Sample {
    char32_t codepoint;
    std::string_view bytes;
};

constexpr std::array valid_samples{
    Sample{0x0000, "\0"sv},
    Sample{0x007f, "\x7f"sv},
    Sample{0x0080, "\xc2\x80"sv},
    Sample{0x07ff, "\xdf\xbf"sv},
    Sample{0x0800, "\xe0\xa0\x80"sv},
    Sample{0xd7ff, "\xed\x9f\xbf"sv},
    Sample{0xe000, "\xee\x80\x80"sv},
    Sample{0xfffd, "\xef\xbf\xbd"sv},
    Sample{0xfffe, "\xef\xbf\xbe"sv},
    Sample{0xffff, "\xef\xbf\xbf"sv},
    Sample{0x10000, "\xf0\x90\x80\x80"sv},
    Sample{0x1f600, "\xf0\x9f\x98\x80"sv},
    Sample{0x10ffff, "\xf4\x8f\xbf\xbf"sv},
};

constexpr std::array invalid_codepoints{
    char32_t{0xd800},
    char32_t{0xdbff},
    char32_t{0xdc00},
    char32_t{0xdfff},
    char32_t{0x110000},
    std::numeric_limits<char32_t>::max(),
};

static_assert(utf8::is_valid_codepoint(0x10ffff));
static_assert(!utf8::is_valid_codepoint(0xd800));
constexpr auto two_bytes = "\xc2\x80"sv;
constexpr auto replacement_bytes = "\xef\xbf\xbd"sv;
static_assert(utf8::is_boundary(end_of(two_bytes), end_of(two_bytes)));
static_assert(!utf8::is_boundary(two_bytes.data() + 1, end_of(two_bytes)));
static_assert(
    utf8::decode(replacement_bytes.data(), end_of(replacement_bytes)).status
    == utf8::DecodeStatus::Ok
);
static_assert([] {
    std::array<char, 4> output{};
    return utf8::encode(0x10000, output) == output.size()
        && std::string_view(output.data(), output.size()) == "\xf0\x90\x80\x80"sv;
}());

TEST_CASE("UTF-8 decoding distinguishes EOF, malformed input and U+FFFD", "[utf8]") {
    const auto eof = utf8::decode(nullptr, nullptr);
    CHECK(eof.status == utf8::DecodeStatus::Eof);
    CHECK(eof.bytes == 0);

    const auto malformed = "\xff"sv;
    const auto invalid = utf8::decode(malformed.data(), end_of(malformed));
    CHECK(invalid.status == utf8::DecodeStatus::Invalid);
    CHECK(invalid.codepoint == 0xfffd);
    CHECK(invalid.bytes == 0);

    const auto replacement_then_ascii = "\xef\xbf\xbd\x41"sv;
    const auto replacement
        = utf8::decode(replacement_then_ascii.data(), end_of(replacement_then_ascii));
    CHECK(replacement.status == utf8::DecodeStatus::Ok);
    CHECK(replacement.codepoint == 0xfffd);
    CHECK(replacement.bytes == 3);

    const auto nul_then_ascii = "\0A"sv;
    const auto nul = utf8::decode(nul_then_ascii.data(), end_of(nul_then_ascii));
    CHECK(nul.status == utf8::DecodeStatus::Ok);
    CHECK(nul.codepoint == 0);
    CHECK(nul.bytes == 1);
    CHECK(
        utf8::decode(end_of(nul_then_ascii), end_of(nul_then_ascii)).status
        == utf8::DecodeStatus::Eof
    );
}

TEST_CASE("UTF-8 encodes and decodes scalar values at width boundaries", "[utf8]") {
    constexpr std::array unchanged{'x', 'x', 'x', 'x', 'x'};
    for (const auto &sample : valid_samples) {
        CAPTURE(static_cast<unsigned int>(sample.codepoint));
        CHECK(utf8::is_valid_codepoint(sample.codepoint));

        const auto decoded = utf8::decode(sample.bytes.data(), end_of(sample.bytes));
        CHECK(decoded.status == utf8::DecodeStatus::Ok);
        CHECK(decoded.codepoint == sample.codepoint);
        CHECK(decoded.bytes == sample.bytes.size());

        auto output = unchanged;
        const auto written = utf8::encode(sample.codepoint, output);
        REQUIRE(written == sample.bytes.size());
        CHECK(std::string_view(output.data(), written) == sample.bytes);
        for (std::size_t i = written; i < output.size(); ++i) {
            CHECK(output[i] == unchanged[i]);
        }

        output = unchanged;
        const auto exact = std::span{output}.first(written);
        CHECK(utf8::encode(sample.codepoint, exact) == written);
        CHECK(std::string_view(output.data(), written) == sample.bytes);

        for (std::size_t capacity = 0; capacity < written; ++capacity) {
            CAPTURE(capacity);
            output = unchanged;
            CHECK(utf8::encode(sample.codepoint, std::span{output}.first(capacity)) == 0);
            CHECK(output == unchanged);
        }
    }
}

TEST_CASE("UTF-8 encoding leaves output unchanged for invalid scalar values", "[utf8]") {
    constexpr std::array unchanged{'x', 'x', 'x', 'x'};
    for (const auto codepoint : invalid_codepoints) {
        CAPTURE(static_cast<unsigned int>(codepoint));
        auto output = unchanged;
        CHECK_FALSE(utf8::is_valid_codepoint(codepoint));
        CHECK(utf8::encode(codepoint, output) == 0);
        CHECK(output == unchanged);
    }
    CHECK(utf8::encode(U'A', {}) == 0);
}

TEST_CASE("UTF-8 rejects malformed sequences within the supplied range", "[utf8]") {
    constexpr std::array malformed{
        "\x80"sv,
        "\xbf"sv,
        "\xc0\x80"sv,
        "\xc1\xbf"sv,
        "\xe0\x9f\xbf"sv,
        "\xf0\x8f\xbf\xbf"sv,
        "\xed\xa0\x80"sv,
        "\xed\xbf\xbf"sv,
        "\xf4\x90\x80\x80"sv,
        "\xf5\x80\x80\x80"sv,
        "\xf8\x88\x80\x80\x80"sv,
        "\xfe"sv,
        "\xff"sv,
        "\xc2\0"sv,
        "\xe2"
        "A\xac"sv,
        "\xe2\x82"
        "A"sv,
        "\xf0"
        "A\x98\x80"sv,
        "\xf0\x9f\xc2\x80"sv,
        "\xf0\x9f\x98"
        "A"sv,
    };
    for (const auto bytes : malformed) {
        CAPTURE(bytes);
        const auto decoded = utf8::decode(bytes.data(), end_of(bytes));
        CHECK(decoded.status == utf8::DecodeStatus::Invalid);
        CHECK(decoded.bytes == 0);
        CHECK_FALSE(utf8::is_valid(bytes.data(), end_of(bytes)));
    }

    for (const auto &sample : valid_samples) {
        for (std::size_t size = 1; size < sample.bytes.size(); ++size) {
            CAPTURE(static_cast<unsigned int>(sample.codepoint), size);
            const auto first = sample.bytes.data();
            const auto last = first + size;
            const auto decoded = utf8::decode(first, last);
            CHECK(decoded.status == utf8::DecodeStatus::Invalid);
            CHECK(decoded.bytes == 0);
            CHECK_FALSE(utf8::is_valid(first, last));
        }
    }
}

TEST_CASE("UTF-8 validates an entire sequence including embedded NULs", "[utf8]") {
    CHECK(utf8::is_valid(nullptr, nullptr));
    for (const auto bytes : {"A\0\xc2\xa2\xe2\x82\xac\xf0\x9f\x98\x80"sv, replacement_bytes}) {
        CHECK(utf8::is_valid(bytes.data(), end_of(bytes)));
    }
    for (const auto bytes : {"A\xff\x42"sv, "A\0\xed\xa0\x80"sv, "\xc2\xa2\xf0\x9f\x98"sv}) {
        CHECK_FALSE(utf8::is_valid(bytes.data(), end_of(bytes)));
    }
}

TEST_CASE("UTF-8 boundaries identify candidate starts and the end of the range", "[utf8]") {
    constexpr auto bytes = "\0\xc2\xa2\xe2\x82\xac\xf0\x9f\x98\x80"sv;
    for (std::size_t offset = 0; offset <= bytes.size(); ++offset) {
        CAPTURE(offset);
        const bool expected
            = offset == 0 || offset == 1 || offset == 3 || offset == 6 || offset == bytes.size();
        CHECK(utf8::is_boundary(bytes.data() + offset, end_of(bytes)) == expected);
    }
    CHECK(utf8::is_boundary(nullptr, nullptr));
    CHECK_FALSE(utf8::is_boundary(bytes.data() + 2, bytes.data() + 1));

    for (unsigned int byte = 0; byte <= 0xff; ++byte) {
        CAPTURE(byte);
        const auto chr = static_cast<char>(byte);
        const bool expected = byte <= 0x7f || (byte >= 0xc2 && byte <= 0xf4);
        CHECK(utf8::is_boundary(&chr, &chr + 1) == expected);
    }
}

TEST_CASE("UTF-8 resynchronization preserves candidate starts after invalid bytes", "[utf8]") {
    CHECK(utf8::resync(nullptr, nullptr) == 0);
    const auto invalid_byte = "\xff"sv;
    CHECK(utf8::resync(invalid_byte.data(), end_of(invalid_byte)) == 1);
    const auto invalid_run = "\xff\x80\xc0\xf5"sv;
    CHECK(utf8::resync(invalid_run.data(), end_of(invalid_run)) == 4);
    const auto ascii = "AB"sv;
    CHECK(utf8::resync(ascii.data(), end_of(ascii)) == 1);

    const auto malformed_then_nul = "\x80\xbf\xc0\xc1\xf5\xff\0"sv;
    const auto nul_offset = utf8::resync(malformed_then_nul.data(), end_of(malformed_then_nul));
    REQUIRE(nul_offset == 6);
    CHECK(
        utf8::decode(malformed_then_nul.data() + nul_offset, end_of(malformed_then_nul)).codepoint
        == 0
    );

    const auto malformed_then_scalar = "\xe2\x82\xf0\x9f\x98\x80"sv;
    const auto scalar_first = malformed_then_scalar.data();
    const auto scalar_last = end_of(malformed_then_scalar);
    CHECK(utf8::decode(scalar_first, scalar_last).status == utf8::DecodeStatus::Invalid);
    const auto scalar_offset = utf8::resync(scalar_first, scalar_last);
    REQUIRE(scalar_offset == 2);
    CHECK(utf8::decode(scalar_first + scalar_offset, scalar_last).codepoint == 0x1f600);

    const auto malformed_then_truncated = "\xff\xe2\x82"sv;
    const auto truncated_last = end_of(malformed_then_truncated);
    const auto candidate_offset = utf8::resync(malformed_then_truncated.data(), truncated_last);
    REQUIRE(candidate_offset == 1);
    const auto candidate = malformed_then_truncated.data() + candidate_offset;
    CHECK(utf8::decode(candidate, truncated_last).status == utf8::DecodeStatus::Invalid);
    CHECK(
        utf8::resync(candidate, truncated_last)
        == malformed_then_truncated.size() - candidate_offset
    );
}

TEST_CASE("UTF-8 round trips every Unicode scalar value", "[utf8]") {
    std::array<char, 4> output{};
    for (char32_t codepoint = 0; codepoint <= 0x10ffff; ++codepoint) {
        if (codepoint >= 0xd800 && codepoint <= 0xdfff) {
            continue;
        }

        const auto written = utf8::encode(codepoint, output);
        if (written == 0 || written > output.size()) {
            CAPTURE(static_cast<unsigned int>(codepoint), written);
            FAIL("invalid encoded width");
        }
        const auto decoded = utf8::decode(output.data(), output.data() + written);
        if (decoded.status != utf8::DecodeStatus::Ok || decoded.codepoint != codepoint
            || decoded.bytes != written) {
            CAPTURE(static_cast<unsigned int>(codepoint), written);
            FAIL("encoded scalar did not round trip");
        }
    }
}

} // namespace
