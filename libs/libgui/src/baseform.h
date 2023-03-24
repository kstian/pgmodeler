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
\class BaseForm
\brief A basic form implementation, contains only an apply and a cancel button.
*/

#ifndef BASE_FORM_H
#define BASE_FORM_H

#include <QtWidgets>
#include "ui_baseform.h"
#include "exception.h"
#include "messagebox.h"
#include "dbobjects/baseobjectwidget.h"

class __libgui BaseForm: public QDialog, public Ui::BaseForm {
	private:
		Q_OBJECT
		/*! \brief Resizes the dialog according to the minimum sizes of the provided widget.
				If the widget size exceed 70% of the screen size a scroll area will be created and
				the widget reparented to it. If the minimum size of the widget is 0 then the size
				is automatically calculated (no so precise). This method acts only in height and
				try to preserve the minimum width */
		void resizeForm(QWidget *widget);

		void closeEvent(QCloseEvent *);

	public:
		BaseForm(QWidget * parent = nullptr, Qt::WindowFlags f = Qt::Widget);

		void setButtonConfiguration(Messagebox::ButtonsId button_conf = Messagebox::OkCancelButtons);

		/*! \brief Injects the specified object into the form and turns it the main widget.
		 * The widget is reparented to the stack widget within the form. */
		void setMainWidget(QWidget *widget);

		/*! \brief Injects the specified object into the form and turns it the main widget.
		 * The widget is reparented to the stack widget within the form.
		 * The accept_slot is the fucntion pointer to the slot from the provided widget that can be
		 * optionally used to replace the default accept() of the form's footer aplly button. */
		template <class Class, typename Slot>
		void setMainWidget(Class *widget, Slot accept_slot);

		/*! \brief Injects the specified object into the form and turns it the main widget.
				The widget is reparented to the stack widget within the form. This version of method
				does additional configurations like signal connection, automatic sizing and
				custom title configuration based upont the object handled by the BaseObjectWidget instance */
		void setMainWidget(BaseObjectWidget *widget);
};

template <class Class, typename Slot>
void BaseForm::setMainWidget(Class *widget, Slot accept_slot)
{
	if(!widget)
		return;

	setMainWidget(widget);
	disconnect(apply_ok_btn, nullptr, widget, nullptr);
	connect(apply_ok_btn, &QPushButton::clicked, widget, accept_slot);
}

#endif
