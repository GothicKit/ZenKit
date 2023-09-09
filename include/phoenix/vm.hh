// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/DaedalusVm.hh"

#include "phoenix/script.hh"

namespace phoenix {
	namespace execution_flag = zenkit::DaedalusVmExecutionFlag;

	using illegal_external_definition
	    ZKREM("renamed to zenkit::DaedalusIllegalExternalDefinition") = zenkit::DaedalusIllegalExternalDefinition;
	using illegal_external_rtype
	    ZKREM("renamed to zenkit::DaedalusIllegalExternalReturnType") = zenkit::DaedalusIllegalExternalReturnType;
	using illegal_external_param
	    ZKREM("renamed to zenkit::DaedalusIllegalExternalParameter") = zenkit::DaedalusIllegalExternalParameter;
	using vm_exception ZKREM("renamed to zenkit::DaedalusVmException") = zenkit::DaedalusVmException;
	using vm_exception_strategy
	    ZKREM("renamed to zenkit::DaedalusVmExceptionStrategy") = zenkit::DaedalusVmExceptionStrategy;
	using daedalus_stack_frame ZKREM("renamed to zenkit::DaedalusStackFrame") = zenkit::DaedalusStackFrame;
	using daedalus_call_stack_frame ZKREM("renamed to zenkit::DaedalusCallStackFrame") = zenkit::DaedalusCallStackFrame;
	using vm ZKREM("renamed to zenkit::DaedalusVm") = zenkit::DaedalusVm;
	using func ZKREM("renamed to zenkit::DaedalusFunction") = zenkit::DaedalusFunction;
	using naked_call ZKREM("renamed to zenkit::DaedalusNakedCall") = zenkit::DaedalusNakedCall;

	inline zenkit::DaedalusVmExceptionStrategy lenient_vm_exception_handler(zenkit::DaedalusVm& v,
	                                                                        zenkit::DaedalusScriptError const& exc,
	                                                                        zenkit::DaedalusInstruction const& instr) {
		return zenkit::lenient_vm_exception_handler(v, exc, instr);
	}
} // namespace phoenix
