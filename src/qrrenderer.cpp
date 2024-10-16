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

// C++ includes
#include <iostream>
#include <cmath>

// Qt includes
#include <QGraphicsRectItem>
#include <QPixmap>

Qt::GlobalColor random_color(const int x0, const int y0) noexcept {
	return static_cast<Qt::GlobalColor>(x0*y0/(x0 + y0)%16 + 2);
}

static constexpr
bool is_alignment_pattern_point
(const std::size_t x, const std::size_t y, const std::size_t S)
noexcept
{
	return
		(x <= 7 and y <= 7) or
		(x >= S - 7 and y <= 7) or
		(x <= 7 and y >= S - 7);
}

// -----------------------------------------------------------------------------

/* PUBLIC */

QRrenderer::QRrenderer(QWidget *parent) noexcept :
	QGraphicsView(parent),
	m_QR_matrix(qrcodegen::QrCode::encodeText("", qrcodegen::QrCode::Ecc::LOW))
{
	setScene(&m_scene);
	setAlignment(Qt::AlignCenter);
}

void QRrenderer::set_QR_code(qrcodegen::QrCode&& QR_matrix) noexcept {
	m_QR_matrix = std::move(QR_matrix);
	update();
}

void QRrenderer::load_QR_image(const QString& path) noexcept {
	if (m_QR_image != nullptr) {
		delete m_QR_image;
	}
	m_QR_image = new QPixmap(path);
	if (m_QR_image == nullptr) {
		std::cerr
			<< "Error! Image "
			<< "'" << path.toUtf8().toStdString() << "'"
			<< " could not be loaded."
			<< std::endl;
	}

	update();
}

void QRrenderer::remove_QR_image() noexcept {
	delete m_QR_image;
	m_QR_image = nullptr;
	update();
}

/* PUBLIC SLOTS */

void QRrenderer::set_point_fill_color(const int color) noexcept {
	m_point_fill = static_cast<Qt::GlobalColor>(color + 2);
	update();
}

void QRrenderer::set_point_border_color(const int color) noexcept {
	m_point_border = static_cast<Qt::GlobalColor>(color + 2);
	update();
}

void QRrenderer::set_point_shape(const int shape) noexcept {
	m_points = static_cast<shapes>(shape + 1);
	update();
}

void QRrenderer::set_alignment_pattern_shape(const int shape) noexcept {
	m_alignment_patterns = static_cast<shapes>(shape + 1);
	update();
}

void QRrenderer::set_image_background_fill_color(const int color) noexcept {
	m_image_background_fill = static_cast<Qt::GlobalColor>(color + 2);
	update();
}

void QRrenderer::set_image_background_border_color(const int color) noexcept {
	m_image_background_border = static_cast<Qt::GlobalColor>(color + 2);
	update();
}

void QRrenderer::resize_QR(const int value) noexcept {
	m_redim = value/1000.0;
	update();
}

void QRrenderer::resize_QR_image(const int value) noexcept {
	m_QR_image_scale = value/1000.0;
	if (m_QR_image != nullptr) {
		update();
	}
}

void QRrenderer::set_QR_image_background_shape(const int shape) noexcept {
	m_QR_image_background_shape = static_cast<shapes>(shape);
	if (m_QR_image != nullptr) {
		update();
	}
}

void QRrenderer::resize_QR_image_background(const int value) noexcept {
	m_QR_image_background_scale = value/1000.0;
	if (m_QR_image != nullptr) {
		update();
	}
}

/* PRIVATE */

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
	m_inner_square_size = m_redim*m_outer_square_size;

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

void QRrenderer::draw_circle
(
	const double x, const double y, const double radius,
	const QColor& fill_color,
	const QColor& pen_color
)
noexcept
{
	// paint the current cell
	QGraphicsEllipseItem *circle = new QGraphicsEllipseItem(x, y, radius, radius);
	circle->setBrush(fill_color);
	circle->setPen(pen_color);
	m_scene.addItem(circle);
}

void QRrenderer::draw_rectangle
(
	const double x, const double y,
	const double width, const double height,
	const QColor& fill_color,
	const QColor& pen_color
)
noexcept
{
	// paint the current cell
	QGraphicsRectItem *rect = new QGraphicsRectItem(x, y, width, height);
	rect->setBrush(fill_color);
	rect->setPen(pen_color);
	m_scene.addItem(rect);
}

void QRrenderer::draw_alignment_patterns_round(
	const std::vector<std::pair<int,int>>& points,
	const int outer_x, const int outer_y, const int outer_radius,
	const int blank_x, const int blank_y, const int blank_radius,
	const int inner_x, const int inner_y, const int inner_radius
)
noexcept
{
	const std::size_t QR_size = m_QR_matrix.getSize();
	const double QR_cell_size = m_inner_square_size/QR_size;

	for (const auto& [x, y] : points) {
		const double x0 = m_inner_square_x0 + x*QR_cell_size;
		const double y0 = m_inner_square_y0 + y*QR_cell_size;

		draw_circle(
			x0 - outer_x*QR_cell_size,
			y0 - outer_y*QR_cell_size,
			outer_radius*QR_cell_size,
			m_point_fill, m_point_border
		);

		draw_circle(
			x0 - blank_x*QR_cell_size,
			y0 - blank_y*QR_cell_size,
			blank_radius*QR_cell_size,
			Qt::GlobalColor::white, Qt::GlobalColor::white
		);

		draw_circle(
			x0 - inner_x*QR_cell_size,
			y0 - inner_y*QR_cell_size,
			inner_radius*QR_cell_size,
			m_point_fill, m_point_border
		);
	}
}

void QRrenderer::draw_alignment_patterns_square() noexcept {
	const std::size_t QR_size = m_QR_matrix.getSize();
	const double QR_cell_size = m_inner_square_size/QR_size;

	for (std::size_t x = 0; x <= 7; ++x) {
		for (std::size_t y = 0; y <= 7; ++y) {
			if (m_QR_matrix.getModule(x, y)) {
				draw_rectangle(
					m_inner_square_x0 + x*QR_cell_size,
					m_inner_square_y0 + y*QR_cell_size,
					QR_cell_size, QR_cell_size,
					m_point_fill, m_point_border
				);
			}
		}

		for (std::size_t y = QR_size - 7; y < QR_size; ++y) {
			if (m_QR_matrix.getModule(x, y)) {
				draw_rectangle(
					m_inner_square_x0 + x*QR_cell_size,
					m_inner_square_y0 + y*QR_cell_size,
					QR_cell_size, QR_cell_size,
					m_point_fill, m_point_border
				);
			}
		}
	}

	for (std::size_t x = QR_size - 7; x < QR_size; ++x) {
		for (std::size_t y = 0; y <= 7; ++y) {
			if (m_QR_matrix.getModule(x, y)) {
				draw_rectangle(
					m_inner_square_x0 + x*QR_cell_size,
					m_inner_square_y0 + y*QR_cell_size,
					QR_cell_size, QR_cell_size,
					m_point_fill, m_point_border
				);
			}
		}
	}
}

void QRrenderer::draw_alignment_patterns() noexcept {
	if (m_alignment_patterns == shapes::circles) {
		draw_alignment_patterns_round(
			m_QR_matrix.finder_centers,
			3, 3, 7,
			2, 2, 5,
			1, 1, 3
		);
	}
	else if (m_alignment_patterns == shapes::squares) {
		draw_alignment_patterns_square();
	}
}

void QRrenderer::draw_points_circle() noexcept {
	const std::size_t QR_size = m_QR_matrix.getSize();
	const double QR_cell_size = m_inner_square_size/QR_size;

	for (std::size_t x = 0; x < QR_size; ++x) {
		for (std::size_t y = 0; y < QR_size; ++y) {

			if (is_alignment_pattern_point(x, y, QR_size)) { continue; }

			if (m_QR_matrix.getModule(x, y)) {
				draw_circle(
					m_inner_square_x0 + x*QR_cell_size,
					m_inner_square_y0 + y*QR_cell_size,
					QR_cell_size,
					m_point_fill, m_point_border
				);
			}
		}
	}
}

void QRrenderer::draw_points_square() noexcept {
	const std::size_t QR_size = m_QR_matrix.getSize();
	const double QR_cell_size = m_inner_square_size/QR_size;

	for (std::size_t x = 0; x < QR_size; ++x) {
		for (std::size_t y = 0; y < QR_size; ++y) {

			if (is_alignment_pattern_point(x, y, QR_size)) { continue; }

			if (m_QR_matrix.getModule(x, y)) {
				draw_rectangle(
					m_inner_square_x0 + x*QR_cell_size,
					m_inner_square_y0 + y*QR_cell_size,
					QR_cell_size, QR_cell_size,
					m_point_fill, m_point_border
				);
			}
		}
	}
}

void QRrenderer::draw_points() noexcept {
	if (m_points == shapes::circles) {
		draw_points_circle();
	}
	else if (m_points == shapes::squares) {
		draw_points_square();
	}
}

void QRrenderer::draw_QR_image_background() noexcept {
	const int width = m_QR_image->width();
	const int height = m_QR_image->height();

	const double cx = m_inner_square_x0 + m_inner_square_size/2;
	const double cy = m_inner_square_y0 + m_inner_square_size/2;

	const double w = m_redim*m_QR_image_background_scale*width;
	const double h = m_redim*m_QR_image_background_scale*height;

	if (m_QR_image_background_shape == shapes::squares) {

		draw_rectangle(
			cx - w/2,
			cy - h/2,
			w, h,
			m_image_background_fill, m_image_background_border
		);
	}
	else if (m_QR_image_background_shape == shapes::circles) {
		const double r = std::sqrt(w*w + h*h)/2.0;

		draw_circle(
			cx - r,
			cy - r,
			2*r,
			m_image_background_fill, m_image_background_border
		);
	}
}

void QRrenderer::draw_QR_image() noexcept {
	QGraphicsPixmapItem *pixmap_item = new QGraphicsPixmapItem(*m_QR_image);

	const double width = m_QR_image->width();
	const double height = m_QR_image->height();
	pixmap_item->setOffset(
		m_inner_square_x0 + m_inner_square_size/2 - width/2,
		m_inner_square_y0 + m_inner_square_size/2 - height/2
	);

	pixmap_item->setTransformOriginPoint(pixmap_item->boundingRect().center());
	pixmap_item->setScale(m_redim*m_QR_image_scale);

	m_scene.addItem(pixmap_item);
}

void QRrenderer::update() noexcept {
	if (m_QR_matrix.getSize() == 0) { return; }

	// clean up drawing area
	m_scene.clear();

	// calculate the coordinates of the outer and inner drawing squares
	update_outer_square();
	update_inner_square();

	draw_points();
	draw_alignment_patterns();

	if (m_QR_image != nullptr) {
		draw_QR_image_background();
		draw_QR_image();
	}

	setSceneRect(m_drawing_area);
	centerOn(m_drawing_area.center());
}
