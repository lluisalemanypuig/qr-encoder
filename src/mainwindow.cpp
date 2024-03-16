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

// C++ includes
#include <iostream>

// custom includes
#include "resizablemessagebox.hpp"
#include "qrcode/qrcodegen.hpp"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow() {
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

void MainWindow::generateQR() const noexcept {

	// string to encode
	const auto str = ui->textToEncode->text().toUtf8().toStdString();
	// ECC quality
	const auto ecc_str = ui->ECCComboBox->currentText().toUtf8().toStdString();

	const qrcodegen::QrCode::Ecc ecc = string_to_ECC(ecc_str);

	const qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(str.c_str(), ecc);
	const std::size_t s = qr.getSize() > 0 ? qr.getSize() : 1;

	std::vector<int> QR_matrix(s*s);
	std::size_t i = 0;
	for (std::size_t x = 0; x < s; ++x) {
		for (std::size_t y = 0; y < s; ++y) {
			const int value = qr.getModule(x, y);
			QR_matrix[i] = value;
			++i;
		}
	}

	ui->QRRenderArea->set_QR_code(std::move(QR_matrix), s);
	ui->QRRenderArea->update();
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
		"4. Finally, click on 'Save QR Code'.\n"
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
		"Distributed under GNU Affero GPL.\n"
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
