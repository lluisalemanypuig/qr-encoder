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

#include "resizablemessagebox.hpp"

// Qt includes
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QWidget>
#include <QLabel>

ResizableMessageBox::ResizableMessageBox(QWidget *parent) :
	QDialog(parent)
{

}

void ResizableMessageBox::show(const QString& title, const QString& message) noexcept {
	setWindowTitle(title);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);

	QVBoxLayout *layout = new QVBoxLayout(this);
	QLabel *label = new QLabel(message);
	label->setWordWrap(true);
	layout->addWidget(label);

	QPushButton *okButton = new QPushButton("OK");
	// Close dialog on button click
	connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
	layout->addWidget(okButton);
	setLayout(layout);
}
