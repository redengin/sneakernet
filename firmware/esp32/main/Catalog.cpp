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
    const auto filename = entry.path().filename();
    // remove any INWORK files
    if (filename.string().starts_with(INWORK_PREFIX)) {
      ESP_LOGI(TAG, "removing INWORK %s", entry.path().c_str());
      std::error_code ec;
      if (!std::filesystem::remove(entry, ec)) {
        ESP_LOGE(TAG, "failed to remove %s [%s]", entry.path().c_str(),
                 ec.message().c_str());
      }
    }
    // TODO remove any TITLE files without a file
    // TODO remove any ICON files without a file
  }
}

// Folder Support
//==============================================================================
bool Catalog::hasFolder(const std::filesystem::path &folderpath) const {
  // don't allow catalog folders to conflict with hidden files/folders
  if (isHidden(folderpath)) return false;

  std::error_code ec;
  bool ret = std::filesystem::is_directory(root / folderpath, ec);
  if (ec)
    ESP_LOGE(TAG, "failed is_directory [%s ec:%s]", folderpath.c_str(),
             ec.message().c_str());

  return ret;
}

std::optional<bool> Catalog::isLocked(
    const std::filesystem::path &folderpath) const {
  if (!hasFolder(folderpath)) return std::nullopt;

  std::error_code ec;
  bool ret =
      std::filesystem::is_regular_file(root / folderpath / LOCKED_FILENAME, ec);
  if (ec)
    ESP_LOGE(TAG, "failed is_regular_file [%s ec:%s]", folderpath.c_str(),
             ec.message().c_str());

  return ret;
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
  // ignore empty
  if (folderpath.empty()) return false;

  // don't allow catalog folders to conflict with hidden files/folders
  if (isHidden(folderpath)) return false;

  std::error_code ec;
  bool ret = std::filesystem::create_directory(root / folderpath, ec);
  if (ec)
    ESP_LOGE(TAG, "failed to create directory [%s ec:%s]", folderpath.c_str(),
             ec.message().c_str());

  return ret;
}

bool Catalog::removeFolder(const std::filesystem::path &folderpath) {
  // never remove the root folder
  if (folderpath.empty()) return false;

  if (!hasFolder(folderpath)) return false;

  // remove the folder
  std::error_code ec;
  bool ret = std::filesystem::remove(root / folderpath, ec);
  if (ec)
    ESP_LOGE(TAG, "failed to remove directory [%s ec:%s]", folderpath.c_str(),
             ec.message().c_str());

  return ret;
}

// File Support
//==============================================================================
bool Catalog::hasFile(const std::filesystem::path &filepath) const {
  // don't allow catalog folders/files to conflict with hidden files/folders
  if (isHidden(filepath)) return false;

  std::error_code ec;
  bool ret = std::filesystem::is_regular_file(root / filepath, ec);
  if (ec)
    ESP_LOGE(TAG, "failed is_regular_file [%s ec:%s]", filepath.c_str(),
             ec.message().c_str());

  return ret;
}

std::optional<std::ifstream> Catalog::readContent(
    const std::filesystem::path &filepath) const {
  if (!hasFile(filepath)) return std::nullopt;

  return std::ifstream(root / filepath,
                       std::ios_base::in | std::ios_base::binary);
}

std::optional<std::string> Catalog::getTitle(
    const std::filesystem::path &filepath) const {
  auto titlepath = titlepathFor(filepath);
  if (!titlepath.has_value()) return std::nullopt;

  std::error_code ec;
  bool exists = std::filesystem::is_regular_file(titlepath.value(), ec);
  if (ec)
    ESP_LOGE(TAG, "failed is_regular_file [%s ec:%s]",
             titlepath.value().c_str(), ec.message().c_str());

  if (!exists) return std::nullopt;

  std::string ret;
  std::ifstream ifs(titlepath.value());
  ifs >> ret;
  return ret;
}

bool Catalog::setTitle(const std::filesystem::path &filepath,
                       const std::string &title) const {
  auto titlepath = titlepathFor(filepath);
  if (!titlepath.has_value()) return false;

  std::ofstream ofs(titlepath.value());
  ofs << title;
  ofs.close();
  return true;
}

bool Catalog::hasIcon(const std::filesystem::path &filepath) const {
  auto iconpath = iconpathFor(filepath);
  if (!iconpath.has_value()) return false;

  std::error_code ec;
  bool ret = std::filesystem::is_regular_file(iconpath.value(), ec);
  if (ec)
    ESP_LOGE(TAG, "failed is_regular_file [%s ec:%s]", iconpath.value().c_str(),
             ec.message().c_str());

  return ret;
}

std::optional<std::ifstream> Catalog::readIcon(
    const std::filesystem::path &filepath) const {
  if (!hasIcon(filepath)) return std::nullopt;

  auto iconpath = iconpathFor(filepath);
  return std::ifstream(iconpath.value(),
                       std::ios_base::in | std::ios_base::binary);
}

bool Catalog::removeFile(const std::filesystem::path &filepath) const {
  if (!hasFile(filepath)) return false;

  // remove the file
  std::error_code ec;
  bool ret = std::filesystem::remove(root / filepath, ec);
  if (ec)
    ESP_LOGE(TAG, "failed to remove file [%s ec:%s]", filepath.c_str(),
             ec.message().c_str());

  return ret;
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
    const size_t size) const {
  if (!hasFolder(filepath.parent_path())) return std::nullopt;

  // FIXME implement

  return std::nullopt;
  //   if (timestamp) {
  //     if (timestamp > latest_timestamp) latest_timestamp = timestamp.value();
  //     return InWorkContent(root / filepath, timestamp);
  //   } else
  //     return InWorkContent(root / filepath, latest_timestamp);
}

std::optional<Catalog::InWorkContent> Catalog::setIcon(
    const std::filesystem::path &filepath) const {
  // FIXME implement
  auto iconpath = ICON_PREFIX;
  // return InWorkContent(root / iconpath, std::nullopt);

  return std::nullopt;
}

// InWork Content
//==============================================================================
Catalog::InWorkContent::InWorkContent(
    const std::filesystem::path &filepath,
    const std::optional<std::filesystem::file_time_type> timestamp)
    : filepath(filepath), timestamp(timestamp) {
  const auto inworkfile = filepath.filename().string().insert(0, INWORK_PREFIX);
  inwork_filepath = filepath.parent_path() / inworkfile;

  ofs = std::ofstream(inwork_filepath,
                      std::ios_base::out | std::ios_base::binary);
}

void Catalog::InWorkContent::done() {
  // flush all writes
  ofs.close();

  // remove the old file
  std::error_code ec;
  if (std::filesystem::exists(filepath, ec))
    std::filesystem::remove(filepath, ec);
  if (ec)
    ESP_LOGE(TAG, "unable to remove old file [%s ec:%s]", filepath.c_str(),
             ec.message().c_str());

  // swap in the new file
  std::filesystem::rename(inwork_filepath, filepath, ec);
  if (ec)
    ESP_LOGE(TAG, "unable to rename inwork file [from:%s to:%s ec:%s]",
             inwork_filepath.c_str(), filepath.c_str(), ec.message().c_str());

  if (timestamp.has_value()) {
    // set the timestamp
    std::filesystem::last_write_time(filepath, timestamp.value(), ec);
    if (ec) {
      // FIXME workaround for incomplete ESP32 std::filesystem implementation
      ESP_LOGE(TAG, "unable to set timestamp [%s ec:%s], will attemp via C api",
               filepath.c_str(), ec.message().c_str());

      // use c api (since not implemented in std::filesystem)
      const auto timestamp_s = std::chrono::system_clock::to_time_t(
          std::chrono::file_clock::to_sys(timestamp.value()));
      struct utimbuf c_timestamp{.actime = timestamp_s, .modtime = timestamp_s};
      if (0 != utime(filepath.c_str(), &c_timestamp)) {
        ESP_LOGE(TAG, "unable to set timestamp [%s ec:%s]", filepath.c_str(),
                 std::strerror(errno));
        ESP_LOGD(TAG, "utimebuf{.actime = %lli, .modtime = %lli}",
                 c_timestamp.actime, c_timestamp.modtime);
      }
    }
  }
}

Catalog::InWorkContent::~InWorkContent() {
  if (ofs.is_open()) {
    ofs.close();
    std::error_code ec;
    std::filesystem::remove(inwork_filepath, ec);
    if (ec)
      ESP_LOGE(TAG, "remove inwork file failed [%s]", ec.message().c_str());
  }
}

// PRIVATE
//==============================================================================

bool Catalog::isHidden(const std::filesystem::path &path) {
  for (const auto &i : path) {
    // ignore hidden files/folders and relative paths
    if (i.filename().string().front() == HIDDEN_PREFIX) return true;
  }
  return false;
}

std::optional<std::filesystem::path> Catalog::titlepathFor(
    const std::filesystem::path &filepath) const {
  // ignore hidden files/folders and relative paths
  if (isHidden(filepath)) return std::nullopt;

  const auto titlefile = filepath.filename().string().insert(0, TITLE_PREFIX);
  return root / filepath.parent_path() / titlefile;
}

std::optional<std::filesystem::path> Catalog::iconpathFor(
    const std::filesystem::path &filepath) const {
  // ignore hidden files/folders and relative paths
  if (isHidden(filepath)) return std::nullopt;

  const auto iconfile = filepath.filename().string().insert(0, ICON_PREFIX);
  return root / filepath.parent_path() / iconfile;
}