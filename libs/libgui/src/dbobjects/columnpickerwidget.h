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
\class ColumnPickerWidget
\brief This widget allows picking up columns of a table from a combobox and placing the selected ones in a grid.
*/

#ifndef COLUMN_PICKER_WIDGET_H
#define COLUMN_PICKER_WIDGET_H

#include <QWidget>
#include "ui_columnpickerwidget.h"
#include "physicaltable.h"
#include "objectstablewidget.h"

class __libgui ColumnPickerWidget: public QWidget, public Ui::ColumnPickerWidget {
	private:
		Q_OBJECT

		//! \brief Stores the object in which the columns to be listed will be retrieved (must be a table or relationship)
		BaseObject *parent_obj;

		ObjectsTableWidget *columns_tab;

		//! \brief Adds the column to the specified row in the columns grid
		void addColumn(Column *column, int row);

		//! \brief Adds the view column to the specified row in the columns grid
		void addColumn(const SimpleColumn &column, int row);

	public:
		explicit ColumnPickerWidget(QWidget *parent = nullptr);

		/*! \brief Specifies the parent table or relationship in which the columns are retrived
		 * If a null object is specified the widget will be disabled, if an non-null object is assigned
		 * but is not a table or relationship the form will also be disabled. */
		void setParentObject(BaseObject *p_obj);

		/*! \brief Fills the columns grid with the elements in the vector.
		 * Columns that aren't child of the parent object specified by setParentObject()
		 * will be ignored */
		void setColumns(const std::vector<Column *> &cols);

		//! \brief Fills the columns grid with the elements view's columns
		void setColumns(const std::vector<SimpleColumn> &cols);

		//! \brief Returns a list of the columns that are in the grid.
		std::vector<Column *> getColumns();

		//! \brief Returns a list of the view columns that are in the grid.
		std::vector<SimpleColumn> getSimpleColumns();

	private slots:
		void updateColumnsCombo();
		void addColumn(int row);
};

#endif
