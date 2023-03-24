/*
# PostgreSQL Database Modeler (pgModeler)
#
# Copyright 2006-2023 - Raphael Araújo e Silva <raphael@pgmodeler.io>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# The complete text of GPLv3 is at LICENSE file on source code root directory.
# Also, you can get the complete GNU General Public License at <http://www.gnu.org/licenses/>
*/

/**
\ingroup libutils
\namespace UtilsNs
\brief This namespace is used to reunite all functions or constants that can be used globally by all subprojects.
*/

#ifndef UTILS_NS_H
#define UTILS_NS_H

#include "utilsglobal.h"
#include <QString>

namespace UtilsNs {
	//! \brief Default char for data/value separator for special usage
	static const QString DataSeparator("•");

	//! \brief Default char used as unescaped value start delimiter
	static const QChar UnescValueStart='/';

	//! \brief Default char used as unescaped value end delimiter
	static const QChar	UnescValueEnd='/';

	//! \brief Indicates the default wildcard character expected to be found in wildcard patterns
	static const QChar WildcardChar('*');

	//! \brief Indicates the character used to separate filter fields in the filtering string
	static const QChar FilterSeparator = ':';

	//! \brief Indicates the wildcard filtering mode in the object listing
	static const QString FilterWildcard("wildcard"),

	//! \brief Indicates the regexp (POSIX) filtering mode in the object listing
	FilterRegExp("regexp");

	/*! \brief Writes the provided buffer to the file specified by its filename
	 * Raises an exception in case of the file couldn,t be open */
	extern __libutils void saveFile(const QString &filename, const QByteArray &buffer);

	/*! \brief Read the contents of the file specified by its filename returning its contents.
	 * Raises an exception in case of the file couldn,t be open */
	extern __libutils QByteArray loadFile(const QString &filename);
}

#endif
