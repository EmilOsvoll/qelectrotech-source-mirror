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
#include "replaceelementdiaLog.h"

#include "../../qetapp.h"
#include "../../qetinformation.h"
#include "../../ui/elementinfopartwidget.h"
#include "../searchandreplaceworker.h"
#include "replaceelementdiaLog.h"
#include "ui_replaceelementdiaLog.h"

#include <QAbstractButton>

ReplaceElementDiaLog::ReplaceElementDiaLog(DiagramContext context, QWidget *parent) :
	QDiaLog(parent),
	ui(new Ui::ReplaceElementDiaLog)
{
	ui->setupUi(this);
	buildWidget();
	setContext(context);
}

ReplaceElementDiaLog::~ReplaceElementDiaLog()
{
	delete ui;
}

/**
	@brief ReplaceElementDiaLog::setContext
	Set the current diagram context to be edited
	@param context
*/
void ReplaceElementDiaLog::setContext(DiagramContext context)
{
	m_context = context;
	
	for (ElementInfoPartWidget *eipw : m_eipw_list) {
		eipw->setText(m_context[eipw->key()].toString());
	}
}

/**
	@brief ReplaceElementDiaLog::context
	@return The edited diagram context
*/
DiagramContext ReplaceElementDiaLog::context() const
{
	DiagramContext context;
	for (ElementInfoPartWidget *eipw : m_eipw_list) {
		context.addValue(eipw->key(), eipw->text());
	}
	
	return context;
}

void ReplaceElementDiaLog::buildWidget()
{
	connect(ui->m_button_box, &QDiaLogButtonBox::clicked, [this](QAbstractButton *button_) {
		this->done(ui->m_button_box->buttonRole(button_));
	});
	
	for (QString str : QETInformation::elementInfoKeys())
	{
		ElementInfoPartWidget *eipw = new ElementInfoPartWidget(str, QETInformation::translatedInfoKey(str), this);
		eipw->setEraseTextVisible(true);
		eipw->setPlaceHolderText(tr("Do not change"));
		ui->m_scroll_layout->addWidget(eipw);
		m_eipw_list << eipw;
	}
}
