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
\class LayersWidget
\brief Implements the widget that allows the user to quickly setup object's layers
*/

#ifndef LAYERS_WIDGET_H
#define LAYERS_WIDGET_H

#include "guiglobal.h"
#include <QWidget>
#include "ui_layerswidget.h"
#include "basegraphicobject.h"

class __libgui LayersWidget : public QWidget, Ui::LayersWidget {
	private:
		Q_OBJECT

		//! \brief The current object selection in a model widget that the layer widget will operate on.
		std::vector<BaseGraphicObject *> selected_objs;

		//! \brief Indicates if the user have interacted with the layers checkboxes changing objects layers.
		bool layers_changed;

	public:
		explicit LayersWidget(QWidget *parent = nullptr);

		//! \brief Configures the widget with layer names and a set of object selected in a model widget
		void setAttributes(const QStringList &layers, const std::vector<BaseObject *> &selected_objs);

		//! \brief Returns true if the layers of the selected object have been changed by the user.
		bool isLayersChanged();

	private slots:
		void updateObjectsLayers();
};

#endif
