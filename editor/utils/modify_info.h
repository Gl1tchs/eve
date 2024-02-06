#ifndef MODIFY_INFO_H
#define MODIFY_INFO_H

struct SceneModifyInfo {
	bool modified;

	inline void set_modified() { modified = true; }
	inline void on_save() { modified = false; }
};

inline SceneModifyInfo g_modify_info{};

#endif
