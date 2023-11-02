#include "breakpoint.hpp"
#include "amd64_breakpoint_inject.hpp"

Breakpoint::Breakpoint(std::string& modname, uintptr_t offset, uintptr_t bk_addr,
    std::string* _label, BreakpointType brk_type) :
    m_modname(modname), m_offset(offset), m_type(brk_type) {
        if(_label == nullptr) {
            m_label = spdlog::fmt_lib::format("{}@{:x}", m_modname.c_str(), offset);
        }
        m_bkpt_injector = new X86BreakpointInjector();
    };


int Breakpoint::enable(DebugOpts& debug_opts) {
    m_bkpt_injector->inject(debug_opts, m_backupData);
    m_enabled = true;
    return 1;
};

int Breakpoint::disable(DebugOpts& debug_opts) {
    m_bkpt_injector->restore(debug_opts, m_backupData);
    m_enabled = false;
    return 1;
};