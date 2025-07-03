#include "Catalog.hpp"

#include <esp_log.h>
#include <sdkconfig.h>
// override LOG_LOCAL_LEVEL per project setting
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL CONFIG_SNEAKERNET_LOG_LEVEL

#include <utime.h>
#include <cstring>

Catalog::Catalog(const std::filesystem::path _root) : root(_root)
{
  // cleanup catalog state
  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(root))
  {
    const auto filename = entry.path().filename();
    // remove any INWORK files
    if (filename.string().starts_with(INWORK_PREFIX))
    {
      ESP_LOGI(TAG, "removing INWORK %s", entry.path().c_str());
      std::error_code ec;
      std::filesystem::remove(entry, ec);
      if (ec)
        ESP_LOGE(TAG, "failed to remove %s [%s]",
                 entry.path().c_str(), ec.message().c_str());
    }
    // TODO remove any TITLE files without a file
    // TODO remove any ICON files without a file
  }
}

// Folder Support
//==============================================================================
bool Catalog::hasFolder(const std::filesystem::path &folderpath) const
{
  // always have the root
  if (folderpath.empty())
    return true;

  // don't allow catalog folders to conflict with hidden files/folders
  if (isHidden(folderpath))
    return false;

  // check if folder exists
  std::error_code ec;
  const bool ret = std::filesystem::is_directory(root / folderpath, ec);
  if (ec && (ec.value() != ENOENT))
    ESP_LOGE(TAG, "failed hasFolder [%s ec:[%d]%s]",
             folderpath.c_str(), ec.value(), ec.message().c_str());

  return ret;
}

std::optional<Catalog::FolderInfo> Catalog::getFiles() const
{
  Catalog::FolderInfo ret;

  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(root))
  {
    // skip hidden files
    if (isHidden(entry.path()))
      continue;

    if (entry.is_regular_file())
      ret.emplace_back(entry.path().filename(), hasIcon(entry.path()),
                       entry.last_write_time(),
                       entry.file_size(), getTitle(entry.path()));
  }

  return ret;
}

std::optional<Catalog::FolderInfo> Catalog::getFolder(
    const std::filesystem::path &folderpath) const
{
  if (!hasFolder(folderpath))
    return std::nullopt;

  Catalog::FolderInfo ret;

  // inspect the entries
  for (const auto &entry : std::filesystem::directory_iterator(root / folderpath))
  {
    // skip hidden files
    if (isHidden(entry.path()))
      continue;

    if (entry.is_directory())
    {
      // handle directories
      ret.emplace_back(entry.path().filename(), hasIcon(entry.path()));
    }
    else if (entry.is_regular_file())
    {
      // handle files
      ret.emplace_back(entry.path().filename(), hasIcon(entry.path()),
                       entry.last_write_time(),
                       entry.file_size(), getTitle(entry.path()));
    }
  }

  return ret;
}

bool Catalog::makeFolder(const std::filesystem::path &folderpath) const
{
  // ignore empty
  if (folderpath.empty())
    return true;

  // don't allow catalog folders to conflict with hidden files/folders
  if (isHidden(folderpath))
    return false;

  // create the directory
  std::error_code ec;
  std::filesystem::create_directories(root / folderpath, ec);
  if (ec)
  {
    ESP_LOGE(TAG, "failed to create directory [%s ec:%s]",
             folderpath.c_str(), ec.message().c_str());
    return false;
  }
  else
    return true;
}

bool Catalog::removeFolder(const std::filesystem::path &folderpath) const
{
  // if it doesn't exist we're done
  if (!hasFolder(folderpath))
    return true;

  // remove any icon file
  std::error_code ec;
  std::filesystem::remove(iconpathFor(root / folderpath), ec);
  if (ec && (ec.value() != ENOENT))
    ESP_LOGE(TAG, "failed to remove file [%s ec:%s]",
             iconpathFor(folderpath).c_str(), ec.message().c_str());

  // remove the folder
  const bool ret = std::filesystem::remove(root / folderpath, ec);
  // if (ec && (ec.value() != ENOENT))
  if (ec)
    ESP_LOGE(TAG, "failed to remove directory [%s ec:%s]",
             folderpath.c_str(), ec.message().c_str());

  return ret;
}

// File Support
//==============================================================================
bool Catalog::hasFile(const std::filesystem::path &filepath) const
{
  // don't allow catalog folders/files to conflict with hidden files/folders
  if (isHidden(filepath))
    return false;

  // check that file exists
  std::error_code ec;
  const bool ret = std::filesystem::is_regular_file(root / filepath, ec);
  if (ec && (ec.value() != ENOENT))
    ESP_LOGE(TAG, "failed hasFile ['%s' ec:[%d]%s]",
             filepath.c_str(), ec.value(), ec.message().c_str());

  return ret;
}

std::optional<std::ifstream> Catalog::readContent(
    const std::filesystem::path &filepath) const
{
  if (!hasFile(filepath))
    return std::nullopt;

  return std::ifstream(root / filepath,
                       std::ios_base::in | std::ios_base::binary);
}

bool Catalog::removeFile(const std::filesystem::path &filepath) const
{
  if (!hasFile(filepath))
    return true;

  std::error_code ec;
  // remove any title file
  std::filesystem::remove(titlepathFor(root / filepath), ec);
  if (ec && (ec.value() != ENOENT))
    ESP_LOGE(TAG, "failed to remove file [%s ec:%s]",
             titlepathFor(filepath).c_str(), ec.message().c_str());

  // remove any icon file
  std::filesystem::remove(iconpathFor(root / filepath), ec);
  if (ec && (ec.value() != ENOENT))
    ESP_LOGE(TAG, "failed to remove file [%s ec:%s]",
             iconpathFor(filepath).c_str(), ec.message().c_str());

  // remove the file
  const bool ret = std::filesystem::remove(root / filepath, ec);
  if (ec)
    ESP_LOGE(TAG, "failed to remove file [%s ec:%s]",
             filepath.c_str(), ec.message().c_str());

  return ret;
}

bool Catalog::hasIcon(const std::filesystem::path &path) const
{
  const auto iconpath = iconpathFor(root / path);

  // check if icon exists
  std::error_code ec;
  const bool ret = std::filesystem::is_regular_file(iconpath, ec);
  if (ec && (ec.value() != ENOENT))
    ESP_LOGE(TAG, "failed search for icon ['%s' ec:[%d]%s]",
             iconpath.c_str(), ec.value(), ec.message().c_str());

  return ret;
}

std::optional<std::ifstream> Catalog::readIcon(
    const std::filesystem::path &path) const
{
  if (!hasIcon(root / path))
    return std::nullopt;

  const auto iconpath = iconpathFor(root / path);
  return std::ifstream(iconpath, std::ios_base::in | std::ios_base::binary);
}

bool Catalog::removeIcon(const std::filesystem::path &path) const
{
  if (!hasIcon(root / path))
    return true;

  const auto iconpath = iconpathFor(root / path);
  std::error_code ec;
  const bool ret = std::filesystem::remove(iconpath, ec);
  if (ec)
    ESP_LOGE(TAG, "failed to remove file [%s ec:%s]", iconpath.c_str(),
             ec.message().c_str());

  return ret;
}

bool Catalog::setTitle(const std::filesystem::path &filepath,
                       const std::string &title) const
{
  if (!hasFile(root / filepath))
    return false;

  const auto titlepath = titlepathFor(root / filepath);

  // set the new title file contents
  std::ofstream ofs(titlepath);
  ofs << title;
  ofs.close();
  return true;
}

std::optional<std::string> Catalog::getTitle(
    const std::filesystem::path &filepath) const
{
  auto titlepath = titlepathFor(root / filepath);

  // check if title file exists
  std::error_code ec;
  bool exists = std::filesystem::is_regular_file(titlepath, ec);
  if (ec && (ec.value() != ENOENT))
    ESP_LOGE(TAG, "failed search for title ['%s' ec:[%d]%s]",
             titlepath.c_str(), ec.value(), ec.message().c_str());

  if (exists)
  {
    // return title file contents
    std::string ret;
    std::ifstream ifs(titlepath);
    std::getline(ifs, ret);
    return ret;
  }
  else
    return std::nullopt;
}

bool Catalog::removeTitle(const std::filesystem::path &filepath) const
{
  const auto titlepath = titlepathFor(root / filepath);
  std::error_code ec;
  const bool ret = std::filesystem::remove(titlepath, ec);
  if (ec)
    ESP_LOGE(TAG, "failed to remove file [%s ec:%s]",
             titlepath.c_str(), ec.message().c_str());

  return ret;
}

// Upload Support
//==============================================================================
std::optional<Catalog::InWorkContent> Catalog::addFile(
    const std::filesystem::path &filepath,
    const std::optional<std::filesystem::file_time_type> timestamp,
    const size_t size) const
{
  // make sure there is a folder to put it in
  const auto parentpath = filepath.parent_path();
  if (!makeFolder(parentpath))
  {
    ESP_LOGE(TAG, "failed to create parent folder [%s]", filepath.parent_path().c_str());
    return std::nullopt;
  }

  // TODO make room for content?

  return InWorkContent(root / filepath, timestamp);
}

std::optional<Catalog::InWorkContent> Catalog::setIcon(
    const std::filesystem::path &path, const size_t size) const
{
  if (!hasFile(path) && !hasFolder(path))
    return std::nullopt;

  // TODO make room for icon?

  const auto iconpath = iconpathFor(root / path);

  return InWorkContent(iconpath);
}

// InWork Content
//==============================================================================
Catalog::InWorkContent::InWorkContent(
    const std::filesystem::path &filepath,
    const std::optional<std::filesystem::file_time_type> timestamp)
    : filepath(filepath), timestamp(timestamp)
{
  const auto inworkfile = filepath.filename().string().insert(0, INWORK_PREFIX);
  inwork_filepath = filepath.parent_path() / inworkfile;
}

#include <fcntl.h>
int Catalog::InWorkContent::open()
{
  return ::open(inwork_filepath.c_str(), (O_CREAT | O_WRONLY));
}

void Catalog::InWorkContent::done()
{
  // remove the old file
  std::error_code ec;
  if (std::filesystem::exists(filepath, ec))
  {
    std::filesystem::remove(filepath, ec);
    if (ec)
      ESP_LOGE(TAG, "unable to remove old file [%s ec:%s]", filepath.c_str(),
               ec.message().c_str());
  }

  // swap in the new file
  std::filesystem::rename(inwork_filepath, filepath, ec);
  if (ec)
    ESP_LOGE(TAG, "unable to rename inwork file [from:%s to:%s ec:%s]",
             inwork_filepath.c_str(), filepath.c_str(), ec.message().c_str());

  if (timestamp.has_value())
  {
    // set the timestamp
    std::filesystem::last_write_time(filepath, timestamp.value(), ec);
    if (ec)
    {
      // FIXME incomplete ESP32 std::filesystem implementation
      ESP_LOGD(TAG, "unable to set timestamp [%s ec:%s], will use C api",
               filepath.c_str(), ec.message().c_str());

      // use C api (since not implemented in std::filesystem)
      const auto timestamp_s = std::chrono::system_clock::to_time_t(
          std::chrono::file_clock::to_sys(timestamp.value()));
      struct utimbuf c_timestamp{.actime = timestamp_s, .modtime = timestamp_s};
      if (0 != utime(filepath.c_str(), &c_timestamp))
      {
        ESP_LOGE(TAG, "unable to set timestamp [%s ec:%s]", filepath.c_str(),
                 std::strerror(errno));
        ESP_LOGD(TAG, "utimebuf{.actime = %lli, .modtime = %lli}",
                 c_timestamp.actime, c_timestamp.modtime);
      }
    }
  }
}

Catalog::InWorkContent::~InWorkContent()
{
  // remove the inwork file
  std::error_code ec;
  std::filesystem::remove(inwork_filepath, ec);
  if (ec)
    ESP_LOGE(TAG, "unable to remove old file [%s ec:%s]", filepath.c_str(),
             ec.message().c_str());
}

// PRIVATE
//==============================================================================

bool Catalog::isHidden(const std::filesystem::path &path)
{
  for (const auto &i : path)
  {
    // ignore hidden files/folders and relative paths
    if (i.filename().string().front() == HIDDEN_PREFIX)
    {
      ESP_LOGD(TAG, "ignoring hidden path [%s]", path.c_str());
      return true;
    }
  }
  return false;
}

std::filesystem::path Catalog::titlepathFor(
    const std::filesystem::path &path) const
{
  const auto titlefile = path.filename().string().insert(0, TITLE_PREFIX);
  return path.parent_path() / titlefile;
}

std::filesystem::path Catalog::iconpathFor(
    const std::filesystem::path &path) const
{
  auto work = path.string();
  // remove trailing slash
  if (work.ends_with('/'))
    work.pop_back();

  const std::filesystem::path workpath(work);
  const auto iconfile = workpath.filename().string().insert(0, ICON_PREFIX);
  return workpath.parent_path() / iconfile;
}
