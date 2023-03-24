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
\ingroup libgui
\class ObjectTypesListWidget
\brief Implements a list widget specialized in show object types where the user can change check state of the items.
*/

#ifndef OBJECT_TYPES_LIST_WIDGET_H
#define OBJECT_TYPES_LIST_WIDGET_H

#include <QWidget>
#include "ui_objecttypeslistwidget.h"
#include "baseobject.h"
#include "guiglobal.h"

class __libgui ObjectTypesListWidget : public QWidget, public Ui::ObjectTypesListWidget {
	private:
		Q_OBJECT

		void setItemsCheckState(Qt::CheckState state);

	public:
		explicit ObjectTypesListWidget(QWidget *parent = nullptr);

		void setTypeNamesCheckState(const QStringList &obj_types, Qt::CheckState state);
		void setTypesCheckState(const std::vector<ObjectType> &obj_types, Qt::CheckState state);

		std::vector<ObjectType> getTypesPerCheckState(Qt::CheckState state);
		QStringList getTypeNamesPerCheckState(Qt::CheckState state);

	signals:
		void s_typeCheckStateChanged(ObjectType, Qt::CheckState);
		void s_typesCheckStateChanged(Qt::CheckState);
};

#endif
