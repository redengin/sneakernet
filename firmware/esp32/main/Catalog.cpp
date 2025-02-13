#include "Catalog.hpp"

#include <esp_log.h>

Catalog::Catalog(const std::filesystem::path _root)
    : root(_root)
{
    // cleanup incomplete temporary files
    for(const auto& entry : std::filesystem::recursive_directory_iterator(root))
    {
        const auto filename = entry.path().filename();
        if (filename.string().starts_with(INWORK_PREFIX))
        {
            ESP_LOGI(TAG, "removing INWORK %s", entry.path().c_str());
            std::error_code ec;
            std::filesystem::remove(entry, ec);
            if (ec)
            {
                ESP_LOGW(TAG, "failed to remove %s [%s]"
                     ,entry.path().c_str()
                     ,ec.message().c_str()
                );
            }
        }
    }
}

bool Catalog::isValid(const std::filesystem::path& catalogpath)
{
    // iterate through path segments
    for(const auto& i : catalogpath)
    {
        // ignore hidden files/folders and relative paths
        if (i.c_str()[0] == HIDDEN_PREFIX)
            return false;

    }
    return true;
}

bool Catalog::hasFolder(const std::string& folderpath) const
{
    if (! isValid(folderpath))
        return false;

    std::error_code ec;
    return std::filesystem::is_directory(root/folderpath, ec);
}

std::filesystem::directory_iterator Catalog::folderIterator(const std::string folderpath) const
{
    return std::filesystem::directory_iterator(root/folderpath);
}


bool Catalog::isLocked(const std::string& folderpath) const
{
    if (! isValid(folderpath))
        return true;

    std::error_code ec;
    return std::filesystem::is_regular_file(root/folderpath/LOCKED_FILENAME, ec);
}

std::optional<std::string> Catalog::getTitle(const std::string& filepath) const
{
    if (! isValid(filepath))
        return std::nullopt;

    // TODO
    return std::nullopt;
}

bool Catalog::hasIcon(const std::string& filepath) const
{
    if (! isValid(filepath))
        return false;

    // TODO
    return false;
}

bool Catalog::hasFile(const std::string& filepath) const
{
    if (isHidden(filepath))
        return false;

    std::error_code ec;
    return std::filesystem::is_regular_file(root/filepath, ec);
}

std::time_t Catalog::timestamp(const std::string& filepath) const
{
    const std::filesystem::path path{root/filepath};

    std::error_code ec;
    auto timestamp = std::filesystem::last_write_time(path, ec);
    if (ec)
        ESP_LOGW(TAG, "last_write_time() failed for \"%s\" [%s]", path.c_str(), ec.message().c_str());

    return timestamp.time_since_epoch().count();
}


std::ifstream Catalog::readContent(const std::string& filepath) const
{
    if (isHidden(filepath))
        // return a null stream
        return std::ifstream();

    return std::ifstream(root/filepath, std::ios_base::in | std::ios_base::binary);
}


// PRIVATE
//==============================================================================

bool Catalog::isHidden(const std::filesystem::path& filepath)
{
    for(const auto& i : filepath)
    {
        // ignore hidden files/folders and relative paths
        if (*(i.c_str()) == HIDDEN_PREFIX)
            return false;

    }
    return true;
}

bool Catalog::isLocked(const std::filesystem::path& path) const
{
    return std::filesystem::exists(path/LOCKED_FILENAME);
}


std::string Catalog::title(const std::filesystem::path& filepath) const
{
    std::string title_filename = TITLE_PREFIX;
    title_filename.append(filepath.filename());

    auto titlePath = filepath.parent_path() / title_filename;
    if (std::filesystem::is_regular_file(titlePath))
    {
        std::ifstream ifs(titlePath);
        std::stringstream ss; 
        ss << ifs.rdbuf();
        return ss.str();
    }

    // no title file, return empty string
    return "";
}

bool Catalog::hasIcon(const std::filesystem::path& filepath) const
{
    std::string icon_filename = ICON_PREFIX;
    icon_filename.append(filepath.filename());

    auto iconPath = filepath.parent_path() / icon_filename;
    return std::filesystem::is_regular_file(iconPath);
}
