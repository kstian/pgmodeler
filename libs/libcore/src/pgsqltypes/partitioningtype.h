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
\ingroup libcore/pgsqltypes
\class PartitioningType
\brief Implements the operations to manipulate table partitining types.
**/

#ifndef PARTITIONING_TYPE
#define PARTITIONING_TYPE

#include "templatetype.h"

class __libcore PartitioningType: public TemplateType<PartitioningType> {
	private:
		static QStringList type_names;

	public:
		static constexpr unsigned Range = 1,
		List = 2,
		Hash = 3;

		PartitioningType(const QString &type_name);
		PartitioningType(unsigned type_id);
		PartitioningType();

		static QStringList getTypes();

		unsigned setType(unsigned type_id) override;
		unsigned setType(const QString &type_name) override;
		QString getTypeName(unsigned type_id) override;
};

#endif 
