#include "stampimage.h"

#include <QFileInfo>

StampImage::StampImage() {}

StampImage::StampImage(QString filepath, QPixmap image)
{
    QFileInfo fileInfo(filepath);
    m_Filename = fileInfo.fileName();
    m_Filepath = filepath;
    m_Image = image;
}

void StampImage::setFilepath(QString filepath)
{
    QFileInfo fileInfo(filepath);
    m_Filename = fileInfo.fileName();
    m_Filepath = filepath;
}

StampImage::StampImage(const StampImage& stampImage)
{
    m_Image = stampImage.getImage();
    m_Filename = stampImage.getFilename();
    m_Filepath = stampImage.getFilepath();
}

const StampImage& StampImage::operator=(const StampImage& stampImage)
{
    if (this != &stampImage) {
        m_Image = stampImage.getImage();
        m_Filename = stampImage.getFilename();
        m_Filepath = stampImage.getFilepath();
    }
    return *this;
}
