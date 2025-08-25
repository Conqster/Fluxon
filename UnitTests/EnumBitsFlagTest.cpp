//#pragma once
//#include "UnitTestFramework.h"
//#include "src/Core/EnumBitsFlag.h"
//
//
//enum class TestBits : uint32_t
//{
//	None = 0,
//	A = 1 << 0,
//	B = 1 << 1,
//	C = 1 << 2,
//};
//ENABLE_BIT_MASK_OPERATORS(TestBits, true)
//
//
//TEST_SUITE("Flag Class Tests")
//{
//
//	TEST_CASE("Default constructed flag are empty")
//	{
//		Flag<TestBits> flags;
//
//		CHECK(!flags.Contains(TestBits::A));
//		CHECK(!flags.Contains(TestBits::B));
//		CHECK(!flags.Contains(TestBits::C));
//	}
//
//	TEST_CASE("Equality and inequality")
//	{
//		Flag<TestBits> f1(TestBits::A | TestBits::B);
//		Flag<TestBits> f2(TestBits::A | TestBits::B);
//		Flag<TestBits> f3(TestBits::C);
//
//		CHECK(f1 == f2);
//		CHECK(f1 != f3);
//	}
//
//
//	TEST_CASE("Add and check single bit")
//	{
//		Flag<TestBits> flags;
//		flags.AddBit(TestBits::A);
//		CHECK(flags.Contains(TestBits::A));
//		CHECK(!flags.Contains(TestBits::B));
//		CHECK(!flags.Contains(TestBits::C));
//
//		flags = flags | TestBits::B;
//		CHECK(flags.Contains(TestBits::A));
//		CHECK(flags.Contains(TestBits::B));   //<-- should fail
//		CHECK(!flags.Contains(TestBits::C));
//	}
//
//	
//
//	TEST_CASE("Bitwise OR operation and assignment")
//	{
//		Flag<TestBits> flags;
//
//
//		flags = flags | TestBits::A;
//		CHECK(flags.Contains(TestBits::A));
//		CHECK(!flags.Contains(TestBits::B));
//		CHECK(!flags.Contains(TestBits::C));
//
//		flags |= TestBits::B;
//		CHECK(flags.Contains(TestBits::A));
//		CHECK(flags.Contains(TestBits::B));
//		CHECK(!flags.Contains(TestBits::C));
//
//
//		Flag<TestBits> flags1;
//		CHECK(!flags1.Contains(TestBits::A));
//		CHECK(!flags1.Contains(TestBits::B));
//		CHECK(!flags1.Contains(TestBits::C));
//
//		flags1 |= TestBits::A | TestBits::B | TestBits::C;
//		CHECK(flags1.Contains(TestBits::A));
//		CHECK(flags1.Contains(TestBits::B));
//		CHECK(flags1.Contains(TestBits::C));
//	}
//
//
//	TEST_CASE("Bitwise AND operation and assignment")
//	{
//		Flag<TestBits> flags;
//
//		flags |= TestBits::A;
//		flags |= TestBits::B;
//
//		Flag<TestBits> flags_2(TestBits::A | TestBits::C);
//		flags = flags & flags_2;
//
//		CHECK(flags.Contains(TestBits::A));
//		CHECK(!flags.Contains(TestBits::B));
//		CHECK(!flags.Contains(TestBits::C));
//
//		CHECK(flags_2.Contains(TestBits::A));
//		CHECK(!flags_2.Contains(TestBits::B));
//		CHECK(flags_2.Contains(TestBits::C));
//
//		flags |= TestBits::B;
//		flags_2 |= TestBits::B;
//
//		flags &= flags_2;
//		CHECK(flags.Contains(TestBits::A));
//		CHECK(flags.Contains(TestBits::B));
//		CHECK(!flags.Contains(TestBits::C));
//	}
//
//	TEST_CASE("Bitwise XOR operation and assignment")
//	{
//		Fluxon::Flag<TestBits> flags;
//
//		flags = flags ^ TestBits::A;
//		CHECK(flags.Contains(TestBits::A));
//		CHECK(!flags.Contains(TestBits::B));
//		CHECK(!flags.Contains(TestBits::C));
//
//		flags ^= TestBits::B;
//		CHECK(flags.Contains(TestBits::A));
//		CHECK(flags.Contains(TestBits::B));
//		CHECK(!flags.Contains(TestBits::C));
//	}
//
//
//	TEST_CASE("Bitwise NOT operation and assignment")
//	{
//		Fluxon::Flag<TestBits> flags;
//
//		flags.AddBit(TestBits::A);
//		CHECK(flags.Contains(TestBits::A));
//
//		flags = ~flags;
//		CHECK(!flags.Contains(TestBits::A));
//		CHECK(flags.Contains(TestBits::B));
//		CHECK(flags.Contains(TestBits::C));
//	}
//
//	TEST_CASE("Bit toggle")
//	{
//		Flag<TestBits> flags;
//
//		CHECK(!flags.Contains(TestBits::A));
//		CHECK(!flags.Contains(TestBits::B));
//		CHECK(!flags.Contains(TestBits::C));
//
//		flags.AddBit(TestBits::A);
//		CHECK(flags.Contains(TestBits::A));
//
//		flags.RemoveBit(TestBits::A);
//		CHECK(!flags.Contains(TestBits::A));
//
//		flags |= TestBits::B;
//		CHECK(((flags & TestBits::B) == TestBits::B));
//		flags &= ~TestBits::B;
//		CHECK(!flags.Contains(TestBits::B));
//	}
//
//	TEST_CASE("Underlying value check")
//	{
//		Flag<TestBits> flags(TestBits::A | TestBits::B);
//		auto raw = flags.Value();
//
//		using u_type = std::underlying_type_t<TestBits>;
//		CHECK(raw == (static_cast<u_type>(TestBits::A) | static_cast<u_type>(TestBits::B)));
//	}
//
//	TEST_CASE("Add and remove")
//	{
//		Flag<TestBits> flags;
//		flags.AddBit(TestBits::A);
//		flags.AddBit(TestBits::A);
//		CHECK(flags.Contains(TestBits::A));
//
//
//		flags.RemoveBit(TestBits::A);
//		CHECK(!flags.Contains(TestBits::A));
//
//		flags.RemoveBit(TestBits::A);
//		CHECK(!flags.Contains(TestBits::A));
//	}
//}
