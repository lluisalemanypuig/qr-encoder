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

#define __PAINT_CONTROL_SHAPES

// C++ includes
#include <iostream>
#include <cmath>

// Qt includes
#include <QGraphicsRectItem>
#include <QPixmap>

[[nodiscard]]
inline Qt::GlobalColor random_color(const int x0, const int y0) noexcept {
	return static_cast<Qt::GlobalColor>(x0*y0/(x0 + y0)%16 + 2);
}

[[nodiscard]]
static constexpr inline
bool is_point_within_alignment_pattern
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

	m_background_alpha = 255;

	setStyleSheet("background:transparent");
	setBackgroundBrush(QBrush(QColor(255,255,255, m_background_alpha)));

	m_scene_to_be_updated = false;
	m_transformations_to_be_updated = false;
}

void QRrenderer::set_QR_code(qrcodegen::QrCode&& QR_matrix) noexcept {
	m_QR_matrix = std::move(QR_matrix);
	m_scene_to_be_updated = true;
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
	m_scene_to_be_updated = true;
	update();
}

void QRrenderer::set_point_border_color(const int color) noexcept {
	m_point_border = static_cast<Qt::GlobalColor>(color + 2);
	m_scene_to_be_updated = true;
	update();
}

void QRrenderer::set_point_shape(const int shape) noexcept {
	m_points = static_cast<shapes>(shape + 1);
	m_scene_to_be_updated = true;
	update();
}

void QRrenderer::set_alignment_pattern_shape(const int shape) noexcept {
	m_alignment_patterns = static_cast<shapes>(shape + 1);
	m_scene_to_be_updated = true;
	update();
}

void QRrenderer::set_image_background_fill_color(const int color) noexcept {
	m_image_background_fill = static_cast<Qt::GlobalColor>(color + 2);
	m_scene_to_be_updated = true;
	update();
}

void QRrenderer::set_image_background_border_color(const int color) noexcept {
	m_image_background_border = static_cast<Qt::GlobalColor>(color + 2);
	m_scene_to_be_updated = true;
	update();
}

void QRrenderer::resize_QR(const int value) noexcept {
	m_redim = value/1000.0;
	m_transformations_to_be_updated = true;
	update();
}

void QRrenderer::set_transparent_background(const bool alpha) noexcept {
	m_background_alpha = 255*(1 - alpha);
	setBackgroundBrush(QColor{255,255,255, m_background_alpha});
	m_scene_to_be_updated = true;
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
}

void QRrenderer::add_circle
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

void QRrenderer::add_rectangle
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

void QRrenderer::add_complementary_quarter_circle(
	const double x, const double y,
	const double radius,
	const double start,
	const QColor& fill_color,
	const QColor& pen_color
)
noexcept
{
	double quarter_circle_x = x + radius;
	double quarter_circle_y = y + radius;

	QPainterPath quarter_circle(QPointF{quarter_circle_x, quarter_circle_y});
	quarter_circle.arcTo(x, y, 2*radius, 2*radius, start, 90);

	QPainterPath complementary;
	complementary.addRect(quarter_circle.boundingRect());
	complementary = complementary.subtracted(quarter_circle);

	QGraphicsPathItem *item = new QGraphicsPathItem(complementary);
	item->setBrush(fill_color);
	item->setPen(pen_color);
	m_scene.addItem(item);
}

void QRrenderer::add_quarter_circle(
	const double x, const double y,
	const double radius,
	const double start,
	const QColor& fill_color,
	const QColor& pen_color
)
noexcept
{
	QPainterPath quarter_circle(QPointF{x + radius, y + radius});
	quarter_circle.arcTo(x, y, 2*radius, 2*radius, start, 90);

	QGraphicsPathItem *item = new QGraphicsPathItem(quarter_circle);
	item->setBrush(fill_color);
	item->setPen(pen_color);
	m_scene.addItem(item);
}

void QRrenderer::add_points_corrugate(const bool draw_alignment_patterns) noexcept {
	const std::size_t QR_size = m_QR_matrix.getSize();
	const double QR_cell_size = m_outer_square_size/QR_size;
	const double s = QR_cell_size/2;

	const auto is_cell_set =
	[&](std::size_t i, std::size_t j) noexcept -> bool {
		if (i >= QR_size or j >= QR_size) { return false; }
		return m_QR_matrix.getModule(i, j);
	};

	for (std::size_t x = 0; x < QR_size; ++x) {
		for (std::size_t y = 0; y < QR_size; ++y) {

			if (not draw_alignment_patterns and
				is_point_within_alignment_pattern(x, y, QR_size))
			{ continue; }

			if (draw_alignment_patterns and not
				is_point_within_alignment_pattern(x, y, QR_size))
			{ continue; }

			const double base_x = m_outer_square_x0 + x*QR_cell_size;
			const double base_y = m_outer_square_y0 + y*QR_cell_size;

			if (m_QR_matrix.getModule(x, y)) {

				if (not is_cell_set(x - 1, y) and not is_cell_set(x - 1, y - 1) and not is_cell_set(x, y - 1)) {
					add_quarter_circle(
						base_x, base_y, s, 90,
						m_point_fill, m_point_border
					);
				}
				else {
					add_rectangle(
						base_x, base_y, s, s,
						m_point_fill, m_point_border
					);
				}

				if (not is_cell_set(x, y - 1) and not is_cell_set(x + 1, y - 1) and not is_cell_set(x + 1, y)) {
					add_quarter_circle(
						base_x, base_y, s, 0,
						m_point_fill, m_point_border
					);
				}
				else {
					add_rectangle(
						base_x + s, base_y, s, s,
						m_point_fill, m_point_border
					);
				}

				if (not is_cell_set(x + 1, y) and not is_cell_set(x + 1, y + 1) and not is_cell_set(x, y + 1)) {
					add_quarter_circle(
						base_x, base_y, s, 270,
						m_point_fill, m_point_border
					);
				}
				else {
					add_rectangle(
						base_x + s, base_y + s, s, s,
						m_point_fill, m_point_border
					);
				}

				if (not is_cell_set(x, y + 1) and not is_cell_set(x - 1, y + 1) and not is_cell_set(x - 1, y)) {
					add_quarter_circle(
						base_x, base_y, s, 180,
						m_point_fill, m_point_border
					);
				}
				else {
					add_rectangle(
						base_x, base_y + s, s, s,
						m_point_fill, m_point_border
					);
				}
			}
			else {
				if (is_cell_set(x - 1, y) and is_cell_set(x, y - 1)) {
					add_complementary_quarter_circle(
						base_x, base_y, s, 90,
						m_point_fill, m_point_border
					);
				}
				if (is_cell_set(x, y - 1) and is_cell_set(x + 1, y)) {
					add_complementary_quarter_circle(
						base_x, base_y, s, 0,
						m_point_fill, m_point_border
					);
				}
				if (is_cell_set(x + 1, y) and is_cell_set(x, y + 1)) {
					add_complementary_quarter_circle(
						base_x, base_y, s, 270,
						m_point_fill, m_point_border
					);
				}
				if (is_cell_set(x, y + 1) and is_cell_set(x - 1, y)) {
					add_complementary_quarter_circle(
						base_x, base_y, s, 180,
						m_point_fill, m_point_border
					);
				}
			}
		}
	}
}

void QRrenderer::add_alignment_patterns_round(
	const std::vector<std::pair<int,int>>& points,
	const int outer_x, const int outer_y, const int outer_radius,
	const int blank_x, const int blank_y, const int blank_radius,
	const int inner_x, const int inner_y, const int inner_radius
)
noexcept
{
	const std::size_t QR_size = m_QR_matrix.getSize();
	const double QR_cell_size = m_outer_square_size/QR_size;

	for (const auto& [x, y] : points) {
		const double x0 = m_outer_square_x0 + x*QR_cell_size;
		const double y0 = m_outer_square_y0 + y*QR_cell_size;

		add_circle(
			x0 - outer_x*QR_cell_size,
			y0 - outer_y*QR_cell_size,
			outer_radius*QR_cell_size,
			m_point_fill, m_point_border
		);

		add_circle(
			x0 - blank_x*QR_cell_size,
			y0 - blank_y*QR_cell_size,
			blank_radius*QR_cell_size,
			Qt::GlobalColor::white, Qt::GlobalColor::white
		);

		add_circle(
			x0 - inner_x*QR_cell_size,
			y0 - inner_y*QR_cell_size,
			inner_radius*QR_cell_size,
			m_point_fill, m_point_border
		);
	}
}

void QRrenderer::add_alignment_patterns_square() noexcept {
	const std::size_t QR_size = m_QR_matrix.getSize();
	const double QR_cell_size = m_outer_square_size/QR_size;

	for (std::size_t x = 0; x <= 7; ++x) {
		for (std::size_t y = 0; y <= 7; ++y) {
			if (m_QR_matrix.getModule(x, y)) {
				add_rectangle(
					m_outer_square_x0 + x*QR_cell_size,
					m_outer_square_y0 + y*QR_cell_size,
					QR_cell_size, QR_cell_size,
					m_point_fill, m_point_border
				);
			}
		}

		for (std::size_t y = QR_size - 7; y < QR_size; ++y) {
			if (m_QR_matrix.getModule(x, y)) {
				add_rectangle(
					m_outer_square_x0 + x*QR_cell_size,
					m_outer_square_y0 + y*QR_cell_size,
					QR_cell_size, QR_cell_size,
					m_point_fill, m_point_border
				);
			}
		}
	}

	for (std::size_t x = QR_size - 7; x < QR_size; ++x) {
		for (std::size_t y = 0; y <= 7; ++y) {
			if (m_QR_matrix.getModule(x, y)) {
				add_rectangle(
					m_outer_square_x0 + x*QR_cell_size,
					m_outer_square_y0 + y*QR_cell_size,
					QR_cell_size, QR_cell_size,
					m_point_fill, m_point_border
				);
			}
		}
	}
}

void QRrenderer::add_alignment_patterns() noexcept {
	if (m_alignment_patterns == shapes::circles) {
		add_alignment_patterns_round(
			m_QR_matrix.finder_centers,
			3, 3, 7,
			2, 2, 5,
			1, 1, 3
		);
	}
	else if (m_alignment_patterns == shapes::squares) {
		add_alignment_patterns_square();
	}
	else if (m_alignment_patterns == shapes::round_edges) {
		add_points_corrugate(true);
	}
}

void QRrenderer::add_points_circle() noexcept {
	const std::size_t QR_size = m_QR_matrix.getSize();
	const double QR_cell_size = m_outer_square_size/QR_size;

	for (std::size_t x = 0; x < QR_size; ++x) {
		for (std::size_t y = 0; y < QR_size; ++y) {

			if (is_point_within_alignment_pattern(x, y, QR_size)) { continue; }

			if (m_QR_matrix.getModule(x, y)) {
				add_circle(
					m_outer_square_x0 + x*QR_cell_size,
					m_outer_square_y0 + y*QR_cell_size,
					QR_cell_size,
					m_point_fill, m_point_border
				);
			}
		}
	}
}

void QRrenderer::add_points_square() noexcept {
	const std::size_t QR_size = m_QR_matrix.getSize();
	const double QR_cell_size = m_outer_square_size/QR_size;

	for (std::size_t x = 0; x < QR_size; ++x) {
		for (std::size_t y = 0; y < QR_size; ++y) {

			if (is_point_within_alignment_pattern(x, y, QR_size)) { continue; }

			if (m_QR_matrix.getModule(x, y)) {
				add_rectangle(
					m_outer_square_x0 + x*QR_cell_size,
					m_outer_square_y0 + y*QR_cell_size,
					QR_cell_size, QR_cell_size,
					m_point_fill, m_point_border
				);
			}
		}
	}
}

void QRrenderer::add_points() noexcept {
	if (m_points == shapes::circles) {
		add_points_circle();
	}
	else if (m_points == shapes::squares) {
		add_points_square();
	}
	else if (m_points == shapes::round_edges) {
		add_points_corrugate(false);
	}
}

void QRrenderer::add_QR_image_background() noexcept {
	const int width = m_QR_image->width();
	const int height = m_QR_image->height();

	const double cx = m_outer_square_x0 + m_outer_square_size/2;
	const double cy = m_outer_square_y0 + m_outer_square_size/2;

	const double w = m_redim*m_QR_image_background_scale*width;
	const double h = m_redim*m_QR_image_background_scale*height;

	if (m_QR_image_background_shape == shapes::squares) {

		add_rectangle(
			cx - w/2,
			cy - h/2,
			w, h,
			m_image_background_fill, m_image_background_border
		);
	}
	else if (m_QR_image_background_shape == shapes::circles) {
		const double r = std::sqrt(w*w + h*h)/2.0;

		add_circle(
			cx - r,
			cy - r,
			2*r,
			m_image_background_fill, m_image_background_border
		);
	}
}

void QRrenderer::add_QR_image() noexcept {
	QGraphicsPixmapItem *pixmap_item = new QGraphicsPixmapItem(*m_QR_image);

	const double width = m_QR_image->width();
	const double height = m_QR_image->height();
	pixmap_item->setOffset(
		m_outer_square_x0 + m_outer_square_size/2 - width/2,
		m_outer_square_y0 + m_outer_square_size/2 - height/2
	);

	pixmap_item->setTransformOriginPoint(pixmap_item->boundingRect().center());
	pixmap_item->setScale(m_redim*m_QR_image_scale);

	m_scene.addItem(pixmap_item);
}

void QRrenderer::set_transformations() noexcept {
	QList<QGraphicsItem*> item_list = m_scene.items();
	for (qsizetype i = 0; i < item_list.size(); ++i) {

		QGraphicsItem *item = item_list[i];

#if defined __PAINT_CONTROL_SHAPES
		const QVariant d = item->data(1);
		if (d.isValid()) {
			const bool is_outer = d.toString() == "outer_square";
			if (is_outer) {
				continue;
			}
		}
#endif

		item->setTransformOriginPoint(m_drawing_area.center());
		item->setScale(m_redim);
	}
}

void QRrenderer::window_was_resized() noexcept {
	m_scene_to_be_updated = true;
	update();
}

void QRrenderer::update() noexcept {
	if (m_QR_matrix.getSize() == 0) { return; }

	// calculate the coordinates of the outer and inner drawing squares
	update_outer_square();

	if (m_scene_to_be_updated) {
		// clean up drawing area
		m_scene.clear();

		add_points();
		add_alignment_patterns();

		if (m_QR_image != nullptr) {
			add_QR_image_background();
			add_QR_image();
		}

#if defined __PAINT_CONTROL_SHAPES
		QGraphicsRectItem *outer_square = new QGraphicsRectItem(QRectF(
			m_outer_square_x0,   m_outer_square_y0,
			m_outer_square_size, m_outer_square_size
		));
		outer_square->setData(1, "outer_square");
		outer_square->setPen({Qt::blue});
		m_scene.addItem(outer_square);
#endif

		setSceneRect(m_drawing_area);
		centerOn(m_drawing_area.center());

		set_transformations();

		m_scene_to_be_updated = false;
	}

	if (m_transformations_to_be_updated) {
		set_transformations();
		m_transformations_to_be_updated = true;
	}
}
