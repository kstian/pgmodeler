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
\class PlainTextItemDelegate
\brief Implements a custom item delegate based upon a QPlainTextEdit to help edit long texts with line breaks
*/

#ifndef PLAIN_TEXT_ITEM_DELEGATE_H
#define PLAIN_TEXT_ITEM_DELEGATE_H

#include "guiglobal.h"
#include <QObject>
#include <QStyledItemDelegate>

class __libgui PlainTextItemDelegate : public QStyledItemDelegate {
	private:
		Q_OBJECT
		bool read_only;

	public:
		explicit PlainTextItemDelegate(QObject * parent, bool read_only);
		virtual ~PlainTextItemDelegate();

	protected:
		//! \brief Overrides the default implementation and set the editor as read only/disable to prevent change the item data
		virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;

		virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const;
};

#endif
