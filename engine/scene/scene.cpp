#include "scene/scene.h"

#include "asset/asset_registry.h"
#include "core/json_utils.h"
#include "core/uid.h"
#include "physics/physics_system.h"
#include "project/project.h"
#include "renderer/font.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/transform.h"

Scene::Scene(const std::string& name) :
		name(name),
		physics_system(this) {
}

void Scene::start() {
	EVE_PROFILE_FUNCTION();

	running = true;

	// let scripts modify the values then start the physics system
	physics_system.start();
}

void Scene::update(float dt) {
	if (paused && step_frames-- <= 0) {
		return;
	}

	EVE_PROFILE_FUNCTION();

	physics_system.update(dt);
}

void Scene::stop() {
	EVE_PROFILE_FUNCTION();

	running = false;

	physics_system.stop();
}

void Scene::set_paused(bool _paused) {
	paused = _paused;
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
	entity.add_component<Transform>();
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


	if (is_running()) {
		// TODO destroy object on script

		physics_system.mark_deleted(entity);
	}

	// unselect if selected
	if (is_entity_selected(entity)) {
		unselect_entity(entity);
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

bool Scene::exists(Entity entity) const {
	if (!entity.has_component<IdComponent>()) {
		return false;
	}

	return entity_map.find(entity.get_uid()) != entity_map.end();
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
	const auto view = registry.view<IdComponent>();
	const auto entity_it = std::find_if(view.begin(), view.end(), [&](const auto& entity) {
		return get_component<IdComponent>(entity).tag == name;
	});

	if (entity_it != view.end()) {
		return { *entity_it, this };
	} else {
		return {}; // Return an empty entity if not found
	}
}

void Scene::toggle_entity_selection(Entity entity) {
	const auto it = std::find(selected_entities.begin(), selected_entities.end(), entity);

	if (it != selected_entities.end()) {
		// found
		selected_entities.erase(it);
	} else {
		// not found
		selected_entities.push_back(entity);
	}
}

void Scene::select_entity_only(Entity entity) {
	clear_selected_entities();
	selected_entities.push_back(entity);
}

void Scene::select_entity(Entity entity) {
	if (!is_entity_selected(entity)) {
		selected_entities.push_back(entity);
	}
}

void Scene::unselect_entity(Entity entity) {
	const auto it = std::find(selected_entities.begin(), selected_entities.end(), entity);
	if (it == selected_entities.end()) {
		return;
	}

	selected_entities.erase(it);
}

void Scene::clear_selected_entities() {
	selected_entities.clear();
}

bool Scene::is_entity_selected(Entity entity) const {
	return std::find(selected_entities.begin(), selected_entities.end(), entity) != selected_entities.end();
}

const std::vector<Entity>& Scene::get_selected_entities() const {
	return selected_entities;
}

template <typename... Component>
inline static void copy_component(
		entt::registry& dst, entt::registry& src,
		const std::unordered_map<UID, entt::entity>& entt_map) {
	(
			[&]() {
				for (const entt::entity src_entity : src.view<Component>()) {
					const entt::entity dst_entity =
							entt_map.at(src.get<IdComponent>(src_entity).id);

					const auto& src_component = src.get<Component>(src_entity);
					dst.emplace_or_replace<Component>(dst_entity, src_component);
				}
			}(),
			...);
}

template <typename... Component>
inline static void copy_component(
		ComponentGroup<Component...>, entt::registry& dst, entt::registry& src,
		const std::unordered_map<UID, entt::entity>& entt_map) {
	copy_component<Component...>(dst, src, entt_map);
}

template <typename... Component>
inline static void copy_component_if_exists(Entity dst, Entity src) {
	(
			[&]() {
				if (src.has_component<Component>()) {
					dst.add_or_replace_component<Component>(src.get_component<Component>());
				}
			}(),
			...);
}

template <typename... Component>
inline static void copy_component_if_exists(ComponentGroup<Component...>, Entity dst,
		Entity src) {
	copy_component_if_exists<Component...>(dst, src);
}

Ref<Scene> Scene::copy(Ref<Scene> src) {
	EVE_PROFILE_FUNCTION();

	Ref<Scene> dst = create_ref<Scene>(src->name);

	auto& src_registry = src->registry;
	auto& dst_registry = dst->registry;
	std::unordered_map<UID, entt::entity> entt_map;

	// Create entities in new scene
	for (auto entity_id : src_registry.view<IdComponent>()) {
		const auto& id_comp = src->get_component<IdComponent>(entity_id);

		const UID& uid = id_comp.id;
		const auto& name = id_comp.tag;

		Entity new_entity =
				dst->create(uid, name);

		// set parent id but do not set children vectors
		const auto& relation = src->get_component<RelationComponent>(entity_id);
		new_entity.get_relation().parent_id = relation.parent_id;

		entt_map[uid] = (entt::entity)new_entity;
	}

	copy_component(AllComponents{}, dst_registry, src_registry,
			entt_map);

	const auto has_parent = [](const auto& pair) -> bool {
		const auto& relation = pair.second.get_relation();
		return (bool)relation.parent_id;
	};

	// set child/parent relations
	for (auto& [uid, entity] : dst->entity_map | std::views::filter(has_parent)) {
		// Set parent entity
		auto& relation = entity.get_relation();
		auto parent_entity = dst->find_by_id(relation.parent_id);
		if (parent_entity) {
			entity.set_parent(parent_entity);
		}
	}

	return dst;
}

NLOHMANN_JSON_SERIALIZE_ENUM(Rigidbody2D::BodyType, {
															{ Rigidbody2D::BodyType::STATIC, "static" },
															{ Rigidbody2D::BodyType::DYNAMIC, "dynamic" },
															{ Rigidbody2D::BodyType::KINEMATIC, "kinematic" },
													})

static Json serialize_entity(Entity& entity) {
	bool has_required_components = entity.has_component<IdComponent, RelationComponent, Transform>();
	EVE_ASSERT_ENGINE(has_required_components);

	Json out;

	out["id"] = entity.get_uid();
	out["tag"] = entity.get_name();
	out["parent_id"] = entity.get_relation().parent_id;

	const Transform& transform = entity.get_transform();
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

	if (entity.has_component<SpriteRenderer>()) {
		const SpriteRenderer& sc = entity.get_component<SpriteRenderer>();

		const auto texture = asset_registry::get_asset<Texture2D>(sc.texture);

		out["sprite_renderer_component"] = Json{
			{ "texture", texture ? texture->path : "" },
			{ "color", sc.color },
			{ "tex_tiling", sc.tex_tiling }
		};
	}

	if (entity.has_component<TextRenderer>()) {
		const TextRenderer& tc = entity.get_component<TextRenderer>();

		const auto font = asset_registry::get_asset<Font>(tc.font);

		out["text_renderer_component"] = Json{
			{ "text", tc.text },
			{ "font", font ? font->path : "" },
			{ "fg_color", tc.fg_color },
			{ "bg_color", tc.bg_color },
			{ "kerning", tc.kerning },
			{ "line_spacing", tc.line_spacing },
			{ "is_screen_space", tc.is_screen_space },
		};
	}

	if (entity.has_component<Rigidbody2D>()) {
		auto& rb2d = entity.get_component<Rigidbody2D>();

		out["rigidbody2d_component"] = Json{
			{ "type", rb2d.type },
			{ "fixed_rotation", rb2d.fixed_rotation },
		};
	}

	if (entity.has_component<BoxCollider2D>()) {
		auto& box_collider = entity.get_component<BoxCollider2D>();

		out["box_collider"] = Json{
			{ "offset", box_collider.offset },
			{ "size", box_collider.size },
			{ "is_trigger", box_collider.is_trigger },
			{ "density", box_collider.density },
			{ "friction", box_collider.friction },
			{ "restitution", box_collider.restitution },
			{ "restitution_threshold", box_collider.restitution_threshold },
		};
	}

	if (entity.has_component<CircleCollider2D>()) {
		auto& circle_collider = entity.get_component<CircleCollider2D>();

		out["circle_collider"] = Json{
			{ "offset", circle_collider.offset },
			{ "radius", circle_collider.radius },
			{ "is_trigger", circle_collider.is_trigger },
			{ "density", circle_collider.density },
			{ "friction", circle_collider.friction },
			{ "restitution", circle_collider.restitution },
			{ "restitution_threshold", circle_collider.restitution_threshold },
		};
	}

	if (entity.has_component<PostProcessVolume>()) {
		const PostProcessVolume& volume = entity.get_component<PostProcessVolume>();

		out["post_process_volume"] = Json{
			{ "is_global", volume.is_global },
			{ "gray_scale", Json{ { "enabled", volume.gray_scale.enabled } } },
			{ "chromatic_aberration", Json{
											  { "enabled", volume.chromatic_aberration.enabled },
											  { "offset", volume.chromatic_aberration.offset },
									  } },
			{ "blur", Json{
							  { "enabled", volume.blur.enabled },
							  { "size", volume.blur.size },
							  { "seperation", volume.blur.seperation },
					  } },
			{ "sharpen", Json{
								 { "enabled", volume.sharpen.enabled },
								 { "amount", volume.sharpen.amount },
						 } },
			{ "vignette", Json{
								  { "enabled", volume.vignette.enabled },
								  { "inner", volume.vignette.inner },
								  { "outer", volume.vignette.outer },
								  { "strength", volume.vignette.strength },
								  { "curvature", volume.vignette.curvature },
						  } }
		};
	}

	return out;
}

void Scene::serialize(Ref<Scene> scene, std::string path) {
	path = Project::get_asset_path(path).string();

	Json scene_json;
	scene_json["uid"] = scene->handle;
	scene_json["name"] = scene->name;
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

bool Scene::deserialize(Ref<Scene>& scene, std::string path) {
	path = Project::get_asset_path(path).string();

	if (!scene->entity_map.empty()) {
		EVE_LOG_ENGINE_WARNING("Given scene to deserialize is not empty.\nClearing the data...");
		scene->entity_map.clear();
		scene->registry.clear();
	}

	Json data;
	if (!json_utils::read_file(path, data)) {
		EVE_LOG_ENGINE_ERROR("Failed to load scene file at '{}'", path);
		return false;
	}

	scene->handle = data["uid"].get<AssetHandle>();
	scene->name = data["name"].get<std::string>();
	scene->path = Project::get_relative_asset_path(path);

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

		if (const auto& transform_json = entity_json["transform_component"];
				!transform_json.is_null()) {
			auto& tc = deserialing_entity.get_component<Transform>();

			tc.local_position = transform_json["local_position"].get<glm::vec3>();
			tc.local_rotation = transform_json["local_rotation"].get<glm::vec3>();
			tc.local_scale = transform_json["local_scale"].get<glm::vec3>();
		}

		if (const auto& camera_comp_json = entity_json["camera_component"];
				!camera_comp_json.is_null()) {
			auto& camera_component =
					deserialing_entity.add_component<CameraComponent>();

			{
				const auto& camera_json = camera_comp_json["camera"];
				camera_component.camera.aspect_ratio =
						camera_json["aspect_ratio"].get<float>();
				camera_component.camera.zoom_level =
						camera_json["zoom_level"].get<float>();
				camera_component.camera.near_clip =
						camera_json["near_clip"].get<float>();
				camera_component.camera.far_clip =
						camera_json["far_clip"].get<float>();
			}

			camera_component.is_primary = camera_comp_json["is_primary"].get<bool>();
			camera_component.is_fixed_aspect_ratio =
					camera_comp_json["is_fixed_aspect_ratio"].get<bool>();
		}

		if (const auto& sprite_comp_json = entity_json["sprite_renderer_component"];
				!sprite_comp_json.is_null()) {
			auto& sprite_component =
					deserialing_entity.add_component<SpriteRenderer>();

			const std::string texture_path = sprite_comp_json["texture"].get<std::string>();

			sprite_component.texture = INVALID_UID;
			if (fs::exists(Project::get_asset_path(texture_path))) {
				if (AssetHandle handle = asset_registry::load_asset(texture_path, AssetType::TEXTURE); handle) {
					sprite_component.texture = handle;
				}
			}

			sprite_component.color = sprite_comp_json["color"].get<Color>();
			sprite_component.tex_tiling =
					sprite_comp_json["tex_tiling"].get<glm::vec2>();
		}

		if (const auto& text_comp_json = entity_json["text_renderer_component"]; !text_comp_json.is_null()) {
			auto& text_component = deserialing_entity.add_component<TextRenderer>();

			text_component.text = text_comp_json["text"].get<std::string>();

			const std::string font_path = text_comp_json["font"].get<std::string>();

			text_component.font = INVALID_UID;
			if (fs::exists(Project::get_asset_path(font_path))) {
				if (AssetHandle handle = asset_registry::load_asset(font_path, AssetType::FONT); handle) {
					text_component.font = handle;
				}
			}

			text_component.fg_color = text_comp_json["fg_color"].get<Color>();
			text_component.bg_color = text_comp_json["bg_color"].get<Color>();
			text_component.kerning = text_comp_json["kerning"].get<float>();
			text_component.line_spacing = text_comp_json["line_spacing"].get<float>();
			text_component.is_screen_space = text_comp_json["is_screen_space"].get<bool>();
		}

		if (const auto& rb2d_json = entity_json["rigidbody2d_component"]; !rb2d_json.is_null()) {
			auto& rb2d = deserialing_entity.add_component<Rigidbody2D>();

			rb2d.type = rb2d_json["type"].get<Rigidbody2D::BodyType>();
			rb2d.fixed_rotation = rb2d_json["fixed_rotation"].get<bool>();
		}

		if (const auto& box_collider_json = entity_json["box_collider"];
				!box_collider_json.is_null()) {
			auto& box_collider = deserialing_entity.add_component<BoxCollider2D>();

			box_collider.offset = box_collider_json["offset"].get<glm::vec2>();
			box_collider.size = box_collider_json["size"].get<glm::vec2>();

			box_collider.is_trigger = box_collider_json["is_trigger"].get<bool>();

			box_collider.density = box_collider_json["density"].get<float>();
			box_collider.friction = box_collider_json["friction"].get<float>();
			box_collider.restitution = box_collider_json["restitution"].get<float>();
			box_collider.restitution_threshold = box_collider_json["restitution_threshold"].get<float>();
		}

		if (const auto& circle_collider_json = entity_json["circle_collider"];
				!circle_collider_json.is_null()) {
			auto& circle_collider = deserialing_entity.add_component<CircleCollider2D>();

			circle_collider.offset = circle_collider_json["offset"].get<glm::vec2>();
			circle_collider.radius = circle_collider_json["radius"].get<float>();

			circle_collider.is_trigger = circle_collider_json["is_trigger"].get<bool>();

			circle_collider.density = circle_collider_json["density"].get<float>();
			circle_collider.friction = circle_collider_json["friction"].get<float>();
			circle_collider.restitution = circle_collider_json["restitution"].get<float>();
			circle_collider.restitution_threshold = circle_collider_json["restitution_threshold"].get<float>();
		}

		if (const auto& post_process_json = entity_json["post_process_volume"]; !post_process_json.is_null()) {
			auto& post_process_volume = deserialing_entity.add_component<PostProcessVolume>();

			post_process_volume.is_global = post_process_json["is_global"].get<bool>();
			{
				const auto& gray_scale_json = post_process_json["gray_scale"];
				post_process_volume.gray_scale.enabled = gray_scale_json["enabled"].get<bool>();
			}
			{
				const auto& chromatic_aberration_json = post_process_json["chromatic_aberration"];

				post_process_volume.chromatic_aberration.enabled = chromatic_aberration_json["enabled"].get<bool>();
				post_process_volume.chromatic_aberration.offset = chromatic_aberration_json["offset"].get<glm::vec3>();
			}
			{
				const auto& blur_json = post_process_json["blur"];

				post_process_volume.blur.enabled = blur_json["enabled"].get<bool>();
				post_process_volume.blur.size = blur_json["size"].get<uint32_t>();
				post_process_volume.blur.seperation = blur_json["seperation"].get<float>();
			}
			{
				const auto& sharpen_json = post_process_json["sharpen"];

				post_process_volume.sharpen.enabled = sharpen_json["enabled"].get<bool>();
				post_process_volume.sharpen.amount = sharpen_json["amount"].get<float>();
			}
			{
				const auto& vignette_json = post_process_json["vignette"];

				post_process_volume.vignette.enabled = vignette_json["enabled"].get<bool>();
				post_process_volume.vignette.inner = vignette_json["inner"].get<float>();
				post_process_volume.vignette.outer = vignette_json["outer"].get<float>();
				post_process_volume.vignette.strength = vignette_json["strength"].get<float>();
				post_process_volume.vignette.curvature = vignette_json["curvature"].get<float>();
			}
		}
	}

	return true;
}
