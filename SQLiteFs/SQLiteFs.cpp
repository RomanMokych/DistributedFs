
#include "SQLiteFs.h"

#include "SqliteFsFolder.h"

dfs::SQLiteFs::SQLiteFs(const dfs::Path& fsDbPath)
: m_gateway(fsDbPath)
{}

dfs::FsError dfs::SQLiteFs::createFolder(const Path& folderPath, const Permissions permissions)
{
    try
    {
        if (folderPath == "/")
            return FsError::kFileExists;
        
        SQLiteEntities::Folder folder = m_gateway.getFolderByPath(folderPath.parent_path());
        m_gateway.createFolder(folder.id, folderPath.leaf(), permissions);
    }
    catch (const SQLiteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::kSuccess;
}

dfs::FsError dfs::SQLiteFs::openFolder(const Path& folderPath, std::unique_ptr<IFolder>& outFolder)
{
    try
    {
        SQLiteEntities::Folder folder = m_gateway.getFolderByPath(folderPath);
        outFolder.reset(new SqliteFsFolder(folder.id, &m_gateway));
    }
    catch (const SQLiteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::kSuccess;
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
