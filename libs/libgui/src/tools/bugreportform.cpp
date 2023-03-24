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

#include "bugreportform.h"
#include "exception.h"
#include "globalattributes.h"
#include "messagebox.h"
#include "guiutilsns.h"
#include "utilsns.h"

BugReportForm::BugReportForm(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);
	setWindowFlags(Qt::Dialog |
								 Qt::WindowTitleHint |
								 Qt::WindowMinMaxButtonsHint |
								 Qt::WindowCloseButtonHint);

	output_sel = new FileSelectorWidget(this);
	output_sel->setWindowTitle(tr("Select report output folder"));
	output_sel->setFileMode(QFileDialog::Directory);
	output_sel->setAllowFilenameInput(true);
	output_sel->setSelectedFile(GlobalAttributes::getTemporaryPath());

	output_lt->addWidget(output_sel);
	connect(close_btn, &QPushButton::clicked, this, &BugReportForm::close);
	connect(create_btn, &QPushButton::clicked, this, qOverload<>(&BugReportForm::generateReport));
	connect(attach_mod_chk, &QCheckBox::toggled, attach_tb, &QToolButton::setEnabled);
	connect(attach_tb, &QToolButton::clicked, this, qOverload<>(&BugReportForm::attachModel));
	connect(details_txt, &QPlainTextEdit::textChanged, this,  &BugReportForm::enableGeneration);
	connect(output_sel, &FileSelectorWidget::s_selectorChanged, this, &BugReportForm::enableGeneration);

	//Installs a syntax highlighter on model_txt widget
	hl_model_txt=new SyntaxHighlighter(model_txt);
	hl_model_txt->loadConfiguration(GlobalAttributes::getXMLHighlightConfPath());

	try
	{
		attachModel(GlobalAttributes::getTemporaryFilePath(GlobalAttributes::LastModelFile));
	}
	catch(Exception &){}
}

void BugReportForm::attachModel(const QString &filename)
{
	model_txt->setPlainText(UtilsNs::loadFile(filename));
}

QByteArray BugReportForm::generateReportBuffer()
{
	QByteArray buf;

	buf.append(details_txt->toPlainText().toUtf8());
	buf.append(CharDelimiter);

	if(attach_mod_chk->isChecked())
		buf.append(model_txt->toPlainText().toUtf8());

	buf.append(CharDelimiter);
	return buf;
}

void BugReportForm::generateReport()
{ 
	generateReport(generateReportBuffer());
	this->accept();
}

void BugReportForm::enableGeneration()
{
	create_btn->setEnabled(!output_sel->getSelectedFile().isEmpty() &&
												 !output_sel->hasWarning() &&
												 !details_txt->toPlainText().isEmpty());
}

void BugReportForm::generateReport(const QByteArray &buf)
{
	Messagebox msgbox;
	QFile output;
	QFileInfo fi(QString(output_sel->getSelectedFile() +
											 GlobalAttributes::DirSeparator +
											 GlobalAttributes::BugReportFile)
											.arg(QDateTime::currentDateTime().toString(QString("_yyyyMMdd_hhmm"))));
	QString filename=fi.absoluteFilePath();

	//Opens the file for writting
	output.setFileName(filename);
	output.open(QFile::WriteOnly);

	if(!output.isOpen())
		msgbox.show(Exception::getErrorMessage(ErrorCode::FileDirectoryNotWritten).arg(filename), Messagebox::ErrorIcon);
	else
	{
		QByteArray comp_buf;

		//Compress the buffer (using zlib) in a compression rate at 8
		comp_buf=qCompress(buf, 8);

		//Saves the buffer
		output.write(comp_buf.data(), comp_buf.size());
		output.close();

		msgbox.show(tr("Bug report successfuly generated! Please, send the file <strong><a href='file://%1'>%2<a/></strong> to <em>%3</em> in order be analyzed. Thank you for the collaboration!")
								.arg(fi.absolutePath(), QDir::toNativeSeparators(filename), GlobalAttributes::BugReportEmail),
					Messagebox::InfoIcon);
	}
}

void BugReportForm::attachModel()
{
	QFileDialog file_dlg;

	try
	{
		file_dlg.setDefaultSuffix(GlobalAttributes::DbModelExt);
		file_dlg.setWindowTitle(tr("Load model"));
		file_dlg.setNameFilter(tr("Database model (*%1);;All files (*.*)").arg(GlobalAttributes::DbModelExt));
		file_dlg.setFileMode(QFileDialog::ExistingFile);
		file_dlg.setModal(true);

		GuiUtilsNs::restoreFileDialogState(&file_dlg);

		if(file_dlg.exec()==QFileDialog::Accepted)
			attachModel(file_dlg.selectedFiles().at(0));

		GuiUtilsNs::saveFileDialogState(&file_dlg);
	}
	catch(Exception &e)
	{
		Messagebox msgbox;
		msgbox.show(e);
	}
}
