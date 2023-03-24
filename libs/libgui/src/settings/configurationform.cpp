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

#include "configurationform.h"

ConfigurationForm::ConfigurationForm(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);

	general_conf=new GeneralConfigWidget(this);
	appearance_conf=new AppearanceConfigWidget(this);
	connections_conf=new ConnectionsConfigWidget(this);
	relationships_conf=new RelationshipConfigWidget(this);
	snippets_conf=new SnippetsConfigWidget(this);
	plugins_conf=new PluginsConfigWidget(this);

	QWidgetList wgt_list={ general_conf, appearance_conf, relationships_conf,
												 connections_conf, snippets_conf, plugins_conf };

	for(auto &wgt : wgt_list)
		confs_stw->addWidget(wgt);

	connect(cancel_btn, &QPushButton::clicked, this, &ConfigurationForm::reject);
	connect(apply_btn,  &QPushButton::clicked, this, &ConfigurationForm::applyConfiguration);
	connect(defaults_btn,  &QPushButton::clicked, this, &ConfigurationForm::restoreDefaults);

	setMinimumSize(890, 740);

	QFont fnt;
	int view_idx = GeneralConfWgt;
	QList<QToolButton *> btns = { general_tb, appearance_tb, relationships_tb,
																connections_tb, snippets_tb, plugins_tb };

	for(auto &btn : btns)
	{
		fnt = btn->font();
		fnt.setWeight(QFont::Normal);
		btn->setFont(fnt);
		GuiUtilsNs::createDropShadow(btn, 1, 1, 5);
		btn->setProperty(Attributes::ObjectId.toStdString().c_str(), view_idx++);
		connect(btn, &QToolButton::toggled, this, &ConfigurationForm::changeCurrentView);
	}
}

ConfigurationForm::~ConfigurationForm()
{
	connections_conf->destroyConnections();
}

void ConfigurationForm::changeCurrentView()
{
	QToolButton *btn = nullptr,
			*btn_sender = qobject_cast<QToolButton *>(sender());

	for(auto &obj : bnts_parent_wgt->children())
	{
		btn = dynamic_cast<QToolButton *>(obj);
		if(!btn || btn == btn_sender) continue;
		btn->blockSignals(true);
		btn->setChecked(false);
		btn->blockSignals(false);
	}

	confs_stw->setCurrentIndex(btn_sender->property(Attributes::ObjectId.toStdString().c_str()).toInt());
}

void ConfigurationForm::hideEvent(QHideEvent *)
{
	general_tb->setChecked(true);
}

void ConfigurationForm::showEvent(QShowEvent *)
{
	snippets_conf->snippet_txt->updateLineNumbers();
}

void ConfigurationForm::reject()
{
	try
	{
		if(sender() == cancel_btn)
		{
			QWidgetList wgt_list={ appearance_conf, connections_conf, snippets_conf };
			BaseConfigWidget *conf_wgt=nullptr;

			QApplication::setOverrideCursor(Qt::WaitCursor);

			for(QWidget *wgt : wgt_list)
			{
				conf_wgt = qobject_cast<BaseConfigWidget *>(wgt);

				if(conf_wgt->isConfigurationChanged())
					conf_wgt->loadConfiguration();
			}

			QApplication::restoreOverrideCursor();
		}
	}
	catch(Exception &)
	{}

	QDialog::reject();
}

void ConfigurationForm::applyConfiguration()
{
	BaseConfigWidget *conf_wgt=nullptr;
	bool curr_escape_comments = BaseObject::isEscapeComments();

	QApplication::setOverrideCursor(Qt::WaitCursor);

	for(int i=GeneralConfWgt; i <= SnippetsConfWgt; i++)
	{
		conf_wgt=qobject_cast<BaseConfigWidget *>(confs_stw->widget(i));

		if(conf_wgt->isConfigurationChanged())
			conf_wgt->saveConfiguration();
	}

	general_conf->applyConfiguration();
	relationships_conf->applyConfiguration();

	if(curr_escape_comments != BaseObject::isEscapeComments())
		emit s_invalidateModelsRequested();

	QApplication::restoreOverrideCursor();
	QDialog::accept();
}

void ConfigurationForm::loadConfiguration()
{
	BaseConfigWidget *config_wgt = nullptr;

	for(int i=GeneralConfWgt; i <= PluginsConfWgt; i++)
	{
		try
		{
			config_wgt = qobject_cast<BaseConfigWidget *>(confs_stw->widget(i));
			config_wgt->loadConfiguration();
		}
		catch(Exception &e)
		{
			Messagebox msg_box;

			if(e.getErrorCode()==ErrorCode::PluginsNotLoaded)
			{
				msg_box.show(e);
			}
			else
			{
				Exception ex = Exception(Exception::getErrorMessage(ErrorCode::ConfigurationNotLoaded).arg(e.getExtraInfo()),__PRETTY_FUNCTION__,__FILE__,__LINE__, &e);
				msg_box.show(ex, QString("%1 %2").arg(ex.getErrorMessage()).arg(tr("In some cases restore the default settings related to it may solve the problem. Would like to do that?")),
										 Messagebox::AlertIcon, Messagebox::YesNoButtons, tr("Restore"), "", "", GuiUtilsNs::getIconPath("refresh"));

				if(msg_box.result() == QDialog::Accepted)
					config_wgt->restoreDefaults();
			}
		}
	}
}

void ConfigurationForm::restoreDefaults()
{
	Messagebox msg_box;
	msg_box.show(tr("Any modification made until now in the current section will be lost! Do you really want to restore default settings?"),
				 Messagebox::ConfirmIcon,
				 Messagebox::YesNoButtons);

	if(msg_box.result()==QDialog::Accepted)
		qobject_cast<BaseConfigWidget *>(confs_stw->currentWidget())->restoreDefaults();
}

BaseConfigWidget *ConfigurationForm::getConfigurationWidget(unsigned idx)
{
	if(idx >= static_cast<unsigned>(confs_stw->count()))
		return nullptr;

	return qobject_cast<BaseConfigWidget *>(confs_stw->widget(static_cast<unsigned>(idx)));
}

