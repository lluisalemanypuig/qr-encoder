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

#pragma once

// Qt includes
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr) noexcept;
	~MainWindow() noexcept;

public slots:
	void generateQR() noexcept;
	void loadQRImage() noexcept;
	void removeQRImage() noexcept;
	void backgroundShapeChosen(const int shape) noexcept;

	void saveQR() const noexcept;
	void show_Usage_menu() const noexcept;
	void show_About_menu() const noexcept;

private:
	void enable_QR_image_resize(const bool v) noexcept;
	void enable_QR_image_background_shape(const bool v) noexcept;
	void enable_QR_image_background_color(const bool v) noexcept;
	void enable_QR_image_background_size(const bool v) noexcept;

	void resizeEvent(QResizeEvent *event) override;

private:
	bool m_has_QR = false;

	// Qt GUI
	Ui::MainWindow *ui;
};
