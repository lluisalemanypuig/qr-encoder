/**
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
 */

// custom includes
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "qrcode/qrcodegen.hpp"

// Qt includes
#include <QFileDialog>
#include <QPainter>

// C++ includes
#include <iostream>

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
	if (s == "Low") { return qrcodegen::QrCode::Ecc::LOW; }
	if (s == "Medium") { return qrcodegen::QrCode::Ecc::MEDIUM; }
	if (s == "Quartile") { return qrcodegen::QrCode::Ecc::QUARTILE; }
	if (s == "High") { return qrcodegen::QrCode::Ecc::HIGH; }
	std::cout << "Wrong input string '" << s << '\'' << std::endl;
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
