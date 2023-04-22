#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <time.h>
#include <stdarg.h>

#include "util.h"
#include "singleton.h"
#include "thread.h"
#include "mutex.h"

/**
 * @brief ʹ����ʽ��ʽ����־����level����־д�뵽logger
 * 
 */
#define SYLAR_LOG_LEVEL(logger, level) \
	if (logger->getLevel() <= level) \
		sylar::LogEventWarp(sylar::LogEvent::ptr (new sylar::LogEvent(logger, level, \
				__FILE__, __LINE__, 0, sylar::GetThreadId(), \
			sylar::GetFiberId(), time(0), sylar::Thread::GetName()))).getSS()

/**
 * @brief ʹ����ʽ��ʽ����־����debug����־д�뵽logger
 * 
 */
#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)

/**
 * @brief ʹ����ʽ��ʽ����־����info����־д�뵽logger
 * 
 */
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)

/**
 * @brief ʹ����ʽ��ʽ����־����warn����־д�뵽logger
 * 
 */
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)

/**
 * @brief ʹ����ʽ��ʽ����־����error����־д�뵽logger
 * 
 */
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)

/**
 * @brief ʹ����ʽ��ʽ����־����fatal����־д�뵽logger
 * 
 */
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

/**
 * @brief ʹ�ø�ʽ����ʽ����־����level����־д�뵽logger
 * 
 */
#define SYLARY_LOG_FMT_LEVEL(logger, level, fmt, ...) \
	if (logger->getLevel() <= level) \
		sylar::LogEventWarp(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, \
							__FILE__, __LINE__, 0, sylar::GetThreadId(), \
					sylar::GetFiberId(), time(0), sylar::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__)

/**
 * @brief ʹ�ø�ʽ����ʽ����־����debug����־д�뵽logger
 * 
 */
#define SYLARY_LOG_FMT_DEBUG(logger, fmt, ...) SYLARY_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)

/**
 * @brief ʹ�ø�ʽ����ʽ����־����info����־д�뵽logger
 * 
 */
#define SYLARY_LOG_FMT_INFO(logger, fmt, ...) SYLARY_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)

/**
 * @brief ʹ�ø�ʽ����ʽ����־����warn����־д�뵽logger
 * 
 */
#define SYLARY_LOG_FMT_WARN(logger, fmt, ...) SYLARY_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)

/**
 * @brief ʹ�ø�ʽ����ʽ����־����error����־д�뵽logger
 * 
 */
#define SYLARY_LOG_FMT_ERROR(logger, fmt, ...) SYLARY_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)

/**
 * @brief ʹ�ø�ʽ����ʽ����־����fatal����־д�뵽logger
 * 
 */
#define SYLARY_LOG_FMT_FATAL(logger, fmt, ...) SYLARY_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

/**
 * @brief ���Ĭ��root Log
*/
#define SYLAR_LOG_ROOT() sylar::LoggerMgr::GetInstance()->getRoot()

/**
 * @brief ��� name Log
*/
#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::GetInstance()->getLogger(name)

#define SYLAR__ROOT__LOG(name) sylar::Logger::ptr name  = SYLAR_LOG_ROOT()

#define SYLAR__SYSTEM__LOG(name) sylar::Logger::ptr name = SYLAR_LOG_NAME("system")


namespace sylar{

class Logger;

/**
 * @brief ��־����
 * 
 */
class LogLevel{
public:
	enum Level{
		//	δ֪ ����
		UNKNOW = 0,
		//	DEBUG ����
		DEBUG = 1,
		//	INFO ����
		INFO = 2,
		//	WARN ����
		WARN = 3,
		//	ERROR ����
		ERROR = 4,
		//	FATAL ����
		FATAL = 5
	};

	/**
	 * @brief ����־����ת���ı����
	 * 
	 * @param[in] level ��־����
	 * @return const char* 
	 * 
	 * @details { ERROR��û�ж����޷����ó�Ա���� } ��Ա������Ҫ������ã���������Ϊstatic
	 * @details ��̬��Ա�����ڱ���ǰ�Ϳ��ٿռ�  ֱ��ʹ�� Level::ToString ����
	 */
	static const char* ToString(LogLevel::Level level);

	static LogLevel::Level FromString(const std::string &str);
};

/**
 * @brief  ��־�¼�
 * 
 */
class LogEvent{
public:
	typedef std::shared_ptr<LogEvent> ptr;
	/**
	 * @brief ���캯��
	 * 
	 * @param[in] logger ��־��
	 * @param[in] level ��־����
	 * @param[in] file �ļ���
	 * @param[in] line �ļ��к�
	 * @param[in] elapse �������������ĺ�ʱ�����룩
	 * @param[in] thread_id �߳�id
	 * @param[in] fiber_id Э��id
	 * @param[in] time ��־ʱ��
	 */
	LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
			, const char* file, int32_t line, uint32_t elapse
			, uint32_t thread_id, uint32_t fiber_id, uint64_t time
			, const std::string& thread_name);
	/**
	 * @brief �����ļ���
	 * 
	 * @return const char* 
	 */
	const char* getFile() const { return m_file; }

	/**
	 * @brief �����к�
	 * 
	 * @return int32_t 
	 */
	int32_t getLine() const { return m_line; }

	/**
	 * @brief ���غ�ʱ
	 * 
	 * @return uint32_t 
	 */
	uint32_t getElapse() const { return m_elapse; }

	/**
	 * @brief �����߳�ID
	 * 
	 * @return uint32_t 
	 */
	uint32_t getThieadId() const { return m_thieadId; }

	/**
	 * @brief ����Э��ID
	 * 
	 * @return uint32_t 
	 */
	uint32_t getFiberId() const { return m_fiberId; }

	/**
	 * @brief ����ʱ��
	 * 
	 * @return uint64_t 
	 */
	uint64_t getTime() const { return m_time; }

	/**
	 * @brief �����߳�����
	*/
	std::string getThreadName() const { return m_threadName; }

	/**
	 * @brief ������־����
	 * 
	 * @return std::string 
	 */
	std::string getContent() const { return m_ss.str(); }

	/**
	 * @brief ������־��
	 * 
	 * @return std::shared_ptr<Logger> 
	 */
	std::shared_ptr<Logger> getLogger() const { return m_logger; }

	/**
	 * @brief ������־����
	 * 
	 * @return LogLevel::Level 
	 */
	LogLevel::Level getLevel() const { return m_level; }

	/**
	 * @brief ������־�����ַ�����
	 * 
	 * @return std::stringstream& 
	 */
	std::stringstream& getSS() { return m_ss; }

	/**
	 * @brief ��ʽ��д����־����
	 * 
	 * @param fmt 
	 * @param ... 
	 */
	void format(const char* fmt, ...);

	/**
	 * @brief ��ʽ��д����־����
	 * 
	 * @param fmt 
	 * @param al 
	 */
	void format(const char* fmt, va_list al);

private:
	const char* m_file = nullptr;	//�ļ���
	int32_t m_line = 0;  	  	//�к�
	uint32_t m_elapse = 0;		//����������ʼ�����ڵĺ�����
	uint32_t m_thieadId = 0;  	//�߳�id
	uint32_t m_fiberId = 0;   	//Э��id
	uint64_t m_time;         	//ʱ���
	std::string m_threadName;	//�߳�����
	std::stringstream m_ss;    	//��־������
	std::shared_ptr<Logger> m_logger;	//��־��
	LogLevel::Level m_level;	//��־�ȼ�
};

/**
 * @brief ��־�¼���װ��
 * 
 */
class LogEventWarp {
public:
	/**
	 * @brief ���캯��
	 * 
	 * @param[in] e ��־�¼�
	 */
	LogEventWarp(LogEvent::ptr e);
	
	/**
	 * @brief ��������
	 * 
	 */
	~LogEventWarp();

	/**
	 * @brief ��ȡ��־�¼�
	 * 
	 * @return LogEvent::ptr 
	 */
	LogEvent::ptr getEvent() const { return m_event; }

	/**
	 * @brief ��ȡ��־������
	 * 
	 * @return std::stringstream& 
	 */
	std::stringstream& getSS();
private:
	LogEvent::ptr m_event;
};

/**
 * @brief ��־��ʽ��
 * 
 */
class LogFormatter{
public:
	typedef std::shared_ptr<LogFormatter> ptr;
	/**
	 * @brief ���캯��
	 * 
	 * @param pattern[in] pattern ��ʽģ��
	 * 
	 * @details 
     *  %m ��Ϣ
     *  %p ��־����
     *  %r �ۼƺ�����
     *  %c ��־����
     *  %t �߳�id
     *  %n ����
     *  %d ʱ��
     *  %f �ļ���
     *  %l �к�
     *  %T �Ʊ��
     *  %F Э��id
     *  %N �߳�����
     *
     *  Ĭ�ϸ�ʽ "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
     */
	LogFormatter(const std::string& pattern);
	
	/**
	 * @brief ���ظ�ʽ����־�ı�
	 * 
	 * @param[in] logger ��־��
	 * @param[in] level ��־����
	 * @param[in] event ��־�¼�
	 * @return std::string 
	 */
	std::string format (std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
	/**
	 * @brief ��־���ݸ�ʽ��
	 * 
	 */
	class FormatItem{
	public:
		typedef std::shared_ptr<FormatItem> ptr;
		/**
		 * @brief ��������
		 * 
		 */
		virtual ~FormatItem() {}
		/**
		 * @brief ��ʽ����־����
		 * 
		 * @param[in, out] os ��־�����
		 * @param[in] logger ��־��
		 * @param[in] level ��־�ȼ�
		 * @param[in] event ��־�¼�
		 */
		virtual void format(std::ostream& os, std::shared_ptr<Logger> logger
							, LogLevel::Level level, LogEvent::ptr event) = 0;
	};
	/**
	 * @brief ��ʼ����������־ģ��
	 * 
	 */
	void init();

	bool isError() const { return m_error; }

	const std::string getPattern() const { return m_pattern; }

private:
	// ��־��ʽģ��
	std::string m_pattern;
	// ��־��ʽ�������ʽ
	std::vector<FormatItem::ptr> m_items;
	// �ж���־��ʽ����
	bool m_error = false;
};

/**
 * @brief ��־���Ŀ��
 * 
 */
class LogAppender{
friend class Logger;
public:
	typedef std::shared_ptr<LogAppender> ptr;
	typedef Spinlock MutexType;
	/**
	 * @brief ��������
	 *
	 */
	virtual ~LogAppender() {}

	/**
	 * @brief д����־
	 * 
	 * @param[in] logger 
	 * @param[in] level 
	 * @param[in] event 
	 */
	virtual void log(std::shared_ptr<Logger> logger
				, LogLevel::Level level, LogEvent::ptr event) = 0;

	/**
	 * @brief 
	*/
	virtual std::string toYamlString() = 0;

	/**
	 * @brief ������־��ʽ��
	 * 
	 * @param val
	 */
	void setFormatter(LogFormatter::ptr val);

	/**
	 * @brief ��ȡ��־��ʽ��
	 * 
	 * @return LogFormatter::ptr 
	 */
	LogFormatter::ptr getFormatter();

	/**
	 * @brief ��ȡ��־����
	 * 
	 * @param val 
	 */
	void setLevel(LogLevel::Level val) { m_level = val; }
protected:
	//��־����
	LogLevel::Level m_level = LogLevel::DEBUG;
	//��־��ʽ��
	LogFormatter::ptr m_formatter;
	// ������
	MutexType m_mutex;
	// �Ƿ���formatter
	bool m_hasFormatter = false;
};

/**
 * @brief ��־��
 * 
 */
class Logger : public std::enable_shared_from_this<Logger>{
friend class LoggerManager;
public:
	typedef std::shared_ptr<Logger> ptr;
	typedef Spinlock MutexType;

	/**
	 * @brief ���캯��
	 * 
	 * @param[in] name ��־������  
	 */
	Logger(const std::string& name = "root");

	/**	
	 * @brief д��־
	 * 
	 * @param[in] level ��־���� 
	 * @param[in] event ��־�¼�
	 */
	void log(LogLevel::Level level, LogEvent::ptr event);

	/**
	 * @brief дdebug��־
	 * 
	 * @param[in] event 
	 */
	void debug(LogEvent::ptr event);

	/**
	 * @brief дinfo��־
	 * 
	 * @param[in] event 
	 */
	void info(LogEvent::ptr event);

	/**
	 * @brief дwarn��־
	 * 
	 * @param[in] event 
	 */
	void warn(LogEvent::ptr event);

	/**
	 * @brief дerror��־
	 * 
	 * @param[in] event 
	 */
	void error(LogEvent::ptr event);

	/**
	 * @brief дfatal��־
	 * 
	 * @param[in] event 
	 */
	void fatal(LogEvent::ptr event);

	/**
	 * @brief �����־Ŀ��
	 * 
	 * @param[in] appender ��־Ŀ�� 
	 */
	void addAppender(LogAppender::ptr appender);

	/**
	 * @brief ɾ����־Ŀ��
	 * 
	 * @param[in] appender ��־Ŀ��ld
	 */
	void delAppender(LogAppender::ptr appender);

	/**
	 * @brief �����־Ŀ�꼯��
	*/
	void clearAppenders();

	/**
	 * @brief ������־����
	 * 
	 * @return LogLevel::Level 
	 */
	LogLevel::Level getLevel() const {return m_level;}

	/**
	 * @brief ������־����
	 * 
	 * @param val 
	 */
	void setLevel(LogLevel::Level val) {m_level = val;}

	/**
	 * @brief ������־����
	 * 
	 * @return const std::string& 
	 */
	const std::string& getName() const { return m_name; }

	/**
	 * @brief ����formatter ͨ������ָ��
	*/
	void setFormatter(LogFormatter::ptr val);

	/**
	 * @brief ����formatter ͨ���ַ���
	*/
	void setFormatter(const std::string &val);

	/**
	 * @brief ���formatter
	*/
	LogFormatter::ptr getFormatter();

	std::string toYamlString();

private:
	//��־����
	std::string m_name;
	//��־����
	LogLevel::Level m_level;
	// ������
	MutexType m_mutex;
	// ��־Ŀ�꼯��
	std::list<LogAppender::ptr> m_appenders;
	//��־��ʽ��
	LogFormatter::ptr m_formatter;
	// root Log
	Logger::ptr m_root;
};

/**
 * @brief ���������̨��Appender
 * 
 */
class StdoutLogAppender : public LogAppender {
public:
	typedef std::shared_ptr<StdoutLogAppender> ptr;
	void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
	std::string toYamlString() override;
};

/**
 * @brief ������ļ���Appender
 * 
 */
class FileLogAppender : public LogAppender {
public:
	typedef std::shared_ptr<FileLogAppender> ptr;
	FileLogAppender(const std::string& filename);
	void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
	std::string toYamlString() override;

	/**
	 * @brief ���´���־�ļ�
	 * 
	 * @return �ɹ����� true
	 * @return false 
	 */
	bool reopen();
private:
	// �ļ�·��
	std::string m_filename;
	// �ļ���
	std::ofstream m_filestream;
	// ÿ��reopenһ�Σ��ж��ļ���û�б�ɾ
	uint64_t m_lastTime = 0;
};

/**
 * @brief ��־������
 * 
 */
class LoggerManager {
public:
	typedef Spinlock MutexType;
	/**
	 * @brief ���캯��
	 *
	 */
	LoggerManager();

	/**
	 * @brief ��ȡ��־��
	 * 
	 * @param[in] name ��־������ 
	 * @return Logger::ptr 
	 */
	Logger::ptr getLogger(const std::string& name);

	/**
	 * @brief ��ʼ��
	 * 
	 */
	void init();

	/**
	 * @brief ��ȡ����־��
	*/
	Logger::ptr getRoot() const { return m_root; }

	std::string toYamlString();

private:
	// ������
	MutexType m_mutex;
	// ��־������
	std::map<std::string, Logger::ptr> m_loggers;
	// ����־��
	Logger::ptr m_root;
};

// ��־�������൥��ģʽ
typedef sylar::Singleton<LoggerManager> LoggerMgr;

}
#endif 

