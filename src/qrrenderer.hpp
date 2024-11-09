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
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>

// custom includes
#include "qrcode/qrcodegen.hpp"

class QRrenderer : public QGraphicsView {
	Q_OBJECT

public:
	explicit QRrenderer(QWidget *parent = nullptr) noexcept;

	void set_QR_code(qrcodegen::QrCode&& QR_matrix) noexcept;

	// This method should be called only *after* the QR code has been generated
	void load_QR_image(const QString& path) noexcept;
	void remove_QR_image() noexcept;

	[[nodiscard]] const QRect& get_drawing_area() const noexcept {
		return m_drawing_area;
	}

	enum class shapes {
		none,
		squares,
		circles,
		round_edges
	};

public slots:
	void set_point_fill_color(const int color) noexcept;
	void set_point_border_color(const int color) noexcept;

	void set_point_shape(const int color) noexcept;
	void set_alignment_pattern_shape(const int shape) noexcept;

	void set_transparent_background(const bool alpha) noexcept;

	void resize_QR(const int value) noexcept;

	void set_image_background_fill_color(const int color) noexcept;
	void set_image_background_border_color(const int color) noexcept;

	void resize_QR_image(const int value) noexcept;
	void set_QR_image_background_shape(const int shape) noexcept;
	void resize_QR_image_background(const int value) noexcept;

signals:

private:
	void update_outer_square() noexcept;

	void add_circle
	(
		const double x, const double y, const double radius,
		const QColor& fill_color,
		const QColor& pen_color
	)
	noexcept;

	void add_rectangle
	(
		const double x, const double y,
		const double width, const double height,
		const QColor& fill_color,
		const QColor& pen_color
	)
	noexcept;

	void add_complementary_quarter_circle
	(
		const double x, const double y,
		const double radius,
		const double start,
		const QColor& fill_color,
		const QColor& pen_color
	)
	noexcept;

	void add_quarter_circle(
		const double x, const double y,
		const double radius,
		const double start,
		const QColor& fill_color,
		const QColor& pen_color
	)
	noexcept;

	void add_points_corrugate(const bool draw_alignment_patterns) noexcept;

	void add_alignment_patterns_round
	(
		const std::vector<std::pair<int,int>>& points,
		const int outer_x, const int outer_y,
		const int outer_radius,
		const int blank_x, const int blank_y,
		const int blank_radius,
		const int inner_x, const int inner_y,
		const int inner_radius
	)
	noexcept;

	void add_alignment_patterns_square() noexcept;
	void add_alignment_patterns() noexcept;

	void add_points_circle() noexcept;
	void add_points_square() noexcept;
	void add_points() noexcept;

	void add_QR_image_background() noexcept;
	void add_QR_image() noexcept;

public slots:
	void set_transformations() noexcept;
	void window_was_resized() noexcept;

	void update() noexcept;

private:
	// 'Points' of the QR code
	Qt::GlobalColor m_point_fill = Qt::GlobalColor::black;
	Qt::GlobalColor m_point_border = Qt::GlobalColor::black;

	// Shape-related
	shapes m_points = shapes::squares;
	shapes m_alignment_patterns = shapes::squares;

	// QR matrix
	qrcodegen::QrCode m_QR_matrix;
	bool m_QR_set = false;
	bool m_scene_to_be_updated = false;
	bool m_transformations_to_be_updated = false;

	// redimension factor from the slide and its spin box
	double m_redim = 0.9;

	// Objects used to render the graphics scene
	QGraphicsScene m_scene;

	// Image loaded into the QR code
	QPixmap *m_QR_image = nullptr;
	double m_QR_image_scale = 1;
	double m_QR_image_background_scale = 1;
	shapes m_QR_image_background_shape = shapes::circles;
	Qt::GlobalColor m_image_background_fill = Qt::GlobalColor::black;
	Qt::GlobalColor m_image_background_border = Qt::GlobalColor::black;

	// rectangle that describes the drawing area of the widget
	// (which is a bit larger than the total area of the widget)
	QRect m_drawing_area;

	// background
	int m_background_alpha = 255;

	// Outer square
	double m_outer_square_size;
	double m_outer_square_x0;
	double m_outer_square_x1;
	double m_outer_square_y0;
	double m_outer_square_y1;
};
