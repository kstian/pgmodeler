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

#include "procedure.h"

Procedure::Procedure() : BaseFunction()
{
	obj_type = ObjectType::Procedure;
}

void Procedure::addParameter(Parameter param)
{
	if(param.isOut() && !param.isIn())
		throw Exception(ErrorCode::InvProcedureParamOutMode, __PRETTY_FUNCTION__, __FILE__, __LINE__);

	BaseFunction::addParameter(param);
}

QString Procedure::getSourceCode(SchemaParser::CodeType def_type, bool)
{
	QString code_def = getCachedCode(def_type, false);
	if(!code_def.isEmpty()) return code_def;

	setBasicFunctionAttributes(def_type);

	return BaseObject::getSourceCode(def_type, false);
}

QString Procedure::getSourceCode(SchemaParser::CodeType def_type)
{
	return getSourceCode(def_type, false);
}

QString Procedure::getAlterCode(BaseObject *object)
{
	Procedure *proc = dynamic_cast<Procedure *>(object);

	if(!proc)
		throw Exception(ErrorCode::OprNotAllocatedObject,__PRETTY_FUNCTION__,__FILE__,__LINE__);

	try
	{
		attribs_map attribs;
		attribs = BaseFunction::getAlterCodeAttributes(proc);

		if(this->func_source.simplified() != proc->func_source.simplified() ||
			 this->library != proc->library || this->symbol != proc->symbol)
		{
			attribs[Attributes::Definition] = proc->getSourceCode(SchemaParser::SqlCode);
			attribs[Attributes::Definition].replace(QString("CREATE PROCEDURE").arg(this->getSQLName()), QString("CREATE OR REPLACE PROCEDURE"));
		}

		copyAttributes(attribs);
		return BaseObject::getAlterCode(this->getSchemaName(), attributes, false, true);
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(),e.getErrorCode(),__PRETTY_FUNCTION__,__FILE__,__LINE__,&e);
	}
}
