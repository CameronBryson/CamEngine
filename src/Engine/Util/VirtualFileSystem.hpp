#pragma once

#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <filesystem>
#include <unordered_map>

namespace vfs {

    // Simple path handling
    class Path {
    public:
        Path() = default;
        Path(const std::string& path);
        Path(const char* path);

        Path join(const Path& other) const;
        Path parent() const;
        std::string filename() const;
        std::string extension() const;
        std::string string() const;

        bool operator==(const Path& other) const;
        bool operator!=(const Path& other) const;

    private:
        std::string m_path;
    };

    // File interface
    class File {
    public:
        virtual ~File() = default;

        virtual std::vector<uint8_t> readBytes() = 0;
        virtual std::string readString() = 0;
        virtual bool writeBytes(const std::vector<uint8_t>& data) = 0;
        virtual bool writeString(const std::string& data) = 0;
        virtual size_t size() const = 0;
        virtual bool exists() const = 0;
    };

    // FileSystem singleton
    class FileSystem {
    public:
        static FileSystem& instance();

        // Mount/unmount directories
        bool mount(const std::string& mountPoint, const std::string& physicalPath);
        bool unmount(const std::string& mountPoint);

        // File operations
        bool fileExists(const std::string& path) const;
        std::vector<uint8_t> readFile(const std::string& path);
        std::string readTextFile(const std::string& path);
        bool writeFile(const std::string& path, const std::vector<uint8_t>& data);
        bool writeTextFile(const std::string& path, const std::string& data);

        // Directory operations
        bool createDirectory(const std::string& path);
        bool directoryExists(const std::string& path) const;
        std::vector<std::string> listDirectory(const std::string& path) const;

    private:
        FileSystem() = default;
        std::string resolvePath(const std::string& virtualPath) const;
        std::unordered_map<std::string, std::string> m_mountPoints;
    };

    // Helper functions
    std::string readTextFile(const std::string& path);
    std::vector<uint8_t> readBinaryFile(const std::string& path);
    bool writeTextFile(const std::string& path, const std::string& data);
    bool writeBinaryFile(const std::string& path, const std::vector<uint8_t>& data);

} // namespace vfs
