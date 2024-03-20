#ifndef CONSTANTS_H
#define CONSTANTS_H

#include<QString>

//**************************************************************************
/*! \class Constants
 *  \brief Encapulate constants in a typesafe way. These values are used in the application configuration file, for example.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2011-2020
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

  /*! Last place a CSV Export was done */
  static QString Settings_LastCSVDirWrite;

  /*! Settings Name to access  */
  static QString Settings_CSVDialogGeometry;

  static QString Settings_SQLDialogGeometry;
  static QString Settings_LastSQLDialogString;
  static QString Settings_MainWindowGeometry;

  static QString Settings_ConfigureDlgGeometry;
  static QString Settings_SortFieldDlgGeometry;
  static QString Settings_FilterFieldDlgGeometry;
  static QString SortFieldConfigDialogLastConfigPath;
  static QString SortFieldConfigDialogRoutingColumnWidths;

  static QString Settings_GenericDataCollectionDlgGeometry;
  static QString Settings_GenericDataCollectionDlgColumnWidths;
  static QString Settings_GenericDataCollectionDlgSorting;

  static QString Settings_GenericDataCollectionLastEditedTable;

  static QString Settings_SearchFindValue;
  static QString Settings_SearchReplaceValue;
  static QString Settings_SearchOptions;

  /*! Settings Name to access the path to the base catalog images. */
  static QString Settings_CatalogImagePath;

  /*! Settings Name to access the path to the user images. */
  static QString Settings_UserImagePath;

};

#endif // CONSTANTS_H
