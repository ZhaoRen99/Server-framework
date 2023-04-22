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
 * @brief 使用流式方式将日志级别level的日志写入到logger
 * 
 */
#define SYLAR_LOG_LEVEL(logger, level) \
	if (logger->getLevel() <= level) \
		sylar::LogEventWarp(sylar::LogEvent::ptr (new sylar::LogEvent(logger, level, \
				__FILE__, __LINE__, 0, sylar::GetThreadId(), \
			sylar::GetFiberId(), time(0), sylar::Thread::GetName()))).getSS()

/**
 * @brief 使用流式方式将日志级别debug的日志写入到logger
 * 
 */
#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)

/**
 * @brief 使用流式方式将日志级别info的日志写入到logger
 * 
 */
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)

/**
 * @brief 使用流式方式将日志级别warn的日志写入到logger
 * 
 */
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)

/**
 * @brief 使用流式方式将日志级别error的日志写入到logger
 * 
 */
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)

/**
 * @brief 使用流式方式将日志级别fatal的日志写入到logger
 * 
 */
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

/**
 * @brief 使用格式化方式将日志级别level的日志写入到logger
 * 
 */
#define SYLARY_LOG_FMT_LEVEL(logger, level, fmt, ...) \
	if (logger->getLevel() <= level) \
		sylar::LogEventWarp(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, \
							__FILE__, __LINE__, 0, sylar::GetThreadId(), \
					sylar::GetFiberId(), time(0), sylar::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别debug的日志写入到logger
 * 
 */
#define SYLARY_LOG_FMT_DEBUG(logger, fmt, ...) SYLARY_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别info的日志写入到logger
 * 
 */
#define SYLARY_LOG_FMT_INFO(logger, fmt, ...) SYLARY_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别warn的日志写入到logger
 * 
 */
#define SYLARY_LOG_FMT_WARN(logger, fmt, ...) SYLARY_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别error的日志写入到logger
 * 
 */
#define SYLARY_LOG_FMT_ERROR(logger, fmt, ...) SYLARY_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别fatal的日志写入到logger
 * 
 */
#define SYLARY_LOG_FMT_FATAL(logger, fmt, ...) SYLARY_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

/**
 * @brief 获得默认root Log
*/
#define SYLAR_LOG_ROOT() sylar::LoggerMgr::GetInstance()->getRoot()

/**
 * @brief 获得 name Log
*/
#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::GetInstance()->getLogger(name)

#define SYLAR__ROOT__LOG(name) sylar::Logger::ptr name  = SYLAR_LOG_ROOT()

#define SYLAR__SYSTEM__LOG(name) sylar::Logger::ptr name = SYLAR_LOG_NAME("system")


namespace sylar{

class Logger;

/**
 * @brief 日志级别
 * 
 */
class LogLevel{
public:
	enum Level{
		//	未知 级别
		UNKNOW = 0,
		//	DEBUG 级别
		DEBUG = 1,
		//	INFO 级别
		INFO = 2,
		//	WARN 级别
		WARN = 3,
		//	ERROR 级别
		ERROR = 4,
		//	FATAL 级别
		FATAL = 5
	};

	/**
	 * @brief 将日志级别转成文本输出
	 * 
	 * @param[in] level 日志级别
	 * @return const char* 
	 * 
	 * @details { ERROR：没有对象无法调用成员函数 } 成员函数需要对象调用，所以声明为static
	 * @details 静态成员函数在编译前就开辟空间  直接使用 Level::ToString 调用
	 */
	static const char* ToString(LogLevel::Level level);

	static LogLevel::Level FromString(const std::string &str);
};

/**
 * @brief  日志事件
 * 
 */
class LogEvent{
public:
	typedef std::shared_ptr<LogEvent> ptr;
	/**
	 * @brief 构造函数
	 * 
	 * @param[in] logger 日志器
	 * @param[in] level 日志级别
	 * @param[in] file 文件名
	 * @param[in] line 文件行号
	 * @param[in] elapse 程序启动依赖的耗时（毫秒）
	 * @param[in] thread_id 线程id
	 * @param[in] fiber_id 协程id
	 * @param[in] time 日志时间
	 */
	LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
			, const char* file, int32_t line, uint32_t elapse
			, uint32_t thread_id, uint32_t fiber_id, uint64_t time
			, const std::string& thread_name);
	/**
	 * @brief 返回文件名
	 * 
	 * @return const char* 
	 */
	const char* getFile() const { return m_file; }

	/**
	 * @brief 返回行号
	 * 
	 * @return int32_t 
	 */
	int32_t getLine() const { return m_line; }

	/**
	 * @brief 返回耗时
	 * 
	 * @return uint32_t 
	 */
	uint32_t getElapse() const { return m_elapse; }

	/**
	 * @brief 返回线程ID
	 * 
	 * @return uint32_t 
	 */
	uint32_t getThieadId() const { return m_thieadId; }

	/**
	 * @brief 返回协程ID
	 * 
	 * @return uint32_t 
	 */
	uint32_t getFiberId() const { return m_fiberId; }

	/**
	 * @brief 返回时间
	 * 
	 * @return uint64_t 
	 */
	uint64_t getTime() const { return m_time; }

	/**
	 * @brief 返回线程名称
	*/
	std::string getThreadName() const { return m_threadName; }

	/**
	 * @brief 返回日志内容
	 * 
	 * @return std::string 
	 */
	std::string getContent() const { return m_ss.str(); }

	/**
	 * @brief 返回日志器
	 * 
	 * @return std::shared_ptr<Logger> 
	 */
	std::shared_ptr<Logger> getLogger() const { return m_logger; }

	/**
	 * @brief 返回日志级别
	 * 
	 * @return LogLevel::Level 
	 */
	LogLevel::Level getLevel() const { return m_level; }

	/**
	 * @brief 返回日志内容字符串流
	 * 
	 * @return std::stringstream& 
	 */
	std::stringstream& getSS() { return m_ss; }

	/**
	 * @brief 格式化写入日志内容
	 * 
	 * @param fmt 
	 * @param ... 
	 */
	void format(const char* fmt, ...);

	/**
	 * @brief 格式化写入日志内容
	 * 
	 * @param fmt 
	 * @param al 
	 */
	void format(const char* fmt, va_list al);

private:
	const char* m_file = nullptr;	//文件名
	int32_t m_line = 0;  	  	//行号
	uint32_t m_elapse = 0;		//程序启动开始到现在的毫秒数
	uint32_t m_thieadId = 0;  	//线程id
	uint32_t m_fiberId = 0;   	//协程id
	uint64_t m_time;         	//时间戳
	std::string m_threadName;	//线程名称
	std::stringstream m_ss;    	//日志内容流
	std::shared_ptr<Logger> m_logger;	//日志器
	LogLevel::Level m_level;	//日志等级
};

/**
 * @brief 日志事件包装器
 * 
 */
class LogEventWarp {
public:
	/**
	 * @brief 构造函数
	 * 
	 * @param[in] e 日志事件
	 */
	LogEventWarp(LogEvent::ptr e);
	
	/**
	 * @brief 析构函数
	 * 
	 */
	~LogEventWarp();

	/**
	 * @brief 获取日志事件
	 * 
	 * @return LogEvent::ptr 
	 */
	LogEvent::ptr getEvent() const { return m_event; }

	/**
	 * @brief 获取日志内容流
	 * 
	 * @return std::stringstream& 
	 */
	std::stringstream& getSS();
private:
	LogEvent::ptr m_event;
};

/**
 * @brief 日志格式化
 * 
 */
class LogFormatter{
public:
	typedef std::shared_ptr<LogFormatter> ptr;
	/**
	 * @brief 构造函数
	 * 
	 * @param pattern[in] pattern 格式模板
	 * 
	 * @details 
     *  %m 消息
     *  %p 日志级别
     *  %r 累计毫秒数
     *  %c 日志名称
     *  %t 线程id
     *  %n 换行
     *  %d 时间
     *  %f 文件名
     *  %l 行号
     *  %T 制表符
     *  %F 协程id
     *  %N 线程名称
     *
     *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
     */
	LogFormatter(const std::string& pattern);
	
	/**
	 * @brief 返回格式化日志文本
	 * 
	 * @param[in] logger 日志器
	 * @param[in] level 日志级别
	 * @param[in] event 日志事件
	 * @return std::string 
	 */
	std::string format (std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
	/**
	 * @brief 日志内容格式化
	 * 
	 */
	class FormatItem{
	public:
		typedef std::shared_ptr<FormatItem> ptr;
		/**
		 * @brief 析构函数
		 * 
		 */
		virtual ~FormatItem() {}
		/**
		 * @brief 格式化日志到流
		 * 
		 * @param[in, out] os 日志输出流
		 * @param[in] logger 日志器
		 * @param[in] level 日志等级
		 * @param[in] event 日志事件
		 */
		virtual void format(std::ostream& os, std::shared_ptr<Logger> logger
							, LogLevel::Level level, LogEvent::ptr event) = 0;
	};
	/**
	 * @brief 初始化，解析日志模板
	 * 
	 */
	void init();

	bool isError() const { return m_error; }

	const std::string getPattern() const { return m_pattern; }

private:
	// 日志格式模板
	std::string m_pattern;
	// 日志格式解析后格式
	std::vector<FormatItem::ptr> m_items;
	// 判断日志格式错误
	bool m_error = false;
};

/**
 * @brief 日志输出目标
 * 
 */
class LogAppender{
friend class Logger;
public:
	typedef std::shared_ptr<LogAppender> ptr;
	typedef Spinlock MutexType;
	/**
	 * @brief 析构函数
	 *
	 */
	virtual ~LogAppender() {}

	/**
	 * @brief 写入日志
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
	 * @brief 更改日志格式器
	 * 
	 * @param val
	 */
	void setFormatter(LogFormatter::ptr val);

	/**
	 * @brief 获取日志格式器
	 * 
	 * @return LogFormatter::ptr 
	 */
	LogFormatter::ptr getFormatter();

	/**
	 * @brief 获取日志级别
	 * 
	 * @param val 
	 */
	void setLevel(LogLevel::Level val) { m_level = val; }
protected:
	//日志级别
	LogLevel::Level m_level = LogLevel::DEBUG;
	//日志格式器
	LogFormatter::ptr m_formatter;
	// 互斥锁
	MutexType m_mutex;
	// 是否有formatter
	bool m_hasFormatter = false;
};

/**
 * @brief 日志器
 * 
 */
class Logger : public std::enable_shared_from_this<Logger>{
friend class LoggerManager;
public:
	typedef std::shared_ptr<Logger> ptr;
	typedef Spinlock MutexType;

	/**
	 * @brief 构造函数
	 * 
	 * @param[in] name 日志器名称  
	 */
	Logger(const std::string& name = "root");

	/**	
	 * @brief 写日志
	 * 
	 * @param[in] level 日志级别 
	 * @param[in] event 日志事件
	 */
	void log(LogLevel::Level level, LogEvent::ptr event);

	/**
	 * @brief 写debug日志
	 * 
	 * @param[in] event 
	 */
	void debug(LogEvent::ptr event);

	/**
	 * @brief 写info日志
	 * 
	 * @param[in] event 
	 */
	void info(LogEvent::ptr event);

	/**
	 * @brief 写warn日志
	 * 
	 * @param[in] event 
	 */
	void warn(LogEvent::ptr event);

	/**
	 * @brief 写error日志
	 * 
	 * @param[in] event 
	 */
	void error(LogEvent::ptr event);

	/**
	 * @brief 写fatal日志
	 * 
	 * @param[in] event 
	 */
	void fatal(LogEvent::ptr event);

	/**
	 * @brief 添加日志目标
	 * 
	 * @param[in] appender 日志目标 
	 */
	void addAppender(LogAppender::ptr appender);

	/**
	 * @brief 删除日志目标
	 * 
	 * @param[in] appender 日志目标ld
	 */
	void delAppender(LogAppender::ptr appender);

	/**
	 * @brief 清空日志目标集合
	*/
	void clearAppenders();

	/**
	 * @brief 返回日志级别
	 * 
	 * @return LogLevel::Level 
	 */
	LogLevel::Level getLevel() const {return m_level;}

	/**
	 * @brief 设置日志级别
	 * 
	 * @param val 
	 */
	void setLevel(LogLevel::Level val) {m_level = val;}

	/**
	 * @brief 返回日志名称
	 * 
	 * @return const std::string& 
	 */
	const std::string& getName() const { return m_name; }

	/**
	 * @brief 设置formatter 通过智能指针
	*/
	void setFormatter(LogFormatter::ptr val);

	/**
	 * @brief 设置formatter 通过字符串
	*/
	void setFormatter(const std::string &val);

	/**
	 * @brief 获得formatter
	*/
	LogFormatter::ptr getFormatter();

	std::string toYamlString();

private:
	//日志名称
	std::string m_name;
	//日志级别
	LogLevel::Level m_level;
	// 互斥锁
	MutexType m_mutex;
	// 日志目标集合
	std::list<LogAppender::ptr> m_appenders;
	//日志格式器
	LogFormatter::ptr m_formatter;
	// root Log
	Logger::ptr m_root;
};

/**
 * @brief 输出到控制台的Appender
 * 
 */
class StdoutLogAppender : public LogAppender {
public:
	typedef std::shared_ptr<StdoutLogAppender> ptr;
	void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
	std::string toYamlString() override;
};

/**
 * @brief 输出到文件的Appender
 * 
 */
class FileLogAppender : public LogAppender {
public:
	typedef std::shared_ptr<FileLogAppender> ptr;
	FileLogAppender(const std::string& filename);
	void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
	std::string toYamlString() override;

	/**
	 * @brief 重新打开日志文件
	 * 
	 * @return 成功返回 true
	 * @return false 
	 */
	bool reopen();
private:
	// 文件路径
	std::string m_filename;
	// 文件流
	std::ofstream m_filestream;
	// 每秒reopen一次，判断文件有没有被删
	uint64_t m_lastTime = 0;
};

/**
 * @brief 日志管理器
 * 
 */
class LoggerManager {
public:
	typedef Spinlock MutexType;
	/**
	 * @brief 构造函数
	 *
	 */
	LoggerManager();

	/**
	 * @brief 获取日志器
	 * 
	 * @param[in] name 日志器名称 
	 * @return Logger::ptr 
	 */
	Logger::ptr getLogger(const std::string& name);

	/**
	 * @brief 初始化
	 * 
	 */
	void init();

	/**
	 * @brief 获取主日志器
	*/
	Logger::ptr getRoot() const { return m_root; }

	std::string toYamlString();

private:
	// 互斥锁
	MutexType m_mutex;
	// 日志器容器
	std::map<std::string, Logger::ptr> m_loggers;
	// 主日志器
	Logger::ptr m_root;
};

// 日志器管理类单列模式
typedef sylar::Singleton<LoggerManager> LoggerMgr;

}
#endif 

