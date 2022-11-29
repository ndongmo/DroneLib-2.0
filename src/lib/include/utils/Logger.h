/*!
* \file Logger.h
* \brief Write log in console and file at the same time.
* \author Ndongmo Silatsa
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#ifdef _WIN32 
#pragma warning(disable : 4996)
#endif

#include <iostream>
#include <ctime>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <unordered_map>

#ifndef logE
#define logE utils::Logger::log() << " ERROR @ "
#define logW utils::Logger::log() << " WARNING @ "
#define logI utils::Logger::log() << " INFO @ "
#define logX(x) utils::Logger::logx(x)
#define logError(x) utils::Logger::log().getError(x)
#endif

#define LOG_FILE "log.txt"

namespace utils
{
/*!
 * Write log in console and file at the same time.
 */
class Logger
{
public:
	/*!
	 * Reditect the given data to the file and std output.
	 * \param data the data to write
	 * \return the current logger.
	 */
	template<typename T>
	Logger& operator<<(const T& data) {
		m_fs << data;
		*m_os << data;

		return *this;
	}

	/*!
	 * Reditect the given stream to the file and std output.
	 * \param f the stream to write
	 * \return the current logger.
	 */
	Logger& operator<<(std::ostream&(*f)(std::ostream&)) {
		m_fs << f;
		*m_os << f;

		return *this;
	}

	/*!
	 * Obtains the error value regarding the given code.
	 * @param code error code
	 * \return the error string value if the code is valid, otherwise empty string.
	 */
	std::string getError(int code) const {
		if(m_logger.m_os == nullptr) {
			init(std::cout);
		}
		
		if(m_errors.find(code) != m_errors.end()) {
			return m_errors.at(code);
		}
		return m_errors.at(0);
	}

	/*!
	 * Obtains the error value regarding the given code
	 * and write it on the logger stream.
	 * @param x error code
	 * \return the current logger.
	 */
	static Logger& logx(int x) {
		return log() << " ERROR @ " << m_logger.getError(x) << ": ";
	}

	/*!
	 * Obtains the Logger singleton object.
	 * \return the current logger.
	 */
	static Logger& log() {
		if(m_logger.m_os == nullptr) {
			init(std::cout);
		}

		auto now = std::chrono::system_clock::now();
		auto now_time_t = std::chrono::system_clock::to_time_t(now);
		auto now_tm = std::localtime(&now_time_t);

		m_logger << "[" << std::put_time(now_tm, "%c") << "] ";

		return m_logger;
	}

	/*!
	 * Initialize the log file and output stream.
	 * \param os output stream refference to write into
	 */
	static void init(std::ostream& os) {
		m_logger.m_fs.open(LOG_FILE);
		m_logger.m_os = &os;
	}

private:
	/*!
	 * Private logger constructor.
	 */
	Logger();
	/*!
	 * Private logger destructor to close the opened log file.
	 */
	~Logger() {
		m_os = nullptr;
		if (m_fs.is_open()) {
			m_fs.close();
		}
	}

	/* Logger singleton object. */
	static Logger m_logger;

	/* Logger output stream pointer. */
	std::ostream* m_os = nullptr;

	/* Logger file stream. */
	std::ofstream m_fs;

	/* Errors map. */
	std::unordered_map<int, std::string> m_errors;
};
}

