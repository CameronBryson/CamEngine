#include "pch.hpp"
#include "VirtualFileSystem.hpp"
#include "Logging.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace vfs {

    // Path implementation
    Path::Path(const std::string& path) : m_path(path) {
        // Normalize path separators
        std::replace(m_path.begin(), m_path.end(), '\\', '/');
    }

    Path::Path(const char* path) : Path(std::string(path)) {}

    Path Path::join(const Path& other) const {
        std::filesystem::path fsPath(m_path);
        std::filesystem::path otherFsPath(other.m_path);
        return Path((fsPath / otherFsPath).string());
    }

    Path Path::parent() const {
        std::filesystem::path fsPath(m_path);
        return Path(fsPath.parent_path().string());
    }

    std::string Path::filename() const {
        std::filesystem::path fsPath(m_path);
        return fsPath.filename().string();
    }

    std::string Path::extension() const {
        std::filesystem::path fsPath(m_path);
        return fsPath.extension().string();
    }

    std::string Path::string() const {
        return m_path;
    }

    bool Path::operator==(const Path& other) const {
        return m_path == other.m_path;
    }

    bool Path::operator!=(const Path& other) const {
        return m_path != other.m_path;
    }

    // Physical File implementation
    class PhysicalFile : public File {
    public:
        PhysicalFile(const std::string& path, bool write)
            : m_path(path), m_exists(false)
        {
            std::ios_base::openmode mode = std::ios::binary;
            mode |= write ? std::ios::out : std::ios::in;

            if (write) {
                m_outStream.open(m_path, mode);
                m_exists = m_outStream.is_open();
            } else {
                m_inStream.open(m_path, mode);
                m_exists = m_inStream.is_open();
            }

            if (!m_exists) {
                LOG_ERROR(logging::gEngineLogger, "Failed to open file: {}", m_path);
            }
        }

        ~PhysicalFile() override {
            if (m_inStream.is_open()) {
                m_inStream.close();
            }
            if (m_outStream.is_open()) {
                m_outStream.close();
            }
        }

        std::vector<uint8_t> readBytes() override {
            if (!m_exists || !m_inStream.is_open()) {
                LOG_ERROR(logging::gEngineLogger, "Attempting to read from closed/non-existent file: {}", m_path);
                return {};
            }

            m_inStream.seekg(0, std::ios::end);
            size_t fileSize = m_inStream.tellg();
            m_inStream.seekg(0, std::ios::beg);

            std::vector<uint8_t> buffer(fileSize);
            m_inStream.read(reinterpret_cast<char*>(buffer.data()), fileSize);

            return buffer;
        }

        std::string readString() override {
            if (!m_exists || !m_inStream.is_open()) {
                LOG_ERROR(logging::gEngineLogger, "Attempting to read from closed/non-existent file: {}", m_path);
                return {};
            }

            std::stringstream buffer;
            buffer << m_inStream.rdbuf();
            return buffer.str();
        }

        bool writeBytes(const std::vector<uint8_t>& data) override {
            if (!m_outStream.is_open()) {
                LOG_ERROR(logging::gEngineLogger, "Attempting to write to closed file: {}", m_path);
                return false;
            }

            m_outStream.write(reinterpret_cast<const char*>(data.data()), data.size());
            return m_outStream.good();
        }

        bool writeString(const std::string& data) override {
            if (!m_outStream.is_open()) {
                LOG_ERROR(logging::gEngineLogger, "Attempting to write to closed file: {}", m_path);
                return false;
            }

            m_outStream << data;
            return m_outStream.good();
        }

        size_t size() const override {
            if (!m_exists) {
                return 0;
            }

            std::error_code ec;
            uintmax_t size = std::filesystem::file_size(m_path, ec);
            return ec ? 0 : static_cast<size_t>(size);
        }

        bool exists() const override {
            return m_exists;
        }

    private:
        std::string m_path;
        std::ifstream m_inStream;
        std::ofstream m_outStream;
        bool m_exists;
    };

    // FileSystem implementation
    FileSystem& FileSystem::instance() {
        static FileSystem instance;
        return instance;
    }

    bool FileSystem::mount(const std::string& mountPoint, const std::string& physicalPath) {
        if (m_mountPoints.find(mountPoint) != m_mountPoints.end()) {
            LOG_WARN(logging::gEngineLogger, "Mount point already exists: {}", mountPoint);
            return false;
        }

        // Ensure the physical path exists
        std::error_code ec;
        if (!std::filesystem::exists(physicalPath, ec)) {
            std::filesystem::create_directories(physicalPath, ec);
            if (ec) {
                LOG_ERROR(logging::gEngineLogger, "Failed to create mount directory: {}", physicalPath);
                return false;
            }
        }

        m_mountPoints[mountPoint] = physicalPath;
        LOG_INFO(logging::gEngineLogger, "Mounted '{}' to '{}'", physicalPath, mountPoint);

        return true;
    }

    bool FileSystem::unmount(const std::string& mountPoint) {
        auto it = m_mountPoints.find(mountPoint);
        if (it == m_mountPoints.end()) {
            LOG_WARN(logging::gEngineLogger, "Mount point does not exist: {}", mountPoint);
            return false;
        }

        m_mountPoints.erase(it);
        LOG_INFO(logging::gEngineLogger, "Unmounted '{}'", mountPoint);

        return true;
    }

    std::string FileSystem::resolvePath(const std::string& virtualPath) const {
        // Find the longest matching mount point
        std::string bestMountPoint;
        std::string bestPhysicalPath;

        for (const auto& [mp, physPath] : m_mountPoints) {
            if (virtualPath.compare(0, mp.length(), mp) == 0) {
                if (mp.length() > bestMountPoint.length()) {
                    bestMountPoint = mp;
                    bestPhysicalPath = physPath;
                }
            }
        }

        if (bestPhysicalPath.empty()) {
            LOG_WARN(logging::gEngineLogger, "No mount point found for path: {}", virtualPath);
            return "";
        }

        // Remove mount point prefix from path
        std::string relativePath = virtualPath.substr(bestMountPoint.length());
        if (!relativePath.empty() && (relativePath[0] == '/' || relativePath[0] == '\\')) {
            relativePath = relativePath.substr(1);
        }

        // Replace backslashes with forward slashes
        std::replace(relativePath.begin(), relativePath.end(), '\\', '/');

        // Join with physical path
        std::filesystem::path result = std::filesystem::path(bestPhysicalPath) / relativePath;
        return result.string();
    }

    bool FileSystem::fileExists(const std::string& path) const {
        std::string physicalPath = resolvePath(path);
        if (physicalPath.empty()) {
            return false;
        }

        std::error_code ec;
        return std::filesystem::exists(physicalPath, ec) && std::filesystem::is_regular_file(physicalPath, ec);
    }

    std::vector<uint8_t> FileSystem::readFile(const std::string& path) {
        std::string physicalPath = resolvePath(path);
        if (physicalPath.empty()) {
            return {};
        }

        PhysicalFile file(physicalPath, false);
        if (!file.exists()) {
            LOG_ERROR(logging::gEngineLogger, "Failed to open file for reading: {}", path);
            return {};
        }

        return file.readBytes();
    }

    std::string FileSystem::readTextFile(const std::string& path) {
        std::string physicalPath = resolvePath(path);
        if (physicalPath.empty()) {
            return {};
        }

        PhysicalFile file(physicalPath, false);
        if (!file.exists()) {
            LOG_ERROR(logging::gEngineLogger, "Failed to open file for reading: {}", path);
            return {};
        }

        return file.readString();
    }

    bool FileSystem::writeFile(const std::string& path, const std::vector<uint8_t>& data) {
        std::string physicalPath = resolvePath(path);
        if (physicalPath.empty()) {
            return false;
        }

        // Create parent directories if needed
        std::filesystem::path parentPath = std::filesystem::path(physicalPath).parent_path();
        if (!parentPath.empty()) {
            std::error_code ec;
            std::filesystem::create_directories(parentPath, ec);
            if (ec) {
                LOG_ERROR(logging::gEngineLogger, "Failed to create parent directory: {}", parentPath.string());
                return false;
            }
        }

        PhysicalFile file(physicalPath, true);
        return file.writeBytes(data);
    }

    bool FileSystem::writeTextFile(const std::string& path, const std::string& data) {
        std::string physicalPath = resolvePath(path);
        if (physicalPath.empty()) {
            return false;
        }

        // Create parent directories if needed
        std::filesystem::path parentPath = std::filesystem::path(physicalPath).parent_path();
        if (!parentPath.empty()) {
            std::error_code ec;
            std::filesystem::create_directories(parentPath, ec);
            if (ec) {
                LOG_ERROR(logging::gEngineLogger, "Failed to create parent directory: {}", parentPath.string());
                return false;
            }
        }

        PhysicalFile file(physicalPath, true);
        return file.writeString(data);
    }

    bool FileSystem::createDirectory(const std::string& path) {
        std::string physicalPath = resolvePath(path);
        if (physicalPath.empty()) {
            return false;
        }

        std::error_code ec;
        bool success = std::filesystem::create_directories(physicalPath, ec);
        if (ec) {
            LOG_ERROR(logging::gEngineLogger, "Failed to create directory: {}", physicalPath);
        }
        return success && !ec;
    }

    bool FileSystem::directoryExists(const std::string& path) const {
        std::string physicalPath = resolvePath(path);
        if (physicalPath.empty()) {
            return false;
        }

        std::error_code ec;
        return std::filesystem::exists(physicalPath, ec) && std::filesystem::is_directory(physicalPath, ec);
    }

    std::vector<std::string> FileSystem::listDirectory(const std::string& path) const {
        std::string physicalPath = resolvePath(path);
        std::vector<std::string> result;

        if (physicalPath.empty() || !directoryExists(path)) {
            LOG_ERROR(logging::gEngineLogger, "Cannot list non-existent directory: {}", path);
            return result;
        }

        std::error_code ec;
        std::filesystem::path basePath = physicalPath;

        for (const auto& entry : std::filesystem::directory_iterator(physicalPath, ec)) {
            if (ec) {
                LOG_ERROR(logging::gEngineLogger, "Error iterating directory: {}", physicalPath);
                continue;
            }

            // Get relative path from mount point
            std::filesystem::path relativePath = std::filesystem::relative(entry.path(), basePath);
            result.push_back(relativePath.string());
        }

        return result;
    }

    // Helper functions
    std::string readTextFile(const std::string& path) {
        return FileSystem::instance().readTextFile(path);
    }

    std::vector<uint8_t> readBinaryFile(const std::string& path) {
        return FileSystem::instance().readFile(path);
    }

    bool writeTextFile(const std::string& path, const std::string& data) {
        return FileSystem::instance().writeTextFile(path, data);
    }

    bool writeBinaryFile(const std::string& path, const std::vector<uint8_t>& data) {
        return FileSystem::instance().writeFile(path, data);
    }

} // namespace vfs
