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

#include "objectdepsrefswidget.h"
#include "guiutilsns.h"
#include "objectfinderwidget.h"

ObjectDepsRefsWidget::ObjectDepsRefsWidget(QWidget *parent): BaseObjectWidget(parent)
{
	Ui_ObjectDepsRefsWidget::setupUi(this);
	configureFormLayout(objectdepsrefs_grid, ObjectType::BaseObject);

	//GuiUtilsNs::configureWidgetFont(message_lbl, GuiUtilsNs::MediumFontFactor);

	model_wgt=nullptr;
	alert_frm->setVisible(false);

	connect(exc_ind_deps_chk,	&QCheckBox::toggled, this, &ObjectDepsRefsWidget::updateObjectTables);
	connect(inc_ind_refs_chk,	&QCheckBox::toggled, this, &ObjectDepsRefsWidget::updateObjectTables);
	connect(dependences_tbw, &QTableWidget::itemDoubleClicked, this, &ObjectDepsRefsWidget::handleItemSelection);
	connect(references_tbw, &QTableWidget::itemDoubleClicked, this, &ObjectDepsRefsWidget::handleItemSelection);

	setMinimumSize(580, 350);
}

void ObjectDepsRefsWidget::setAttributes(DatabaseModel *model, BaseObject *object, BaseObject *parent_obj)
{
	BaseObjectWidget::setAttributes(model, object, parent_obj);

	if(object->getObjectType() == ObjectType::Constraint ||
		 object->getObjectType() == ObjectType::UserMapping)
		name_edt->setText(object->getName());

	this->name_edt->setReadOnly(true);
	this->protected_obj_frm->setVisible(false);
	this->comment_edt->setVisible(false);
	this->comment_lbl->setVisible(false);
	obj_icon_lbl->setPixmap(QPixmap(GuiUtilsNs::getIconPath(object->getObjectType())));
	updateObjectTables();
}

void ObjectDepsRefsWidget::setAttributes(ModelWidget *model_wgt, BaseObject *object, BaseObject *parent_obj)
{
	if(!model_wgt)
		throw Exception(ErrorCode::OprNotAllocatedObject ,__PRETTY_FUNCTION__,__FILE__,__LINE__);

	this->model_wgt=model_wgt;
	setAttributes(model_wgt->getDatabaseModel(), object, parent_obj);
}

void ObjectDepsRefsWidget::applyConfiguration()
{
	emit s_closeRequested();
}

void ObjectDepsRefsWidget::clearTables()
{
	dependences_tbw->clearContents();
	dependences_tbw->setRowCount(0);

	references_tbw->clearContents();
	references_tbw->setRowCount(0);
}

void ObjectDepsRefsWidget::updateObjectTables()
{
	std::vector<BaseObject *> objs;
	model->getObjectDependecies(object, objs, !exc_ind_deps_chk->isChecked());

	/* As the list of dependencies include the this->object itself is necessary
	to remove only for semantics reasons */
	objs.erase(std::find(objs.begin(), objs.end(), this->object));
	GuiUtilsNs::updateObjectTable(dependences_tbw, objs);

	objs.clear();
	if(!inc_ind_refs_chk->isChecked())
		model->getObjectReferences(object, objs);
	else
		model->__getObjectReferences(object, objs);

	GuiUtilsNs::updateObjectTable(references_tbw, objs);

	references_tbw->resizeColumnsToContents();
	dependences_tbw->resizeColumnsToContents();
}

void ObjectDepsRefsWidget::handleItemSelection(QTableWidgetItem *item)
{
	BaseObject *sel_obj=nullptr, *parent=nullptr;
	Table *parent_tab=nullptr;
	View *parent_view=nullptr;

	sel_obj=reinterpret_cast<BaseObject*>(item->data(Qt::UserRole).value<void *>());

	if(sel_obj)
	{
		if(TableObject::isTableObject(sel_obj->getObjectType()))
			parent=dynamic_cast<TableObject *>(sel_obj)->getParentTable();

		model_wgt->showObjectForm(sel_obj->getObjectType(), sel_obj, parent);
		clearTables();

		if(TableObject::isTableObject(this->object->getObjectType()))
		{
			parent=dynamic_cast<TableObject *>(this->object)->getParentTable();

			if(parent->getObjectType()==ObjectType::Table)
				parent_tab=dynamic_cast<Table *>(parent);
			else
				parent_view=dynamic_cast<View *>(parent);
		}

		if((parent_tab && parent_tab->getObjectIndex(this->object) >= 0) ||
				(parent_view && parent_view->getObjectIndex(this->object) >= 0) ||
				(model->getObjectIndex(this->object) >= 0))
		{
			updateObjectTables();
		}
		else
		{
			references_tbw->setEnabled(false);
			dependences_tbw->setEnabled(false);
			exc_ind_deps_chk->setEnabled(false);
			alert_frm->setVisible(true);
		}
	}
}
