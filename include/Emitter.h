#ifndef UNNAMED_EMITTER_H
#define UNNAMED_EMITTER_H
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
namespace B00289996 {
	template <typename T> class Emitter;
	struct nt {};
	class Event;
	class Connection {
		template <typename T> friend class Emitter;
	public:
		Connection(const std::string & Type = "InvalidEmitter", const std::size_t & eID = std::numeric_limits<std::size_t>::max()) : type(Type), valid(false), blocked(true) ,id(eID) {}
		virtual ~Connection() {  }
		virtual void Disconnect() = 0;
		virtual void Block() = 0;
		virtual void Unblock() = 0;
		virtual const bool IsBlocked() { return blocked; }
		virtual const bool IsValid() const = 0;
		const std::string Type() const { return type; }
		const std::size_t DispatcherID() const { return id; }
	protected:
		void Validate() { if (!valid) valid = true; }
		void Invalidate() { if (valid) valid = false; }
		bool valid, blocked;
	private:
		std::size_t id;
		std::string type;
	};

	template <typename T>
	class DConnection : public Connection {
	public:
		DConnection(std::shared_ptr<Emitter<T>> src, const std::size_t & id, const std::string & emitterType) : Connection(emitterType, id), emmiter(src) {} 
		~DConnection() { Disconnect(); }
		void Disconnect() override { if (IsValid()) emmiter->Disconnect(*this); }
		void Block() override { if (IsValid()) emmiter->Block(*this); }
		void Unblock() override { if (IsValid()) emmiter->Unblock(*this); }
		const bool IsValid() const override { return valid && !IsNull(); }
		const bool IsNull() const { return !emmiter; }
	private:
		typename std::shared_ptr<Emitter<T>> emmiter;
	};



	template <typename T>
	class Emitter : public std::enable_shared_from_this<Emitter<T>> {
	public:
		Emitter(const std::string & name = "InvalidEmitter");
		~Emitter() {}
		std::shared_ptr<Connection> Connect(const std::function<void(T)> & function) {
			std::size_t id = 0;
			if (freeIDs.empty()) id = idCount++;
			else {
				do {
					id = freeIDs.back();
					freeIDs.pop_back();
				} while (!freeSlots[id] && freeIDs.size() > 0);
				if (!freeSlots[id]) id = idCount++;
			}

			std::shared_ptr<Emitter<T>> me = std::enable_shared_from_this<Emitter<T>>::shared_from_this();
			std::shared_ptr<Connection> connection = std::make_shared<DConnection<T>>(me, id, mName);
			connection->Validate();
			connection->blocked = false;
			if (id == idCount - 1) {
				std::size_t newSize = listeners.size() + 1;
				blocks.resize(newSize, true);
				freeSlots.resize(newSize, true);
				listeners.resize(newSize);
			}
			listeners[id] = std::make_shared<std::function<void(T)>>(function);
			blocks[id] = false;
			freeSlots[id] = false;
			return connection;
		}
		void Disconnect(Connection & connection) {
			if (connection.IsValid()) {
				std::size_t id = connection.id;
				if (id < listeners.size()) {
					std::shared_ptr<std::function<void(T)>> function = listeners[id];
					if (function) {
						listeners[id] = std::shared_ptr<std::function<void(T)>>();
						connection.blocked = true;
						blocks[id] = true;
						freeSlots[id] = true;
						connection.Invalidate();
						freeIDs.push_back(id);
					}
				}
			}
		}
		void Block(Connection & connection) {
			if (connection.IsValid()) {
				std::size_t id = connection.id;
				if (id < blocks.size()) {
					const bool _val = blocks[id];
					if (!_val) {
						blocks[id] = true;
						connection.blocked = true;
					}
				}
			}
		}
		void Unblock(Connection & connection) {
			if (connection.IsValid()) {
				std::size_t id = connection.id;
				if (id < blocks.size()) {
					const bool _val = blocks[id];
					if (_val) {
						blocks[id] = false;
						connection.blocked = false;
					}
				}
			}
		}

		void Emit(T&& params) {
			for (std::size_t i = 0; i < listeners.size(); i++) {
				if (listeners[i] && !blocks[i]) listeners[i]->operator()(std::forward<T>(params));
			}
		}
		const std::shared_ptr<Emitter<T>> GetShared();
	private:
		std::vector<std::size_t> freeIDs;
		std::size_t idCount;
		std::string mName;
		std::vector<bool> blocks;
		std::vector<bool> freeSlots;
		std::vector<std::shared_ptr<std::function<void(T)>>> listeners;
	};

	

	typedef std::unordered_map<std::size_t, std::shared_ptr<Connection>> ConnectionList;
	typedef std::unordered_map<std::string, ConnectionList> ConnectionMap;
	class ConnectionMananger {
	public:

		ConnectionMananger() : connectionMap(ConnectionMap()) {}
		virtual ~ConnectionMananger() {
		}
		const ConnectionMap GetConnections() const { return connectionMap; }

		void AddConnection(const std::shared_ptr<Connection> & connection) {
			const std::string type = connection->Type();
			const std::size_t id = connection->DispatcherID();
			if (connectionMap.count(type) == 0) connectionMap[type] = ConnectionList();
			connectionMap[type][id] = connection;
		}

		void DestroyConnection(const std::string & type) {
			if (connectionMap.count(type) > 0) {
				ConnectionList list = connectionMap[type];
				for (ConnectionList::iterator i = list.begin(); i != list.end(); ++i) {
					(*i).second->Disconnect();
				}
				connectionMap.erase(type);
			}
		}
		virtual void ProcessEvent(const Event & toProcess);
		void DisableConnection(const std::string & type) {
			if (connectionMap.count(type) > 0) {
				ConnectionList list = connectionMap[type];
				for (ConnectionList::iterator i = list.begin(); i != list.end(); ++i) {
					(*i).second->Block();
				}
			}
		}

		void EnableConnection(const std::string & type) {
			if (connectionMap.count(type) > 0) {
				ConnectionList list = connectionMap[type];
				for (ConnectionList::iterator i = list.begin(); i != list.end(); ++i) {
					(*i).second->Unblock();
				}
			}
		}
	private:
		ConnectionMap connectionMap;
	};
	template<typename T>
	inline Emitter<T>::Emitter(const std::string & name) : std::enable_shared_from_this<Emitter<T>>(), freeIDs(std::vector<std::size_t>()), idCount(0), mName(name),
		blocks(std::vector<bool>()), freeSlots(std::vector<bool>()), listeners(std::vector<std::shared_ptr<std::function<void(T)>>>()) {
	}
	template<typename T>
	inline const std::shared_ptr<Emitter<T>> Emitter<T>::GetShared() {
		return shared_from_this();
	}
}
#endif // !UNNAMED_EMITTER_H
