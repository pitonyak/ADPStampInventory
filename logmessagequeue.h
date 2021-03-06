#ifndef LOGMESSAGEQUEUE_H
#define LOGMESSAGEQUEUE_H

#include <QObject>
#include <QQueue>
#include <QString>
#include <QDateTime>
#include <QMutex>

#include "simpleloggerroutinginfo.h"
#include "logmessagecontainer.h"

//**************************************************************************
//! Queue of log messages
/*!
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2011-2013
 ***************************************************************************/class LogMessageQueue : public QObject
{
  Q_OBJECT
public:
  //**************************************************************************
  /*! \brief Default constructor with no backup set.
   *
   *  \param [in] parent is this object's owner and the destructor will destroys all child objects.
   ***************************************************************************/
  explicit LogMessageQueue(QObject *parent = 0);

  ~LogMessageQueue();

  //**************************************************************************
  /*! \brief Receive a message and queue it for logging.
   *  \param [in] message Primary message to log.
   *  \param [in] location Location provided compiler macros; usually file name and line number.
   *  \param [in] dateTime Date and time the message was created.
   *  \param [in] category Message category flags such as error, warning, informational, etc.
   *  \param [in] level Severity level. Lower is more sever, so level 100 is less likely to be logged than 3.
   ***************************************************************************/
  void enqueue(const QString& message, const QString& location, const QDateTime dateTime, const SimpleLoggerRoutingInfo::MessageCategory category, const int level);

  //**************************************************************************
  /*! \brief Receive a message and queue it for logging. This is thread-safe.
   *  \param [in] message is the encapsulated message to log. I own it, you don't.
   ***************************************************************************/
  void enqueue(LogMessageContainer* message);

  //**************************************************************************
  /*! \brief Determine if the queue is empty.
   *  \return True if the queue is empty, false otherwise.
   ***************************************************************************/
  bool isEmpty() const;

  //**************************************************************************
  /*! \brief Take the contents of the contained queue and place them into the parameter.
   *  On return, the queue in this object will be empty and the parameter queue will
   *  contain all of the messages in the current queue.
   *  Any values in the parameter queue are lost.
   *
   *  \param [in, out] queue contains the existing elements on return.
   ***************************************************************************/
  void getAll(QQueue<LogMessageContainer*>& queue);

  QQueue<LogMessageContainer*>& getQueue() { return m_queue; }

signals:

public slots:
  //**************************************************************************
  /*! \brief Receive a message and queue it for logging.
   *  \param [in] message Primary message to log.
   *  \param [in] location Location provided compiler macros; usually file name and line number.
   *  \param [in] dateTime Date and time the message was created.
   *  \param [in] category Message category flags such as error, warning, informational, etc.
   *  \param [in] level Severity level. Lower is more sever, so level 100 is less likely to be logged than 3.
   ***************************************************************************/
  void receiveMessage(const QString& message, const QString& location, const QDateTime dateTime, const SimpleLoggerRoutingInfo::MessageCategory category, const int level);

private:
  mutable QMutex m_mutex;
  QQueue<LogMessageContainer*> m_queue;

};

#endif // LOGMESSAGEQUEUE_H
