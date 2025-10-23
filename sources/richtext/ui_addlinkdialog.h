/********************************************************************************
** Form generated from reading UI file 'addlinkdiaLog.ui'
**
** Created: Thu 4. Apr 17:13:59 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDLINKDIALOG_H
#define UI_ADDLINKDIALOG_H

#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_AddLinkDiaLog
{
public:
	QVBoxLayout *verticalLayout;
	QFormLayout *formLayout;
	QLabel *label;
	QLineEdit *titleInput;
	QLabel *label_2;
	QLineEdit *urlInput;
	QSpacerItem *verticalSpacer;
	QFrame *line;
	QDialogButtonBox *buttonBox;
	
	void setupUi(QDialog *AddLinkDiaLog)
	{
		if (AddLinkDiaLog->objectName().isEmpty())
			AddLinkDiaLog->setObjectName(QString::fromUtf8("AddLinkDiaLog"));
		AddLinkDiaLog->setSizeGripEnabled(false);
		AddLinkDiaLog->setModal(true);
		verticalLayout = new QVBoxLayout(AddLinkDiaLog);
		verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
		formLayout = new QFormLayout();
		formLayout->setObjectName(QString::fromUtf8("formLayout"));
		label = new QLabel(AddLinkDiaLog);
		label->setObjectName(QString::fromUtf8("label"));
		
		formLayout->setWidget(0, QFormLayout::LabelRole, label);
		
		titleInput = new QLineEdit(AddLinkDiaLog);
		titleInput->setObjectName(QString::fromUtf8("titleInput"));
		titleInput->setMinimumSize(QSize(337, 0));
		
		formLayout->setWidget(0, QFormLayout::FieldRole, titleInput);
		
		label_2 = new QLabel(AddLinkDiaLog);
		label_2->setObjectName(QString::fromUtf8("label_2"));
		
		formLayout->setWidget(1, QFormLayout::LabelRole, label_2);
		
		urlInput = new QLineEdit(AddLinkDiaLog);
		urlInput->setObjectName(QString::fromUtf8("urlInput"));
		
		formLayout->setWidget(1, QFormLayout::FieldRole, urlInput);
		
		
		verticalLayout->addLayout(formLayout);
		
		verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
		
		verticalLayout->addItem(verticalSpacer);
		
		line = new QFrame(AddLinkDiaLog);
		line->setObjectName(QString::fromUtf8("line"));
		line->setFrameShape(QFrame::HLine);
		line->setFrameShadow(QFrame::Sunken);
		
		verticalLayout->addWidget(line);
		
		buttonBox = new QDialogButtonBox(AddLinkDiaLog);
		buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
		
		verticalLayout->addWidget(buttonBox);
		
		
		retranslateUi(AddLinkDiaLog);
		QObject::connect(buttonBox, SIGNAL(accepted()), AddLinkDiaLog, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), AddLinkDiaLog, SLOT(reject()));
		
		QMetaObject::connectSlotsByName(AddLinkDiaLog);
	} // setupUi
	
	void retranslateUi(QDialog *AddLinkDiaLog)
	{
		AddLinkDiaLog->setWindowTitle(QApplication::translate("AddLinkDiaLog", "Insert Link", nullptr));
		label->setText(QApplication::translate("AddLinkDiaLog", "Title:", nullptr));
		label_2->setText(QApplication::translate("AddLinkDiaLog", "URL:", nullptr));
	} // retranslateUi
	
};

namespace Ui {
	class AddLinkDiaLog: public Ui_AddLinkDiaLog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDLINKDIALOG_H
