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

#include "securitytype.h"

QStringList SecurityType::type_names =
{
	"", // Reserved for Class::Null

	"SECURITY INVOKER",
	"SECURITY DEFINER"
};

SecurityType::SecurityType()
{
	type_idx = Invoker;
}

QStringList SecurityType::getTypes()
{
	return TemplateType<SecurityType>::getTypes(type_names);
}

SecurityType::SecurityType(const QString &type_name)
{
	setType(type_name);
}

SecurityType::SecurityType(unsigned type_id)
{
	setType(type_id);
}

unsigned SecurityType::setType(unsigned type_id)
{
	return TemplateType<SecurityType>::setType(type_id, type_names);
}

unsigned SecurityType::setType(const QString &type_name)
{
	return TemplateType<SecurityType>::setType(type_name, type_names);
}

QString SecurityType::getTypeName(unsigned type_id)
{
	return TemplateType<SecurityType>::getTypeName(type_id, type_names);
}
