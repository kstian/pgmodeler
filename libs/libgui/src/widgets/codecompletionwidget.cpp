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

#include "codecompletionwidget.h"
#include "settings/generalconfigwidget.h"
#include "guiutilsns.h"
#include "settings/snippetsconfigwidget.h"

CodeCompletionWidget::CodeCompletionWidget(QPlainTextEdit *code_field_txt, bool enable_snippets) :	QWidget(dynamic_cast<QWidget *>(code_field_txt))
{
	if(!code_field_txt)
		throw Exception(ErrorCode::AsgNotAllocattedObject,__PRETTY_FUNCTION__,__FILE__,__LINE__);

	this->enable_snippets = enable_snippets;
	popup_timer.setInterval(300);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	completion_wgt=new QWidget(this);
	completion_wgt->setWindowFlags(Qt::Popup);
	completion_wgt->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	completion_wgt->setMinimumSize(200, 150);
	completion_wgt->setMaximumHeight(300);

	always_on_top_chk=new QCheckBox(completion_wgt);
	always_on_top_chk->setText(tr("&Always on top"));
	always_on_top_chk->setToolTip(tr("The widget will be always displayed while typing. It can be closable only by ESC key or when focus changes to another widget."));
	always_on_top_chk->setFocusPolicy(Qt::NoFocus);

	name_list=new QListWidget(completion_wgt);
	name_list->setSpacing(2);
	name_list->setIconSize(QSize(22, 22));
	name_list->setSortingEnabled(false);
	name_list->setSizeAdjustPolicy(QListWidget::AdjustToContents);
	name_list->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	name_list->setMaximumHeight(completion_wgt->maximumHeight() - always_on_top_chk->height() - GuiUtilsNs::LtSpacing);

	QVBoxLayout *vbox=new QVBoxLayout(completion_wgt);
	vbox->addWidget(name_list);
	vbox->addWidget(always_on_top_chk);
	vbox->setContentsMargins(GuiUtilsNs::LtMargin,GuiUtilsNs::LtMargin,GuiUtilsNs::LtMargin,GuiUtilsNs::LtMargin);
	vbox->setSpacing(GuiUtilsNs::LtSpacing);
	completion_wgt->setLayout(vbox);

	GuiUtilsNs::configureWidgetFont(name_list, GuiUtilsNs::MediumFontFactor);

	this->code_field_txt=code_field_txt;
	auto_triggered=false;

	db_model=nullptr;
	setQualifyingLevel(nullptr);

	connect(name_list, &QListWidget::itemDoubleClicked, this, &CodeCompletionWidget::selectItem);
	connect(name_list, &QListWidget::currentRowChanged, this, &CodeCompletionWidget::showItemTooltip);

	connect(&popup_timer, &QTimer::timeout, this, [this](){
		if(qualifying_level < 2)
		{
			auto_triggered=true;
			this->show();
		}
	});

	this->setVisible(false);

	if(enable_snippets)
		connect(this, &CodeCompletionWidget::s_wordSelected, this, &CodeCompletionWidget::handleSelectedWord);
}

void CodeCompletionWidget::handleSelectedWord(QString word)
{
	if(SnippetsConfigWidget::isSnippetExists(word))
	{
		QTextCursor tc=code_field_txt->textCursor();
		tc.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
		tc.removeSelectedText();
		tc.insertText(SnippetsConfigWidget::getParsedSnippet(word));
	}
}

bool CodeCompletionWidget::eventFilter(QObject *object, QEvent *event)
{
	QKeyEvent *k_event=dynamic_cast<QKeyEvent *>(event);

	if(k_event && k_event->type()==QEvent::KeyPress)
	{
		if(object==code_field_txt)
		{
			//Filters the trigger char and shows up the code completion only if there is a valid database model in use
			if(k_event->key() == completion_trigger.unicode() && db_model)
			{
				/* If the completion widget is not visible start the timer to give the user
				a small delay in order to type another character. If no char is typed the completion is triggered */
				if(!completion_wgt->isVisible() && !popup_timer.isActive())
					popup_timer.start();

				if(name_list->isVisible())
				{
					this->selectItem();
					this->show();
				}
			}
			else
			{
				popup_timer.stop();

				//Filters the Crtl+Space to trigger the code completion
				if(k_event->key()==Qt::Key_Space && (k_event->modifiers()==Qt::ControlModifier || k_event->modifiers()==Qt::MetaModifier))
				{
					setQualifyingLevel(nullptr);
					this->show();
					return true;
				}
				else if(k_event->key()==Qt::Key_Space || k_event->key()==Qt::Key_Backspace || k_event->key()==Qt::Key_Delete)
				{
					QTextCursor tc=code_field_txt->textCursor();
					tc.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

					/* Avoiding deleting text using backspace or delete if the current char is the completion trigger (.).
						 This will block the cursor and cause the list to stay in the current qualifying level */
					if(completion_wgt->isVisible() &&
						 (k_event->key()==Qt::Key_Backspace || k_event->key()==Qt::Key_Delete) &&
						 tc.selectedText().contains(completion_trigger))
					{
						event->ignore();
						return true;
					}
					else if(k_event->key()==Qt::Key_Space)
					{
						setQualifyingLevel(nullptr);

						if(!always_on_top_chk->isChecked())
							this->close();
					}

					if(always_on_top_chk->isChecked())
						this->show();
				}
			}
		}
		else if(object==name_list)
		{
			if(k_event->key()==Qt::Key_Escape)
			{
				this->close();
				return true;
			}
			//Filters the ENTER/RETURN press to close the code completion widget select the name
			else if(k_event->key()==Qt::Key_Enter || k_event->key()==Qt::Key_Return)
			{
				if(!always_on_top_chk->isChecked())
					this->selectItem();
				else
				{
					//Forcing the line break on the code field when holding Control key and hit return/enter
					if(k_event->modifiers()==Qt::ControlModifier)
					{
						QTextCursor cursor=code_field_txt->textCursor();
						code_field_txt->insertPlainText(QChar(QChar::LineFeed));
						cursor.movePosition(QTextCursor::Down);
						code_field_txt->setTextCursor(cursor);
					}
					else
						this->selectItem();

					this->show();
				}

				return true;
			}
			//Filters other key press and redirects to the code input field
			else if(k_event->key()!=Qt::Key_Up && k_event->key()!=Qt::Key_Down &&
					k_event->key()!=Qt::Key_PageUp && k_event->key()!=Qt::Key_PageDown &&
					k_event->key()!=Qt::Key_Home && k_event->key()!=Qt::Key_End &&
					k_event->modifiers()!=Qt::AltModifier)
			{

				QCoreApplication::sendEvent(code_field_txt, k_event);
				this->updateList();
				return true;
			}
		}

		name_list->adjustSize();
		adjustSize();
	}

	return QWidget::eventFilter(object, event);
}

void CodeCompletionWidget::configureCompletion(DatabaseModel *db_model, SyntaxHighlighter *syntax_hl, const QString &keywords_grp)
{
	std::map<QString, attribs_map> confs=GeneralConfigWidget::getConfigurationParams();

	name_list->clear();
	word.clear();
	setQualifyingLevel(nullptr);
	auto_triggered=false;
	this->db_model=db_model;

	if(confs[Attributes::Configuration][Attributes::CodeCompletion]==Attributes::True)
	{
		code_field_txt->installEventFilter(this);
		name_list->installEventFilter(this);

		if(syntax_hl && keywords.isEmpty())
		{
			//Get the keywords from the highlighter
			std::vector<QRegularExpression> exprs=syntax_hl->getExpressions(keywords_grp);

			while(!exprs.empty())
			{
				/* Since keywords are exact match patterns in the form \A(?:keyword)\z"
				 * we need to remove from the pattern the initial and final regexp operators in
				 * order to use only the word itself */
				keywords.push_front(exprs.back().pattern().remove("\\A(?:").remove(")\\z"));
				exprs.pop_back();
			}

			completion_trigger=syntax_hl->getCompletionTrigger();
		}
		else
			completion_trigger=QChar('.');

		if(enable_snippets)
		{
			clearCustomItems();
			insertCustomItems(SnippetsConfigWidget::getAllSnippetsAttribute(Attributes::Id),
												SnippetsConfigWidget::getAllSnippetsAttribute(Attributes::Label),
												QPixmap(GuiUtilsNs::getIconPath("codesnippet")));
		}
	}
	else
	{
		code_field_txt->removeEventFilter(this);
		name_list->removeEventFilter(this);
	}
}

void CodeCompletionWidget::insertCustomItem(const QString &name, const QString &tooltip, const QPixmap &icon)
{
	if(!name.isEmpty())
	{
		QString item_name=name.simplified();
		custom_items[item_name]=icon;
		custom_items_tips[item_name]=tooltip;
	}
}

void CodeCompletionWidget::insertCustomItems(const QStringList &names, const QStringList &tooltips, const QPixmap &icon)
{
	for(int i=0; i < names.size(); i++)
	{
		insertCustomItem(names[i], (i < tooltips.size() ? tooltips[i] : ""), icon);
	}
}

void CodeCompletionWidget::insertCustomItems(const QStringList &names, const QString &tooltip, ObjectType obj_type)
{
	for(auto &name : names)
		insertCustomItem(name, tooltip, QPixmap(GuiUtilsNs::getIconPath(obj_type)));
}

void CodeCompletionWidget::clearCustomItems()
{
	custom_items.clear();
}

void CodeCompletionWidget::populateNameList(std::vector<BaseObject *> &objects, QString filter)
{
	QListWidgetItem *item=nullptr;
	QString obj_name;
	ObjectType obj_type;
	QRegularExpression regexp(QRegularExpression::wildcardToRegularExpression(filter.remove('"') + QString("*")),
														QRegularExpression::CaseInsensitiveOption);

	name_list->clear();

	//for(unsigned i=0; i < objects.size(); i++)
	for(auto &obj : objects)
	{
		obj_type = obj->getObjectType();
		obj_name.clear();

		//Formatting the object name according to the object type
		if(obj_type == ObjectType::Function)
		{
			dynamic_cast<Function *>(obj)->createSignature(false);
			obj_name = dynamic_cast<Function *>(obj)->getSignature();
		}
		else if(obj_type == ObjectType::Operator)
			obj_name = dynamic_cast<Operator *>(obj)->getSignature(false);
		else
			obj_name += obj->getName(false, false);

		//The object will be inserted if its name matches the filter or there is no filter set
		if(filter.isEmpty() || regexp.match(obj_name).hasMatch())
		{
			item= new QListWidgetItem(QPixmap(GuiUtilsNs::getIconPath(obj->getSchemaName())), obj_name);
			item->setToolTip(QString("%1 (%2)").arg(obj->getName(true)).arg(obj->getTypeName()));
			item->setData(Qt::UserRole, QVariant::fromValue<void *>(obj));
			item->setToolTip(BaseObject::getTypeName(obj_type));
			name_list->addItem(item);
		}
	}

	name_list->sortItems();
}

void CodeCompletionWidget::show()
{
	prev_txt_cur = code_field_txt->textCursor();
	updateList();
	completion_wgt->show();	
	showItemTooltip();
	popup_timer.stop();
	completion_wgt->adjustSize();
	adjustSize();
}

void CodeCompletionWidget::setQualifyingLevel(BaseObject *obj)
{
	if(!obj)
		qualifying_level=-1;
	else if(obj->getObjectType()==ObjectType::Schema)
		qualifying_level=0;
	else if(BaseTable::isBaseTable(obj->getObjectType()))
		qualifying_level=1;
	else
		qualifying_level=2;

	if(qualifying_level < 0)
	{
		sel_objects={ nullptr, nullptr, nullptr };
	}
	else
	{
		sel_objects[qualifying_level]=obj;
		lvl_cur=code_field_txt->textCursor();
	}
}

void CodeCompletionWidget::updateList()
{
	QListWidgetItem *item=nullptr;
	QString pattern;
	std::vector<BaseObject *> objects;
	std::vector<ObjectType> types=BaseObject::getObjectTypes(false, 	{ ObjectType::Textbox, ObjectType::Relationship, ObjectType::BaseRelationship });
	QTextCursor tc;

	name_list->clear();
	word.clear();
	new_txt_cur=tc=code_field_txt->textCursor();

	/* Try to move the cursor to the previous char in order to check if the user is
	calling the completion without an attached word */
	tc.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

	if(!tc.selectedText().trimmed().isEmpty() && new_txt_cur.movePosition(QTextCursor::WordLeft, QTextCursor::KeepAnchor))
	{
		//Move the cursor right before the trigger char in order to get the complete word
		code_field_txt->setTextCursor(new_txt_cur);
		word=code_field_txt->textCursor().selectedText();
		word.remove('"');

		//Case the completion was triggered using the trigger char
		if(db_model && (auto_triggered || completion_trigger==word))
		{
			/* The completion will try to find a schema, table or view that matches the word,
			if the serach returns one item the completion will start/continue an qualifying level */
			new_txt_cur.movePosition(QTextCursor::WordLeft, QTextCursor::KeepAnchor);
			code_field_txt->setTextCursor(new_txt_cur);
			word=code_field_txt->textCursor().selectedText();
			word.remove(completion_trigger);
			word.remove('"');

			objects=db_model->findObjects(word, { ObjectType::Schema, ObjectType::Table, ObjectType::ForeignTable, ObjectType::View }, false, false, true);

			if(objects.size()==1)
				setQualifyingLevel(objects[0]);
		}

		code_field_txt->setTextCursor(prev_txt_cur);
	}

	if(!word.isEmpty() && !auto_triggered)
		pattern=QString("(^") + word.simplified() + QString(")");
	else if(auto_triggered)
		pattern=word;

	if(db_model)
	{
		//Negative qualifying level means that user called the completion before a space (empty word)
		if(qualifying_level < 0)
			//The default behavior for this is to search all the objects on the model
			objects=db_model->findObjects(pattern, types, false, !auto_triggered, auto_triggered);
		else
		{
			QString left_word;

			//Searching objects according to qualifying level.
			tc=code_field_txt->textCursor();
			tc.movePosition(QTextCursor::WordLeft, QTextCursor::KeepAnchor);

			/* Retrieving the word at the left in order to compare it to the object's name at the current qualifying level,
		 if the word does not matches the object then children objects will not be retrieved */
			if(tc.selectedText().contains('\"'))
			{
				tc.movePosition(QTextCursor::WordLeft, QTextCursor::KeepAnchor);
				left_word=tc.selectedText();
				left_word.remove('"');
			}
			else
				left_word=tc.selectedText();

			//Level 0 indicates that user selected a schema, so all objects of the schema are retrieved
			if(qualifying_level==0 /*&& left_word==sel_objects[qualifying_level]->getName()*/)
				objects=db_model->getObjects(sel_objects[qualifying_level]);

			/* Level 1 indicates that user selected a table or view, so all child objects are retrieved.
		 If the current level is 1 and the table/view name isn't present then the children will not be listed */
			else if(qualifying_level==1 /*&& left_word==sel_objects[qualifying_level]->getName()*/)
				objects=dynamic_cast<BaseTable *>(sel_objects[qualifying_level])->getObjects();

			/* If the current qualifying level and current word does retrieve any object as a fallback
		 we try to find any object in the model and reset the qualifying level */
			else
			{
				objects=db_model->findObjects(pattern, types, false, !auto_triggered, auto_triggered);
				setQualifyingLevel(nullptr);
			}

			/* If the typed word is equal to the current level object's name clear the order in order
			to avoid listing the same object */
			if(qualifying_level >=0 && word==sel_objects[qualifying_level]->getName())
				word.clear();
		}

		populateNameList(objects, word);
	}

	/* List the keywords if the qualifying level is negative or the
	completion wasn't triggered using the special char */
	if(qualifying_level < 0 && !auto_triggered)
	{
		QRegularExpression regexp(pattern, QRegularExpression::CaseInsensitiveOption);

		for(auto &kw : keywords.filter(regexp))
		{
			item=new QListWidgetItem(QPixmap(GuiUtilsNs::getIconPath("keyword")), kw);
			item->setToolTip(tr("SQL Keyword"));
			name_list->addItem(item);
		}

		name_list->sortItems();

		//If there are custom items, they wiill be placed at the very beggining of the list
		if(!custom_items.empty())
		{
			QStringList list;
			int row=0;
			QListWidgetItem *item=nullptr;

			for(auto &itr : custom_items)
			{
				if(itr.first.contains(regexp))
					list.push_back(itr.first);
			}

			list.sort();
			for(auto &item_name : list)
			{
				item=new QListWidgetItem(custom_items[item_name], item_name);
				item->setToolTip(custom_items_tips[item_name]);
				name_list->insertItem(row++, item);
			}
		}
	}

	if(name_list->count()==0)
	{
		name_list->addItem(tr("(no items found)"));
		name_list->item(0)->setFlags(Qt::NoItemFlags);
		QToolTip::hideText();
	}
	else
		name_list->item(0)->setSelected(true);

	//Sets the list position right below of text cursor
	completion_wgt->move(code_field_txt->viewport()->mapToGlobal(code_field_txt->cursorRect().bottomLeft()));
	name_list->setFocus();
}

void CodeCompletionWidget::selectItem()
{
	if(!name_list->selectedItems().isEmpty())
	{
		QListWidgetItem *item=name_list->selectedItems().at(0);
		BaseObject *object=nullptr;
		QTextCursor tc;

		if(qualifying_level < 0)
			code_field_txt->setTextCursor(new_txt_cur);

		//If the selected item is a object (data not null)
		if(!item->data(Qt::UserRole).isNull())
		{
			//Retrieve the object
			object=reinterpret_cast<BaseObject *>(item->data(Qt::UserRole).value<void *>());

			/* Move the cursor to the start of the word because all the chars will be replaced
			with the object name */
			prev_txt_cur.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);

			tc=prev_txt_cur;
			tc.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

			/* An small workaround to correctly write the object name in the current
			qualifying level without remove the parent's name. This happens only when
			the completion is marked as persistent */
			if(always_on_top_chk->isChecked())
			{
				if(tc.selectedText().startsWith('.'))
				{
					prev_txt_cur.movePosition(QTextCursor::EndOfWord, QTextCursor::MoveAnchor);

					if(!tc.selectedText().endsWith('.'))
						prev_txt_cur.insertText(completion_trigger);
				}
				else if(qualifying_level >= 0 && !tc.selectedText().endsWith('.'))
				{
					prev_txt_cur.movePosition(QTextCursor::EndOfWord, QTextCursor::MoveAnchor);
					prev_txt_cur.insertText(completion_trigger);
				}
			}
			else if(tc.selectedText().contains('"'))
				prev_txt_cur=tc;

			code_field_txt->setTextCursor(prev_txt_cur);

			insertObjectName(object);
			setQualifyingLevel(object);
		}
		else
		{
			code_field_txt->insertPlainText(item->text() + QString(" "));
			setQualifyingLevel(nullptr);
		}

		emit s_wordSelected(item->text());
	}
	else
		setQualifyingLevel(nullptr);

	name_list->clearSelection();
	auto_triggered=false;

	if(!always_on_top_chk->isChecked())
		this->close();
}

void CodeCompletionWidget::showItemTooltip()
{
	QListWidgetItem *item=name_list->currentItem();

	if(item)
	{
		QPoint pos=name_list->mapToGlobal(QPoint(name_list->width(), name_list->geometry().top()));
		QToolTip::showText(pos, item->toolTip());
	}
}

void CodeCompletionWidget::close()
{
	name_list->clearSelection();
	completion_wgt->close();
	auto_triggered=false;
}

void CodeCompletionWidget::insertObjectName(BaseObject *obj)
{
	bool sch_qualified=!sel_objects[0],
			modify_name=QApplication::keyboardModifiers()==Qt::AltModifier;
	QString name=obj->getName(true, sch_qualified);
	ObjectType obj_type=obj->getObjectType();
	int move_cnt=0;


	if(modify_name &&
			(PhysicalTable::isPhysicalTable(obj_type) || TableObject::isTableObject(obj_type)))
	{
		if(PhysicalTable::isPhysicalTable(obj_type))
		{
			PhysicalTable *tab=dynamic_cast<PhysicalTable *>(obj);

			name+=QString("(");
			for(unsigned i=0; i < tab->getColumnCount(); i++)
				name+=tab->getColumn(i)->getName(true) + QString(",");

			name.remove(name.size()-1, 1);
			name+=QString(")");
		}
		else
		{
			if(sel_objects[0])
				move_cnt=2;
			else
				move_cnt=3;

			lvl_cur.movePosition(QTextCursor::WordLeft, QTextCursor::KeepAnchor, move_cnt);
			code_field_txt->setTextCursor(lvl_cur);
		}
	}
	else if(obj_type==ObjectType::Function)
	{
		Function *func=dynamic_cast<Function *>(obj);
		func->createSignature(true, sch_qualified);
		name=func->getSignature();
	}
	else if(obj_type==ObjectType::Cast)
	{
		name.replace(',', QLatin1String(" AS "));
	}
	else if(obj_type==ObjectType::Aggregate)
	{
		Aggregate *agg;
		agg=dynamic_cast<Aggregate *>(obj);
		name+=QString("(");

		if(agg->getDataTypeCount()==0)
			name+='*';
		else
		{
			for(unsigned i=0; i < agg->getDataTypeCount(); i++)
				name+=~agg->getDataType(i) + ',';
			name.remove(name.size()-1, 1);
		}

		name+=')';
	}

	code_field_txt->insertPlainText(name);
}
