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
\class MessageBox
\brief Implements the message box to show errors / alerts / infos to the user.
*/

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "guiglobal.h"
#include "ui_messagebox.h"
#include "exception.h"

class __libgui Messagebox: public QDialog, public Ui::Messagebox {
	private:
		Q_OBJECT

		//! \brief Indicates wether the message box was cancelled
		bool cancelled,

		/*! \brief Indicates if the message box was manually resized by the user when visible
		 * This flag is use know if the message box will have its height doubled when
		 * the user clicks the stack trace button */
		has_custom_size;

	protected:
		void resizeEvent(QResizeEvent *);

	public:
		//! \brief Constants used to define the message icon
		enum IconType: unsigned {
			NoIcon,
			ErrorIcon,
			InfoIcon,
			AlertIcon,
			ConfirmIcon
		};

		//! \brief Constants used to configure the visible buttons
		enum ButtonsId {
			YesNoButtons,
			OkCancelButtons,
			OkButton,
			AllButtons
		};

		Messagebox(QWidget * parent = nullptr, Qt::WindowFlags f = Qt::Widget);

		/*! \brief Shows the message box defining the icons and available buttons.
	User can specify custom button labels as well custom icons. The icons can be a path to a local file
	or a Qt  resource icon ':/path/icon' and will be enabled only specifing custom labels for the respective button. */
		void show(const QString &title, const QString &msg, IconType icon_type=NoIcon, ButtonsId buttons=OkButton,
				  const QString &yes_lbl="", const QString &no_lbl="", const QString &cancel_lbl="",
				  const QString &yes_ico="", const QString &no_ico="", const QString &cancel_ico="");

		//! \brief Shows the message box using an excpetion as message
		void show(Exception e, const QString &msg="", IconType icon_type=ErrorIcon, ButtonsId buttons=OkButton,
				  const QString &yes_lbl="", const QString &no_lbl="", const QString &cancel_lbl="",
				  const QString &yes_ico="", const QString &no_ico="", const QString &cancel_ico="");

		//! \brief Shows a simple message box with the title automatically defined by the icon type
		void show(const QString &msg, IconType icon_type=NoIcon, ButtonsId buttons=OkButton);

		bool isCancelled();

		void setCustomOptionText(const QString &text);
		bool isCustomOptionChecked();

	private slots:
		void handleYesOkClick();
		void handleNoCancelClick();
};

#endif
