// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/DaedalusScript.hh"

namespace phoenix {
	namespace symbol_flag = zenkit::DaedalusSymbolFlag;

	using datatype ZKREM("renamed to zenkit::DaedalusDataType") = zenkit::DaedalusDataType;
	using opcode ZKREM("renamed to zenkit::DaedalusOpcode") = zenkit::DaedalusOpcode;
	using symbol ZKREM("renamed to zenkit::DaedalusSymbol") = zenkit::DaedalusSymbol;
	using instance ZKREM("renamed to zenkit::DaedalusInstance") = zenkit::DaedalusInstance;
	using script_error ZKREM("renamed to zenkit::DaedalusScriptError") = zenkit::DaedalusScriptError;
	using symbol_not_found ZKREM("renamed to zenkit::DaedalusSymbolNotFound") = zenkit::DaedalusSymbolNotFound;
	using member_registration_error
	    ZKREM("renamed to zenkit::DaedalusMemberRegistrationError") = zenkit::DaedalusMemberRegistrationError;
	using invalid_registration_datatype
	    ZKREM("renamed to zenkit::DaedalusInvalidRegistrationDataType") = zenkit::DaedalusInvalidRegistrationDataType;
	using illegal_access ZKREM("renamed to zenkit::DaedalusIllegalAccess") = zenkit::DaedalusIllegalAccess;
	using illegal_type_access ZKREM("renamed to zenkit::DaedalusIllegalTypeAccess") = zenkit::DaedalusIllegalTypeAccess;
	using illegal_index_access
	    ZKREM("renamed to zenkit::DaedalusIllegalIndexAccess") = zenkit::DaedalusIllegalIndexAccess;
	using illegal_const_access
	    ZKREM("renamed to zenkit::DaedalusIllegalConstAccess") = zenkit::DaedalusIllegalConstAccess;
	using illegal_instance_access
	    ZKREM("renamed to zenkit::DaedalusIllegalInstanceAccess") = zenkit::DaedalusIllegalInstanceAccess;
	using unbound_member_access
	    ZKREM("renamed to zenkit::DaedalusUnboundMemberAccess") = zenkit::DaedalusUnboundMemberAccess;
	using no_context ZKREM("renamed to zenkit::DaedalusNoContextError") = zenkit::DaedalusNoContextError;
	using illegal_context_type
	    ZKREM("renamed to zenkit::DaedalusIllegalContextType") = zenkit::DaedalusIllegalContextType;
	using instruction ZKREM("renamed to zenkit::DaedalusInstruction") = zenkit::DaedalusInstruction;
	using script ZKREM("renamed to zenkit::DaedalusScript") = zenkit::DaedalusScript;
	using transient_instance ZKREM("renamed to zenkit::DaedalusTransientInstance") = zenkit::DaedalusTransientInstance;
	using opaque_instance ZKREM("renamed to zenkit::DaedalusOpaqueInstance") = zenkit::DaedalusOpaqueInstance;
} // namespace phoenix
