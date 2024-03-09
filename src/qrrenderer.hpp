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

#pragma once

// Qt includes
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>

// C++ includes
#include <vector>

class QRrenderer : public QGraphicsView {
	Q_OBJECT

public:
	explicit QRrenderer(QWidget *parent = nullptr);

	void set_QR_code(std::vector<int>&& QR_matrix, std::size_t QR_size) noexcept;

	const QRect& get_drawing_area() const noexcept {
		return m_drawing_area;
	}

public slots:
	void redimensionQR(int value) noexcept;
	void update() noexcept;

signals:

protected:
	void update_outer_square() noexcept;
	void update_inner_square() noexcept;

private:
	// QR matrix
	std::vector<int> m_QR_matrix;
	// size of the QR
	std::size_t m_QR_size = 0;

	// redimension factor from the slide and its spin box
	double m_redim = 0.05;

	// Scene used to render the graphics
	QGraphicsScene m_scene;

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
