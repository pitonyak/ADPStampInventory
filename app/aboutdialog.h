#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

//**************************************************************************
/*! \class AboutDialog
 * \brief About Dialog from Help | About menu option
 *
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2026
 **************************************************************************/

#include <QDialog>

class AboutDialog : public QDialog {
  Q_OBJECT
public:
  explicit AboutDialog(QWidget *parent = nullptr);
  ~AboutDialog() override = default;
};

#endif // ABOUTDIALOG_H
