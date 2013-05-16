#ifndef CONSTANTS_H
#define CONSTANTS_H

#include<QString>

//**************************************************************************
/*! \class Constants
 *  \brief Encapulate constants in a typesafe way.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2011-2013
 *
 **************************************************************************/

class Constants
{
public:
  Constants();

  /*! Default database name. */
  static QString Default_DBName;

  /*! Settings Name to find the DB Name in the settings. */
  static QString Settings_DBName;

  /*! Settings Name to access the path to the database. */
  static QString Settings_DBPath;

  /*! Settings Name to access the directory last used to read a CSV file. */
  static QString Settings_LastCSVDirOpen;

  /*! Settings Name to access  */
  static QString Settings_CSVDialogGeometry;



};

#endif // CONSTANTS_H
