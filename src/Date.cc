// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Date.hh"
#include "zenkit/Stream.hh"

#include "phoenix/buffer.hh"

namespace zenkit {
	Date Date::parse(phoenix::buffer& buf) {
		Date dt {};

		auto r = Read::from(&buf);
		dt.load(r.get());

		return dt;
	}

	void Date::load(Read* r) {
		this->year = r->read_uint();
		this->month = r->read_ushort();
		this->day = r->read_ushort();
		this->hour = r->read_ushort();
		this->minute = r->read_ushort();
		this->second = r->read_ushort();
		(void) r->read_ushort(); // padding
	}
} // namespace zenkit
