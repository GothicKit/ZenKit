// Copyright © 2022-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Date.hh"
#include "zenkit/Stream.hh"

namespace zenkit {
	void Date::load(Read* r) {
		this->year = r->read_uint();
		this->month = r->read_ushort();
		this->day = r->read_ushort();
		this->hour = r->read_ushort();
		this->minute = r->read_ushort();
		this->second = r->read_ushort();
		(void) r->read_ushort(); // padding
	}

	void Date::save(Write* w) const {
		w->write_uint(this->year);
		w->write_ushort(this->month);
		w->write_ushort(this->day);
		w->write_ushort(this->hour);
		w->write_ushort(this->minute);
		w->write_ushort(this->second);
		w->write_ushort(0); // padding
	}
} // namespace zenkit
