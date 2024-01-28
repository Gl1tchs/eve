#include "scene/scene.h"

#include "core/json_utils.h"
#include "core/uid.h"
#include "nlohmann/json.hpp"
#include "renderer/font.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/transform.h"

Scene::Scene(const char* name) :
		name(name) {
}

void Scene::start() {
	running = true;
}

void Scene::update(float dt) {
	if (paused && step_frames-- <= 0) {
		return;
	}
}

void Scene::stop() {
	running = false;
}

void Scene::set_paused(bool p_paused) {
	paused = p_paused;
}

void Scene::step(uint32_t frames) {
	step_frames = frames;
}

bool Scene::is_running() {
	return running;
}

bool Scene::is_paused() {
	return paused;
}

Entity Scene::create(const std::string& name, UID parent_id) {
	return create(UID(), name, parent_id);
}

Entity Scene::create(UID uid, const std::string& name, UID parent_id) {
	Entity entity{ registry.create(), this };

	entity.add_component<IdComponent>(uid, name);
	entity.add_component<TransformComponent>();
	entity.add_component<RelationComponent>();

	if (parent_id) {
		Entity parent = find_by_id(parent_id);
		if (parent) {
			entity.set_parent(parent);
		}
	}

	entity_map[uid] = entity;

	return entity;
}

void Scene::destroy(Entity entity) {
	for (auto child : entity.get_children()) {
		destroy(child);
	}

	entity_map.erase(entity.get_uid());
	registry.destroy(entity);
}

void Scene::destroy(UID uid) {
	Entity entity = find_by_id(uid);
	if (!entity) {
		return;
	}

	destroy(entity);
}

bool Scene::exists(UID uid) const {
	return entity_map.find(uid) != entity_map.end();
}

Entity Scene::find_by_id(UID uid) {
	if (entity_map.find(uid) != entity_map.end()) {
		return { entity_map.at(uid), this };
	}
	return {};
}

Entity Scene::find_by_name(const std::string& name) {
	auto view = registry.view<IdComponent>();
	for (auto entity : view) {
		const IdComponent& id_comp = get_component<IdComponent>(entity);
		if (id_comp.tag == name) {
			return { entity, this };
		}
	}
	return {};
}

template <typename... Component>
inline static void copy_component(
		entt::registry& dst, entt::registry& src,
		const std::unordered_map<UID, entt::entity>& entt_map) {
	(
			[&]() {
				auto view = src.view<Component>();
				for (auto src_entity : view) {
					entt::entity dst_entity =
							entt_map.at(src.get<IdComponent>(src_entity).id);

					auto& src_component = src.get<Component>(src_entity);
					dst.emplace_or_replace<Component>(dst_entity, src_component);
				}
			}(),
			...);
}

template <typename... Component>
static void copy_component(
		ComponentGroup<Component...>, entt::registry& dst, entt::registry& src,
		const std::unordered_map<UID, entt::entity>& entt_map) {
	copy_component<Component...>(dst, src, entt_map);
}

template <typename... Component>
static void copy_component_if_exists(Entity dst, Entity src) {
	(
			[&]() {
				if (src.has_component<Component>()) {
					dst.add_or_replace_component<Component>(src.get_component<Component>());
				}
			}(),
			...);
}

template <typename... Component>
static void copy_component_if_exists(ComponentGroup<Component...>, Entity dst,
		Entity src) {
	copy_component_if_exists<Component...>(dst, src);
}

void Scene::copy_to(Ref<Scene> dst) {
	auto& src_registry = registry;
	auto& dst_registry = dst->registry;
	std::unordered_map<UID, entt::entity> entt_map;

	// Create entities in new scene
	auto id_view = src_registry.view<IdComponent>();
	for (auto entity_id : id_view) {
		const auto& id_comp = get_component<IdComponent>(entity_id);

		const UID& uid = id_comp.id;
		const auto& name = id_comp.tag;

		Entity new_entity =
				dst->create(uid, name);

		// set parent id but do not set children vectors
		const auto& relation = get_component<RelationComponent>(entity_id);
		new_entity.get_relation().parent_id = relation.parent_id;

		entt_map[uid] = (entt::entity)new_entity;
	}

	copy_component(AllComponents{}, dst_registry, src_registry,
			entt_map);

	// set child/parent relations
	for (auto& [uid, entity] : dst->entity_map) {
		// Set parent entity
		auto& relation = entity.get_relation();
		if (relation.parent_id) {
			auto parent_entity = dst->find_by_id(relation.parent_id);
			if (parent_entity) {
				entity.set_parent(parent_entity);
			}
		}
	}
}

static Json serialize_entity(Entity& entity) {
	bool has_required_components = entity.has_component<IdComponent, RelationComponent, TransformComponent>();
	EVE_ASSERT_ENGINE(has_required_components);

	Json out;

	out["id"] = entity.get_uid();
	out["tag"] = entity.get_name();
	out["parent_id"] = entity.get_relation().parent_id;

	const TransformComponent& transform = entity.get_transform();
	out["transform_component"] = Json{
		{ "local_position", transform.local_position },
		{ "local_rotation", transform.local_rotation },
		{ "local_scale", transform.local_scale },
	};

	if (entity.has_component<CameraComponent>()) {
		const CameraComponent& cc = entity.get_component<CameraComponent>();

		out["camera_component"] = Json{
			{ "camera",
					{ { "aspect_ratio", cc.camera.aspect_ratio },
							{ "zoom_level", cc.camera.zoom_level },
							{ "near_clip", cc.camera.near_clip },
							{ "far_clip", cc.camera.far_clip } } },
			{ "is_primary", cc.is_primary },
			{ "is_fixed_aspect_ratio", cc.is_fixed_aspect_ratio }
		};
	}

	if (entity.has_component<SpriteRendererComponent>()) {
		const SpriteRendererComponent& sc = entity.get_component<SpriteRendererComponent>();

		out["sprite_renderer_component"] = Json{
			{ "texture", sc.texture },
			{ "color", sc.color },
			{ "tex_tiling", sc.tex_tiling }
		};
	}

	if (entity.has_component<TextRendererComponent>()) {
		const TextRendererComponent& tc = entity.get_component<TextRendererComponent>();

		out["text_renderer_component"] = Json{
			{ "text", tc.text },
			{ "font", tc.font },
			{ "fg_color", tc.fg_color },
			{ "bg_color", tc.bg_color },
			{ "kerning", tc.kerning },
			{ "line_spacing", tc.line_spacing },
		};
	}

	return out;
}

void Scene::serialize(Ref<Scene> scene, const fs::path& path) {
	Json scene_json;
	scene_json["scene"] = path;
	scene_json["entities"] = Json::array();

	scene->view<entt::entity>().each([&](auto entity_id) {
		Entity entity = { entity_id, scene.get() };
		if (!entity) {
			return;
		}

		Json entity_json = serialize_entity(entity);
		scene_json["entities"].push_back(entity_json);
	});

	json_utils::write_file(path, scene_json);
}

bool Scene::deserialize(Ref<Scene>& scene, const fs::path& path) {
	if (!scene->entity_map.empty()) {
		EVE_LOG_ENGINE_WARNING("Given scene to deserialize is not empty.\nClearing the data...");
		scene->entity_map.clear();
		scene->registry.clear();
	}

	Json data;
	if (!json_utils::read_file(path, data)) {
		EVE_LOG_ENGINE_ERROR("Failed to load scene file at '{}'", path.c_str());
		return false;
	}

	if (!data.contains("scene")) {
		return false;
	}

	scene->name = data["scene"].get<std::string>();

	if (!data.contains("entities")) {
		return false;
	}

	Json entities_json = data["entities"];

	// Create entities before adding components in order to build parent/child relations.
	std::vector<Entity> entities;
	for (const auto& entity_json : entities_json) {
		UID uid = entity_json["id"].get<UID>();
		std::string name = entity_json["tag"].get<std::string>();
		entities.push_back(scene->create(uid, name));
	}

	for (uint32_t i = 0; i < entities.size(); i++) {
		auto& entity_json = entities_json.at(i);
		auto& deserialing_entity = entities.at(i);

		// Set parent entity
		UID parent_id = entity_json["parent_id"].get<UID>();
		if (parent_id) {
			auto parent_entity = scene->find_by_id(parent_id);
			if (parent_entity) {
				deserialing_entity.set_parent(parent_entity);
			}
		}

		if (auto transform_json = entity_json["transform_component"];
				!transform_json.is_null()) {
			auto& tc = deserialing_entity.get_component<TransformComponent>();

			tc.local_position = transform_json["local_position"].get<glm::vec3>();
			tc.local_rotation = transform_json["local_rotation"].get<glm::vec3>();
			tc.local_scale = transform_json["local_scale"].get<glm::vec3>();
		}

		if (auto camera_comp_json = entity_json["camera_component"];
				!camera_comp_json.is_null()) {
			auto& camera_component =
					deserialing_entity.add_component<CameraComponent>();

			auto camera_json = camera_comp_json["camera"];
			camera_component.camera.aspect_ratio =
					camera_json["aspect_ratio"].get<float>();
			camera_component.camera.zoom_level =
					camera_json["zoom_level"].get<float>();
			camera_component.camera.near_clip =
					camera_json["near_clip"].get<float>();
			camera_component.camera.far_clip =
					camera_json["far_clip"].get<float>();

			camera_component.is_primary = camera_comp_json["is_primary"].get<bool>();
			camera_component.is_fixed_aspect_ratio =
					camera_comp_json["is_fixed_aspect_ratio"].get<bool>();
		}

		if (auto sprite_comp_json = entity_json["sprite_renderer_component"];
				!sprite_comp_json.is_null()) {
			auto& sprite_component =
					deserialing_entity.add_component<SpriteRendererComponent>();

			sprite_component.texture = sprite_comp_json["texture"].get<AssetHandle>();
			sprite_component.color = sprite_comp_json["color"].get<Color>();
			sprite_component.tex_tiling =
					sprite_comp_json["tex_tiling"].get<glm::vec2>();
		}

		if (auto text_comp_json = entity_json["text_renderer_component"]; !text_comp_json.is_null()) {
			auto& text_component = deserialing_entity.add_component<TextRendererComponent>();

			text_component.text = text_comp_json["text"].get<std::string>();
			text_component.font = text_comp_json["font"].get<AssetHandle>();
			text_component.fg_color = text_comp_json["fg_color"].get<Color>();
			text_component.bg_color = text_comp_json["bg_color"].get<Color>();
			text_component.kerning = text_comp_json["kerning"].get<float>();
			text_component.line_spacing = text_comp_json["line_spacing"].get<float>();
		}
	}

	return true;
}
