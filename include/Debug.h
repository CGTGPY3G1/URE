#ifndef UNNAMED_DEBUG_H
#define UNNAMED_DEBUG_H
#include <sstream>
#include <vector>
namespace B00289996 {
	
	// LOG can now be used to produce a log message that includes the line on which the log occurrred
	enum DebugMessageType {
		DEBUG_TYPE_NEUTRAL = 15,
		DEBUG_TYPE_SUCCESS = 10,
		DEBUG_TYPE_FAILURE_LOW = 14,
		DEBUG_TYPE_FAILURE_MEDIUM = 13,
		DEBUG_TYPE_FAILURE_CRITICAL = 12
	};
	static const char * ToChar(const DebugMessageType & target) {
		switch (target) {
		case DebugMessageType::DEBUG_TYPE_NEUTRAL: return "DEBUG_TYPE_NEUTRAL : "; break;
		case DebugMessageType::DEBUG_TYPE_SUCCESS: return "DEBUG_TYPE_SUCCESS : "; break;
		case DebugMessageType::DEBUG_TYPE_FAILURE_LOW: return "DEBUG_TYPE_FAILURE_LOW : "; break;
		case DebugMessageType::DEBUG_TYPE_FAILURE_MEDIUM: return "DEBUG_TYPE_FAILURE_MEDIUM : "; break;
		case DebugMessageType::DEBUG_TYPE_FAILURE_CRITICAL: return "DEBUG_TYPE_FAILURE_CRITICAL : "; break;
		default: return "UNKNOWN_DEBUG_MESSAGE"; break;
		}
	}
	struct DebugMessage {
		DebugMessage(const std::string & debugMessage, const DebugMessageType & debugType, const std::string & time) : message(debugMessage), type(debugType), dateTime(time) {}
		std::string message;
		std::string dateTime;
		DebugMessageType type;
	};

	class Debug {
	public:
		
		~Debug();
		static void Log(const std::string & message, const DebugMessageType & type = DEBUG_TYPE_NEUTRAL);
		static void PrintImmediately(const std::string & message, const DebugMessageType & type = DEBUG_TYPE_NEUTRAL);
		static void PrintException(const std::exception & e);
		void PrintLog();
	private:
		static std::string GetDateTimeAsString();
		static std::vector<DebugMessage> loggedMessages;
	};
}
#endif // !UNNAMED_DEBUG_H
