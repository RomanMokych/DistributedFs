
#include "SQLiteFs.h"

dfs::SQLiteFs::SQLiteFs(const dfs::Path& fsDpPath)
{

}

dfs::FsError dfs::SQLiteFs::createFolder(const Path& folderPath, const Permissions permissions)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::openFolder(const Path& folderPath, std::unique_ptr<IFolder>& outFolder)
{
    return dfs::FsError::kNotImplemented;
}

//files
dfs::FsError dfs::SQLiteFs::openFile(const Path& filePath, const int fileOpenMode, std::unique_ptr<IFile>& outFile)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::truncateFile(const Path& filePath, const uint64_t newSize)
{
    return dfs::FsError::kNotImplemented;
}

//links
dfs::FsError dfs::SQLiteFs::createSymLink(const Path& linkPath, const Path& targetPath)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::readSymLink(const Path& linkPath, Path* symLinkValue)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::createHardLink(const Path& linkPath, const Path& targetPath)
{
    return dfs::FsError::kNotImplemented;
}

//general
dfs::FsError dfs::SQLiteFs::rename(const Path& oldPath, const Path& newPath)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::remove(const Path& path)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::getFileInfo(const Path& path, FileInfo* info)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::setPermissions(const Path& path, const Permissions permissions)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::getPermissions(const Path& path, Permissions* permissions)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::setCreationTime(const Path& path, const std::time_t time)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::getCreationTime(const Path& path, std::time_t* time)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::setModificationTime(const Path& path, const std::time_t time)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::getModificationTime(const Path& path, std::time_t* time)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::setExtendedAttribute(const Path& path, const char* attributeKey, const char* attributeValue, const size_t attributeValueSize)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::getExtendedAttribute(const Path& path, const char* attributeKey, std::vector<char>* attributeValue)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::deleteExtendedAttribute(const Path& path, const char* attributeKey)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SQLiteFs::getAllExtendedAttributes(const Path& path, std::vector<std::string>* attributesNames)
{
    return dfs::FsError::kNotImplemented;
}
