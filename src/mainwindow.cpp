/*********************************************************************
 *
 * QR Encoder -- A small gui to generate png images of QR codes
 * Copyright (C) 2024  Lluís Alemany Puig
 *
 * https://github.com/lluisalemanypuig/qr-encoder
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *********************************************************************/

// custom includes
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

// Qt includes
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QSpinBox>

// C++ includes
#if defined DEBUG
#include <cassert>
#endif
#include <iostream>

// custom includes
#include "resizablemessagebox.hpp"
#include "qrcode/qrcodegen.hpp"

/* PUBLIC */

MainWindow::MainWindow(QWidget *parent) noexcept :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->redimQR->set_label(ui->QRSizeLabel);
	ui->redimQR->set_value_at_100(ui->redimQR->maximum());

	ui->imageSizeSlider->set_label(ui->imageSizeLabel);
	ui->imageSizeSlider->set_value_at_100(1000);

	ui->imageBackgroundSizeSlider->set_label(ui->imageBackgroundSizeLabel);
	ui->imageBackgroundSizeSlider->set_value_at_100(1000);
}

MainWindow::~MainWindow() noexcept {
	delete ui;
}

qrcodegen::QrCode::Ecc string_to_ECC(const std::string& s) noexcept {
	const QString ss = QString::fromStdString(s);
	if (ss == QObject::tr("Low")) { return qrcodegen::QrCode::Ecc::LOW; }
	if (ss == QObject::tr("Medium")) { return qrcodegen::QrCode::Ecc::MEDIUM; }
	if (ss == QObject::tr("Quartile")) { return qrcodegen::QrCode::Ecc::QUARTILE; }
	if (ss == QObject::tr("High")) { return qrcodegen::QrCode::Ecc::HIGH; }
	std::cerr << "Wrong input string '" << s << '\'' << std::endl;
	return qrcodegen::QrCode::Ecc::HIGH;
}

void MainWindow::generateQR() noexcept {

	// string to encode
	const auto str = ui->textToEncode->text().toUtf8().toStdString();
	// ECC quality
	const auto ecc_str = ui->ECCComboBox->currentText().toUtf8().toStdString();

	const qrcodegen::QrCode::Ecc ecc = string_to_ECC(ecc_str);

	qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(str.c_str(), ecc);
	ui->QRRenderArea->set_QR_code(std::move(qr));

	if (not m_has_QR) {
		// enable "load image" tab
		QWidget *tab = ui->editQRTabWidget->widget(1);
#if defined DEBUG
		assert(tab != nullptr);
#endif
		tab->setEnabled(true);

		QPushButton *loadImageButton = tab->findChild<QPushButton *>("loadQRImageButton");
#if defined DEBUG
		assert(loadImageButton != nullptr);
#endif
		loadImageButton->setEnabled(true);

		QPushButton *removeImageButton = tab->findChild<QPushButton *>("removeQRImageButton");
#if defined DEBUG
		assert(removeImageButton != nullptr);
#endif
		removeImageButton->setEnabled(true);

		enable_QR_image_resize(false);
		enable_QR_image_background_shape(false);
		enable_QR_image_background_color(false);
		enable_QR_image_background_size(false);
	}

	m_has_QR = true;
}

void MainWindow::loadQRImage() noexcept {
	QString file_name = QFileDialog::getOpenFileName(
		this,
		tr("Choose image"),
		"",
		tr("Images (*.png *.jpg *.jpeg *.svg *.bmp)")
	);

	if (file_name.size() > 0) {
		enable_QR_image_resize(true);
		enable_QR_image_background_shape(true);
		enable_QR_image_background_color(true);
		enable_QR_image_background_size(true);
		ui->QRRenderArea->load_QR_image(file_name);
	}
}

void MainWindow::removeQRImage() noexcept {
	enable_QR_image_resize(false);
	enable_QR_image_background_shape(false);
	enable_QR_image_background_color(false);
	enable_QR_image_background_size(false);

	ui->QRRenderArea->remove_QR_image();
}

void MainWindow::backgroundShapeChosen(const int shape) noexcept {
	if (shape == 0) {
		enable_QR_image_background_color(false);
		enable_QR_image_background_size(false);
	}
	else {
		enable_QR_image_background_color(true);
		enable_QR_image_background_size(true);
	}
}

void MainWindow::saveQR() const noexcept {
	QFileDialog file_dialog;
	QString save = file_dialog.getSaveFileName(nullptr, "Save QR Code");
	const int compression = 100 - ui->QualitySpinBox->value();

	const QRect& drawing_area = ui->QRRenderArea->get_drawing_area();
	if (not save.contains(".png")) {
		save += ".png";
	}
	ui->QRRenderArea->grab(drawing_area).save(save, "PNG", compression);
}

void MainWindow::show_Usage_menu() const noexcept {
	ResizableMessageBox msgBox;
	msgBox.show(
		QObject::tr("How to use QR-Encoder"),
		QObject::tr("Using QR-Encode is easy! Just follow these steps:\n"
		"1. Write the tetx you want to encode (a sentence, a url, random text, ...) "
		"into the 'Text' text box.\n"
		"2. Click on 'Generate QR' to generate the QR code.\n"
		"3. Resize the code with the slider to an appropriate size.\n"
		"4. Choose the color you like the most and a shape.\n"
		"5. Finally, click on 'Save QR Code'.\n"
		"\n"
		"Other options:\n"
		"QR-Encoder offers a simple way to add an image at the center of your QR, "
		"with a background shape whose color and size you can change. After generating "
		"the QR code, open the 'Load Image' tab and load an image to be displayed "
		"on top of your QR. Change its size using the slider. Add a background by "
		"selecting a shape; after that, you will be able to select its color and size."
		"\n"
		"Advanced options:\n"
		"1. Quality of the QR code. Make sure your code will be readable (even when "
		"it is partially occluded) by setting ECC to 'High'. Or if the QR looks too "
		"cluttered, though, set it to 'Low' or another value. Click 'Generate QR' "
		"everytime the value of ECC changes.\n"
		"\n"
		"2. Size of the png file. The QR code will be saved in a png file; you can "
		"set the level of compression of the image file by changing the value next "
		"to 'PNG Compression'. It is a percentage (%) and is currently set at the "
		"maximum value.")
	);

	// Execute the message box
	msgBox.exec();
}

void MainWindow::show_About_menu() const noexcept {
	ResizableMessageBox msgBox;
	msgBox.show(
		QObject::tr("About QR-Encoder"),
		QObject::tr("Developed and maintained by Lluís Alemany Puig.\n"
		"\n"
		"Contact:\n"
		"\n"
		"\tGithub: https://github.com/lluisalemanypuig/qr-encoder\n"
		"\n"
		"Version yy.mm -- Distributed under GNU Affero GPL.\n"
		"\n"
		"Feel free to distribute and modify the program (while complying with the "
		"terms and conditions imposed by the licence).\n"
		"\n"
		"This program uses Nayuki's QR Code generator, a third-party library (read "
		"more about it in the Github page of this program).")
	);

	// Execute the message box
	msgBox.exec();
}

/* PRIVATE */

void MainWindow::enable_QR_image_resize(const bool v) noexcept {
	ui->imageSizeSlider->setEnabled(v);
	ui->imageSizeLabel->setEnabled(v);
}

void MainWindow::enable_QR_image_background_shape(const bool v) noexcept {
	ui->imageBackgroundShapeComboBox->setEnabled(v);
}

void MainWindow::enable_QR_image_background_color(const bool v) noexcept {
	const int cur_index = ui->imageBackgroundShapeComboBox->currentIndex();

	ui->imageBackgroundFillColorComboBox->setEnabled(v and (cur_index > 0));
	ui->imageBackgroundBorderColorComboBox->setEnabled(v and (cur_index > 0));
}

void MainWindow::enable_QR_image_background_size(const bool v) noexcept {
	const int cur_index = ui->imageBackgroundShapeComboBox->currentIndex();

	ui->imageBackgroundSizeSlider->setEnabled(v and (cur_index > 0));
	ui->imageBackgroundSizeLabel->setEnabled(v and (cur_index > 0));
}

void MainWindow::resizeEvent(QResizeEvent *) {
	ui->QRRenderArea->window_was_resized();
}
