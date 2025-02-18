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
  bool isLocked(const std::filesystem::path& folderpath) const;

  //   struct FileInfo {
  //     std::string name;
  //     std::size_t size;
  //     std::filesystem::file_time_type timestamp;
  //     std::optional<std::string> title;
  //     bool hasIcon;
  //   };
  //   struct FolderInfo {
  //     bool isLocked;
  //     std::vector<std::string> subfolders;
  //     std::vector<FileInfo> files;

  //     FolderInfo() : isLocked(true) {};
  //   };
  //   FolderInfo folderInfo(const std::filesystem::path& folderpath) const;

  bool addFolder(const std::filesystem::path& folderpath);

  bool removeFolder(const std::filesystem::path& folderpath);

  //------------------------------------------------------------------------------
  // File support
  //------------------------------------------------------------------------------
  bool hasFile(const std::filesystem::path& filepath) const;

  std::optional<std::ifstream> readContent(
      const std::filesystem::path& filepath) const;

  std::optional<std::string> getTitle(const std::filesystem::path& filepath) const;

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
   public:
    std::ofstream ofs;

    /// @brief swaps original file (if exists) with new file
    bool done();

    /// @brief  cleans up inwork content
    ~InWorkContent();

    // FIXME limit access
    // protected:
    InWorkContent(
        const std::filesystem::path& filepath,
        const std::optional<std::filesystem::file_time_type> timestamp);

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

};