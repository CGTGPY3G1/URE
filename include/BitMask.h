#ifndef UNNAMED_BITSET_H
#define UNNAMED_BITSET_H
#include <bitset>

namespace B00289996 {
	struct BitMask {
		friend struct std::hash<BitMask>;
	public:
		BitMask() : mask(0), lastSet(std::numeric_limits<std::int_least32_t>::digits) {
			
		}
		BitMask(const int_least32_t & position) : mask(0) {
			lastSet = position;
			Set(position);
		}
		const BitMask Set(const int_least32_t & position) {
			lastSet = position;
			mask = 1 << position;
			return *this;
		}
		const bool AnySet() const { return mask > 0; }
		const bool AllSet() const { return mask == std::numeric_limits<std::int_least32_t>::max(); }
		const bool NoneSet() const { return mask == 0; }
		const unsigned int RelevantBit() const { return lastSet; }
		const BitMask & Flip() {
			mask = ~mask;
			return *this; 
		}

		const BitMask Flipped() {
			BitMask bitmask;
			bitmask.mask = mask;
			bitmask.mask = ~bitmask.mask;
			return bitmask;
		}

		inline const bool Contains(const BitMask bitmask) const {
			return (bitmask.mask & mask) == bitmask.mask;
		}
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
			return mask;
		}
		const auto GetMask() {
			return mask;
		}
	private:
		unsigned int lastSet;
		std::int_least32_t mask;
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
