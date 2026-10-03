/* tests/foundation/codec_test.cpp — the encoding every format rests on, the body fields,
 * the SecretBuffer and the generator's alphabet. No crypto. */
#include "foundation/core/codec.hpp"
#include "foundation/record/fields.hpp"
#include "foundation/secret/secret_buffer.hpp"
#include "foundation/secrets/generator.hpp"
#include "support/check.hpp"

using namespace verguenza;

int main() {
    // little-endian round trip; reads past the end fail and stay failed
    {
        Writer w;
        w.u8(7);
        w.u16(0x1234);
        w.u32(0xdeadbeef);
        w.i64(-5);
        w.blob("hello");
        Reader r(w.bytes());
        CHECK(r.u8() == 7);
        CHECK(r.u16() == 0x1234);
        CHECK(r.u32() == 0xdeadbeef);
        CHECK(r.i64() == -5);
        CHECK(r.blob(16) == "hello");
        CHECK(r.finish());
        Reader short_read(w.bytes().substr(0, 4)); // a u8 and only 3 of a u32's 4 bytes
        short_read.u8();
        short_read.u32();
        CHECK(short_read.failed());
        Reader too_long(w.bytes());
        too_long.raw(15);
        CHECK(too_long.blob(3).empty() && too_long.failed()); // past its ceiling: refused
        Reader trailing(w.bytes() + "x");
        trailing.raw(w.bytes().size());
        CHECK(!trailing.finish());
    }
    // hex
    {
        Bytes b;
        CHECK(from_hex("00ff10", b) && b == std::string("\x00\xff\x10", 3));
        CHECK(to_hex(b) == "00ff10");
        CHECK(!from_hex("0g", b));
        CHECK(!from_hex("abc", b));
    }
    // fields: canonical (sorted), and a non-canonical body is refused
    {
        Fields f;
        f.set(5, "five").set(1, "one").set_u32(3, 42);
        const Bytes enc = f.encode();
        Fields back;
        CHECK_OK(Fields::decode(enc, back));
        CHECK(back.get(1) == "one" && back.get(5) == "five" && back.get_u32(3) == 42u);
        CHECK(back.encode() == enc);
        Writer w; // tags out of order
        w.u8(2);
        w.blob("b");
        w.u8(1);
        w.blob("a");
        CHECK(!Fields::decode(w.bytes(), back));
    }
    // SecretBuffer: move-only, wiped on resize, counted while alive
    {
        const std::size_t before = SecretBuffer::live_count();
        {
            SecretBuffer a = SecretBuffer::copy_of("vg-test-abc", 11);
            CHECK(SecretBuffer::live_count() == before + 1);
            SecretBuffer b = std::move(a);
            CHECK(a.size() == 0 && b.size() == 11);
            CHECK(SecretBuffer::live_count() == before + 1);
            b.resize(3);
            CHECK(b.view() == "vg-");
            b.resize(11);
            CHECK(b.data()[5] == 0); // what resize cut off was wiped, not kept
        }
        CHECK(SecretBuffer::live_count() == before);
    }
    // the generator's alphabet leaves out look-alikes
    {
        const std::string a = secrets::alphabet({});
        CHECK(a.find('l') == std::string::npos && a.find('O') == std::string::npos && a.find('0') == std::string::npos);
    }
    return vtest::finish("codec");
}
