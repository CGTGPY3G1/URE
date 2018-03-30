#ifndef UNNAMED_ID_GENERATOR_H
#define UNNAMED_ID_GENERATOR_H
#include <vector>
namespace B00289996 {

	template<typename T, typename V = std::size_t, typename = std::enable_if<std::is_integral<V>::value>>
	class IDGenerator {
	public:
		~IDGenerator() {}
		static IDGenerator<T, V> & GetInstance() {
			static IDGenerator<T, V> instance;
			return instance;
		}
		const V GetNewID() {
			if (retiredIDs.size() == 0) {
				currentID++;
				return currentID == 0 ? currentID++ : currentID;
			}
			const V toReturn = retiredIDs.back();
			retiredIDs.pop_back();
			return toReturn;
		}
		void RetireID(const V & id) {
			if (id != 0) retiredIDs.push_back(id);
		}
	private:

		IDGenerator() : currentID(0), retiredIDs(std::vector<V>()) {}
		V currentID;
		std::vector<V> retiredIDs;
	};
}
#endif // !UNNAMED_ID_GENERATOR_H