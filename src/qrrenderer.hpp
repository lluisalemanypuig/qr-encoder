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
	explicit QRrenderer(QWidget *parent = nullptr);

	void set_QR_code(qrcodegen::QrCode&& QR_matrix) noexcept;

	// This method should be called only *after* the QR code has been generated
	void load_QR_image(const QString& path) noexcept;

	const QRect& get_drawing_area() const noexcept {
		return m_drawing_area;
	}

	enum class shapes {
		squares,
		circles
	};

public slots:
	void resize_QR(const int value) noexcept;

	void resize_QR_image(const int value) noexcept;
	void set_QR_image_background_shape(const int shape) noexcept;
	void resize_QR_image_background(const int value) noexcept;

	void update() noexcept;

	void set_fill_color(const int color) noexcept;
	void set_border_color(const int color) noexcept;

	void set_point_shape(const int color) noexcept;
	void set_alignment_pattern_shape(const int shape) noexcept;

signals:

private:
	void update_outer_square() noexcept;
	void update_inner_square() noexcept;

	void draw_square
	(const std::size_t x, const std::size_t y, const double QR_cell_size)
	noexcept;
	void draw_circle
	(const std::size_t x, const std::size_t y, const double QR_cell_size)
	noexcept;

	void draw_alignment_patterns_round(
		const std::vector<std::pair<int,int>>& points,
		const int outer_x, const int outer_y,
		const int outer_w, const int outer_h,
		const int blank_x, const int blank_y,
		const int blank_w, const int blank_h,
		const int inner_x, const int inner_y,
		const int inner_w, const int inner_h
	)
	noexcept;
	void draw_alignment_patterns_square() noexcept;
	void draw_alignment_patterns() noexcept;

	void draw_points_circle() noexcept;
	void draw_points_square() noexcept;
	void draw_points() noexcept;

private:
	// Color-related
	Qt::GlobalColor m_point_fill = Qt::GlobalColor::black;
	Qt::GlobalColor m_point_border = Qt::GlobalColor::black;

	// Shape-related
	shapes m_points = shapes::squares;
	shapes m_alignment_patterns = shapes::squares;

	// QR matrix
	qrcodegen::QrCode m_QR_matrix;

	// redimension factor from the slide and its spin box
	double m_redim = 0.05;

	// Objects used to render the graphics scene
	QGraphicsScene m_scene;

	// Image loaded into the QR code
	QPixmap *m_image = nullptr;
	double m_image_scale = 1;
	double m_background_image_scale = 1;
	shapes m_background_shape = shapes::squares;

	// rectangle that describes the drawing area of the widget
	// (which is a bit larger than the total area of the widget)
	QRect m_drawing_area;

	// Outer square
	double m_outer_square_size;
	double m_outer_square_x0;
	double m_outer_square_x1;
	double m_outer_square_y0;
	double m_outer_square_y1;

	// Inner square
	double m_inner_square_size;
	double m_inner_square_x0;
	double m_inner_square_x1;
	double m_inner_square_y0;
	double m_inner_square_y1;
};
