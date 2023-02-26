#ifndef H_TRACEE_H
#define H_TRACEE_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "syscall_mngr.hpp"
#include "memory.hpp"
#include "debugger.hpp"
#include "modules.hpp"
#include "breakpoint_mngr.hpp"
#include "linux_debugger.hpp"
#include "registers.hpp"


enum DebugType {
	DEFAULT        = (1 << 1),
	BREAKPOINT     = (1 << 2),
	FOLLOW_FORK    = (1 << 3),
	SYSCALL        = (1 << 4),
	SINGLE_STEP    = (1 << 5)
};

class Debugger;

class TraceeProgram {

	// this is current state of the tracee
	enum TraceeState {
		// once the tracee is spawned it is assigned this state
		// tracee is then started with the desired ptrace options
		INITIAL_STOP = 1,
		// on the initialization is done it is set in the running
		// state
		RUNNING,
		// tracee is put in this state when it has sent request to
		// kernel and the kernel is processing system call, this 
		// mean syscall enter has already occured
		SYSCALL,
		// the process has existed and object is avaliable to free
		EXITED, 
		// Invalid state, not to be used anywhere!
		UNKNOWN
	} m_state ;

	Debugger* m_debugger;

	DebugOpts* m_debug_opts = nullptr;
	
	SyscallManager* m_syscallMngr = nullptr;
  	std::shared_ptr<spdlog::logger> m_log = spdlog::get("main_log");
  	bool m_followFork = false;
public:

	DebugType debugType;
	BreakpointMngr* m_breakpointMngr;

	// pid of the program we are tracing/debugging
	pid_t getPid() {
		return m_debug_opts->m_pid;
	}
	
	~TraceeProgram () {
	}

	// this is used when new tracee is found
	TraceeProgram(DebugType debug_type):
		m_state(TraceeState::INITIAL_STOP),
		debugType(debug_type) {}
	
	TraceeProgram():
		TraceeProgram(DebugType::DEFAULT) {}


	TraceeProgram& setSyscallMngr(SyscallManager* sys_mngr) {
		m_syscallMngr = sys_mngr;
		return *this;
	};

	TraceeProgram& setBreakpointMngr(BreakpointMngr* brk_mngr) {
		m_breakpointMngr = brk_mngr;
		return *this;
	};

	TraceeProgram& setDebugOpts(DebugOpts* debug_opts) {
		m_debug_opts = debug_opts;
		return *this;
	};

	TraceeProgram& setDebugger(Debugger* debugger) {
		m_debugger = debugger;
		return *this;
	};
	
	TraceeProgram& setPid(pid_t tracee_pid) {
		m_debug_opts->setPid(tracee_pid);
		return *this;
	};

	TraceeProgram& setLogFile(string log_name) {
		auto log_file_name = spdlog::fmt_lib::format("{}_{}.log", log_name, getPid());
		auto log_inst_name = spdlog::fmt_lib::format("tc-{}",getPid());
		m_log = spdlog::basic_logger_mt(log_inst_name, log_file_name);
		return *this;
	};

	TraceeProgram& followFork() {
		m_followFork = true;
		return *this;
	}

	bool isValidState();

	DebugType getChildDebugType();

	bool isInitialized();

	void toStateRunning();

	void toStateSysCall();

	void toStateExited();

	bool hasExited();

	int contExecution(uint32_t sig = 0);

	int singleStep();

	std::string getStateString();

	void printStatus();

	void processPtraceEvent(TraceeEvent event, TrapReason trap_reason);

	void processINITState();

	void processRUNState(TraceeEvent event, TrapReason trap_reason);

	void processSYSCALLState(TraceeEvent event, TrapReason trap_reason);

	void processState(TraceeEvent event, TrapReason trap_reason);

	void addPendingBrkPnt(std::vector<std::string>& brk_pnt_str);
	
	void addSyscallHandler(SyscallHandler* syscall_hdlr) {
		m_syscallMngr->addSyscallHandler(syscall_hdlr);
	};

	void addFileOperationHandler(FileOperationTracer* file_opts) {
		m_syscallMngr->addFileOperationHandler(file_opts);
	};

};


class TraceeFactory {

	Debugger* m_debugger;
	std::list<TraceeProgram *> m_cached_tracee;

public:
	
	// this function fills the cache with dummy tracees
	// this will reduce the creation time
	void createDummyTracee();

	TraceeProgram* createTracee(pid_t tracee_pid, DebugType debug_type);
	void releaseTracee(TraceeProgram* tracee_obj);
};

#endif