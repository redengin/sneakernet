#include "Catalog.hpp"

#include <esp_log.h>
#include <sdkconfig.h>
// override LOG_LOCAL_LEVEL per project setting
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL CONFIG_SNEAKERNET_LOG_LEVEL

#include <utime.h>

#include <cstring>

Catalog::Catalog(const std::filesystem::path _root) : root(_root) {
  // cleanup catalog state
  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(root)) {
    // track the latest filetime
    if (entry.last_write_time() > latest_timestamp)
      latest_timestamp = entry.last_write_time();

    const auto filename = entry.path().filename();
    // remove any INWORK files
    if (filename.string().starts_with(INWORK_PREFIX)) {
      ESP_LOGI(TAG, "removing INWORK %s", entry.path().c_str());
      std::error_code ec;
      std::filesystem::remove(entry, ec);
      if (ec) {
        ESP_LOGW(TAG, "failed to remove %s [%s]", entry.path().c_str(),
                 ec.message().c_str());
      }
    }
    // TODO remove any TITLE files without a file
    // TODO remove any ICON files without a file
  }
}

// Folder Support
//==============================================================================
bool Catalog::isLocked(const std::filesystem::path &folderpath) const {
  if (isHidden(folderpath)) return true;

  std::error_code ec;
  return std::filesystem::is_regular_file(root / folderpath / LOCKED_FILENAME,
                                          ec);
}

// Catalog::FolderInfo Catalog::folderInfo(const std::filesystem::path
// &folderpath) const
// {
//     FolderInfo ret;

//     if (isHidden(folderpath))
//         return ret;

//     for (auto &entry : std::filesystem::directory_iterator(root /
//     folderpath))
//     {
//         if (isHidden(entry.path()))
//             continue;

//         if (entry.is_directory())
//             ret.subfolders.push_back(entry.path().filename());

//         if (entry.is_regular_file())
//         {
//             FileInfo fileInfo;
//             fileInfo.name = entry.path().filename();
//             fileInfo.size = entry.file_size();
//             fileInfo.timestamp = entry.last_write_time();
//             fileInfo.title = getTitle(folderpath / fileInfo.name);
//             fileInfo.hasIcon = hasIcon(folderpath / fileInfo.name);

//             ret.files.push_back(fileInfo);
//         }
//     }

//     return ret;
// }

bool Catalog::addFolder(const std::filesystem::path &folderpath) {
  if (folderpath.empty()) return false;

  // don't allow catalog folders to conflict with hidden files/folders
  if (isHidden(folderpath)) return false;

  std::error_code ec;
  bool ret = std::filesystem::create_directory(root / folderpath, ec);
  if (!ret)
    ESP_LOGE(TAG, "failed to create directory [%s] (error: %s)",
             folderpath.c_str(), ec.message().c_str());

  return ret;
}

bool Catalog::removeFolder(const std::filesystem::path &folderpath) {
  // never remove the root folder
  if (folderpath.empty()) return false;

  // don't allow catalog folders to conflict with hidden files/folders
  if (isHidden(folderpath)) return false;

  std::error_code ec;
  bool ret = std::filesystem::remove(root / folderpath, ec);
  if (!ret)
    ESP_LOGE(TAG, "failed to remove directory [%s] (error: %s)",
             folderpath.c_str(), ec.message().c_str());

  return ret;
}

// File Support
//==============================================================================
std::optional<std::ifstream> Catalog::readContent(
    const std::filesystem::path &filepath) const {
  // don't allow catalog folders to conflict with hidden files/folders
  if (isHidden(filepath)) return std::nullopt;

  std::error_code ec;
  if (std::filesystem::is_regular_file(root / filepath, ec))
    return std::ifstream(root / filepath,
                         std::ios_base::in | std::ios_base::binary);
  else
    return std::nullopt;
}

std::optional<std::ifstream> Catalog::readIcon(
    const std::filesystem::path &filepath) const {
  // don't allow catalog folders to conflict with hidden files/folders
  if (isHidden(filepath)) return std::nullopt;

  // FIXME use a generic FILE->ICON naming
  // auto parentpath = std::filesystem::path(filepath).parent_path();
  // auto filename = std::filesystem::path(filepath).filename().string();
  // auto iconfilename = filename.insert(0, ICON_PREFIX);
  // auto iconpath = root/parentpath/iconfilename;
  // ESP_LOGI(TAG, "looking for icon in %s", iconpath.c_str());
  // return std::ifstream(iconpath, std::ios_base::in | std::ios_base::binary);
  return std::nullopt;
}

bool Catalog::removeFile(const std::filesystem::path &filepath) const {
  // don't allow catalog folders to conflict with hidden files/folders
  if (isHidden(filepath)) return false;

  std::error_code ec;
  if (std::filesystem::is_regular_file(root / filepath, ec)) {
    // remove the file
    bool ret = std::filesystem::remove(root / filepath, ec);
    if (ec)
      ESP_LOGE(TAG, "failed to remove file [%s ec:%s]", filepath.c_str(),
               ec.message().c_str());
    return ret;
  }
  else {
    if (ec)
      ESP_LOGE(TAG, "failed is_regular_file [%s ec:%s]", filepath.c_str(),
               ec.message().c_str());
    return false;
  }
}

// std::filesystem::file_time_type Catalog::timestamp(const
// std::filesystem::path &filepath) const
// {
//     std::error_code ec;
//     auto ret = std::filesystem::last_write_time(root / filepath, ec);
//     if (ec)
//         ESP_LOGW(TAG, "unable to get last file write time [%s]",
//         ec.message().c_str());
//     return ret;
// }

// Upload Support
//==============================================================================
std::optional<Catalog::InWorkContent> Catalog::addFile(
    const std::filesystem::path &filepath,
    const std::optional<std::filesystem::file_time_type> timestamp,
    const size_t size) {

  // make sure there is a parent folder
  std::error_code ec;
  if (! std::filesystem::is_directory(root / filepath.parent_path(), ec)) {
    if (ec)
      ESP_LOGE(TAG, "failed is_directory [%s ec:%s]", filepath.parent_path().c_str(),
               ec.message().c_str());
    return std::nullopt;
  }

  return std::nullopt;
//   if (timestamp) {
//     if (timestamp > latest_timestamp) latest_timestamp = timestamp.value();
//     return InWorkContent(root / filepath, timestamp);
//   } else
//     return InWorkContent(root / filepath, latest_timestamp);
}

Catalog::InWorkContent Catalog::setIcon(const std::filesystem::path &filepath) {
  // FIXME use generic filepath->icon
  auto iconpath = ICON_PREFIX;
  return InWorkContent(root / iconpath, std::nullopt);
}





Catalog::InWorkContent::InWorkContent(
    const std::filesystem::path &filepath,
    const std::optional<std::filesystem::file_time_type> timestamp)
    : filepath(filepath), timestamp(timestamp) {
  auto parentpath = filepath.parent_path();
  auto filename = filepath.filename().string();
  auto inwork_filename = filename.insert(0, INWORK_PREFIX);
  inwork_filepath = parentpath / inwork_filename;
  ofs = std::ofstream(inwork_filepath,
                      std::ios_base::out | std::ios_base::binary);
}

bool Catalog::InWorkContent::done() {
  std::error_code ec;

  // flush all writes
  ofs.close();

  // remove the old file
  if (std::filesystem::exists(filepath, ec))
    std::filesystem::remove(filepath, ec);
  if (ec)
    ESP_LOGW(TAG, "unable to remove old file [%s ec:%s]", filepath.c_str(),
             ec.message().c_str());

  // swap in the new file
  std::filesystem::rename(inwork_filepath, filepath, ec);
  if (ec)
    ESP_LOGW(TAG, "unable to rename inwork file [from:%s to:%s ec:%s]",
             inwork_filepath.c_str(), filepath.c_str(), ec.message().c_str());

  if (timestamp) {
    // set the timestamp
    std::filesystem::last_write_time(filepath, timestamp.value(), ec);
    if (ec) {
      // FIXME
      ESP_LOGD(TAG, "unable to set timestamp [%s ec:%s], will attemp via C api",
               filepath.c_str(), ec.message().c_str());

      // use c api (since not implemented in std::filesystem)
      const auto timestamp_s = std::chrono::system_clock::to_time_t(
          std::chrono::file_clock::to_sys(timestamp.value()));
      struct utimbuf c_timestamp{.actime = timestamp_s, .modtime = timestamp_s};
      if (0 != utime(filepath.c_str(), &c_timestamp)) {
        ESP_LOGW(TAG, "unable to set timestamp [%s ec:%s]", filepath.c_str(),
                 std::strerror(errno));
        ESP_LOGD(TAG, "utimebuf{.actime = %lli, .modtime = %lli}",
                 c_timestamp.actime, c_timestamp.modtime);
      }
    }
  }

  // FIXME
  return false;
}

Catalog::InWorkContent::~InWorkContent() {
  if (ofs.is_open()) {
    ofs.close();
    std::error_code ec;
    std::filesystem::remove(inwork_filepath, ec);
    if (ec)
      ESP_LOGW(TAG, "remove inwork file failed [%s]", ec.message().c_str());
  }
}

// PRIVATE
//==============================================================================

bool Catalog::isHidden(const std::filesystem::path &path) {
  for (const auto &i : path) {
    // ignore hidden files/folders and relative paths
    if (*(i.c_str()) == HIDDEN_PREFIX) return true;
  }
  return false;
}

std::optional<std::string> Catalog::getTitle(
    const std::filesystem::path &filepath) const {
  if (isHidden(filepath)) return std::nullopt;

  // FIXME make this a generic prefix modifier (for use by icon methods)
  auto parentpath = std::filesystem::path(filepath).parent_path();
  auto filename = std::filesystem::path(filepath).filename().string();
  auto titlefilename = filename.insert(0, TITLE_PREFIX);
  auto titlepath = root / parentpath / titlefilename;
  ESP_LOGI(TAG, "looking for title in %s", titlepath.c_str());

  std::error_code ec;
  if (std::filesystem::is_regular_file(titlepath, ec)) {
    std::ifstream ifs(titlepath);
    std::stringstream ss;
    ss << ifs.rdbuf();
    ESP_LOGI(TAG, "found title [%s]", ss.str().c_str());
    return ss.str();
  }

  // no title file
  return std::nullopt;
}

bool Catalog::hasIcon(const std::filesystem::path &filepath) const {
  if (isHidden(filepath)) return false;

  // TODO
  return false;

  // auto parentpath = std::filesystem::path(filepath).parent_path();
  // auto filename = std::filesystem::path(filepath).filename().string();
  // auto iconfilename = filename.insert(0, ICON_PREFIX);
  // auto iconpath = root/parentpath/iconfilename;
  // ESP_LOGI(TAG, "looking for icon in %s", iconpath.c_str());

  // std::error_code ec;
  // return std::filesystem::is_regular_file(iconpath, ec);
}

bool Catalog::setTitle(
    const std::filesystem::path &filepath,  ///< relative to catalog
    const std::string &title) const {
  // TODO implement
  return false;
}