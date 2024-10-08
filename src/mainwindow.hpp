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
	void generateQR() const noexcept;
	void saveQR() const noexcept;
	void show_Usage_menu() const noexcept;
	void show_About_menu() const noexcept;

private:
	Ui::MainWindow *ui;
};
