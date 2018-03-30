#ifndef UNNAMED_BITSET_H
#define UNNAMED_BITSET_H
#include <bitset>

namespace B00289996 {
	struct BitMask {
		friend struct std::hash<BitMask>;
	public:
		BitMask() : mask(std::bitset<std::numeric_limits<std::int_least32_t>::digits>()) {
			
		}
		BitMask(const int_least32_t & position) : mask(std::bitset<std::numeric_limits<std::int_least32_t>::digits>()) {
			Set(position, false);
		}
		const BitMask Set(const int_least32_t & position, const bool & value = true) {
			mask.set(position, value);
			return *this;
		}
		const bool AnySet() const { return mask.any(); }
		const bool AllSet() const { return mask.all(); }
		const bool NoneSet() const { return mask.none(); }
		const BitMask & Flip() {
			mask = mask.flip();
			return *this; 
		}

		const BitMask Flipped() {
			BitMask bitmask;
			bitmask.mask = mask;
			bitmask.mask = bitmask.mask.flip();
			return bitmask;
		}
		const int_least32_t ActiveBits() const { return mask.count(); }
		inline const bool operator==(const BitMask & other) const { return mask == other.mask; }
		inline const bool operator!=(const BitMask & other) const { return mask != other.mask; }
		inline const void operator=(const BitMask & other) { mask = other.mask; }
		inline const BitMask & operator&=(const BitMask & other) { 
			mask &= other.mask;
			return *this;
		}
		inline const BitMask & operator|=(const BitMask & other) {
			mask |= other.mask;
			return *this;
		}
		inline const BitMask & operator^=(const BitMask & other) {
			mask ^= other.mask;
			return *this;
		}
		
		
		inline const BitMask & operator<<=(const std::size_t & pos) {
			mask <<= pos;
			return *this;
		}
		
		inline const BitMask & operator>>=(const std::size_t & pos) {
			mask >>= pos;
			return *this;
		}

		inline const BitMask operator~() const {
			BitMask bitmask;
			bitmask.mask = ~mask;
			return bitmask;
		}
		inline const BitMask operator<<(const std::size_t & pos) const {
			BitMask bitmask;
			bitmask.mask = mask << pos;
			return bitmask;
		}
		inline const BitMask operator>>(const std::size_t & pos) const {
			BitMask bitmask;
			bitmask.mask = mask >> pos;
			return bitmask;
		}
		inline const BitMask operator&(const BitMask& other) const {
			BitMask bitmask;
			bitmask.mask = mask & other.mask;
			return bitmask;
		}
		inline const BitMask operator|(const BitMask& other) const  {
			BitMask bitmask;
			bitmask.mask = mask | other.mask;
			return bitmask;
		}
		inline const BitMask operator^(const BitMask& other) const {
			BitMask bitmask;
			bitmask.mask = mask ^ other.mask;
			return bitmask;
		}

		const std::int_least32_t AsNumber() const {
			return (std::int_least32_t)mask.to_ullong();
		}
		const auto GetMask() {
			return mask;
		}
	private:

		std::bitset<std::numeric_limits<std::int_least32_t>::digits> mask;
	};
}

namespace std {
	template <>
	struct hash<B00289996::BitMask> {
		std::size_t operator()(const B00289996::BitMask& mask) const {
			return (std::size_t) mask.AsNumber();
		}
	};
}
#endif // !UNNAMED_BITSET_H
