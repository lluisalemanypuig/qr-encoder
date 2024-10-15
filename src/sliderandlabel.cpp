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

#include "sliderandlabel.hpp"

// C++ includes
#if defined DEBUG
#include <cassert>
#endif
#include <sstream>

// Qt includes
#include <QSlider>
#include <QLabel>

std::string double_to_string(const double d) noexcept {
	std::ostringstream out;
	out.precision(2);
	out << std::fixed << d;
	return std::move(out).str();
}

void SliderAndLabel::update_label() noexcept {
#if defined DEBUG
	assert(m_label != nullptr);
#endif

	const int val = value();
	const double percent = (val*100.0)/m_value_at_100;
	m_label->setText(QString::fromStdString(double_to_string(percent) + "%"));
}
