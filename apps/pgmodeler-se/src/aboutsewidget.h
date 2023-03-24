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
\ingroup pgmodeler-se
\class AboutSEWidget
\brief Form that contains information about software authoring and licensing.
*/

#ifndef ABOUT_SE_WIDGET_H
#define ABOUT_SE_WIDGET_H

#include <QWidget>
#include "ui_aboutsewidget.h"
#include "globalattributes.h"

class AboutSEWidget: public QWidget, public Ui::AboutSEWidget {
	private:
		Q_OBJECT
		
	public:
		AboutSEWidget(QWidget *parent = nullptr);
};

#endif
