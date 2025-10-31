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
#include "projectprintwindow.h"

#include "../diagram.h"
#include "../qeticons.h"
#include "../qetproject.h"
#include "../qetversion.h"

#include "ui_projectprintwindow.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0) // ### Qt 6: remove
#	include <QDesktopWidget>
#else
#	if TODO_LIST
#		pragma message("@TODO remove code for QT 6 or later")
#	endif
#endif
#include <QDebug>
#include <QMarginsF>
#include <QPageSetupDialog>
#include <QPainter>
#include <QPrintDialog>
#include <QPrintPreviewWidget>
#include <QScreen>

/**
 * @brief ProjectPrintWindow::ProjectPrintWindow
 * Use this static function to properly launch the print dialog.
 * @param project : project to print
 * @param format : native format to print in physical printer, or pdf format to export in pdf
 * @param parent : parent widget
 */
void ProjectPrintWindow::launchDialog(QETProject *project, QPrinter::OutputFormat format, QWidget *parent)
{
	auto printer_ = new QPrinter();
	QPrinter printer(QPrinter::HighResolution);
	printer_->setDocName(ProjectPrintWindow::docName(project));
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 1) // ### Qt 6: remove
	printer_->setOrientation(QPrinter::Landscape);
#else
#if TODO_LIST
#pragma message("@TODO remove code for QT 6 or later")
#	endif
	printer_->setPageOrientation(QPageLayout::Landscape);
#endif

	if (format == QPrinter::NativeFormat) //To physical printer
	{
		QPrintDialog print_dialog(printer_, parent);
#ifdef Q_OS_MACOS
		print_dialog.setWindowFlags(Qt::Sheet);
#endif
		print_dialog.setWindowTitle(tr("Print options", "window title"));
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)	// ### Qt 6: remove
		print_dialog.setEnabledOptions(QAbstractPrintDialog::PrintShowPageSize);
#else
#if TODO_LIST
#pragma message("@TODO remove code for QT 6 or later")
#endif
		qDebug()<<"Help code for QT 6 or later";
#endif
		if (print_dialog.exec() == QDialog::Rejected) {
			delete  printer_;
			return;
		}
	}
	else //To pdf file
	{
		auto dir_path = project->currentDir();
		QString file_name = QDir::toNativeSeparators(QDir::cleanPath(dir_path + "/" + printer_->docName()));
		if (!file_name.endsWith(".pdf")) {
			file_name.append(".pdf");
		}
		printer_->setCreator(QString("QElectroTech %1").arg(QetVersion::displayedVersion()));
		printer_->setOutputFileName(file_name);
		printer_->setOutputFormat(QPrinter::PdfFormat);
		// Set printer DPI to match screen DPI to ensure font scaling matches preview
		// Note: DPI must be set before paper size so Qt calculates pixel dimensions correctly
		QScreen *screen = QApplication::primaryScreen();
		qreal screen_dpi = screen ? screen->logicalDotsPerInch() : 96.0;
		printer_->setResolution(qRound(screen_dpi));
	}

	auto w = new ProjectPrintWindow(project, printer_, parent);
	w->showMaximized();
}

QString ProjectPrintWindow::docName(QETProject *project)
{
	QString doc_name;
	if (!project->filePath().isEmpty()) {
		doc_name = QFileInfo(project->filePath()).baseName();
	} else if (!project->title().isEmpty()) {
		doc_name = project->title();
		doc_name = QET::stringToFileName(doc_name);
	}

	if (doc_name.isEmpty()) {
		doc_name = tr("project", "string used to generate a filename");
	}

	return doc_name;
}

/**
 * @brief ProjectPrintWindow::ProjectPrintWindow
 * Constructor, don't use this class directly, instead use ProjectPrintWindow::launchDialog static function.
 * @param project
 * @param printer : QPrinter to use. Note that ProjectPrintWindow take ownerchip of @printer
 * @param parent
 */
ProjectPrintWindow::ProjectPrintWindow(QETProject *project, QPrinter *printer, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::ProjectPrintWindow),
	m_project(project),
	m_printer(printer)
{
	ui->setupUi(this);

	// For PDF exports, set DPI to match screen DPI BEFORE loading page setup
	// This ensures Qt calculates pixel dimensions correctly and font scaling matches preview
	if (m_printer->outputFormat() == QPrinter::PdfFormat)
	{
		QScreen *screen = QApplication::primaryScreen();
		qreal screen_dpi = screen ? screen->logicalDotsPerInch() : 96.0;
		m_printer->setResolution(qRound(screen_dpi));
	}

	loadPageSetupForCurrentPrinter();
	
	// For PDF exports, always use A3 landscape to ensure correct width
	// and re-apply paper size and orientation to ensure Qt recalculates correctly with screen DPI
	if (m_printer->outputFormat() == QPrinter::PdfFormat)
	{
		// Always set A3 landscape for PDF exports (user can't change it on Windows anyway)
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 1) // ### Qt 6: remove
		m_printer->setPaperSize(QPrinter::A3);
		m_printer->setOrientation(QPrinter::Landscape);
#else
		m_printer->setPageSize(QPageSize(QPageSize::A3));
		m_printer->setPageOrientation(QPageLayout::Landscape);
#endif
		
		// Save current orientation before re-applying paper size
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 1) // ### Qt 6: remove
		QPrinter::Orientation saved_orientation = m_printer->orientation();
		QPrinter::PaperSize paper_size = m_printer->paperSize();
		if (paper_size != QPrinter::Custom) {
			m_printer->setPaperSize(paper_size);
		} else {
			QSizeF size_mm = m_printer->paperSize(QPrinter::Millimeter);
			m_printer->setPaperSize(size_mm, QPrinter::Millimeter);
		}
		// Re-apply orientation to ensure it's maintained
		m_printer->setOrientation(saved_orientation);
#else
		QPageLayout::Orientation saved_orientation = m_printer->pageLayout().orientation();
		QPageSize page_size = m_printer->pageLayout().pageSize();
		if (page_size.id() != QPageSize::Custom) {
			m_printer->setPageSize(page_size);
		} else {
			QSizeF size_mm = page_size.size(QPageSize::Millimeter);
			m_printer->setPageSize(QPageSize(size_mm, QPageSize::Millimeter));
		}
		// Re-apply orientation to ensure it's maintained
		m_printer->setPageOrientation(saved_orientation);
#endif
	}
	
	// After loading saved settings, ensure the "Use the whole paper" checkbox 
	// state is properly applied to the printer if it's checked by default
	// This handles the case where the checkbox is checked in UI but saved settings had it unchecked
	if (ui->m_use_full_page_cb->isChecked() && !m_printer->fullPage()) {
		m_printer->setFullPage(true);
	}

	m_preview = new QPrintPreviewWidget(m_printer);
	connect(m_preview, &QPrintPreviewWidget::paintRequested, this, &ProjectPrintWindow::requestPaint);
	ui->m_vertical_layout->addWidget(m_preview);

	setUpDiagramList();

	if (m_printer->outputFormat() == QPrinter::NativeFormat) //Print to physical printer
	{
		auto print_button = new QPushButton(QET::Icons::DocumentPrint, tr("Print"));
		ui->m_button_box->addButton(print_button, QDialogButtonBox::ActionRole);
		connect(print_button, &QPushButton::clicked, this, &ProjectPrintWindow::print);
	}
	else	//export to pdf
	{
		auto pdf_button = new QPushButton(QET::Icons::PDF, tr("Export to PDF"));
		ui->m_button_box->addButton(pdf_button, QDialogButtonBox::ActionRole);
		connect(pdf_button, &QPushButton::clicked, this, &ProjectPrintWindow::exportToPDF);
	}

	auto exp = ExportProperties::defaultPrintProperties();
	ui->m_draw_border_cb->setChecked(exp.draw_border);
	ui->m_draw_titleblock_cb->setChecked(exp.draw_titleblock);
	ui->m_draw_terminal_cb->setChecked(exp.draw_terminals);
	ui->m_keep_conductor_color_cb->setChecked(exp.draw_colored_conductors);

	ui->m_date_cb->blockSignals(true);
	ui->m_date_cb->setDate(QDate::currentDate());
	ui->m_date_cb->blockSignals(false);

#ifdef Q_OS_WINDOWS
	/*
	 * On windows, the QPageSetupDialog use the native dialog.
	 * This dialog can only manage physical printer ("native printer")
	 */
	if (m_printer->outputFormat() == QPrinter::PdfFormat)
	{
		ui->m_page_setup->setDisabled(true);
		ui->m_page_setup->setText(tr("Page setup (not available on Windows for PDF export)"));
	}
#endif

	m_backup_diagram_background_color = Diagram::background_color;
	Diagram::background_color = Qt::white;
	
	// Connect "Use the whole paper" checkbox to enable/disable margin input
	connect(ui->m_use_full_page_cb, &QCheckBox::toggled, this, [this](bool checked) {
		ui->m_margin_label->setEnabled(checked);
		ui->m_margin_sp->setEnabled(checked);
	});
	
	// Connect margin value change to update preview
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
	connect(ui->m_margin_sp, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, [this](double) {
			if (ui->m_use_full_page_cb->isChecked()) {
				m_preview->updatePreview();
			}
		});
#else
	connect(ui->m_margin_sp, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this, [this](double) {
			if (ui->m_use_full_page_cb->isChecked()) {
				m_preview->updatePreview();
			}
		});
#endif
	
	// Connect anchor combo boxes to update preview
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
	connect(ui->m_anchor_horizontal_cb, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, [this](int) { m_preview->updatePreview(); });
	connect(ui->m_anchor_vertical_cb, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, [this](int) { m_preview->updatePreview(); });
#else
	connect(ui->m_anchor_horizontal_cb, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		this, [this](int) { m_preview->updatePreview(); });
	connect(ui->m_anchor_vertical_cb, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		this, [this](int) { m_preview->updatePreview(); });
#endif
	
	// Initially disable margin input if "Use the whole paper" is not checked
	ui->m_margin_label->setEnabled(ui->m_use_full_page_cb->isChecked());
	ui->m_margin_sp->setEnabled(ui->m_use_full_page_cb->isChecked());
	
	// Ensure the printer's fullPage setting matches the checkbox state
	// This is important when the checkbox is checked by default in the UI
	m_printer->setFullPage(ui->m_use_full_page_cb->isChecked());
	
	// Force preview update to apply all settings
	m_preview->updatePreview();
}

/**
 * @brief ProjectPrintWindow::~ProjectPrintWindow
 */
ProjectPrintWindow::~ProjectPrintWindow()
{
	delete ui;
	delete m_printer;
	Diagram::background_color = m_backup_diagram_background_color;
}

/**
 * @brief ProjectPrintWindow::requestPaint
 * @param slot called when m_preview emit paintRequested
 */
void ProjectPrintWindow::requestPaint()
{
	#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
		#ifdef Q_OS_WIN
			// Only apply screen DPI for physical printers, not for PDF exports
			// PDF exports use screen DPI set in launchDialog() to match preview
			if (m_printer->outputFormat() == QPrinter::NativeFormat)
			{
				#ifdef QT_DEBUG
				qDebug() << "--";
				qDebug() << "DiagramPrintDialog::print  printer_->resolution() before " << m_printer->resolution();
				qDebug() << "DiagramPrintDialog::print  screennumber " << QApplication::desktop()->screenNumber();
				#endif

				QScreen *srn = QApplication::screens().at(QApplication::desktop()->screenNumber());
				qreal dotsPerInch = (qreal)srn->logicalDotsPerInch();
				m_printer->setResolution(dotsPerInch);

				#ifdef QT_DEBUG
					qDebug() << "DiagramPrintDialog::print  dotsPerInch " << dotsPerInch;
					qDebug() << "DiagramPrintDialog::print  printer_->resolution() after" << m_printer->resolution();
				qDebug() << "--";
				#endif
			}
		#endif
	#endif

	if (!m_project->diagrams().count()) {
		return;
	}

	bool first = true;
	QPainter painter(m_printer);
	
	// Ensure proper rendering hints for high-quality PDF export
	// This ensures fonts and graphics scale correctly
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	
	for (auto diagram : selectedDiagram())
	{
		first ? first = false : m_printer->newPage();
		printDiagram(diagram, ui->m_fit_in_page_cb->isChecked(), &painter, m_printer);
	}
}

/**
 * @brief ProjectPrintWindow::printDiagram
 * Print @diagram on the @printer
 * @param diagram
 * @param fit_page
 * @param printer
 */
void ProjectPrintWindow::printDiagram(Diagram *diagram, bool fit_page, QPainter *painter, QPrinter *printer)
{

	////Prepare the print////
	// Deselect all
	diagram->deselectAll();
	// Disable focus flags
	QList<QGraphicsItem *> focusable_items;
	for (auto qgi : diagram->items()) {
		if (qgi->flags() & QGraphicsItem::ItemIsFocusable) {
			focusable_items << qgi;
			qgi->setFlag(QGraphicsItem::ItemIsFocusable, false);
		}
	}
	// Disable interaction
	for (auto view : diagram->views()) {
		view->setInteractive(false);
	}
	auto option = exportProperties();
	saveReloadDiagramParameters(diagram, option, true);
	////Prepare end////


	auto full_page = printer->fullPage();
	auto diagram_rect = QRectF(diagramRect(diagram, option));
	
	// Apply printer margin when "Use the whole paper" is enabled
	qreal margin_mm = 0.0;
	if (full_page && ui->m_use_full_page_cb->isChecked()) {
		margin_mm = ui->m_margin_sp->value();
	}
	
	if (fit_page) {
		// When fitting to page, use Qt's pageRect() or paperRect() directly
		// to ensure we're using the same coordinate system Qt uses internally
		QRectF available_rect;
		
		if (full_page) {
			// Use paperRect() - gives full paper size in printer coordinates
			available_rect = printer->paperRect();
		} else {
			// Use pageRect() - gives printable area (excluding margins) in printer coordinates
			available_rect = printer->pageRect();
		}
		
		qDebug() << "Qt pageRect (points):" << printer->pageRect();
		qDebug() << "Qt paperRect (points):" << printer->paperRect();
		
		// Apply user margin if "Use the whole paper" is enabled
		if (full_page && margin_mm > 0.0) {
			// Convert margin from millimeters to points
			qreal margin_pt = margin_mm * 72.0 / 25.4; // Convert mm to points
			available_rect.adjust(margin_pt, margin_pt, -margin_pt, -margin_pt);
		}
		
		// Calculate scaled content size maintaining aspect ratio
		// QGraphicsScene coordinates are in device-independent pixels (DIPs, typically 1/96 inch)
		// Printer coordinates are in points (1/72 inch)
		// Qt's render() when rendering to QPrinter accounts for coordinate system differences
		// but we need to ensure the scale calculation is correct
		
		qDebug() << "=== PDF Export Scaling Debug ===";
		qDebug() << "Printer format:" << printer->outputFormat() << "Resolution:" << printer->resolution();
		qDebug() << "Full page:" << full_page << "Margin (mm):" << margin_mm;
		qDebug() << "Available rect (points):" << available_rect;
		
		// Get printer and screen DPI for coordinate conversion
		qreal printer_dpi = printer->resolution();
		QScreen *screen = QApplication::primaryScreen();
		qreal screen_dpi = screen ? screen->logicalDotsPerInch() : 96.0;
		qDebug() << "Screen logical DPI:" << screen_dpi << "Printer DPI:" << printer_dpi;
		
		// Diagram rect in DIPs (scene coordinates)
		qDebug() << "Diagram rect (DIPs):" << diagram_rect;
		
		// Qt's render() when rendering to QPrinter uses the printer's resolution
		// for coordinate conversion. The key insight: Qt may convert based on printer DPI.
		// 
		// At printer resolution of 300 DPI:
		// - 1 printer pixel = 1/300 inch = 72/300 = 0.24 points
		// - 1 DIP (at 96 DPI) = 1/96 inch = 72/96 = 0.75 points
		//
		// Qt's render() might be using printer pixels internally, converting DIPs → printer pixels → points
		// So: 1 DIP → (96/300) printer pixels → (96/300) * (72/300) points = wrong!
		//
		// Actually, let's try a simpler approach: Qt's render() may just do direct 1:1 mapping
		// where 1 DIP coordinate = 1 point coordinate, ignoring physical sizes.
		// So we need: target_size = desired_size * (screen_dpi/72) = desired_size * (96/72) = desired_size * 1.333
		
		// CRITICAL INSIGHT: Qt's render() does 1:1 coordinate mapping
		// This means it maps source.size() (in DIPs) directly to target.size() (in points)
		// WITHOUT accounting for physical size difference.
		//
		// So if we want output of S points, and source is N DIPs:
		// - Physically: N DIPs = N * (72/screen_dpi) points
		// - Qt maps: N DIPs → M points (1:1), so M = N if we pass target = N points
		// - Physical output will be M points
		// - But we want S points output, where S = N * (72/screen_dpi) * scale
		//
		// The solution: Calculate scale based on coordinate space, not physical space
		// We need to calculate: what target size (in points) will produce desired physical output?
		//
		// If Qt maps N DIPs → M points (1:1), and we want S points physical output:
		// - Since Qt does 1:1 mapping, M = N (target size = source size in DIPs)
		// - But physically: N DIPs = N * (72/screen_dpi) points
		// - So to get S points output, we need: N * (72/screen_dpi) = S
		// - Therefore: N = S / (72/screen_dpi) = S * (screen_dpi/72)
		// - So target (in points) = desired_physical_output * (screen_dpi/72)
		
		// Use the same approach as multi-page rendering: render directly using Qt's coordinate system
		// The multi-page code uses diagram_rect (DIPs) and render_target (points) directly
		// without compensation, suggesting Qt handles conversion automatically.
		//
		// Let's calculate scale to fit available_rect using the coordinate sizes directly
		// Qt's render() should handle the coordinate conversion
		qreal scale_x = available_rect.width() / diagram_rect.width();
		qreal scale_y = available_rect.height() / diagram_rect.height();
		qreal scale = qMin(scale_x, scale_y);
		qDebug() << "Scale factors (DIPs → points coordinate space): X:" << scale_x << "Y:" << scale_y << "Min:" << scale;
		
		// Calculate target size directly: scale diagram size
		QSizeF target_size_pt(
			diagram_rect.width() * scale,
			diagram_rect.height() * scale);
		qDebug() << "Target size (points, matching Qt coordinate system):" << target_size_pt;
		
		// Note: The target size may exceed available space after compensation.
		// This is intentional - Qt will handle the mapping and the physical output
		// will be the desired size. The target rect will be positioned within available_rect.
		
		// Calculate anchored target rectangle in points (printer coordinates)
		QRectF target_rect_pt = calculateAnchoredRect(available_rect, target_size_pt);
		qDebug() << "Final target rect (points):" << target_rect_pt;
		qDebug() << "Source diagram rect (DIPs):" << diagram_rect;
		qDebug() << "=== End Debug ===";
		
		// Ensure proper rendering hints for font scaling
		// Fonts should scale proportionally with geometry when rendering to QPrinter
		painter->setRenderHint(QPainter::TextAntialiasing, true);
		
		// Render diagram - Qt will map diagram_rect (scene coords, DIPs) to target_rect (painter coords, points)
		// Fonts will scale proportionally with the geometry transformation
		diagram->render(painter, target_rect_pt, diagram_rect, Qt::KeepAspectRatio);
	} else {
		// Print on one or several pages
		// Calculate page dimensions from physical size to ensure correct values
		QRectF printed_rect;
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 1) // ### Qt 6: remove
		QSizeF page_size_mm = full_page ? 
			printer->paperSize(QPrinter::Millimeter) : 
			printer->pageSize(QPrinter::Millimeter);
#else
		QSizeF page_size_mm = printer->pageLayout().pageSize().size(QPageSize::Millimeter);
		if (!full_page) {
			// For pageRect (not paperRect), account for margins
			QMarginsF margins_mm = printer->pageLayout().margins(QPageLayout::Millimeter);
			page_size_mm.setWidth(page_size_mm.width() - margins_mm.left() - margins_mm.right());
			page_size_mm.setHeight(page_size_mm.height() - margins_mm.top() - margins_mm.bottom());
		}
#endif
		// Convert millimeters to points (Qt's logical coordinate system: 1 point = 1/72 inch)
		qreal width_pt = page_size_mm.width() * 72.0 / 25.4;
		qreal height_pt = page_size_mm.height() * 72.0 / 25.4;
		printed_rect = QRectF(0, 0, width_pt, height_pt);
		// Apply margin to shrink printable area when "Use the whole paper" is enabled
		if (full_page && margin_mm > 0.0) {
			// Convert margin from millimeters to points
			qreal margin_pt = margin_mm * 72.0 / 25.4; // Convert mm to points
			printed_rect.adjust(margin_pt, margin_pt, -margin_pt, -margin_pt);
		}
		auto used_width  = printed_rect.width();
		auto used_height = printed_rect.height();
		auto h_pages_count = horizontalPagesCount(diagram, option, full_page);
		auto v_pages_count = verticalPagesCount(diagram, option, full_page);

		QVector<QVector<QRectF>> page_grid;
		// The diagram is printed on a matrix of sheet
		// scrolls through the rows of the matrix
		auto y_offset = 0;
		for (auto i = 0; i < v_pages_count; ++i)
		{
			page_grid << QVector<QRectF>();
			// scrolls through the lines of sheet
			auto x_offset = 0;
			for (auto j=0 ; j<h_pages_count ; ++j)
			{
				page_grid.last() << QRectF(
					QPoint(x_offset, y_offset),
					QSize(
						qMin(used_width, diagram_rect.width() - x_offset),
						qMin(used_height, diagram_rect.height() - y_offset)));

				x_offset += used_width;
			}
			y_offset += used_height;
		}

		// Retains only the pages to be printed
		QVector<QRectF> page_to_print;
		for (auto i=0 ; i < v_pages_count ; ++i) {
			for (int j=0 ; j < h_pages_count ; ++j) {
				page_to_print << page_grid.at(i).at(j);
			}
		}

		// Scrolls through the page for print
		bool first_ = true;
		for (auto& page : page_to_print)
		{
			first_ ? first_ = false : m_printer->newPage();
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 1) // ### Qt 6: remove
			// Adjust page rectangle to account for margin when rendering
			// page coordinates are already in points (logical units)
			QRectF render_target = QRectF(QPoint(0, 0), page.size());
			if (full_page && margin_mm > 0.0) {
				// Convert margin from millimeters to points (Qt's logical coordinate system)
				qreal margin_pt = margin_mm * 72.0 / 25.4; // Convert mm to points
				// Shift the render target to account for the margin offset
				render_target = QRectF(QPoint(margin_pt, margin_pt), page.size());
			}
#else
			// For Qt >= 5.15.1, printed_rect already accounts for margin in pixels
			// (margin was converted and applied earlier), so page rectangles are already adjusted
			// No additional offset needed in render target
			QRectF render_target = QRectF(QPoint(0, 0), page.size());
#endif
			diagram->render(
				painter,
				render_target,
				page.translated(diagram_rect.topLeft()),
				Qt::KeepAspectRatio);
		}
	}

	////Print is finished, restore diagram and graphics item properties
	for (auto view : diagram->views()) {
		view->setInteractive(true);
	}
	for (auto qgi : focusable_items) {
		qgi->setFlag(QGraphicsItem::ItemIsFocusable, true);
	}
	saveReloadDiagramParameters(diagram, option, false);
}

/**
 * @brief ProjectPrintWindow::diagramRect
 * @param diagram
 * @param option
 * @return The rectangle of diagram to be printed
 */
QRect ProjectPrintWindow::diagramRect(Diagram *diagram, const ExportProperties &option) const
{
	auto diagram_rect = diagram->border_and_titleblock.borderAndTitleBlockRect();
	if (!option.draw_titleblock) {
		auto titleblock_height = diagram->border_and_titleblock.titleBlockRect().height();
		diagram_rect.setHeight(diagram_rect.height() - titleblock_height);
	}

		//Adjust the border of diagram to 1px (width of the line)
	diagram_rect.adjust(0,0,1,1);

	return (diagram_rect.toAlignedRect());
}

/**
 * @brief ProjectPrintWindow::horizontalPagesCount
 * @param diagram : diagram to print
 * @param option : option used to render
 * @param full_page : full page or not
 * @return The width of the "poster" in number of page for print the diagram
 * with the orientation and the paper format used by the actual printer
 */
int ProjectPrintWindow::horizontalPagesCount(
		Diagram *diagram, const ExportProperties &option, bool full_page) const
{
	// Calculate page dimensions from physical size to ensure correct values
	QRect printable_area;
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 1) // ### Qt 6: remove
	QSizeF page_size_mm = full_page ? 
		m_printer->paperSize(QPrinter::Millimeter) : 
		m_printer->pageSize(QPrinter::Millimeter);
#else
	QSizeF page_size_mm = m_printer->pageLayout().pageSize().size(QPageSize::Millimeter);
	if (!full_page) {
		// For pageRect (not paperRect), account for margins
		QMarginsF margins_mm = m_printer->pageLayout().margins(QPageLayout::Millimeter);
		page_size_mm.setWidth(page_size_mm.width() - margins_mm.left() - margins_mm.right());
		page_size_mm.setHeight(page_size_mm.height() - margins_mm.top() - margins_mm.bottom());
	}
#endif
	// Convert millimeters to points (Qt's logical coordinate system: 1 point = 1/72 inch)
	qreal width_pt = page_size_mm.width() * 72.0 / 25.4;
	qreal height_pt = page_size_mm.height() * 72.0 / 25.4;
	printable_area = QRect(0, 0, (int)width_pt, (int)height_pt);
	QRect diagram_rect = diagramRect(diagram, option);

	int h_pages_count = int(ceil(qreal(diagram_rect.width()) / qreal(printable_area.width())));
	return(h_pages_count);
}

/**
 * @brief ProjectPrintWindow::verticalPagesCount
 * @param diagram : diagram to print
 * @param option : option used to render
 * @param full_page : full page or not
 * @return The height of the "poster" in number of pages for print the diagram
 * with the orientation and paper format used by the actual printer
 */
int ProjectPrintWindow::verticalPagesCount(
		Diagram *diagram, const ExportProperties &option, bool full_page) const
{
	// Calculate page dimensions from physical size to ensure correct values
	QRect printable_area;
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 1) // ### Qt 6: remove
	QSizeF page_size_mm = full_page ? 
		m_printer->paperSize(QPrinter::Millimeter) : 
		m_printer->pageSize(QPrinter::Millimeter);
#else
	QSizeF page_size_mm = m_printer->pageLayout().pageSize().size(QPageSize::Millimeter);
	if (!full_page) {
		// For pageRect (not paperRect), account for margins
		QMarginsF margins_mm = m_printer->pageLayout().margins(QPageLayout::Millimeter);
		page_size_mm.setWidth(page_size_mm.width() - margins_mm.left() - margins_mm.right());
		page_size_mm.setHeight(page_size_mm.height() - margins_mm.top() - margins_mm.bottom());
	}
#endif
	// Convert millimeters to points (Qt's logical coordinate system: 1 point = 1/72 inch)
	qreal width_pt = page_size_mm.width() * 72.0 / 25.4;
	qreal height_pt = page_size_mm.height() * 72.0 / 25.4;
	printable_area = QRect(0, 0, (int)width_pt, (int)height_pt);
	QRect diagram_rect = diagramRect(diagram, option);

	int v_pages_count = int(ceil(qreal(diagram_rect.height()) / qreal(printable_area.height())));
	return(v_pages_count);
}

/**
 * @brief ProjectPrintWindow::calculateAnchoredRect
 * Calculate the anchored target rectangle for rendering content.
 * When Qt::KeepAspectRatio is used, Qt centers the content within the target rect.
 * So we need to calculate where to position the target rect to achieve the desired anchor.
 * @param available_rect The available area (after margins)
 * @param content_size The size of content to render (after scaling with aspect ratio)
 * @return Anchored target rectangle positioned to achieve desired anchor point
 */
QRectF ProjectPrintWindow::calculateAnchoredRect(const QRectF &available_rect, const QSizeF &content_size) const
{
	// Get anchor selections from UI (0=Left/Top, 1=Center, 2=Right/Bottom)
	int h_anchor = ui->m_anchor_horizontal_cb->currentIndex();
	int v_anchor = ui->m_anchor_vertical_cb->currentIndex();
	
	// Calculate horizontal position
	qreal x = available_rect.left();
	if (h_anchor == 1) { // Center
		x = available_rect.left() + (available_rect.width() - content_size.width()) / 2.0;
	} else if (h_anchor == 2) { // Right
		x = available_rect.right() - content_size.width();
	}
	// else h_anchor == 0 (Left) - x already set to left
	
	// Calculate vertical position
	qreal y = available_rect.top();
	if (v_anchor == 1) { // Center
		y = available_rect.top() + (available_rect.height() - content_size.height()) / 2.0;
	} else if (v_anchor == 2) { // Bottom
		y = available_rect.bottom() - content_size.height();
	}
	// else v_anchor == 0 (Top) - y already set to top
	
	// Return target rectangle at calculated position with content size
	// Qt::KeepAspectRatio will center the rendered content within this rect,
	// but since we've calculated the rect size to match the scaled content size,
	// the content will be positioned at our desired anchor point
	return QRectF(QPointF(x, y), content_size);
}

ExportProperties ProjectPrintWindow::exportProperties() const
{
	ExportProperties exp;
	exp.draw_border             = ui->m_draw_border_cb->isChecked();
	exp.draw_titleblock         = ui->m_draw_titleblock_cb->isChecked();
	exp.draw_terminals          = ui->m_draw_terminal_cb->isChecked();
	exp.draw_colored_conductors = ui->m_keep_conductor_color_cb->isChecked();
	exp.draw_grid = false;

	return exp;
}

void ProjectPrintWindow::setUpDiagramList()
{
	auto layout = new QVBoxLayout();
	auto widget = new QWidget();
	widget->setLayout(layout);
	widget->setMinimumSize(170, 0);
	widget->setMaximumSize(470, 10000);
	widget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
	ui->m_diagram_list->setWidget(widget);

	for (auto diagram : m_project->diagrams())
	{
		auto title = diagram->title();
		if (title.isEmpty()) {
			title = tr("Untitled folio");
		}

		auto checkbox = new QCheckBox(title);
		checkbox->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum));
		checkbox->setChecked(true);
		layout->addWidget(checkbox, 0, Qt::AlignLeft | Qt::AlignTop);
		connect(checkbox, &QCheckBox::clicked, m_preview, &QPrintPreviewWidget::updatePreview);
		m_diagram_list_hash.insert(diagram, checkbox);
	}
	layout->addStretch();
}

QString ProjectPrintWindow::settingsSectionName(const QPrinter *printer)
{
	QPrinter::OutputFormat printer_format = printer -> outputFormat();
	if (printer_format == QPrinter::NativeFormat) {
		return(printer -> printerName().replace(" ", "_"));
	} else if (printer_format == QPrinter::PdfFormat) {
		return("QET_PDF_Printing");
	}
	return(QString());
}

void ProjectPrintWindow::loadPageSetupForCurrentPrinter()
{
	QSettings settings;
	QString	  printer_section = settingsSectionName(m_printer);

	while (! settings.group().isEmpty()) settings.endGroup();
	settings.beginGroup("printers");
	if (! settings.childGroups().contains(printer_section))
	{
		settings.endGroup();
		return;
	}

	settings.beginGroup(printer_section);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 1) // ### Qt 6: remove
	if (settings.contains("orientation")) {
		QString value = settings.value("orientation", "landscape").toString();
		m_printer -> setOrientation(value == "landscape" ? QPrinter::Landscape : QPrinter::Portrait);
	}
	if (settings.contains("papersize")) {
		int value = settings.value("papersize", QPrinter::A4).toInt();
		if (value == QPrinter::Custom) {
			bool w_ok, h_ok;
			int w = settings.value("customwidthmm", -1).toInt(&w_ok);
			int h = settings.value("customheightmm", -1).toInt(&h_ok);
			if (w_ok && h_ok && w != -1 && h != -1) {
				m_printer -> setPaperSize(QSizeF(w, h), QPrinter::Millimeter);
			}
		} else if (value < QPrinter::Custom) {
			m_printer -> setPaperSize(static_cast<QPrinter::PaperSize>(value));
		}
	}

	qreal margins[4];
	m_printer -> getPageMargins(&margins[0], &margins[1], &margins[2], &margins[3], QPrinter::Millimeter);
	QStringList margins_names(QStringList() << "left" << "top" << "right" << "bottom");
	for (int i = 0 ; i < 4 ; ++ i) {
		bool conv_ok;
		qreal value = settings.value("margin" + margins_names.at(i), -1.0).toReal(&conv_ok);
		if (conv_ok && value != -1.0) margins[i] = value;
	}
	m_printer->setPageMargins(
		margins[0],
		margins[1],
		margins[2],
		margins[3],
		QPrinter::Millimeter);
#else
#if TODO_LIST
#pragma message("@TODO remove code for QT 6 or later")
#	endif
	if (settings.contains("orientation"))
	{
		QString value = settings.value("orientation", "landscape").toString();
		m_printer->setPageOrientation(
			value == "landscape" ? QPageLayout::Landscape :
								   QPageLayout::Portrait);
	}
	if (settings.contains("papersize"))
	{
		int value = settings.value("papersize", QPageSize::A4).toInt();
		if (value == QPageSize::Custom)
		{
			bool w_ok, h_ok;
			int	 w = settings.value("customwidthmm", -1).toInt(&w_ok);
			int	 h = settings.value("customheightmm", -1).toInt(&h_ok);
			if (w_ok && h_ok && w != -1 && h != -1)
			{
				m_printer->setPageSize(QPageSize(
					QSizeF(w, h),
					QPageSize::Millimeter,
					"Custom",
					QPageSize::FuzzyMatch));
			}
		}
		else if (value < QPageSize::Custom)
		{
			QPageSize var;
			var.id(value);
			m_printer->setPageSize(var);
		}
	}

	qreal margins[4];
	margins[0] = m_printer->pageLayout().margins().left();
	margins[1] = m_printer->pageLayout().margins().top();
	margins[2] = m_printer->pageLayout().margins().right();
	margins[3] = m_printer->pageLayout().margins().bottom();
	QStringList margins_names(
		QStringList() << "left"
					  << "top"
					  << "right"
					  << "bottom");
	for (int i = 0; i < 4; ++i)
	{
		bool  conv_ok;
		qreal value = settings.value("margin" + margins_names.at(i), -1.0)
						  .toReal(&conv_ok);
		if (conv_ok && value != -1.0) margins[i] = value;
	}
	m_printer->setPageMargins(
		QMarginsF(margins[0], margins[1], margins[2], margins[3]),
		QPageLayout::Millimeter);
#endif
	m_printer->setFullPage(
		settings.value("fullpage", "false").toString() == "true");

	settings.endGroup();
	settings.endGroup();
}

void ProjectPrintWindow::savePageSetupForCurrentPrinter()
{
	QSettings settings;
	QString printer_section = settingsSectionName(m_printer);

	while (!settings.group().isEmpty()) settings.endGroup();
	settings.beginGroup("printers");
	settings.beginGroup(printer_section);

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 1) // ### Qt 6: remove
	settings.setValue("orientation", m_printer -> orientation() == QPrinter::Portrait ? "portrait" : "landscape");
	settings.setValue("papersize", int(m_printer -> paperSize()));
	if (m_printer -> paperSize() == QPrinter::Custom) {
		QSizeF size = m_printer -> paperSize(QPrinter::Millimeter);
		settings.setValue("customwidthmm", size.width());
		settings.setValue("customheightmm", size.height());
	} else {
		settings.remove("customwidthmm");
		settings.remove("customheightmm");
	}
	qreal left, top, right, bottom;
	m_printer
		->getPageMargins(&left, &top, &right, &bottom, QPrinter::Millimeter);
	settings.setValue("marginleft", left);
	settings.setValue("margintop", top);
	settings.setValue("marginright", right);
	settings.setValue("marginbottom", bottom);
	settings.setValue("fullpage", m_printer->fullPage() ? "true" : "false");
	settings.endGroup();
	settings.endGroup();
	settings.sync();

#else
#	if TODO_LIST
#		pragma message("@TODO remove code for QT 6 or later")
#	endif
	qDebug() << "Help code for QT 6 or later";

	settings.setValue(
		"orientation",
		m_printer->pageLayout().orientation() == QPageLayout::Portrait ?
			"portrait" :
			"landscape");
	settings.setValue(
		"papersize",
		int(m_printer->pageLayout().pageSize().id()));
	if (m_printer->pageLayout().pageSize().id() == QPageSize::Custom)
	{
		QSizeF size =
			m_printer->pageLayout().pageSize().size(QPageSize::Millimeter);
		settings.setValue("customwidthmm", size.width());
		settings.setValue("customheightmm", size.height());
	}
	else
	{
		settings.remove("customwidthmm");
		settings.remove("customheightmm");
	}
	settings.setValue("marginleft", m_printer->pageLayout().margins().left());
	settings.setValue("margintop", m_printer->pageLayout().margins().top());
	settings.setValue("marginright", m_printer->pageLayout().margins().right());
	settings.setValue(
		"marginbottom",
		m_printer->pageLayout().margins().bottom());
	settings.setValue("fullpage", m_printer->fullPage() ? "true" : "false");
	settings.endGroup();
	settings.endGroup();
	settings.sync();
#endif
}

/**
 * @brief ProjectPrintWindow::saveReloadDiagramParameters
 * Save or restore the parameter of @diagram
 * @param diagram
 * @param options
 * @param save
 */
void ProjectPrintWindow::saveReloadDiagramParameters(Diagram *diagram, const ExportProperties &options, bool save)
{
	static ExportProperties state_exportProperties;

	if (save) {
		state_exportProperties = diagram -> applyProperties(options);
	} else {
		diagram -> applyProperties(state_exportProperties);
	}
}

QList<Diagram *> ProjectPrintWindow::selectedDiagram() const
{
	QList<Diagram *> selected_diagram;
	for (auto diagram : m_project->diagrams()) {
		auto cb = m_diagram_list_hash[diagram];
		if (cb && cb->isChecked()) {
			selected_diagram << diagram;
		}
	}

	return selected_diagram;
}

void ProjectPrintWindow::exportToPDF()
{
	auto file_name = QFileDialog::getSaveFileName(this, tr("Export as : "), m_printer->outputFileName(), tr("File (*.pdf)"));
	if (file_name.isEmpty()) {
		return;
	}
	m_printer->setOutputFileName(file_name);
	m_printer->setOutputFormat(QPrinter::PdfFormat);
	// Set printer DPI to match screen DPI to ensure font scaling matches preview
	QScreen *screen = QApplication::primaryScreen();
	qreal screen_dpi = screen ? screen->logicalDotsPerInch() : 96.0;
	m_printer->setResolution(qRound(screen_dpi));
	print();
}

void ProjectPrintWindow::on_m_draw_border_cb_clicked()          { m_preview->updatePreview(); }
void ProjectPrintWindow::on_m_draw_titleblock_cb_clicked()      { m_preview->updatePreview(); }
void ProjectPrintWindow::on_m_keep_conductor_color_cb_clicked() { m_preview->updatePreview(); }
void ProjectPrintWindow::on_m_draw_terminal_cb_clicked()        { m_preview->updatePreview(); }
void ProjectPrintWindow::on_m_fit_in_page_cb_clicked()          { m_preview->updatePreview(); }
void ProjectPrintWindow::on_m_use_full_page_cb_clicked()
{
	bool checked = ui->m_use_full_page_cb->isChecked();
	m_printer->setFullPage(checked);
	
	// Enable/disable margin input based on checkbox state
	ui->m_margin_label->setEnabled(checked);
	ui->m_margin_sp->setEnabled(checked);
	
	m_preview->updatePreview();
}

void ProjectPrintWindow::on_m_zoom_out_action_triggered() {
	m_preview->zoomOut(4.0/3.0);
}

void ProjectPrintWindow::on_m_zoom_in_action_triggered() {
	m_preview->zoomIn(4.0/3.0);
}

void ProjectPrintWindow::on_m_adjust_width_action_triggered() {
	m_preview->fitToWidth();
}

void ProjectPrintWindow::on_m_adjust_page_action_triggered() {
	m_preview->fitInView();
}

void ProjectPrintWindow::on_m_landscape_action_triggered() {
	m_preview->setLandscapeOrientation();
}

void ProjectPrintWindow::on_m_portrait_action_triggered() {
	m_preview->setPortraitOrientation();
}

void ProjectPrintWindow::on_m_first_page_action_triggered() {
	m_preview->setCurrentPage(1);
}

void ProjectPrintWindow::on_m_previous_page_action_triggered()
{
	auto previous_page = m_preview->currentPage() - 1;
	m_preview->setCurrentPage(std::max(previous_page, 0));
}

void ProjectPrintWindow::on_m_next_page_action_triggered()
{
	auto next_page = m_preview->currentPage() + 1;
	m_preview->setCurrentPage(std::min(next_page, m_preview->pageCount()));
}

void ProjectPrintWindow::on_m_last_page_action_triggered() {
	m_preview->setCurrentPage(m_preview->pageCount());
}

void ProjectPrintWindow::on_m_display_single_page_action_triggered() {
	m_preview->setSinglePageViewMode();
}

void ProjectPrintWindow::on_m_display_two_page_action_triggered() {
	m_preview->setFacingPagesViewMode();
}

void ProjectPrintWindow::on_m_display_all_page_action_triggered() {
	m_preview->setAllPagesViewMode();
}

void ProjectPrintWindow::on_m_page_setup_triggered()
{
	QPageSetupDialog d(m_printer, this);
	if (d.exec() == QDialog::Accepted) {
		m_preview->updatePreview();
	}
}

void ProjectPrintWindow::on_m_check_all_pb_clicked()
{
	for (auto cb : m_diagram_list_hash.values()) {
		cb->setChecked(true);
	}
	m_preview->updatePreview();
}

void ProjectPrintWindow::on_m_uncheck_all_clicked()
{
	for (auto cb : m_diagram_list_hash.values()) {
		cb->setChecked(false);
	}
	m_preview->updatePreview();
}

void ProjectPrintWindow::print()
{
	m_preview->print();
	savePageSetupForCurrentPrinter();
	this->close();
}

void ProjectPrintWindow::on_m_date_cb_userDateChanged(const QDate &date)
{
	auto index = ui->m_date_from_cb->currentIndex();
		// 0 = all date
		// 1 = from the date
		// 2 = at the date

	if (index) { on_m_uncheck_all_clicked();  }
	else       { on_m_check_all_pb_clicked(); }


	for (auto diagram : m_diagram_list_hash.keys())
	{
		auto diagram_date = diagram->border_and_titleblock.date();
		if ( (index == 1 && diagram_date >= date) ||
			 (index == 2 && diagram_date == date) )
			m_diagram_list_hash.value(diagram)->setChecked(true);
	}

	m_preview->updatePreview();
}

void ProjectPrintWindow::on_m_date_from_cb_currentIndexChanged(int index)
{
	Q_UNUSED(index)

	ui->m_date_cb->setEnabled(index);
	ui->m_apply_date_pb->setEnabled(index);
	on_m_date_cb_userDateChanged(ui->m_date_cb->date());

}

void ProjectPrintWindow::on_m_apply_date_pb_clicked() {
	on_m_date_cb_userDateChanged(ui->m_date_cb->date());
}
