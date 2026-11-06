#ifndef STAMPIMAGE_H
#define STAMPIMAGE_H

#include<QString>
#include <QPixmap>

//**************************************************************************
/*! \class StampImage
 *  \brief Hold an image and a path to the image.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2026
 *
 **************************************************************************/

class StampImage
{
public:
    StampImage();

    StampImage(QString filepath, QPixmap image);

    StampImage(const StampImage& stampImage);

    const StampImage& operator=(const StampImage& stampImage);

    QString getFilename() const {return m_Filename;}
    QString getFilepath() const {return m_Filepath;}
    QPixmap getImage() const {return m_Image;}

    /*! \brief Sets both the filepath and the filename. */
    void setFilepath(QString filepath);
    void setImage(QPixmap image) {m_Image = image;}

private:
    /*! \brief Filename held so that it does not need to be pulled from the path everytime. */
    QString m_Filename;
    /*! \brief Path to the file. */
    QString m_Filepath;
    /*! \brief Copy of the image. */
    QPixmap m_Image;
};

#endif // STAMPIMAGE_H
