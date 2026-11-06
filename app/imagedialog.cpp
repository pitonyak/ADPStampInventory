#include "imagedialog.h"
#include "imageutility.h"
#include "stampdb.h"
#include "stampimage.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

ImageDialog::ImageDialog(StampDB *db, QWidget *parent)
    : QDialog{parent}, m_db{db}, m_SelectedCatalogId{-1},
      m_SummaryText{nullptr}, m_CatalogComboBox{nullptr}, m_ImageLabel{nullptr},
      m_PrevImage{nullptr}, m_NextImage{nullptr}, m_PrevCat{nullptr},
      m_NextCat{nullptr}, m_CurrentImageIndex(0), m_NumStockImages(0),
      m_NumPersonalImages(0), m_ImageUtility{nullptr}, m_CurrentScott("") {
  setWindowTitle("Image Dialog");
  resize(800, 600);
  buildDialog();
}

ImageDialog::ImageDialog(StampDB *db, const QString &country,
                         const QString &catNumber, QWidget *parent)
    : ImageDialog(db, parent) {
  setBookImages(country, catNumber);
}

void ImageDialog::buildDialog() {
  m_ImageUtility = new ImageUtility(this);
  m_SummaryText = new QLabel(this);
  m_SummaryText->setTextFormat(Qt::RichText);
  m_SummaryText->setText("TEST EMPTY DATA");

  m_CatalogComboBox = new QComboBox();
  connect(m_CatalogComboBox,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &ImageDialog::onIndexChanged);

  QVBoxLayout *vLayout;
  QHBoxLayout *hLayout;

  m_PrevImage = new QPushButton(tr("<< Image"));
  m_NextImage = new QPushButton(tr("Image >>"));
  m_PrevCat = new QPushButton(tr("<< Scott"));
  m_NextCat = new QPushButton(tr("Scott >>"));

  vLayout = new QVBoxLayout(this);
  hLayout = new QHBoxLayout(this);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  hLayout->addWidget(m_PrevImage);
  connect(m_PrevImage, SIGNAL(clicked()), this, SLOT(prevImagePressed()));
  connect(m_NextImage, SIGNAL(clicked()), this, SLOT(nextImagePressed()));
  connect(m_PrevCat, SIGNAL(clicked()), this, SLOT(prevCatPressed()));
  connect(m_NextCat, SIGNAL(clicked()), this, SLOT(nextCatPressed()));

  hLayout->addWidget(m_PrevImage);
  hLayout->addWidget(m_NextImage);
  hLayout->addSpacing(30);
  hLayout->addWidget(m_PrevCat);
  hLayout->addWidget(m_NextCat);
  hLayout->addSpacing(30);
  hLayout->addWidget(buttonBox);

  m_ImageLabel = new QLabel(this);
  vLayout->addWidget(m_ImageLabel);
  vLayout->addLayout(hLayout);
  vLayout->addWidget(m_CatalogComboBox);
  vLayout->addWidget(m_SummaryText);
}

void ImageDialog::setBookImages(const QString &country,
                                const QString &catNumber) {
  if (m_CatalogEntries.empty() && m_db != nullptr) {
    m_CatalogEntries = m_db->getCatalog(true);
    QStringList tempList;
    tempList.reserve(m_CatalogEntries.size());
    for (const auto &item : m_CatalogEntries) {
      tempList.append(item->getCatalogId() + " / " + item->getCountryA3());
    }
    m_CatalogComboBox->addItems(tempList);
  }

  // There will always be at least one image even if it is the default "empty"
  // image.
  m_CurrentScott = catNumber;

  // Get standard catalog images with no empty image.
  m_StampImageList = m_ImageUtility->getStampImages(country, catNumber, ImageUtility::Image_Flags::Stock, false);
  m_NumStockImages = m_StampImageList.size();
  m_StampImageList.append(m_ImageUtility->getStampImages(country, catNumber, ImageUtility::Image_Flags::Mine, false));
  m_NumPersonalImages = m_StampImageList.size() - m_NumStockImages;
  if (m_StampImageList.isEmpty())
  {
      m_StampImageList.append(StampImage("empty.png", m_ImageUtility->getEmptyImage()));
  }
  setImage(0);

  // Not efficient but this is not really sorted in the conventional sense.
  // IDs such as 1CVP7 really mess with the sorting because 1CVP is part of the
  // category and not part of the number to sort.
  int idx = 0;
  int found = false;
  for (const QSharedPointer<CatalogEntry> &entry : m_CatalogEntries) {
    if (entry->getCatalogId() == catNumber &&
        entry->getCountryA3() == country) {
      found = true;
      break;
    }
    ++idx;
  }
  setSelectedCatalogId(found ? idx : 0);
  enableButtons();
}

void ImageDialog::onIndexChanged(int index) {
  if (index >= 0 && index < m_CatalogEntries.size() &&
      index != m_SelectedCatalogId) {
    setBookImages(m_CatalogEntries.at(index)->getCountryA3(),
                  m_CatalogEntries.at(index)->getCatalogId());
  }
}

void ImageDialog::setSelectedCatalogId(int selectedCatalogId) {
  if (m_CatalogComboBox->count() < 1) {
    return;
  }
  if (m_SelectedCatalogId == selectedCatalogId)
    return;

  m_SelectedCatalogId = selectedCatalogId;
  if (selectedCatalogId < 0 ||
      selectedCatalogId >= m_CatalogComboBox->count()) {
    selectedCatalogId = 0;
  }
  m_CatalogComboBox->setCurrentIndex(selectedCatalogId);
  const QSharedPointer<CatalogEntry> &entry =
      m_CatalogEntries.at(selectedCatalogId);
  m_SummaryText->setText(QString("<table cellpadding='0' cellspacing='5' border='0'>"
                                 "<tr><td><b>#</b></td><td>%2 / %1</td><td></td><td><b>Postage</b></td><td>$%5</td><td><b>Released:</b></td><td>%4</td><td><b>Updated:</b></td><td>%6</td></tr>"
                                 "</table>"
                                 "<p><b>Description:</b> %3</p>")
                             .arg(entry->getCountryA3(), entry->getCatalogId(),
                                  entry->getDescription(),
                                  entry->getReleaseDate().date().toString()).arg(entry->getFaceValue()).arg(entry->getUpdatedDate().toString()));
}

void ImageDialog::enableButtons() {
  if (m_StampImageList.count() < 2) {
    m_PrevImage->setEnabled(false);
    m_NextImage->setEnabled(false);
  } else {
    m_PrevImage->setEnabled(m_CurrentImageIndex > 0);
    m_NextImage->setEnabled((m_CurrentImageIndex + 1) < m_StampImageList.count() &&
                            (m_NumStockImages > 0 || m_NumPersonalImages > 0));
  }
  m_PrevCat->setEnabled(m_SelectedCatalogId > 0);
  m_NextCat->setEnabled((m_SelectedCatalogId + 1) < m_CatalogEntries.size());
  if (m_CurrentImageIndex < m_NumStockImages) {
    setWindowTitle(QString("%1/%2 Stock Image for #%3 : (%4)")
                       .arg(m_CurrentImageIndex + 1)
                       .arg(m_NumStockImages + m_NumPersonalImages)
                       .arg(m_CurrentScott).arg(m_StampImageList.at(m_CurrentImageIndex).getFilename()));
  } else if (m_CurrentImageIndex < m_NumStockImages + m_NumPersonalImages) {
    setWindowTitle(QString("%1/%2 Personal Image for #%3 : (%4)")
                       .arg(m_CurrentImageIndex + 1)
                       .arg(m_NumStockImages + m_NumPersonalImages)
                         .arg(m_CurrentScott).arg(m_StampImageList.at(m_CurrentImageIndex).getFilename()));
  } else {
    setWindowTitle("No Image for #" + m_CurrentScott);
  }
}

void ImageDialog::setImage(int i) {
  if (0 <= i && i < m_StampImageList.count()) {
    m_CurrentImageIndex = i;
    QPixmap pixmap = m_StampImageList.at(m_CurrentImageIndex).getImage();
    m_ImageLabel->setPixmap(
        pixmap.scaled(550, 350, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_ImageLabel->setAlignment(Qt::AlignCenter);
  }
  enableButtons();
}

void ImageDialog::prevImagePressed() { setImage(m_CurrentImageIndex - 1); }

void ImageDialog::nextImagePressed() { setImage(m_CurrentImageIndex + 1); }

void ImageDialog::prevCatPressed() {
  if (m_SelectedCatalogId > 0) {
    onIndexChanged(m_SelectedCatalogId - 1);
  }
}

void ImageDialog::nextCatPressed() {
  if (m_SelectedCatalogId < m_CatalogEntries.size()) {
    onIndexChanged(m_SelectedCatalogId + 1);
  }
}
