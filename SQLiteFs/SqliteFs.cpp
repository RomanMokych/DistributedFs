
#include "SqliteFs.h"

#include "SqliteFsFolder.h"
#include "SqliteFsFile.h"
#include "SqliteFsException.h"

dfs::SqliteFs::SqliteFs(const dfs::Path& fsDbPath)
: m_gateway(new SqliteFsGateway(fsDbPath))
{}

dfs::SqliteFs::SqliteFs(std::unique_ptr<ISqliteFsGateway>&& gateway)
: m_gateway(std::move(gateway))
{}

dfs::FsError dfs::SqliteFs::createFolder(const Path& folderPath, const Permissions permissions)
{
    try
    {
        if (folderPath == "/")
            return FsError::fileExists;
        
        SqliteEntities::Folder folder = m_gateway->getFolderByPath(folderPath.parent_path());
        m_gateway->createFolder(folder.id, folderPath.leaf(), permissions);
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::openFolder(const Path& folderPath, std::unique_ptr<IFolder>& outFolder)
{
    try
    {
        SqliteEntities::Folder folder = m_gateway->getFolderByPath(folderPath);
        outFolder.reset(new SqliteFsFolder(folder.id, m_gateway.get()));
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

//files
dfs::FsError dfs::SqliteFs::openFile(const Path& filePath, const int fileOpenMode, std::unique_ptr<IFile>& outFile)
{
    try
    {
        if (filePath == "/")
            return FsError::fileHasWrongType;
        
        if (fileOpenMode & FileOpenMode::kCreate)
        {
            SqliteEntities::Folder parentFolder = m_gateway->getFolderByPath(filePath.parent_path());
            m_gateway->createFile(parentFolder.id, filePath.leaf(), dfs::Permissions::kAll);
        }
        
        SqliteEntities::Item item = m_gateway->getItemByPath(filePath);
        if (item.type != FileType::kFile)
        {
            return FsError::fileHasWrongType;
        }
        
        outFile.reset(new SqliteFsFile(item.concreteItemId, m_gateway.get()));
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::truncateFile(const Path& filePath, const uint64_t newSize)
{
    return dfs::FsError::notImplemented;
}

//links
dfs::FsError dfs::SqliteFs::createSymLink(const Path& linkPath, const Path& targetPath)
{
    try
    {
        if (linkPath == "/")
            return FsError::fileExists;
        
        SqliteEntities::Folder parentFolder = m_gateway->getFolderByPath(linkPath.parent_path());
        m_gateway->createSymLink(parentFolder.id, linkPath.leaf(), Permissions::kAll, targetPath);
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::readSymLink(const Path& linkPath, Path* symLinkValue)
{
    try
    {
        SqliteEntities::Item item = m_gateway->getItemByPath(linkPath, false);
        if (item.type != FileType::kSymLink)
        {
            return dfs::FsError::fileHasWrongType;
        }
        
        SqliteEntities::SymLink symLink = m_gateway->getSymLinkById(item.concreteItemId);
        *symLinkValue = symLink.path;
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::createHardLink(const Path& linkPath, const Path& targetPath)
{
    try
    {
        if (linkPath == "/")
            return FsError::fileExists;
        
        SqliteEntities::Item targetItem = m_gateway->getItemByPath(targetPath);
        SqliteEntities::Folder parentFolder = m_gateway->getFolderByPath(linkPath.parent_path());
        m_gateway->createHardLink(parentFolder.id, targetItem.id, linkPath.leaf());
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

//general
dfs::FsError dfs::SqliteFs::rename(const Path& oldPath, const Path& newPath)
{
    try
    {
        if (oldPath == "/")
            return FsError::fileExists;
        
        SqliteEntities::Item oldItem = m_gateway->getItemByPath(oldPath);
        SqliteEntities::Folder newParentFolder = m_gateway->getFolderByPath(newPath.parent_path());
        m_gateway->createHardLink(newParentFolder.id, oldItem.id, newPath.leaf());
        m_gateway->removeLink(oldItem.id);
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::remove(const Path& path)
{
    try
    {
        if (path == "/")
            return FsError::permissionDenied;
        
        SqliteEntities::Folder folder = m_gateway->getFolderByPath(path.parent_path());
        SqliteEntities::Link link = m_gateway->getLink(folder.id, path.leaf());
        
        m_gateway->removeLink(link.id);
        
        if (m_gateway->getItemLinksCount(link.itemId) == 0)
        {
            m_gateway->removeItem(link.itemId);
        }
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::getFileInfo(const Path& path, FileInfo* info)
{
    return dfs::FsError::notImplemented;
}

dfs::FsError dfs::SqliteFs::setPermissions(const Path& path, const Permissions permissions)
{
    try
    {
        SqliteEntities::Item item = m_gateway->getItemByPath(path);
        item.permissions = permissions;
        m_gateway->updateItem(item);
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::getPermissions(const Path& path, Permissions* permissions)
{
    try
    {
        SqliteEntities::Item item = m_gateway->getItemByPath(path);
        *permissions = item.permissions;
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::setCreationTime(const Path& path, const std::time_t time)
{
    try
    {
        SqliteEntities::Item item = m_gateway->getItemByPath(path);
        item.creationTime = time;
        m_gateway->updateItem(item);
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::getCreationTime(const Path& path, std::time_t* time)
{
    try
    {
        SqliteEntities::Item item = m_gateway->getItemByPath(path);
        *time = item.creationTime;
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::setModificationTime(const Path& path, const std::time_t time)
{
    try
    {
        SqliteEntities::Item item = m_gateway->getItemByPath(path);
        item.modificationTime = time;
        m_gateway->updateItem(item);
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::getModificationTime(const Path& path, std::time_t* time)
{
    try
    {
        SqliteEntities::Item item = m_gateway->getItemByPath(path);
        *time = item.modificationTime;
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::setExtendedAttribute(const Path& path, const char* attributeKey, const char* attributeValue, const size_t attributeValueSize)
{
    try
    {
        SqliteEntities::Item item = m_gateway->getItemByPath(path);
        m_gateway->addExtendedAttribute(item.id, attributeKey, attributeValue, static_cast<int>(attributeValueSize));
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::getExtendedAttribute(const Path& path, const char* attributeKey, std::vector<char>* attributeValue)
{
    try
    {
        SqliteEntities::Item item = m_gateway->getItemByPath(path);
        m_gateway->getExtendedAttribute(item.id, attributeKey, attributeValue);
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::deleteExtendedAttribute(const Path& path, const char* attributeKey)
{
    try
    {
        SqliteEntities::Item item = m_gateway->getItemByPath(path);
        m_gateway->deleteExtendedAttribute(item.id, attributeKey);
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}

dfs::FsError dfs::SqliteFs::getAllExtendedAttributes(const Path& path, std::vector<std::string>* attributesNames)
{
    try
    {
        SqliteEntities::Item item = m_gateway->getItemByPath(path);
        m_gateway->getExtendedAttributesNames(item.id, attributesNames);
    }
    catch (const SqliteFsException& e)
    {
        return e.getError();
    }
    
    return dfs::FsError::success;
}
