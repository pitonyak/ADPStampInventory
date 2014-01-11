#ifndef SCROLLMESSAGEBOX_H
#define SCROLLMESSAGEBOX_H

#include <QDialog>
#include <QMessageBox>
#include <QDialogButtonBox>

//**************************************************************************
//! Filter to decide if a specific file or directory matches some criteria.
/*!
 *  This routine is Copyright by Andrew Pitonyak, and may be used
 *  without restrictions or renumerations. I make no statement of correctness.
 ***************************************************************************/

class ScrollMessageBox : public QDialog
{
    Q_OBJECT
public:
    explicit ScrollMessageBox(QWidget * parent = 0);

    ScrollMessageBox(QMessageBox::Icon icon, const QString &title, const QString &text,
                QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::NoButton, QWidget *parent = 0);

    void setDefaultButton(QDialogButtonBox::StandardButton button);

    static QDialogButtonBox::StandardButton critical(QWidget* parent, QString const& title, QString const& text, QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok, QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::NoButton);
    static QDialogButtonBox::StandardButton information(QWidget* parent, QString const& title, QString const& text, QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok , QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::NoButton);
    static QDialogButtonBox::StandardButton question(QWidget* parent, QString const& title, QString const& text, QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Yes | QDialogButtonBox::No, QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::NoButton);
    static QDialogButtonBox::StandardButton warning(QWidget* parent, QString const& title, QString const& text, QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok, QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::NoButton);

    void showEvent ( QShowEvent * event );

private:
    QPixmap standardIcon(QMessageBox::Icon icon);
    void setDefaultButton(QPushButton *button);
    void updateSize();

    QLabel *label;
    QDialogButtonBox *buttonBox;

private Q_SLOTS:
    void handle_buttonClicked(QAbstractButton *button);
};

#endif // SCROLLMESSAGEBOX_H
