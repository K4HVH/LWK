#pragma once
#pragma optimize ("", off)

#include <array>

constexpr auto _time = __TIME__;
constexpr auto _seed = static_cast <int> (_time[7]) + static_cast <int> (_time[6]) * 10 + static_cast <int> (_time[4]) * 60 + static_cast <int> (_time[3]) * 600 + static_cast <int> (_time[1]) * 3600 + static_cast <int> (_time[0]) * 36000;

template <int N>
class RandomGenerator
{
	static constexpr unsigned a = 16807;
	static constexpr unsigned m = 2147483647; //-V112

	static constexpr unsigned s = RandomGenerator <N - 1>::value;
	static constexpr unsigned lo = a * (s & 0xFFFF); 
	static constexpr unsigned hi = a * (s >> 16); 
	static constexpr unsigned lo2 = lo + ((hi & 0x7FFF) << 16);
	static constexpr unsigned hi2 = hi >> 15;
	static constexpr unsigned lo3 = lo2 + hi;
public:
	static constexpr unsigned max = m;
	static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
};

template <>
class RandomGenerator <0>
{
public:
	static constexpr unsigned value = _seed;
};

template <int N, int M>
class RandomInt
{
public:
	static constexpr auto value = RandomGenerator <N + 1>::value % M;
};

template < int N >
class RandomChar
{
public:
	static const char value = static_cast <char> (1 + RandomInt <N, 0x7F - 1>::value);
};

template <size_t N, int K>
class XorString
{
	const char _key;
	std::array <char, N + 1> _encrypted;

	constexpr char enc(char c) const
	{
		return c ^ _key;
	}

	char dec(char c) const
	{
		return c ^ _key;
	}
public:
	template <size_t... Is>
	constexpr __forceinline XorString(const char* str, std::index_sequence< Is... >) : _key(RandomChar< K >::value), _encrypted { enc(str[Is])... }
	{

	}

	__forceinline decltype(auto) decrypt()
	{
		for (size_t i = 0; i < N; ++i) 
			_encrypted[i] = dec(_encrypted[i]); //-V120
		
		_encrypted[N] = '\0'; //-V120
		return _encrypted.data();
	}
};


#define crypt_str(s) s


#pragma optimize ("", on)