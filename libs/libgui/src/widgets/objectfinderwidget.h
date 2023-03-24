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
\class ObjectFinderWidget
\brief Implements the operations to find objects over the model
*/

#ifndef OBJECT_FINDER_WIDGET_H
#define OBJECT_FINDER_WIDGET_H

#include "ui_objectfinderwidget.h"
#include "modelwidget.h"
#include "objecttypeslistwidget.h"
#include <QWidget>

class __libgui ObjectFinderWidget : public QWidget, public Ui::ObjectFinderWidget {
	private:
		Q_OBJECT

		/*! \brief Store in a unordered list the attributes in order to allow the user
		 * select which attribute will be used to search objects */
		static const QStringList search_attribs,

		/*! \brief Store in a unordered list the attributes in a translated way in order to
		 * populate the combo box used to select which attribute will be used to search objects */
		search_attribs_i18n;

		std::vector<BaseObject *> found_objs,

		selected_objs;

		QMenu select_menu, fade_menu, filter_menu;

		//! \brief Reference model widget
		ModelWidget *model_wgt;

		//! \brief Stores the selected object on the result list
		BaseObject *selected_obj;

		QWidget *filter_wgt;

		QCheckBox *regexp_chk, *exact_match_chk, *case_sensitive_chk;

		ObjectTypesListWidget *obj_types_lst;

		//! \brief Captures the ENTER press to execute search
		bool eventFilter(QObject *object, QEvent *event);

		void showEvent(QShowEvent *);

		void resizeEvent(QResizeEvent *event);

	public:
		ObjectFinderWidget(QWidget *parent = nullptr);

		/*! \brief Lists the objects of a vector in a QTableWidget. The first column on each row of table contains thereference to the object
		 * The parameter search_attr is used to display the attribute value in which the search was performed. */
		//static void updateObjectTable(QTableWidget *tab_wgt, std::vector<BaseObject *> &objects, const QString &search_attr = Attributes::Name, bool checkable_items = false);
		
		//! \brief Sets the database model to work on
		void setModel(ModelWidget *model_wgt);

	signals:
		void s_hideRequested();
		
	public slots:
		void hide();
		void clearResult();
		void findObjects();

	private slots:
		//! \brief Opens the editing form of the selected object
		void editObject();

		//! \brief Selects a object on result list
		void selectObject();

		void fadeObjects();
		void selectObjects();
		void showObjectMenu();

		friend class MainWindow;
};

#endif
