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

#include "policycmdtype.h"

QStringList PolicyCmdType::type_names =
{
	"", // Reserved for Class::Null

	"ALL",
	"SELECT",
	"INSERT",
	"DELETE",
	"UPDATE"
};

PolicyCmdType::PolicyCmdType()
{
	type_idx = All;
}

QStringList PolicyCmdType::getTypes()
{
	return TemplateType<PolicyCmdType>::getTypes(type_names);
}

PolicyCmdType::PolicyCmdType(unsigned type_id)
{
	setType(type_id);
}

PolicyCmdType::PolicyCmdType(const QString &type_name)
{
	setType(type_name);
}

unsigned PolicyCmdType::setType(unsigned type_id)
{
	return TemplateType<PolicyCmdType>::setType(type_id, type_names);
}

unsigned PolicyCmdType::setType(const QString &type_name)
{
	return TemplateType<PolicyCmdType>::setType(type_name, type_names);
}

QString PolicyCmdType::getTypeName(unsigned type_id)
{
	return TemplateType<PolicyCmdType>::getTypeName(type_id, type_names);
}
