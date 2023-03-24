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

#include "baseform.h"

BaseForm::BaseForm(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);
	this->setWindowFlags((this->windowFlags() | Qt::WindowMinMaxButtonsHint) /* ^ Qt::WindowContextHelpButtonHint */);
}

void BaseForm::setButtonConfiguration(Messagebox::ButtonsId button_conf)
{
	if(button_conf==Messagebox::OkCancelButtons)
	{
		apply_ok_btn->setText(tr("&Apply"));
		cancel_btn->setVisible(true);
	}
	else
	{
		apply_ok_btn->setText(tr("&Ok"));
		cancel_btn->setVisible(false);
	}
}

void BaseForm::resizeForm(QWidget *widget)
{
	if(!widget)
		return;

	QVBoxLayout *vbox=new QVBoxLayout;
	QSize min_size=widget->minimumSize();
	int max_h = 0, max_w = 0, curr_w =0, curr_h = 0;
	QScreen *screen = qApp->primaryScreen();

	max_w = screen->size().width() * 0.70;
	max_h = screen->size().height() * 0.70;
	vbox->setContentsMargins(0, 0, 0, 0);

	/* If the widget's minimum size is zero then we need to do
			a size adjustment on the widget prior to insert it into the dialog */
	if(min_size.height() <= 0 || min_size.width() <= 0)
	{
		widget->adjustSize();
		min_size=widget->size();
	}

	//Insert the widget into a scroll area if it's minimum size exceeds the 70% of screen's dimensions
	if(max_w < min_size.width() || max_h < min_size.height())
	{
		QScrollArea *scrollarea=nullptr;
		scrollarea=new QScrollArea(main_frm);
		scrollarea->setFrameShape(QFrame::NoFrame);
		scrollarea->setFrameShadow(QFrame::Plain);
		scrollarea->setWidget(widget);
		scrollarea->setWidgetResizable(true);
		widget->setParent(scrollarea);
		vbox->addWidget(scrollarea);
	}
	else
	{
		vbox->addWidget(widget);
		widget->setParent(main_frm);
	}

	main_frm->setLayout(vbox);
	this->adjustSize();

	curr_h=this->height();
	curr_w=min_size.width();

	// If the current height is greater than the widget's minimum height we will use a medium value
	if(curr_h > min_size.height() && min_size.height() < max_h)
		curr_h = (curr_h + min_size.height())/2.5;
	//Using the maximum height if the widget's minimum height exceeds the maximum allowed
	else if(min_size.height() >= max_h)
		curr_h = max_h;

	curr_w += (vbox->contentsMargins().left() +
						 vbox->contentsMargins().right()) * 6;

	curr_h += pgmodeler_name_lbl->minimumHeight() +
							((buttons_lt->contentsMargins().top() +
								buttons_lt->contentsMargins().bottom()) * 6);

	if(curr_w > screen->size().width())
		curr_w = screen->size().width() * 0.80;

	if(curr_h > screen->size().height())
		curr_h = screen->size().height() * 0.80;

	this->setMinimumSize(min_size);
	this->resize(curr_w, curr_h);
	this->adjustSize();
}

void BaseForm::closeEvent(QCloseEvent *)
{
	this->reject();
}

void BaseForm::setMainWidget(BaseObjectWidget *widget)
{
	if(!widget)
		return;

	if(widget->getHandledObjectType()!=ObjectType::BaseObject && widget->windowTitle().isEmpty())
		setWindowTitle(tr("%1 properties").arg(BaseObject::getTypeName(widget->getHandledObjectType())));
	else
		setWindowTitle(widget->windowTitle());

	apply_ok_btn->setDisabled(widget->isHandledObjectProtected());
	resizeForm(widget);
	setButtonConfiguration(Messagebox::OkCancelButtons);

	connect(cancel_btn, &QPushButton::clicked, widget, &BaseObjectWidget::cancelConfiguration);
	connect(cancel_btn, &QPushButton::clicked, this, &BaseForm::reject);
	connect(apply_ok_btn, &QPushButton::clicked, widget, &BaseObjectWidget::applyConfiguration);
	connect(widget, &BaseObjectWidget::s_closeRequested, this, &BaseForm::accept);
}

void BaseForm::setMainWidget(QWidget *widget)
{
	if(!widget)	return;

	setWindowTitle(widget->windowTitle());
	setWindowIcon(widget->windowIcon());
	resizeForm(widget);
	setButtonConfiguration(Messagebox::OkButton);
	connect(cancel_btn, &QPushButton::clicked, this, &BaseForm::reject);
	connect(apply_ok_btn, &QPushButton::clicked, this, &BaseForm::accept);
}
