#pragma once

#include <filesystem>
#include <fstream>
#include <optional>
#include <vector>

class Catalog {
 public:
  static constexpr char TAG[] = "Catalog";  ///< ESP logging tag

  Catalog(std::filesystem::path root);

  // Folder support
  //------------------------------------------------------------------------------
  bool hasFolder(const std::filesystem::path& folderpath) const;

  /// @returns true if folder is admin only
  std::optional<bool> isLocked(const std::filesystem::path& folderpath) const;

  struct FolderEntry {
    std::string name;
    bool isFolder;
    /// file information
    struct {
      std::filesystem::file_time_type timestamp;
      std::uintmax_t size;
      std::optional<std::string> title;
      bool hasIcon;
    } fileInfo;

    // directory entry constructor
    FolderEntry(const std::string& name) : name(name), isFolder(true) {}

    // file entry constructor
    FolderEntry(const std::string& name,
                const std::filesystem::file_time_type& timestamp,
                const std::uintmax_t& size,
                const std::optional<std::string>& title, const bool hasIcon)
        : name(name), isFolder(false) {
      fileInfo.timestamp = timestamp;
      fileInfo.size = size;
      fileInfo.title = title;
      fileInfo.hasIcon = hasIcon;
    }
  };
  struct FolderInfo {
    bool isLocked;
    std::vector<FolderEntry> entries;
  };
  std::optional<FolderInfo> getFolder(
      const std::filesystem::path& folderpath) const;

  bool addFolder(const std::filesystem::path& folderpath);

  bool removeFolder(const std::filesystem::path& folderpath);

  //------------------------------------------------------------------------------
  // File support
  //------------------------------------------------------------------------------
  bool hasFile(const std::filesystem::path& filepath) const;

  std::optional<std::ifstream> readContent(
      const std::filesystem::path& filepath) const;

  std::optional<std::string> getTitle(
      const std::filesystem::path& filepath) const;

  bool setTitle(const std::filesystem::path& filepath,
                const std::string& title) const;

  bool hasIcon(const std::filesystem::path& filepath) const;

  std::optional<std::ifstream> readIcon(
      const std::filesystem::path& filepath) const;

  bool removeFile(const std::filesystem::path& filepath) const;

  //------------------------------------------------------------------------------
  // Upload Support
  //------------------------------------------------------------------------------
  class InWorkContent;

  std::optional<InWorkContent> addFile(
      const std::filesystem::path& path,
      const std::optional<std::filesystem::file_time_type> timestamp,
      const size_t size) const;

  std::optional<InWorkContent> setIcon(const std::filesystem::path& path) const;

  class InWorkContent {
    friend std::optional<InWorkContent> Catalog::addFile(
        const std::filesystem::path& path,
        const std::optional<std::filesystem::file_time_type> timestamp,
        const size_t size) const;
    friend std::optional<InWorkContent> Catalog::setIcon(
        const std::filesystem::path& path) const;

   public:
    std::ofstream open();

    /// @brief swaps original file (if exists) with new file
    /// @pre ofstream should be closed to flush writes
    void done();

    /// @brief  cleans up inwork content
    ~InWorkContent();

   protected:
    InWorkContent(const std::filesystem::path& filepath,
                  const std::optional<std::filesystem::file_time_type>
                      timestamp = std::nullopt);

   private:
    const std::filesystem::path filepath;
    std::filesystem::path inwork_filepath;
    const std::optional<std::filesystem::file_time_type> timestamp;
  };

  //------------------------------------------------------------------------------

 private:
  const std::filesystem::path root;

  /// hidden file/folder prefix
  static constexpr char HIDDEN_PREFIX = '.';
  /// file specifying the folder is locked
  static constexpr char LOCKED_FILENAME[] = ".locked";
  /// file for the icon (name follows prefix)
  static constexpr char ICON_PREFIX[] = ".icon-";
  /// file for the title (name follows prefix)
  static constexpr char TITLE_PREFIX[] = ".title-";
  ///< tempororary filename used during receive
  static constexpr char INWORK_PREFIX[] = ".inwork-";

  static bool isHidden(const std::filesystem::path& path);

  // get the absolute path of the title file
  std::filesystem::path titlepathFor(
      const std::filesystem::path& filepath) const;

  // get the absolute path of the icon file
  std::filesystem::path iconpathFor(
      const std::filesystem::path& filepath) const;

  // convert absolute path to catalog relative path
  std::filesystem::path relative_path(
      const std::filesystem::path& absoultepath) const;
};