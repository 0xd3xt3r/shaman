#ifndef H_SYSCALL_HANDLER_H
#define H_SYSCALL_HANDLER_H

#include <unordered_set>
#include <map>
#include <list>
#include <spdlog/spdlog.h>

#include "syscall.hpp"
#include "syscall_x64.hpp"
#include "registers.hpp"
#include "memory.hpp"
#include "debug_opts.hpp"



/**
 * @brief dfd
 * 
 */
struct SyscallTraceData {
	pid_t m_pid;						/* If 0, this tcb is free */
	sysc_id_t sc_id;					/* System call number */
	int64_t v_rval;						/* Return value */
	uint8_t nargs;						/* number of argument */
	uint64_t v_arg[SYSCALL_MAXARGS];	/* System call arguments */
};

enum SyscallState {
	ON_ENTER = 1,
	ON_EXIT = 2
};

/**
 * @brief Get callback for all file related system call 
 *        the tracking id will be file descriptor
 * 
 */
struct FileOperationTracer {

	std::shared_ptr<spdlog::logger> m_log = spdlog::get("main_log");
	
	/**
	 * @brief this function will let you filter the file
	 *        you want to trace, if you want to trace
	 *        all files the return true;
	 * @param debug_opts debugging related API access
	 * @param sc_trace system call parameter
	 * @return true if you want to track the file descriptor
	 * @return false if you want dont want to track the file descriptor
	 */
	virtual bool onFilter(DebugOpts* debug_opts, SyscallTraceData* sc_trace) {
		return true;
	};

	/**
	 * @brief callback on open file event
	 * 
	 */
	virtual void onOpen(SyscallState sys_state, DebugOpts* debug_opts, SyscallTraceData* sc_trace) {
		m_log->error("FT : onOpen");
	};
	virtual void onClose(SyscallState sys_state, DebugOpts* debug_opts, SyscallTraceData* sc_trace) {
		m_log->error("FT : onClose");
	};
	virtual void onRead(SyscallState sys_state, DebugOpts* debug_opts, SyscallTraceData* sc_trace) {
		m_log->error("FT : onRead");
	};
	virtual void onWrite(SyscallState sys_state, DebugOpts* debug_opts, SyscallTraceData* sc_trace) {
		m_log->error("FT : onWrite");
	};
	virtual void onIoctl(SyscallState sys_state, DebugOpts* debug_opts, SyscallTraceData* sc_trace) {
		m_log->error("FT : onIoctl");
	};
};


class SocketOperationTracer {

public:
	virtual void onOpen() = 0;
	virtual void onClose() = 0;
	virtual void onRead() = 0;
	virtual void onWrite() = 0;
	virtual void onIoctl() = 0;
	virtual void onBind() = 0;
};


struct SyscallHandler {

	sysc_id_t syscall_id;
	
	SyscallHandler(sysc_id_t _syscall_id): 
		syscall_id(_syscall_id) {}

	~SyscallHandler();

	virtual int onEnter(DebugOpts* debug_opts, SyscallTraceData* sc_trace) { return 0; };

	virtual int onExit(DebugOpts* debug_opts, SyscallTraceData* sc_trace) { return 0; };

};


class SyscallManager {
	
	// this system call which are related to filer operations
	std::unordered_set<sysc_id_t> file_ops_syscall_id{
		NR_read,
		NR_write,
		// NR_open,
		NR_close,
		NR_ioctl
	};

	// this arguments are preserved between syscall enter and syscall exit
	// arguments should be filled on entry and cleared on exit, Ideal!
	SyscallTraceData* m_cached_args = nullptr;

	SyscallEntry* m_syscall_info = nullptr;

	std::multimap<int, SyscallHandler*> m_syscall_handler_map;

	std::map<int, FileOperationTracer*> m_file_ops_handler;

	std::list<FileOperationTracer*> m_file_ops_pending;

	std::shared_ptr<spdlog::logger> m_log = spdlog::get("main_log");

public:

	SyscallManager(): 
		m_cached_args(new SyscallTraceData()) {}

	~SyscallManager() {
		delete m_cached_args;
	};

	void readParameters(DebugOpts* debug_opts);

	void readRetValue(DebugOpts* debug_opts);

	int handleFileOpt(SyscallState sys_state, DebugOpts* debug_opts);

	virtual int addFileOperationHandler(FileOperationTracer* file_opt_handler);
	virtual int removeFileOperationHandler(FileOperationTracer* file_opt_handler);

	virtual int addSyscallHandler(SyscallHandler* syscall_hdlr);
	virtual int removeSyscallHandler(SyscallHandler* syscall_hdlr);

	virtual int onEnter(DebugOpts* debug_opts);

	virtual int onExit(DebugOpts* debug_opts);

};

#endif