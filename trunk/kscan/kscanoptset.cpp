/* This file is part of the KDE Project
   Copyright (C) 2000 Klaas Freitag <freitag@suse.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <qstring.h>
#include <q3asciidict.h>
#include <qmap.h>
#include <q3dict.h>
//Added by qt3to4:
#include <QByteArray>
#include <QDebug>
#include "kconfig.h"

#include "kscandevice.h"
#include "kscanoption.h"
#include "kscanoptset.h"

KScanOptSet::KScanOptSet( const QByteArray& setName )
{
  name = setName;

  setAutoDelete( false );

  description = "";

  strayCatsList.setAutoDelete( true );
}



KScanOptSet::~KScanOptSet()
{
   /* removes all deep copies from backupOption */
   strayCatsList.clear();
}



KScanOption *KScanOptSet::get( const QByteArray name ) const
{
  KScanOption *ret = 0;

  ret = (*this) [name];

  return( ret );
}

QByteArray KScanOptSet::getValue( const QByteArray name ) const
{
   KScanOption *re = get( name );
   QByteArray retStr = "";

   if( re )
   {
      retStr = re->get();
   }
   else
   {
      qDebug() << "option " << name << " from OptionSet is not available";
   }
   return( retStr );
}


bool KScanOptSet::backupOption( const KScanOption& opt )
{
  bool retval = true;

  /** Allocate a new option and store it **/
  const QByteArray& optName = opt.getName();
  if( !optName.isEmpty() )
    retval = false;

  if( retval )
  {
     KScanOption *newopt = find( optName );

     if( newopt )
     {
	/** The option already exists **/
	/* Copy the new one into the old one. TODO: checken Zuweisungoperatoren OK ? */
	*newopt = opt;
     }
     else
     {
	const QByteArray& qq = opt.get();
	qDebug() << "Value is now: <" << qq << ">";
	const KScanOption *newopt = new KScanOption( opt );

	strayCatsList.append( newopt );

	if( newopt )
	{
	   insert( optName, newopt );
	} else {
	   retval = false;
	}
     }
  }

  return( retval );

}

QString KScanOptSet::getDescription() const
{
   return description;
}

void KScanOptSet::slSetDescription( const QString& str )
{
   description = str;
}

void KScanOptSet::backupOptionDict( const Q3AsciiDict<KScanOption>& optDict )
{
   Q3AsciiDictIterator<KScanOption> it( optDict );

   while ( it.current() )
   {
      qDebug() << "Dict-Backup of Option <" << it.currentKey() << ">";
      backupOption( *(it.current()));
      ++it;
   }


}

/* */
void KScanOptSet::saveConfig( const QString& scannerName, const QString& configName,
			      const QString& descr )
{
   QString confFile = SCANNER_DB_FILE;
   qDebug() << "Creating scan configuration file <" << confFile << ">";

   KConfig *scanConfig = new KConfig( confFile );
   QString cfgName = configName;

   if( configName.isNull() || configName.isEmpty() )
      cfgName = "default";

   scanConfig->setGroup( cfgName );

   scanConfig->writeEntry( "description", descr );
   scanConfig->writeEntry( "scannerName", scannerName );
   Q3AsciiDictIterator<KScanOption> it( *this);

    while ( it.current() )
    {
       const QString line = it.current() -> configLine();
       const QString name = it.current()->getName();

       qDebug() << "writing " << name << " = <" << line << ">";

       scanConfig->writeEntry( name, line );

       ++it;
    }

    scanConfig->sync();
    delete( scanConfig );
}

bool KScanOptSet::load( const QString& /*scannerName*/ )
{
   QString confFile = SCANNER_DB_FILE;
   qDebug() << "** Reading from scan configuration file <" << confFile << ">";
   bool ret = true;

   KConfig *scanConfig = new KConfig( confFile, true );
   QString cfgName = name; /* of the KScanOptSet, given in constructor */

   if( cfgName.isNull() || cfgName.isEmpty() )
      cfgName = "default";

   //XXX WJC - scanner settings loading temporarily disabled
   //if( ! scanConfig->hasGroup( name ) )
   //{
      qDebug() << "Group " << name << " does not exist in configuration !";
      ret = false;
   /*}
   else
   {
      scanConfig->setGroup( name );

      typedef QMap<QString, QString> StringMap;

      StringMap strMap = scanConfig->entryMap( name );

      StringMap::Iterator it;
      for( it = strMap.begin(); it != strMap.end(); ++it )
      {
	 QByteArray optName = it.key().latin1();
	 KScanOption optset( optName );

	 QByteArray val = it.data().latin1();
	 qDebug() << "Reading for " << optName << " value " << val;

	 optset.set( val );

	 backupOption( optset );
      }
   }*/
   delete( scanConfig );

   return( ret );
}

/* END */
