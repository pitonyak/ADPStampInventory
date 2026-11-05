#include "aboutdialog.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent) {
  setWindowTitle(tr("About ADP Stamp Inventory"));
  setFixedSize(800, 400);
  setWindowFlags(windowFlags() &
                 ~Qt::WindowContextHelpButtonHint); // Remove the '?' button

  auto *mainLayout = new QVBoxLayout(this);
  auto *contentLayout = new QHBoxLayout();

  auto *logoLabel = new QLabel(this);
  QPixmap logo(
      ":/images/4_my_stamp.png"); // Path to your Qt Resource File (.qrc)
  if (!logo.isNull()) {
    logoLabel->setPixmap(
        logo.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  } else {
    logoLabel->setText("[LOGO]"); // Fallback
  }
  contentLayout->addWidget(logoLabel);

  auto *infoLabel = new QLabel(this);
  infoLabel->setTextFormat(Qt::RichText);
  infoLabel->setOpenExternalLinks(true); // Allows users to click hyperlinks
  infoLabel->setText(
      "<h2><b>ADP Stamp Inventory</b></h2>"
      "<p>Version 0.5.1 (64-bit)</p>"
      "<p>Built on: " __DATE__ "</p>"
      "<p>&copy; 2026 Andrew Pitonyak</p>"
      "<p><a href='https://github.com/pitonyak/ADPStampInventory'>https://github.com/pitonyak/ADPStampInventory"
      "</a></p>");
  contentLayout->addWidget(infoLabel);
  contentLayout->setStretchFactor(infoLabel, 1);

  auto *closeButton = new QPushButton(tr("Close"), this);
  closeButton->setDefault(true);
  connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

  auto *buttonLayout = new QHBoxLayout();
  buttonLayout->addStretch();
  buttonLayout->addWidget(closeButton);

  mainLayout->addLayout(contentLayout);
  mainLayout->addSpacing(10);
  mainLayout->addLayout(buttonLayout);
}