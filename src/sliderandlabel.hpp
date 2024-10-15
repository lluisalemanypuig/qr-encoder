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
#include <QSlider>
#include <QLabel>

class SliderAndLabel : public QSlider {
	Q_OBJECT

public:
	explicit SliderAndLabel(QWidget *parent = nullptr) noexcept :
		QSlider(parent)
	{
	}

	// The function does *not* take ownership of the pointer.
	void set_label(QLabel *l) noexcept {
		m_label = l;
	}

	void set_value_at_100(const int v) noexcept {
		m_value_at_100 = v;
	}

public slots:
	void update_label() noexcept;

private:
	QLabel *m_label;
	int m_value_at_100;
};
