#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include "catalogentry.h"

#include <QDialog>
#include <QPixmap>

class QLabel;
class QPushButton;
class ImageUtility;
class QComboBox;
class StampDB;

class ImageDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ImageDialog(StampDB* db, QWidget *parent = nullptr);
    explicit ImageDialog(StampDB* db, const QString& country, const QString& catNumber, QWidget *parent = nullptr);
    void setBookImages(const QString& country, const QString& catNumber);

signals:

private slots:
    void prevImagePressed();
    void nextImagePressed();
    void prevCatPressed();
    void nextCatPressed();
    void onIndexChanged(int index);

private:
    void buildDialog();
    void enableButtons();
    void setImage(int i);
    void setSelectedCatalogId(int selectedCatalogId);

    StampDB *m_db;
    int m_SelectedCatalogId;
    QLabel *m_SummaryText;
    QComboBox *m_CatalogComboBox;

    QLabel *m_ImageLabel;
    QPushButton *m_PrevImage;
    QPushButton *m_NextImage;
    QPushButton *m_PrevCat;
    QPushButton *m_NextCat;
    int m_CurrentImage;
    int m_NumCatalogImages;
    int m_NumPersonalImages;
    ImageUtility *m_ImageUtility;
    QString m_CurrentScott;
    QList<QPixmap> m_PixmapList;
    QList<QSharedPointer<CatalogEntry>> m_CatalogEntries;
};

#endif // IMAGEDIALOG_H
