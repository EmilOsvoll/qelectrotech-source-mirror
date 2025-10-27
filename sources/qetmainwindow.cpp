/*
	Copyright 2006-2025 The QElectroTech Team
	This file is part of QElectroTech.

	QElectroTech is free software: you can redistribute it &&/or modify
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
#include <QAction>
#include <QWhatsThis>
#include <QMenu>
#include <QMenuBar>
#include <QDragEnterEvent>
#include <QDesktopServices>

#include "qetmainwindow.h"
#include "qeticons.h"
#include "qetapp.h"
#include "qetdiagrameditor.h"
#include "projectview.h"

/**
	Constructor
*/
QETMainWindow::QETMainWindow(QWidget *widget, Qt::WindowFlags flags) :
	QMainWindow(widget, flags),
	display_toolbars_(nullptr),
	first_activation_(true)
{
	initCommonActions();
	initCommonMenus();

	setAcceptDrops(true);
}

/**
	Tostructor
*/
QETMainWindow::~QETMainWindow()
{
}

/**
	Initialize common actions.
*/
void QETMainWindow::initCommonActions()
{
	QETApp *qet_app = QETApp::instance();

	configure_action_ = new QAction(QET::Icons::Configure, tr("&Configure QElectroTech"), this);
	configure_action_ -> setStatusTip(tr("Allows to specify various parameters for QElectroTech", "status bar tip"));
	connect(configure_action_, &QAction::triggered, [qet_app]()
	{
		qet_app->configureQET();
#if TODO_LIST
#	pragma message("@TODO we use reloadOldElementPanel only to keep up to ")
#	pragma message("datethe string of the folio in the old element panel.")
#	pragma message("then,if user change the option")
#	pragma message(" 'Use labels of folio instead of their ID' the string")
#	pragma message(" of folio in the old element panel is up to date")
#endif
			//TODO we use reloadOldElementPanel only to keep up to date the string of the folio in the old element panel.
			//then, if user change the option "Use labels of folio instead of their ID" the string of folio in the old element panel is up to date
		for (QETDiagramEditor *qde : qet_app->diagramEditors())
		{
			qde->reloadOldElementPanel();
			for (ProjectView *pv : qde->openedProjects())
			{
				pv->updateAllTabsTitle();
			}
		}
	});

	fullscreen_action_ = new QAction(this);
	updateFullScreenAction();
	connect(fullscreen_action_, SIGNAL(triggered()), this, SLOT(toggleFullScreen()));

	whatsthis_action_ = QWhatsThis::createAction(this);

	about_qet_ = new QAction(QET::Icons::QETLogo, tr("About QElectroTech"), this);
	about_qet_ -> setStatusTip(tr("Displays information about QElectroTech", "status bar tip"));
	connect(about_qet_,  SIGNAL(triggered()), qet_app, SLOT(aboutQET()));

	manual_online_ = new QAction(QET::Icons::QETManual, tr("Online manual"), this);
	manual_online_ -> setStatusTip(tr("Launches the default browser to the online manual QElectroTech", "status bar tip"));

	connect(manual_online_, &QAction::triggered, [](bool) {
	QString link = "https://download.qelectrotech.org/qet/manual_0.7/build/index.html";
	QDesktopServices::openUrl(QUrl(link));
	});

	manual_online_ -> setShortcut(Qt::Key_F1);

	youtube_ = new QAction(QET::Icons::QETVideo, tr("Youtube channel"), this);
	youtube_ -> setStatusTip(tr("Launches the default browser on the Youtube channel of QElectroTech", "status bar tip"));

	connect(youtube_, &QAction::triggered, [](bool) {
	QString link = "https://www.youtube.com/user/scorpio8101/videos";
	QDesktopServices::openUrl(QUrl(link));
	});

	upgrade_ = new QAction(QET::Icons::QETDownload, tr("Download a new version (dev)"), this);
	upgrade_ -> setStatusTip(tr("Launches the default browser to the online repository Nightly QElectroTech", "status bar tip"));

	upgrade_M = new QAction(QET::Icons::QETDownload, tr("Download a new version (dev)"), this);
	upgrade_M -> setStatusTip(tr("Launches the default browser to the online repository Nightly QElectroTech", "status bar tip"));

	connect(upgrade_, &QAction::triggered, [](bool) {
	QString link = "https://qelectrotech.org/download_windows.php";
	QDesktopServices::openUrl(QUrl(link));
	});

	connect(upgrade_M, &QAction::triggered, [](bool) {
	QString link = "https://qelectrotech.org/download_mac.php";
	QDesktopServices::openUrl(QUrl(link));
	});

	donate_ = new QAction(QET::Icons::QETDonate, tr("Soutenir le project par un don"), this);
	donate_ -> setStatusTip(tr("Soutenir le project QElectroTech par un don", "status bar tip"));

	connect(donate_, &QAction::triggered, [](bool) {
	QString link = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=ZZHC9D7C3MDPC";
	QDesktopServices::openUrl(QUrl(link));
	});

	about_qt_ = new QAction(QET::Icons::QtLogo,  tr("About Qt"), this);
	about_qt_ -> setStatusTip(tr("Displays information about Qt library", "status bar tip"));
	connect(about_qt_, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

/**
	Initialize common menus.
*/
void QETMainWindow::initCommonMenus()
{
	settings_menu_ = new QMenu(tr("&Settings", "window menu"), this);
	settings_menu_ -> addAction(fullscreen_action_);
	settings_menu_ -> addAction(configure_action_);
	connect(settings_menu_, SIGNAL(aboutToShow()), this, SLOT(checkToolbarsmenu()));

	help_menu_ = new QMenu(tr("&Help", "window menu"), this);
	help_menu_ -> addAction(whatsthis_action_);
	help_menu_ -> addSeparator();
	help_menu_ -> addAction(manual_online_);
	help_menu_ -> addAction(youtube_);
	help_menu_ -> addAction(upgrade_);
	help_menu_ -> addAction(upgrade_M);
	help_menu_ -> addAction(donate_);
	help_menu_ -> addAction(about_qt_);
	help_menu_ -> addAction(about_qet_);

#ifdef Q_OS_WIN32
upgrade_ -> setVisible(true);
#else
upgrade_ -> setVisible(false);
#endif

#ifdef Q_OS_MACOS
upgrade_M -> setVisible(true);
#else
upgrade_M -> setVisible(false);
#endif

	insertMenu(nullptr, settings_menu_);
	insertMenu(nullptr, help_menu_);
}

/**
	Add \a menu before \a before. Unless \a customize is false, this method also
	enables some common settings on the inserted menu.
*/
void QETMainWindow::insertMenu(QMenu *before, QMenu *menu, bool customize) {
	if (!menu) return;

	QAction *before_action = actionForMenu(before);
	QAction *menu_action = menuBar() -> insertMenu(before_action, menu);
	menu_actions_.insert(menu, menu_action);

	if (customize) {
		menu -> setTearOffEnabled(true);
	}
}

/**
	@return the action returned when inserting \a menu
*/
QAction *QETMainWindow::actionForMenu(QMenu *menu) {
	return(menu_actions_.value(menu, nullptr));
}

/**
	Toggle the window from/to full screen.
*/
void QETMainWindow::toggleFullScreen()
{
	setWindowState(windowState() ^ Qt::WindowFullScreen);
}

/**
	Update the look of the full screen action according to the current state of
	the window.
*/
void QETMainWindow::updateFullScreenAction()
{
	if (windowState() & Qt::WindowFullScreen) {
		fullscreen_action_ -> setText(tr("Leave F&ullScreen Mode"));
		fullscreen_action_ -> setIcon(QET::Icons::FullScreenExit);
		fullscreen_action_ -> setStatusTip(tr("Displays QElectroTech in windowed mode", "status bar tip"));
	} else {
		fullscreen_action_ -> setText(tr("F&ullScreen Mode"));
		fullscreen_action_ -> setIcon(QET::Icons::FullScreenEnter);
		fullscreen_action_ -> setStatusTip(tr("Displays QElectroTech in full screen mode", "status bar tip"));
	}
	fullscreen_action_ -> setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_F);
}

/**
	Check whether a sub menu dedicated to docks && toolbars can be inserted on
	top of the settings menu.
*/
void QETMainWindow::checkToolbarsmenu()
{
	if (display_toolbars_) return;
	display_toolbars_ = createPopupMenu();
	if (display_toolbars_) {
		display_toolbars_ -> setTearOffEnabled(true);
		display_toolbars_ -> setTitle(tr("Display", "menu entry"));
		display_toolbars_ -> setIcon(QET::Icons::ConfigureToolbars);
		settings_menu_ -> insertMenu(fullscreen_action_, display_toolbars_);
	}
}

/**
	Handle the \a e event.
*/
bool QETMainWindow::event(QEvent *e) {
	if (e -> type() == QEvent::WindowStateChange) {
		updateFullScreenAction();
	} else if (first_activation_ && e -> type() == QEvent::WindowActivate) {
		firstActivation(e);
		first_activation_ = false;
	}
	return(QMainWindow::event(e));
}

/**
	Bottome implementation of firstActivation (does nothing).
*/
void QETMainWindow::firstActivation(QEvent *) {
}


/**
	Accept or refuse drag'n drop events depending on the dropped mime type;
	especially, accepts only URLs to local files that we could open.
	@param e le QDragEnterEvent correspondant au drag'n drop tente
*/
void QETMainWindow::dragEnterEvent(QDragEnterEvent *e) {
	if (e -> mimeData() -> hasUrls()) {
		if (QETApp::handledFiles(e -> mimeData() -> urls()).count()) {
			e -> acceptProposedAction();
		}
	}
}

/**
	Handle drops accepted on main windows; more specifically, open dropped files
	as long as they are handled by QElectrotech.
	@param e the QDropEvent describing the current drag'n drop
*/
void QETMainWindow::dropEvent(QDropEvent *e) {
	if (e -> mimeData() -> hasUrls()) {
		QStringList filepaths = QETApp::handledFiles(e -> mimeData() -> urls());
		if (filepaths.count()) {
			QETApp::instance() -> openFiles(QETArguments(filepaths));
		}
	}
}
