#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

/**
 * @brief Natively opens folder in file explorer.
 * Uses explorer.exe in windows
 *		finder in mac
 *		xdg-open in linux
 *
 * @return true if successfull false otherwise
 */
bool open_folder_in_explorer(const fs::path& path);

#endif
