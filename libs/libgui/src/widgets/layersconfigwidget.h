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
\class LayersConfigWidget
\brief Implements the widget that allows the user to handle scene layers
*/

#ifndef LAYERS_CONFIG_WIDGET_H
#define LAYERS_CONFIG_WIDGET_H

#include <QWidget>
#include "ui_layersconfigwidget.h"
#include "modelwidget.h"
#include "colorpickerwidget.h"

class __libgui LayersConfigWidget : public QWidget, Ui::LayersConfigWidget {
	private:
		Q_OBJECT

		//! \brief Holds the last mouse position while moving the cursor over the widget (used during resize event filter)
		QPoint old_pos;

		//! \brief Holds the actions for the visibility toggler menu
		QMenu visibility_menu;

		//! \brief Model in which the layer widget will operate on
		ModelWidget *model;

		/*! \brief The current selected item in the layers grid. We need to store it in a separated attribute
		 * for renaming purposes */
		QTableWidgetItem *curr_item;

		//! \brief Stores the current's item text (layer name) to revert the renaming if the user aborts it
		QString curr_text;

		//! \brief Stores the current's item row
		int curr_row;

		QList<ColorPickerWidget *> rect_color_pickers,

		name_color_pickers;

		bool eventFilter(QObject *watched, QEvent *event) override;

		//! \brief Add a new item (layer) to the listing. If the provided name is empty a default name is assigned
		void __addLayer(const QString &name = "", Qt::CheckState chk_state = Qt::Checked);

	public:
		explicit LayersConfigWidget(QWidget *parent = nullptr);

		//! \brief Defines the model in which the widget will work
		void setModel(ModelWidget *model);

	private slots:
		/*! \brief This slot adds an item to the layers grid and performs the need operations
		 * to make the new layer available to the scene and model widget */
		void addLayer(const QString &name = "");

		//! \brief Triggers the renaming operation over a item
		void startLayerRenaming();

		//! \brief Finishes the renaming operation over a item
		void finishLayerRenaming();

		//! \brief Updates the active layeres on the scene causing a redraw of the items
		void updateActiveLayers();

		//! \brief Remove a layer from the listing. If 'clear' is true them all layers (except the default) are removed
		void removeLayer(bool clear = false);

		//! \brief Updates the colors of a layer that is associated with the calling color picker widget.
		void updateLayerColors(int layer_idx = -1);

		//! \brief Enables the control buttons according to the selection on the list
		void enableButtons();

		//! \brief Checks the items in the layers grid reflecting the active layers in the model performs a full update (slower) in the layers rects
		void setLayersActive();

	public slots:
		void setVisible(bool value) override;

		void toggleLayersRects();

		void updateLayersList();

		/*! \brief Performs a simple layers rect update by only recalculating the their size and position
		 * based on the current layers state. This method is useful when you only need to update/redraw the
		 * scene layers without force the update of other graphical objects. */
		void updateLayersRects();

	signals:
		//! \brief Signal emitted whenever the widget changes its visibility
		void s_visibilityChanged(bool);

		//! \brief Signal emitted whenever the current active layers change
		void s_activeLayersChanged();
};

#endif
