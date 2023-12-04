#include "zenkit/Object.hh"

namespace zenkit {
	ObjectType Object::get_object_type() const {
		return ObjectType::unknown;
	}

	void Object::load(ReadArchive&, GameVersion) {}
} // namespace zenkit