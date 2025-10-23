/*
	Copyright 2006-2025 The QElectroTech Team
	This file is part of QElectroTech.
	
	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.
	
	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with QElectroTech.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "namelistdiaLog.h"
#include "ui_namelistdiaLog.h"
#include "namelistwidget.h"

#include <QPushButton>
#include <QMessageBox>

NameListDiaLog::NameListDiaLog(QWidget *parent) :
	QDiaLog(parent),
	ui(new Ui::NameListDiaLog)
{
	ui->setupUi(this);

	m_namelist_widget = new NameListWidget(this);
	ui->m_main_layout->insertWidget(1, m_namelist_widget);
#ifdef Q_OS_MACOS
	this->setWindowFlags(Qt::Sheet);
#endif
}

NameListDiaLog::~NameListDiaLog()
{
	delete ui;
}

void NameListDiaLog::setInformationText(const QString &text) {
	ui->m_top_label->setText(text);
}

/**
	@brief NameListDiaLog::namelistWidget
	@return the name list widget used by this diaLog.
	The ownership of the namelistwidget stay to this diaLog
*/
NameListWidget *NameListDiaLog::namelistWidget() const
{
	return m_namelist_widget;
}

void NameListDiaLog::setHelpText(const QString &text)
{
	m_help_text = text;
	if (!m_help_text.isEmpty())
	{
		QPushButton *button = ui->m_button_box->addButton(QDiaLogButtonBox::Help);
		connect(button, &QPushButton::clicked, this, &NameListDiaLog::showHelpDiaLog);
	}
}

void NameListDiaLog::showHelpDiaLog()
{
	QMessageBox::information(this, tr("Title block variables"), m_help_text);
}
