#include "Debug.h"
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <time.h>
#include <sstream>
#include <typeinfo> 
#include <exception>
namespace B00289996 {

	void Print(const std::string & message, const DebugMessageType & type) {
		std::cout << ToChar(type) << message << std::endl;
	}
	std::vector<DebugMessage> Debug::loggedMessages = std::vector<DebugMessage>();
	Debug::~Debug() {
	}

	void Debug::Log(const std::string & message, const DebugMessageType & type) {
		loggedMessages.push_back(DebugMessage(message, type, GetDateTimeAsString()));
	}

	void Debug::PrintImmediately(const std::string & message, const DebugMessageType & type) {
		std::stringstream m;
		m << GetDateTimeAsString() << std::endl << message;
		Print(m.str(), type);
	}

	void Debug::PrintException(const std::exception & e) {
		std::cout << GetDateTimeAsString() + " | Excepion: " + e.what() << std::endl;
	}

	void Debug::PrintLog() {
		for (size_t i = 0; i < loggedMessages.size(); i++) {
			std::stringstream m;
			m << loggedMessages[i].dateTime << std::endl << loggedMessages[i].message;
			Print(m.str(), loggedMessages[i].type);
		}
		loggedMessages.clear();
	}

	std::string Debug::GetDateTimeAsString() {
		time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char buffer[100];
		struct tm timeinfo;
		if (localtime_s(&timeinfo, &now) == 0) strftime(buffer, sizeof(buffer), "Date/Time = %d-%m-%Y %H:%M:%S", &timeinfo);
		std::stringstream ss;
		ss << buffer;
		return ss.str();
	}
}
