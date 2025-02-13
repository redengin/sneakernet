#pragma once

#include <filesystem>
#include <vector>
#include <optional>
#include <fstream>

class Catalog {
public:
    static constexpr char TAG[] = "Catalog"; ///< ESP logging tag

    Catalog(std::filesystem::path root);

    /// @returns true if valid content reference (FOLDER or FILE)
    static bool isValid(const std::filesystem::path& catalogpath);

    /// @returns true if folder exists
    bool hasFolder(const std::string& folderpath) const;

    std::filesystem::directory_iterator folderIterator(const std::string folderpath) const;

    /// @returns true if folder is admin only
    bool isLocked(const std::string& folderpath) const;

    std::optional<std::string> getTitle(const std::string& filepath) const;

    bool hasIcon(const std::string& filepath) const;

    /// @returns true if content exists
    bool hasFile(const std::string& filepath) const;

    std::time_t timestamp(const std::string& filepath) const;

    std::ifstream readContent(const std::string& filepath) const;

private:
    const std::filesystem::path root;

    /// hidden file/folder prefix
    static constexpr char HIDDEN_PREFIX = '.';
    ///< tempororary filename used during receive
    static constexpr char INWORK_PREFIX[] = ".inwork-";
    /// file specifying the folder is locked
    static constexpr char LOCKED_FILENAME[] = ".locked";
    /// file for the icon (name follows prefix)
    static constexpr char ICON_PREFIX[] = ".icon-";
    /// file for the title (name follows prefix)
    static constexpr char TITLE_PREFIX[] = ".title-";

    static bool isHidden(const std::filesystem::path& path);
    bool isLocked(const std::filesystem::path& path) const;
    std::string title(const std::filesystem::path& filepath) const;
    bool hasIcon(const std::filesystem::path& filepath) const;
};