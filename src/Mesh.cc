// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Mesh.hh"
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"

#include "Internal.hh"

namespace zenkit {
	[[maybe_unused]] static constexpr auto MESH_VERSION_G1 = 9;
	static constexpr auto MESH_VERSION_G2 = 265;

	enum class MeshChunkType : std::uint16_t {
		MARKER = 0xB000,
		BBOX = 0xB010,
		MATERIAL = 0xB020,
		LIGHTMAPS = 0xB025,
		LIGHTMAPS_SHARED = 0xB026,
		VERTICES = 0xB030,
		FEATURES = 0xB040,
		POLYGONS = 0xB050,
		END = 0xB060
	};

	Mesh Mesh::parse(phoenix::buffer& buf, std::vector<std::uint32_t> const& leaf_polygons, bool force_wide_indices) {
		Mesh msh {};

		auto r = Read::from(&buf);
		msh.load(r.get(), leaf_polygons, force_wide_indices);

		return msh;
	}

	bool PolygonFlagSet::operator==(PolygonFlagSet const& b) const {
		return is_portal == b.is_portal && is_occluder == b.is_occluder && is_sector == b.is_sector &&
		    should_relight == b.should_relight && is_outdoor == b.is_outdoor &&
		    is_ghost_occluder == b.is_ghost_occluder && is_dynamically_lit == b.is_dynamically_lit &&
		    sector_index == b.sector_index && is_lod == b.is_lod && normal_axis == b.normal_axis;
	}

	Mesh Mesh::parse(phoenix::buffer&& buf, std::vector<std::uint32_t> const& include_polygons) {
		return Mesh::parse(buf, include_polygons);
	}

	void Mesh::load(Read* r, std::vector<std::uint32_t> const& leaf_polygons, bool force_wide_indices) {
		this->load(r, force_wide_indices);
		this->triangulate(leaf_polygons);
	}

	void Mesh::load(Read* r, bool force_wide_indices) {
		std::uint16_t version {};

		proto::read_chunked<MeshChunkType>(
		    r,
		    "Mesh",
		    [this, &version, force_wide_indices](Read* c, MeshChunkType type) {
			    switch (type) {
			    case MeshChunkType::MARKER:
				    version = c->read_ushort();
				    this->date.load(c);
				    this->name = c->read_line(false);
				    break;
			    case MeshChunkType::BBOX:
				    this->bbox.load(c);
				    this->obb.load(c);
				    break;
			    case MeshChunkType::MATERIAL: {
				    auto matreader = ReadArchive::from(c);

				    this->materials.resize(c->read_uint());
				    for (auto& material : this->materials) {
					    material.load(*matreader);
				    }

				    break;
			    }
			    case MeshChunkType::VERTICES:
				    this->vertices.resize(c->read_uint());

				    for (auto& vertex : this->vertices) {
					    vertex = c->read_vec3();
				    }

				    break;
			    case MeshChunkType::FEATURES:
				    this->features.resize(c->read_uint());

				    for (auto& feature : this->features) {
					    feature.texture = c->read_vec2();
					    feature.light = c->read_uint();
					    feature.normal = c->read_vec3();
				    }

				    break;
			    case MeshChunkType::POLYGONS: {
				    auto poly_count = c->read_uint();
				    this->geometry.resize(poly_count);

				    for (std::uint32_t i = 0; i < poly_count; ++i) {
					    this->geometry[i].material = c->read_ushort();
					    this->geometry[i].lightmap = c->read_short();

					    // TODO(lmichaelis): Figure out what these do.
					    (void) c->read_float();
					    (void) c->read_vec3();

					    PolygonFlagSet& pflags = this->geometry[i].flags;
					    if (version == MESH_VERSION_G2) {
						    std::uint8_t flags = c->read_ubyte();
						    pflags.is_portal = (flags & 0b00000011) >> 0;
						    pflags.is_occluder = (flags & 0b00000100) >> 2;
						    pflags.is_sector = (flags & 0b00001000) >> 3;
						    pflags.should_relight = (flags & 0b00010000) >> 4;
						    pflags.is_outdoor = (flags & 0b00100000) >> 5;
						    pflags.is_ghost_occluder = (flags & 0b01000000) >> 6;
						    pflags.is_dynamically_lit = (flags & 0b10000000) >> 7;
						    pflags.sector_index = c->read_short();
					    } else {
						    std::uint8_t flags1 = c->read_ubyte();
						    std::uint8_t flags2 = c->read_ubyte();

						    pflags.is_portal = (flags1 & 0b00000011) >> 0;
						    pflags.is_occluder = (flags1 & 0b00000100) >> 2;
						    pflags.is_sector = (flags1 & 0b00001000) >> 3;
						    pflags.is_lod = (flags1 & 0b00010000) >> 4;
						    pflags.is_outdoor = (flags1 & 0b00100000) >> 5;
						    pflags.is_ghost_occluder = (flags1 & 0b01000000) >> 6;
						    pflags.normal_axis = ((flags1 & 0b10000000) >> 7) | (flags2 & 0b00000001);
						    pflags.sector_index = c->read_short();
					    }

					    auto vertex_count = c->read_ubyte();
					    auto has_wide_indices = (version == MESH_VERSION_G2) || force_wide_indices;

					    for (int32_t j = 0; j < vertex_count; ++j) {
						    this->geometry[i].vertices.push_back(has_wide_indices ? c->read_uint() : c->read_ushort());
						    this->geometry[i].features.push_back(c->read_uint());
					    }
				    }

				    break;
			    }
			    case MeshChunkType::LIGHTMAPS_SHARED: {
				    auto texture_count = c->read_uint();

				    std::vector<std::shared_ptr<Texture>> lightmap_textures {};
				    lightmap_textures.resize(texture_count);

				    for (std::uint32_t i = 0; i < texture_count; ++i) {
					    lightmap_textures[i] = std::make_shared<Texture>();
					    lightmap_textures[i]->load(c);
				    }

				    auto lightmap_count = c->read_uint();
				    for (std::uint32_t i = 0; i < lightmap_count; ++i) {
					    auto origin = c->read_vec3();
					    auto normal_a = c->read_vec3();
					    auto normal_b = c->read_vec3();
					    std::uint32_t texture_index = c->read_uint();

					    this->lightmaps.emplace_back(
					        LightMap {lightmap_textures[texture_index], {normal_a, normal_b}, origin});
				    }

				    break;
			    }
			    case MeshChunkType::LIGHTMAPS: {
				    auto lightmap_count = c->read_uint();

				    for (std::uint32_t i = 0; i < lightmap_count; ++i) {
					    auto origin = c->read_vec3();
					    auto normal_a = c->read_vec3();
					    auto normal_b = c->read_vec3();

					    Texture lightmap_texture {};
					    lightmap_texture.load(c);

					    this->lightmaps.emplace_back(LightMap {std::make_shared<Texture>(std::move(lightmap_texture)),
					                                           {normal_a, normal_b},
					                                           origin});
				    }

				    break;
			    }
			    case MeshChunkType::END:
				    return true;
			    default:
				    break;
			    }

			    return false;
		    });
	}

	void Mesh::triangulate(std::vector<std::uint32_t> const& leaf_polygons) {
		this->polygons.material_indices.reserve(leaf_polygons.size());
		this->polygons.lightmap_indices.reserve(leaf_polygons.size());
		this->polygons.feature_indices.reserve(leaf_polygons.size() * 4);
		this->polygons.vertex_indices.reserve(leaf_polygons.size() * 4);
		this->polygons.flags.reserve(leaf_polygons.size());

		for (auto i = 0u; i < this->geometry.size(); ++i) {
			if (!std::binary_search(leaf_polygons.begin(), leaf_polygons.end(), i)) {
				continue;
			}

			auto& polygon = this->geometry[i];
			if (polygon.vertices.size() < 3 || polygon.flags.is_portal || polygon.flags.is_ghost_occluder ||
			    polygon.flags.is_outdoor) {
				continue;
			}

			auto root = 0u;
			auto a = 1u;

			// NOTE(lmichaelis): This unpacks triangle fans
			for (auto b = 2u; b < polygon.vertices.size(); ++b) {
				this->polygons.vertex_indices.push_back(polygon.vertices[root]);
				this->polygons.vertex_indices.push_back(polygon.vertices[a]);
				this->polygons.vertex_indices.push_back(polygon.vertices[b]);
				this->polygons.feature_indices.push_back(polygon.features[root]);
				this->polygons.feature_indices.push_back(polygon.features[a]);
				this->polygons.feature_indices.push_back(polygon.features[b]);
				this->polygons.material_indices.push_back(polygon.material);
				this->polygons.lightmap_indices.push_back(polygon.lightmap);
				this->polygons.flags.push_back(polygon.flags);
				a = b;
			}
		}
	}

	void Mesh::save(Write* w, GameVersion version) const {
		proto::write_chunk(w, MeshChunkType::MARKER, [this, version](Write* c) {
			c->write_ushort(version == GameVersion::GOTHIC_1 ? MESH_VERSION_G1 : MESH_VERSION_G2);
			this->date.save(c);
			c->write_line(this->name);
		});

		proto::write_chunk(w, MeshChunkType::BBOX, [this](Write* c) {
			this->bbox.save(c);
			this->obb.save(c);
		});

		proto::write_chunk(w, MeshChunkType::MATERIAL, [this, version](Write* c) {
			auto war = WriteArchive::to(c, ArchiveFormat::BINARY);
			c->write_uint(static_cast<uint32_t>(this->materials.size()));

			for (auto& mat : materials) {
				war->write_string("", mat.name);
				war->write_object("%", &mat, version);
			}
		});

		proto::write_chunk(w, MeshChunkType::VERTICES, [this](Write* c) {
			c->write_uint(static_cast<uint32_t>(this->vertices.size()));

			for (auto& v : this->vertices) {
				c->write_vec3(v);
			}
		});

		proto::write_chunk(w, MeshChunkType::FEATURES, [this](Write* c) {
			c->write_uint(static_cast<uint32_t>(this->features.size()));

			for (auto& feat : this->features) {
				c->write_vec2(feat.texture);
				c->write_uint(feat.light);
				c->write_vec3(feat.normal);
			}
		});

		proto::write_chunk(w, MeshChunkType::POLYGONS, [this, version](Write* c) {
			c->write_uint(this->geometry.size());

			for (auto& poly : this->geometry) {
				c->write_uint(poly.material);
				c->write_uint(static_cast<uint32_t>(poly.lightmap));

				// TODO(lmichaelis): Figure these out.
				c->write_float(0);
				c->write_vec3({0, 0, 0});

				if (version == GameVersion::GOTHIC_1) {
					auto& flags = poly.flags;
					c->write_ubyte((flags.is_portal & 3) | ((flags.is_occluder & 1) << 2) |
					               ((flags.is_sector & 1) << 3) | ((flags.should_relight & 1) << 4) |
					               ((flags.is_outdoor & 1) << 5) | ((flags.is_ghost_occluder & 1) << 6) |
					               ((flags.is_dynamically_lit & 1) << 7));
					c->write_short(flags.sector_index);
				} else {
					auto& flags = poly.flags;
					c->write_ubyte((flags.is_portal & 3) | ((flags.is_occluder & 1) << 2) |
					               ((flags.is_sector & 1) << 3) | ((flags.is_lod & 1) << 4) |
					               ((flags.is_outdoor & 1) << 5) | ((flags.is_ghost_occluder & 1) << 6) |
					               ((flags.normal_axis & 1) << 7));
					c->write_ubyte(flags.normal_axis & 2);
					c->write_short(flags.sector_index);
				}

				c->write_ubyte(poly.vertices.size());
				for (auto i = 0u; i < poly.vertices.size(); ++i) {
					if (version == GameVersion::GOTHIC_1) {
						c->write_ushort(this->polygons.vertex_indices[i]);
						c->write_uint(this->polygons.feature_indices[i]);
					} else {
						c->write_uint(this->polygons.vertex_indices[i]);
						c->write_uint(this->polygons.feature_indices[i]);
					}
				}
			}
		});

		// TODO(lmichaelis): Fixup  lightmaps. We need to figure out all lightmaps which share a texture.
		proto::write_chunk(w, MeshChunkType::LIGHTMAPS_SHARED, [](Write* c) { c->write_uint(0); });
		proto::write_chunk(w, MeshChunkType::LIGHTMAPS, [](Write* c) { c->write_uint(0); });
		proto::write_chunk(w, MeshChunkType::END, [](Write*) {});
	}
} // namespace zenkit
