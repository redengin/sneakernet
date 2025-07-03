#pragma once

#include <filesystem>
#include <fstream>
// #include <cstdio>
#include <optional>
#include <vector>

class Catalog
{
public:
  static constexpr char TAG[] = "Catalog"; ///< ESP logging tag

  Catalog(std::filesystem::path root);

  // Folder support
  //------------------------------------------------------------------------------
  struct FolderEntry
  {
    /// file and folder information
    std::string name;
    bool isFolder;
    bool hasIcon;
    /// file only information
    struct
    {
      std::filesystem::file_time_type timestamp;
      std::uintmax_t size;
      std::optional<std::string> title;
    } fileInfo;

    // subfolder entry constructor
    FolderEntry(const std::string &name, const bool hasIcon)
        : name(name), isFolder(true), hasIcon(hasIcon)
    {
    }

    // file entry constructor
    FolderEntry(const std::string &name,
                const bool hasIcon,
                const std::filesystem::file_time_type &timestamp,
                const std::uintmax_t &size,
                const std::optional<std::string> &title)
        : name(name), isFolder(false), hasIcon(hasIcon),
          fileInfo{.timestamp = timestamp, .size = size, .title = title}
    {
    }
  };
  typedef std::vector<FolderEntry> FolderInfo;

  bool hasFolder(const std::filesystem::path &) const;

  /// get a list of all files (for sync)
  std::optional<FolderInfo> getFiles() const;

  std::optional<FolderInfo> getFolder(const std::filesystem::path &) const;

  /// returns true if folder exists (whether new or not)
  bool makeFolder(const std::filesystem::path &) const;

  bool removeFolder(const std::filesystem::path &) const;

  //------------------------------------------------------------------------------
  // File support
  //------------------------------------------------------------------------------
  bool hasFile(const std::filesystem::path &) const;

  std::optional<std::ifstream> readContent(
      const std::filesystem::path &) const;

  bool removeFile(const std::filesystem::path &) const;

  //------------------------------------------------------------------------------
  // Icon support for folders and files
  //------------------------------------------------------------------------------
  bool hasIcon(const std::filesystem::path &) const;

  std::optional<std::ifstream> readIcon(
      const std::filesystem::path &) const;

  bool removeIcon(const std::filesystem::path &) const;

  // also see setIcon() in Upload Support

  //------------------------------------------------------------------------------
  // Title support for files
  //------------------------------------------------------------------------------
  bool setTitle(const std::filesystem::path &,
                const std::string &) const;

  std::optional<std::string> getTitle(
      const std::filesystem::path &) const;

  bool removeTitle(const std::filesystem::path &) const;

  //------------------------------------------------------------------------------
  // Upload Support
  //------------------------------------------------------------------------------
  class InWorkContent;

  std::optional<InWorkContent> addFile(
      const std::filesystem::path &,
      const std::optional<std::filesystem::file_time_type>,
      const size_t) const;

  std::optional<InWorkContent> setIcon(
      const std::filesystem::path &,
      const size_t) const;

  class InWorkContent
  {
    friend std::optional<InWorkContent> Catalog::addFile(
        const std::filesystem::path &,
        const std::optional<std::filesystem::file_time_type>,
        const size_t) const;
    friend std::optional<InWorkContent> Catalog::setIcon(
        const std::filesystem::path &, const size_t) const;

  public:
    // returns file descriptor
    int open();

    /// @brief swaps original file (if exists) with new file
    /// @pre ofstream should be closed to flush writes
    void done();

    /// @brief  cleans up inwork content
    ~InWorkContent();

  protected:
    InWorkContent(const std::filesystem::path &,
                  const std::optional<std::filesystem::file_time_type> timestamp = std::nullopt);

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
  /// file for the icon (name follows prefix)
  static constexpr char ICON_PREFIX[] = ".icon-";
  /// file for the title (name follows prefix)
  static constexpr char TITLE_PREFIX[] = ".title-";
  ///< temporary filename used during receive
  static constexpr char INWORK_PREFIX[] = ".inwork-";

  static bool isHidden(const std::filesystem::path &);

  // get the path of the title file
  std::filesystem::path titlepathFor(const std::filesystem::path &) const;

  // get the path of the icon file
  std::filesystem::path iconpathFor(const std::filesystem::path &) const;
};