#include "sourceeditorwidget.h"
#include "messagebox.h"
#include "guiutilsns.h"
#include "utilsns.h"

QPalette SourceEditorWidget::def_editor_pal;

attribs_map SourceEditorWidget::snippets = {
	{"ifend", "%if {} %then\n\n%end\n"},
	{"ifelseend", "%if {} %then\n\n%else\n\n%end\n"},
	{"ifexpr", "%if ({}) %then\n\n%end\n"},
	{"ifexprelse", "%if ({}) %then\n\n%else\n\n%end\n"},
	{"setattrstr", "%set {} \"\"\n"},
	{"setattrtxt", "%set {} [ ]\n"},
	{"unsetattr", "%unset {}\n"},
	{"unsetattr", "%unset {}\n"},
};

SourceEditorWidget::SourceEditorWidget(QWidget *parent) : QWidget(parent)
{
	setupUi(this);

	is_modified = false;
	curr_sytax_cfg = GlobalAttributes::SchHighlightConf;

	editor_txt = GuiUtilsNs::createNumberedTextEditor(editor_parent);
	def_editor_pal = editor_txt->palette();

	editor_hl = new SyntaxHighlighter(editor_txt);
	find_wgt = new FindReplaceWidget(editor_txt, find_parent);
	find_parent->setVisible(false);

	code_compl_wgt = new CodeCompletionWidget(editor_txt);
	code_compl_wgt->configureCompletion(nullptr, editor_hl);

	source_file_sel = new FileSelectorWidget(source_file_parent);
	source_file_sel->setReadOnly(true);
	source_file_parent->setVisible(false);

	QVBoxLayout *vbox = new QVBoxLayout(source_file_parent);
	vbox->setContentsMargins(0, 0, 0, 0);
	vbox->addWidget(source_file_sel);

	for(auto &itr : snippets)
		code_compl_wgt->insertCustomItem(itr.first, itr.second, QPixmap(GuiUtilsNs::getIconPath("codesnippet")));

	vbox = new QVBoxLayout(find_parent);
	vbox->setContentsMargins(0, 0, 0, GuiUtilsNs::LtMargin);
	vbox->addWidget(find_wgt);

	indent_tb->setMenu(&indent_opts_menu);
	act_break_inline_ifs = indent_opts_menu.addAction(tr("Break inline ifs"));
	act_break_inline_ifs->setCheckable(true);
	act_break_inline_ifs->setChecked(false);

	connect(code_compl_wgt, &CodeCompletionWidget::s_wordSelected, this, &SourceEditorWidget::handleSelectedSnippet);
	connect(find_wgt, &FindReplaceWidget::s_hideRequested, find_tb, &QToolButton::toggle);
	connect(validate_tb, &QToolButton::clicked, this, &SourceEditorWidget::validateSyntax);
	connect(indent_tb, &QToolButton::clicked, this, &SourceEditorWidget::applyIndentation);
	connect(editor_txt, &NumberedTextEditor::modificationChanged, this, &SourceEditorWidget::restoreEditorPalette);
	connect(editor_txt, &NumberedTextEditor::undoAvailable, this, &SourceEditorWidget::setModified);
	connect(editor_txt, &NumberedTextEditor::cursorPositionChanged, this, &SourceEditorWidget::restoreEditorPalette);
	connect(find_tb, &QToolButton::toggled, find_parent, &QWidget::setVisible);
}

void SourceEditorWidget::saveFile(const QString &filename)
{
	UtilsNs::saveFile(filename, editor_txt->toPlainText().toUtf8());

	validate_tb->setEnabled(filename.endsWith(GlobalAttributes::SchemaExt));
	indent_tb->setEnabled(filename.endsWith(GlobalAttributes::SchemaExt));
	this->filename = filename;
	source_file_sel->setSelectedFile(filename);
	source_file_parent->setVisible(true);
}

void SourceEditorWidget::loadSyntaxConfig(const QString &filename)
{
	try
	{
		editor_hl->loadConfiguration(filename);
		editor_hl->rehighlight();		
		curr_sytax_cfg = QFileInfo(filename).baseName();
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(), e.getErrorCode(), __PRETTY_FUNCTION__, __FILE__, __LINE__, &e);
	}
}

void SourceEditorWidget::handleSelectedSnippet(const QString &snippet)
{
	QTextCursor tc = editor_txt->textCursor();
	tc.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
	tc.removeSelectedText();
	tc.insertText(snippets[snippet]);
}

QString SourceEditorWidget::getCurrentSyntaxConfig()
{
	return curr_sytax_cfg;
}

void SourceEditorWidget::loadFile(const QString &filename)
{
	if(filename.isEmpty())
		return;

	bool enable = filename.endsWith(GlobalAttributes::SchemaExt);

	editor_txt->setPlainText(UtilsNs::loadFile(filename));
	validate_tb->setEnabled(enable);
	indent_tb->setEnabled(enable);
	code_compl_wgt->setEnabled(enable);
	this->filename = filename;
	source_file_sel->setSelectedFile(filename);
	source_file_parent->setVisible(true);

	QString ext = "." + QFileInfo(filename).suffix();

	if(ext == GlobalAttributes::DbModelExt ||
		 ext == ".xml" ||
		 ext == GlobalAttributes::ConfigurationExt ||
		 ext == GlobalAttributes::ObjMetadataExt)
		curr_sytax_cfg = GlobalAttributes::XMLHighlightConf;
	else if(ext == ".sql")
		curr_sytax_cfg = GlobalAttributes::SQLHighlightConf;
	else
		curr_sytax_cfg = GlobalAttributes::SchHighlightConf;
}

void SourceEditorWidget::validateSyntax()
{
	SchemaParser schparser;
	Messagebox msgbox;

	try
	{
		editor_txt->setPalette(def_editor_pal);
		schparser.ignoreEmptyAttributes(true);
		schparser.ignoreUnkownAttributes(true);
		schparser.loadBuffer(editor_txt->toPlainText());
		schparser.getSourceCode({});

		msgbox.show(tr("No lexical or sytactical errors found."), Messagebox::InfoIcon);
	}
	catch(Exception &e)
	{
		// When founding an error we try to highlight the exact portion of the document where to problem is
		QTextCursor cursor(editor_txt->document()->findBlockByLineNumber(schparser.getCurrentLine() - 1));
		cursor.movePosition(QTextCursor::StartOfLine);
		cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, schparser.getCurrentColumn());

		editor_txt->blockSignals(true);
		editor_txt->setTextCursor(cursor);
		editor_txt->blockSignals(false);

		// Changing the text selection color so the user can see where the problem is
		QPalette pal = editor_txt->palette();
		pal.setColor(QPalette::Highlight, QColor("#f00000"));
		pal.setColor(QPalette::HighlightedText, QColor("#ffffff"));
		editor_txt->setPalette(pal);

		msgbox.show(e);
	}
}

void SourceEditorWidget::restoreEditorPalette()
{
	editor_txt->setPalette(def_editor_pal);
}

void SourceEditorWidget::applyIndentation()
{
	QStringList buffer = editor_txt->toPlainText().split(QChar::LineFeed);
	int if_level = 0, comment_pos = -1, line_count = buffer.size();
	bool found_cond = false, found_if = false, inline_ifend = false;
	QString cond_pattern = QString("^(( )|(\\t))*(%1)"), line,
			tk_if = SchemaParser::CharStartConditional + SchemaParser::TokenIf,
			tk_then = SchemaParser::CharStartConditional + SchemaParser::TokenThen,
			tk_else = SchemaParser::CharStartConditional + SchemaParser::TokenElse,
			tk_end = SchemaParser::CharStartConditional + SchemaParser::TokenEnd;
	QRegularExpression inline_if_regexp(QString("(%1)(.)+(%2)").arg(tk_if).arg(tk_end));

	for(int ln_idx = 0; ln_idx < line_count; ln_idx++)
	{
		line = buffer[ln_idx];
		comment_pos = line.indexOf(SchemaParser::CharComment);
		inline_ifend = line.contains(inline_if_regexp);

		if(line.contains(QRegularExpression(cond_pattern.arg(tk_if))) && !inline_ifend)
		{
			if_level++;
			found_if = found_cond = true;
		}
		else if(line.contains(QRegularExpression(cond_pattern.arg(tk_else))) ||
						line.contains(QRegularExpression(cond_pattern.arg(tk_end))))
			found_cond = true;

		// If the current line is an inline if: %if ... %then ... %end, we break it
		if(act_break_inline_ifs->isChecked() && inline_ifend)
		{
			line.replace(tk_if, QChar::LineFeed + tk_if);
			line.replace(tk_then, tk_then + QChar::LineFeed);
			line.replace(tk_else, QChar::LineFeed + tk_else + QChar::LineFeed);
			line.replace(tk_end, QChar::LineFeed + tk_end + QChar::LineFeed );

			/* If the line was broke ( presence of \n) we split the resulting string
			 * and insert the new lines in the buffer and restart the indentation process */
			if(line.contains(QChar::LineFeed))
			{
				QStringList buf_aux = line.split(QChar::LineFeed, Qt::SkipEmptyParts);
				buffer.removeAt(ln_idx);

				for(auto itr = buf_aux.rbegin(); itr != buf_aux.rend(); itr++)
					buffer.insert(ln_idx, *itr);

				ln_idx = 0;
				found_cond = found_if = false;
				if_level = 0;
				line_count = buffer.size();
				continue;
			}
		}

		line = line.simplified();

		if(!line.isEmpty())
		{
			line = line.rightJustified(line.size() + if_level + (found_cond ? -1 : 0), QChar::Tabulation);

			if(!inline_ifend)
				if_level -= line.mid(0, comment_pos).count(QString("%1%2").arg(SchemaParser::CharStartConditional).arg(SchemaParser::TokenEnd));
		}

		buffer[ln_idx] = line;
		found_cond = found_if = false;
	}

	QRegularExpression cond_tk_regexp(QString("^(( )|(\\t))*(%1)[a-z]+").arg(SchemaParser::CharStartConditional));
	QString prev_line, next_line, next_next_line,
			tk_set = SchemaParser::CharStartConditional + SchemaParser::TokenSet,
			tk_unset = SchemaParser::CharStartConditional + SchemaParser::TokenUnset;

	for(int ln_idx = 0; ln_idx < buffer.count() - 1; ln_idx++)
	{
		line = buffer[ln_idx].mid(0, buffer[ln_idx].indexOf(SchemaParser::CharComment));

		// Ignoring the line if it contains a inline if
		if(line.contains(inline_if_regexp))
			continue;

		// Capturing the previous, next lines without comment portion
		prev_line = ln_idx > 0 ? buffer[ln_idx - 1].mid(0, buffer[ln_idx - 1].indexOf(SchemaParser::CharComment)) : "";
		next_line = ln_idx < buffer.count() - 1 ? buffer[ln_idx + 1].mid(0, buffer[ln_idx + 1].indexOf(SchemaParser::CharComment)) : "";
		next_next_line = ln_idx < buffer.count() - 2 ? buffer[ln_idx + 2].mid(0, buffer[ln_idx + 2].indexOf(SchemaParser::CharComment)) : "";

		/* Removing the empty line in the following cases:
		 * 1) Between a two %end tokens
		 * 2) Between an %end and %else
		 * 3) Between an two %if tokens
		 * 4) Between an %else and %if | %set | %unset */
		if(next_line.isEmpty() && !next_next_line.isEmpty() &&
			 ((line.contains(QRegularExpression(cond_pattern.arg(tk_end))) &&
					(next_next_line.contains(QRegularExpression(cond_pattern.arg(tk_else))) ||
					 next_next_line.contains(QRegularExpression(cond_pattern.arg(tk_end))))) ||

				 ((line.contains(QRegularExpression(cond_pattern.arg(tk_if))) ||
					 line.contains(QRegularExpression(cond_pattern.arg(tk_else)))) &&
					(next_next_line.contains(QRegularExpression(cond_pattern.arg(tk_if))) ||
					 next_next_line.contains(QRegularExpression(cond_pattern.arg(tk_set))) ||
					 next_next_line.contains(QRegularExpression(cond_pattern.arg(tk_unset)))))))
		{
			buffer.removeAt(ln_idx + 1);
			ln_idx--;
			continue;
		}

		// Separating an end token from any conditional token in the next line
		if(line.contains(QRegularExpression(cond_pattern.arg(tk_end))) &&
			 !next_line.isEmpty() &&
			 !next_line.contains(cond_tk_regexp))
			buffer[ln_idx].append(QChar::LineFeed);

		// Separating an closed plain text from the next line
		else if(line.endsWith(SchemaParser::CharEndPlainText) &&
						!next_line.isEmpty() &&
						!next_line.contains(cond_tk_regexp))
			buffer[ln_idx].append(QChar::LineFeed);

		// If the current line has an %if and the previous is not a conditional instruction %
		else if(line.contains(QRegularExpression(cond_pattern.arg(tk_if))) &&
						!prev_line.isEmpty() &&
						!prev_line.contains(cond_tk_regexp))
			buffer[ln_idx].prepend(QChar::LineFeed);

		// Separating an if token from previous end, set and unset
		else if(line.contains(QRegularExpression(cond_pattern.arg(tk_if))) &&
						(prev_line.contains(tk_end) ||
						 prev_line.contains(tk_set) ||
						 prev_line.contains(tk_unset)))
			buffer[ln_idx].prepend(QChar::LineFeed);
	}

	/* Replacing the current code with the formatted one in such a way to preserve
	 * the undo/redo stack. This is achieved by selecting the whole text document
	 * and inserting the new text using the cursor. Using QPlainTextEdit::setPlainText
	 * will clear the stack. */
	editor_txt->blockSignals(true);
	QTextCursor cursor = editor_txt->textCursor();
	cursor.movePosition(QTextCursor::Start);
	cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
	cursor.insertText(buffer.join(QChar::LineFeed));
	cursor = editor_txt->textCursor();
	editor_txt->moveCursor(QTextCursor::Start);
	editor_txt->blockSignals(false);
	setModified(true);
}

void SourceEditorWidget::setModified(bool value)
{
	is_modified = value;
	emit s_editorModified(value);
}

void SourceEditorWidget::setDefaultEditorPalette(const QPalette &pal)
{
	def_editor_pal = pal;
}

QString SourceEditorWidget::getFilename()
{
	return filename;
}

bool SourceEditorWidget::isModified()
{
	return is_modified;
}
