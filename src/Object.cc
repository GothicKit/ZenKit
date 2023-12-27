#include "zenkit/Object.hh"

namespace zenkit {
	ObjectType Object::get_object_type() const {
		return ObjectType::unknown;
	}

	uint16_t Object::get_version_identifier(GameVersion) const {
		return 0;
	}

	void Object::load(ReadArchive&, GameVersion) {}

	void Object::save(WriteArchive&, GameVersion) const {}
} // namespace zenkit