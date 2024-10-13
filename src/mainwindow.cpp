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

	enable_buttons_slider_load_image_tab(false);

	ui->QRRenderArea->update();
}

void MainWindow::loadQRImage() noexcept {
	enable_buttons_slider_load_image_tab(true);

	QString file_name = QFileDialog::getOpenFileName(
		this,
		tr("Choose image"),
		"",
		tr("Images (*.png *.jpg *.jpeg *.svg *.bmp)")
	);

	ui->QRRenderArea->load_QR_image(file_name);
}

void MainWindow::removeQRImage() noexcept {
	enable_buttons_slider_load_image_tab(false);
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

void MainWindow::enable_buttons_slider_load_image_tab(const bool v) noexcept {
	QWidget *tab = ui->editQRTabWidget->widget(1);
#if defined DEBUG
	assert(tab != nullptr);
#endif

	QSlider *imageSizeSlider = tab->findChild<QSlider *>("imageSizeSlider");
#if defined DEBUG
	assert(imageSizeSlider != nullptr);
#endif
	imageSizeSlider->setEnabled(v);

	QSpinBox *imageSizeSpinner = tab->findChild<QSpinBox *>("imageSizeSpinBox");
#if defined DEBUG
	assert(imageSizeSpinner != nullptr);
#endif
	imageSizeSpinner->setEnabled(v);

	QComboBox *backgroundShapeComboBox = tab->findChild<QComboBox *>("backgroundShapeComboBox");
#if defined DEBUG
	assert(backgroundShapeComboBox != nullptr);
#endif
	backgroundShapeComboBox->setEnabled(v);

	QSlider *imageBackgroundSizeSlider = tab->findChild<QSlider *>("imageBackgroundSizeSlider");
#if defined DEBUG
	assert(imageBackgroundSizeSlider != nullptr);
#endif
	imageBackgroundSizeSlider->setEnabled(v);

	QSpinBox *imageBackgroundSizeSpinBox = tab->findChild<QSpinBox *>("imageBackgroundSizeSpinBox");
#if defined DEBUG
	assert(imageBackgroundSizeSpinBox != nullptr);
#endif
	imageBackgroundSizeSpinBox->setEnabled(v);
}
