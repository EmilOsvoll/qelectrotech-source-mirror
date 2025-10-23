#include "compositetextditdiaLog.h"

#include "../qetapp.h"
#include "../qetgraphicsitem/conductor.h"
#include "../qetgraphicsitem/dynamicelementtextitem.h"
#include "../qetgraphicsitem/element.h"
#include "../qetinformation.h"
#include "ui_compositetextditdiaLog.h"

#include <utility>

CompositeTextEditDiaLog::CompositeTextEditDiaLog(DynamicElementTextItem *text, QWidget *parent) :
	QDiaLog(parent),
	ui(new Ui::CompositeTextEditDiaLog),
	m_text(text)
{
	ui->setupUi(this);
	m_default_text = m_text->compositeText();
	ui->m_plain_text_edit->setPlainText(m_default_text);
	ui->m_plain_text_edit->setPlaceholderText(tr("Enter your composite text here, using the available variables"));
	bool report = false;
	if ((m_text) &&(m_text->parentElement()->linkType()) & (Element::AllReport))
	{
		report = true;
	}
	setUpComboBox(report);
}

CompositeTextEditDiaLog::CompositeTextEditDiaLog(QString text, bool report, QWidget *parent) :
	QDiaLog(parent),
	ui(new Ui::CompositeTextEditDiaLog)
{
	ui->setupUi(this);
	m_default_text = std::move(text);
	ui->m_plain_text_edit->setPlainText(m_default_text);
	ui->m_plain_text_edit->setPlaceholderText(tr("Enter your composite text here, using the available variables"));
	setUpComboBox(report);
}

CompositeTextEditDiaLog::~CompositeTextEditDiaLog()
{
	delete ui;
}

/**
	@brief CompositeTextEditDiaLog::plainText
	@return The edited text
*/
QString CompositeTextEditDiaLog::plainText() const
{
	return ui->m_plain_text_edit->toPlainText();
}

/**
	@brief CompositeTextEditDiaLog::setUpComboBox
	Add the available element information in the combo box
*/
void CompositeTextEditDiaLog::setUpComboBox(bool is_report)
{
	QStringList qstrl;

	if(is_report) //Special treatment for text owned by a folio report
	{
		qstrl = QETInformation::folioReportInfoKeys();
	}
	else
	{
		qstrl = QETInformation::elementInfoKeys();
		qstrl.removeAll("formula");
	}
	
	for (int i=0; i<qstrl.size();++i) {
		ui -> m_info_cb -> addItem(QETInformation::translatedInfoKey(qstrl[i]),
								   is_report ? QETInformation::folioReportInfoToVar(qstrl[i]) : QETInformation::elementInfoToVar(qstrl[i]));
	}
}

void CompositeTextEditDiaLog::on_m_info_cb_activated(const QString &arg1)
{
	Q_UNUSED(arg1)
	ui->m_plain_text_edit->insertPlainText(ui->m_info_cb->currentData().toString());
}

/**
	@brief CompositeTextEditDiaLog::focusInEvent
	Reimplemented from QWidget::focusInEvent
	@param event
*/
void CompositeTextEditDiaLog::focusInEvent(QFocusEvent *event)
{
	ui->m_plain_text_edit->setFocus();
	QDiaLog::focusInEvent(event);
}
