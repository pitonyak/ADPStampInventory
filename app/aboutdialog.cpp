#include "aboutdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle(tr("About ADP Stamp Inventory"));
    setFixedSize(800, 400);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // Remove the '?' button

    // 2. Create the layout structure
    auto *mainLayout = new QVBoxLayout(this);
    auto *contentLayout = new QHBoxLayout();

    auto *logoLabel = new QLabel(this);
    QPixmap logo(":/images/logo.png"); // Path to your Qt Resource File (.qrc) TODO
    if (!logo.isNull()) {
        logoLabel->setPixmap(logo.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        logoLabel->setText("[LOGO]"); // Fallback
    }
    contentLayout->addWidget(logoLabel);

    auto *infoLabel = new QLabel(this);
    infoLabel->setTextFormat(Qt::RichText);
    infoLabel->setOpenExternalLinks(true); // Allows users to click hyperlinks
    infoLabel->setText(
        "<h2><b>ADP Stamp Inventory</b></h2>"
        "<p>Version 2.4.1 (64-bit) ??? TODO: Can I pull something from here?</p>"
        "<p>Built on: " __DATE__ "</p>"
        "<p>&copy; 2026 Andrew Pitonyak</p>"
        "<p><a href='https://www.pitonyak.org'>TODO reference  the git repo</a></p>"
        );
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