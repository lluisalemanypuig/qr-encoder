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

#include "qrrenderer.hpp"

#undef __PAINT_CONTROL_SHAPES

// Qt includes
#include <QGraphicsRectItem>

QRrenderer::QRrenderer(QWidget *parent)
	: QGraphicsView(parent)
{
	setScene(&m_scene);
}

void QRrenderer::set_QR_code
(std::vector<int>&& QR_matrix, std::size_t QR_size)
noexcept
{
	m_QR_matrix = std::move(QR_matrix);
	m_QR_size = QR_size;
}

void QRrenderer::redimensionQR(int value) noexcept {
	m_redim = value/1000.0;
	update();
}

void QRrenderer::update_outer_square() noexcept {
	// dimensions of our drawing area
	const double width = size().width();
	const double height = size().height();

	const double width2 = width/2.0;
	const double height2 = height/2.0;

	if (width > height) {
		m_outer_square_x0 = width2 - height2;
		m_outer_square_y0 = 0;
		m_outer_square_x1 = width2 + height2;
		m_outer_square_y1 = height;
	}
	else {
		m_outer_square_x0 = 0;
		m_outer_square_y0 = height2 - width2;
		m_outer_square_x1 = width;
		m_outer_square_y1 = height2 + width2;
	}
	m_outer_square_size = m_outer_square_x1 - m_outer_square_x0;

	// the drawing area is actually the outer square
	m_drawing_area.setX(m_outer_square_x0);
	m_drawing_area.setY(m_outer_square_y0);
	m_drawing_area.setWidth(m_outer_square_size);
	m_drawing_area.setHeight(m_outer_square_size);

#if defined __PAINT_CONTROL_SHAPES
	QGraphicsRectItem *outer_square = new QGraphicsRectItem(QRectF(
		m_outer_square_x0,   m_outer_square_y0,
		m_outer_square_size, m_outer_square_size
	));
	outer_square->setPen(QPen(Qt::blue));
	m_scene.addItem(outer_square);
#endif
}

void QRrenderer::update_inner_square() noexcept {
	m_inner_square_size = (1.0 - m_redim)*m_outer_square_size;

	const double x_midpoint = (m_outer_square_x0 + m_outer_square_x1)/2;
	const double y_midpoint = (m_outer_square_y0 + m_outer_square_y1)/2;

	m_inner_square_x0 = x_midpoint - m_inner_square_size/2;
	m_inner_square_x1 = x_midpoint + m_inner_square_size/2;
	m_inner_square_y0 = y_midpoint - m_inner_square_size/2;
	m_inner_square_y1 = y_midpoint + m_inner_square_size/2;

#if defined __PAINT_CONTROL_SHAPES
	QGraphicsRectItem *inner_square = new QGraphicsRectItem(QRectF(
		m_inner_square_x0,   m_inner_square_y0,
		m_inner_square_size, m_inner_square_size
	));
	inner_square->setPen(QPen(Qt::red));
	m_scene.addItem(inner_square);
#endif
}

void QRrenderer::update() noexcept {
	if (m_QR_size == 0) { return; }

	// clean up drawing area
	m_scene.clear();
	setScene(&m_scene);

	// calculate the coordinates of the outer and inner drawing squares
	update_outer_square();
	update_inner_square();

	// the size of a cell of the QR code
	const double QR_cell_size = m_inner_square_size/m_QR_size;

	// draw the QR
	std::size_t i = 0;
	for (std::size_t x = 0; x < m_QR_size; ++x) {
		for (std::size_t y = 0; y < m_QR_size; ++y) {

			if (m_QR_matrix[i] == 1) {
				const double x0 = m_inner_square_x0 + x*QR_cell_size;
				const double y0 = m_inner_square_y0 + y*QR_cell_size;
				const double width = QR_cell_size;
				const double height = QR_cell_size;

				// the rectangle for the current QR cell
				const QRectF QR_cell(x0, y0, width, height);

				// paint the current cell
				QGraphicsRectItem *filledRectItem = new QGraphicsRectItem(QR_cell);
				filledRectItem->setBrush(Qt::black);

				m_scene.addItem(filledRectItem);
			}
			++i;
		}
	}
}
