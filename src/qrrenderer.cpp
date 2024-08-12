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

#include "qrrenderer.hpp"

#undef __PAINT_CONTROL_SHAPES

// Qt includes
#include <QGraphicsRectItem>

QRrenderer::QRrenderer(QWidget *parent) :
	QGraphicsView(parent),
	m_QR_matrix(qrcodegen::QrCode::encodeText("", qrcodegen::QrCode::Ecc::LOW))
{
	setScene(&m_scene);
	setAlignment(Qt::AlignCenter);

	m_fill_color = Qt::GlobalColor::black;
	m_border_color = Qt::GlobalColor::black;
	m_shape = shapes::squares;
}

void QRrenderer::set_fill_color(int color) noexcept {
	m_fill_color = static_cast<Qt::GlobalColor>(color + 2);
	update();
}

void QRrenderer::set_border_color(int color) noexcept {
	m_border_color = static_cast<Qt::GlobalColor>(color + 2);
	update();
}

void QRrenderer::set_shape(int shape) noexcept {
	m_shape = static_cast<shapes>(shape);
	update();
}

// -----------------------------------------------------------------------------

void QRrenderer::set_QR_code(qrcodegen::QrCode&& QR_matrix)
noexcept
{
	m_QR_matrix = std::move(QR_matrix);
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
		m_drawing_area.x(),   m_drawing_area.y(),
		m_drawing_area.width(), m_drawing_area.height()
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

Qt::GlobalColor random_color(int x0, int y0) noexcept {
	return static_cast<Qt::GlobalColor>(x0*y0/(x0 + y0)%16 + 2);
}

void QRrenderer::draw_control_points(
	const std::vector<std::pair<int,int>>& points,
	int outer_x, int outer_y,
	int outer_w, int outer_h,
	int blank_x, int blank_y,
	int blank_w, int blank_h,
	int inner_x, int inner_y,
	int inner_w, int inner_h
)
noexcept
{
	const std::size_t QR_size = m_QR_matrix.getSize();
	const double QR_cell_size = m_inner_square_size/QR_size;

	for (const auto& [x, y] : points) {
		const double x0 = m_inner_square_x0 + x*QR_cell_size;
		const double y0 = m_inner_square_y0 + y*QR_cell_size;
		const double width = QR_cell_size;
		const double height = QR_cell_size;

		// erase superfluous circles
		QGraphicsRectItem *rect = new QGraphicsRectItem(
			x0 - outer_x*width,
			y0 - outer_y*height,
			outer_w*width,
			outer_h*height
		);
		rect->setBrush(Qt::GlobalColor::white);
		rect->setPen(QColor{Qt::GlobalColor::white});
		m_scene.addItem(rect);

		{
		QGraphicsEllipseItem *circle = new QGraphicsEllipseItem(
			x0 - outer_x*width,
			y0 - outer_y*height,
			outer_w*width,
			outer_h*height
		);
		circle->setBrush(m_fill_color);
		circle->setPen(QColor{m_border_color});
		m_scene.addItem(circle);
		}

		{
		QGraphicsEllipseItem *circle = new QGraphicsEllipseItem(
			x0 - blank_x*width,
			y0 - blank_y*height,
			blank_w*width,
			blank_h*height
		);
		circle->setBrush(Qt::GlobalColor::white);
		circle->setPen(QColor{Qt::GlobalColor::white});
		m_scene.addItem(circle);
		}

		{
		QGraphicsEllipseItem *circle = new QGraphicsEllipseItem(
			x0 - inner_x*width,
			y0 - inner_y*height,
			inner_w*width,
			inner_h*height
		);
		circle->setBrush(m_fill_color);
		circle->setPen(QColor{m_border_color});
		m_scene.addItem(circle);
		}
	}
}

void QRrenderer::draw_circles() noexcept {
	const std::size_t QR_size = m_QR_matrix.getSize();
	const double QR_cell_size = m_inner_square_size/QR_size;

	// draw 'normal' circles first
	for (std::size_t x = 0; x < QR_size; ++x) {
		for (std::size_t y = 0; y < QR_size; ++y) {

			if (m_QR_matrix.getModule(x, y)) {
				const double x0 = m_inner_square_x0 + x*QR_cell_size;
				const double y0 = m_inner_square_y0 + y*QR_cell_size;
				const double width = QR_cell_size;
				const double height = QR_cell_size;

				// paint the current cell
				QGraphicsEllipseItem *circle =
					new QGraphicsEllipseItem(x0, y0, width, height);
				circle->setBrush(m_fill_color);
				circle->setPen(QColor{m_border_color});
				m_scene.addItem(circle);
			}
		}
	}

	draw_control_points(
		m_QR_matrix.alignment_centers,
		2, 2, 5, 5,
		1, 1, 3, 3,
		0, 0, 1, 1
	);
	draw_control_points(
		m_QR_matrix.finder_centers,
		3, 3, 7, 7,
		2, 2, 5, 5,
		1, 1, 3, 3
	);
}

void QRrenderer::draw_squares() noexcept {
	const std::size_t QR_size = m_QR_matrix.getSize();
	const double QR_cell_size = m_inner_square_size/QR_size;

	// draw the QR
	for (std::size_t x = 0; x < QR_size; ++x) {
		for (std::size_t y = 0; y < QR_size; ++y) {

			if (m_QR_matrix.getModule(x, y)) {
				const double x0 = m_inner_square_x0 + x*QR_cell_size;
				const double y0 = m_inner_square_y0 + y*QR_cell_size;
				const double width = QR_cell_size;
				const double height = QR_cell_size;

				// paint the current cell
				QGraphicsRectItem *rect =
					new QGraphicsRectItem(x0, y0, width, height);
				rect->setBrush(m_fill_color);
				rect->setPen(QColor{m_border_color});
				m_scene.addItem(rect);
			}
		}
	}
}

void QRrenderer::update() noexcept {
	const std::size_t QR_size = m_QR_matrix.getSize();

	if (QR_size == 0) { return; }

	// clean up drawing area
	m_scene.clear();
	setScene(&m_scene);

	// calculate the coordinates of the outer and inner drawing squares
	update_outer_square();
	update_inner_square();

	if (m_shape == shapes::squares) {
		draw_squares();
	}
	else if (m_shape == shapes::circles) {
		draw_circles();
	}

	setSceneRect(m_drawing_area);
	centerOn(m_drawing_area.center());
}
