
#include "SqliteFs.h"

#include "SqliteFsFolder.h"
#include "SqliteFsException.h"

dfs::SqliteFs::SqliteFs(const dfs::Path& fsDbPath)
: m_gateway(fsDbPath)
{}

dfs::FsError dfs::SqliteFs::createFolder(const Path& folderPath, const Permissions permissions)
{
    try
    {
        if (folderPath == "/")
            return FsError::kFileExists;
        
        SqliteEntities::Folder folder = m_gateway.getFolderByPath(folderPath.parent_path());
        m_gateway.createFolder(folder.id, folderPath.leaf(), permissions);
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::kSuccess;
}

dfs::FsError dfs::SqliteFs::openFolder(const Path& folderPath, std::unique_ptr<IFolder>& outFolder)
{
    try
    {
        SqliteEntities::Folder folder = m_gateway.getFolderByPath(folderPath);
        outFolder.reset(new SqliteFsFolder(folder.id, &m_gateway));
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::kSuccess;
}

//files
dfs::FsError dfs::SqliteFs::openFile(const Path& filePath, const int fileOpenMode, std::unique_ptr<IFile>& outFile)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SqliteFs::truncateFile(const Path& filePath, const uint64_t newSize)
{
    return dfs::FsError::kNotImplemented;
}

//links
dfs::FsError dfs::SqliteFs::createSymLink(const Path& linkPath, const Path& targetPath)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SqliteFs::readSymLink(const Path& linkPath, Path* symLinkValue)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SqliteFs::createHardLink(const Path& linkPath, const Path& targetPath)
{
    try
    {
        if (linkPath == "/")
            return FsError::kFileExists;
        
        SqliteEntities::Item targetItem = m_gateway.getItemByPath(targetPath);
        SqliteEntities::Folder parentFolder = m_gateway.getFolderByPath(linkPath.parent_path());
        m_gateway.createHardLink(parentFolder.id, targetItem.id, linkPath.leaf());
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::kSuccess;
}

//general
dfs::FsError dfs::SqliteFs::rename(const Path& oldPath, const Path& newPath)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SqliteFs::remove(const Path& path)
{
    try
    {
        if (path == "/")
            return FsError::kPermissionDenied;
        
        SqliteEntities::Folder folder = m_gateway.getFolderByPath(path.parent_path());
        SqliteEntities::Link link = m_gateway.getLink(folder.id, path.leaf());
        m_gateway.removeLink(link.id);
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::kSuccess;
}

dfs::FsError dfs::SqliteFs::getFileInfo(const Path& path, FileInfo* info)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SqliteFs::setPermissions(const Path& path, const Permissions permissions)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SqliteFs::getPermissions(const Path& path, Permissions* permissions)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SqliteFs::setCreationTime(const Path& path, const std::time_t time)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SqliteFs::getCreationTime(const Path& path, std::time_t* time)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SqliteFs::setModificationTime(const Path& path, const std::time_t time)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SqliteFs::getModificationTime(const Path& path, std::time_t* time)
{
    return dfs::FsError::kNotImplemented;
}

dfs::FsError dfs::SqliteFs::setExtendedAttribute(const Path& path, const char* attributeKey, const char* attributeValue, const size_t attributeValueSize)
{
    try
    {
        SqliteEntities::Item item = m_gateway.getItemByPath(path);
        m_gateway.addExtendedAttribute(item.id, attributeKey, attributeValue, static_cast<int>(attributeValueSize));
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::kSuccess;
}

dfs::FsError dfs::SqliteFs::getExtendedAttribute(const Path& path, const char* attributeKey, std::vector<char>* attributeValue)
{
    try
    {
        SqliteEntities::Item item = m_gateway.getItemByPath(path);
        m_gateway.getExtendedAttribute(item.id, attributeKey, attributeValue);
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::kSuccess;
}

dfs::FsError dfs::SqliteFs::deleteExtendedAttribute(const Path& path, const char* attributeKey)
{
    try
    {
        SqliteEntities::Item item = m_gateway.getItemByPath(path);
        m_gateway.deleteExtendedAttribute(item.id, attributeKey);
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::kSuccess;
}

dfs::FsError dfs::SqliteFs::getAllExtendedAttributes(const Path& path, std::vector<std::string>* attributesNames)
{
    try
    {
        SqliteEntities::Item item = m_gateway.getItemByPath(path);
        m_gateway.getExtendedAttributesNames(item.id, attributesNames);
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::kSuccess;
}
