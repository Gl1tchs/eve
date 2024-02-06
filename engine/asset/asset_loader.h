#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H

class Texture2D;
class Font;
class Scene;

namespace asset_loader {

Ref<Texture2D> load_texture(const fs::path& path);

Ref<Font> load_font(const fs::path& path);

Ref<Scene> load_scene(const fs::path& path);

} //namespace asset_loader

#endif
