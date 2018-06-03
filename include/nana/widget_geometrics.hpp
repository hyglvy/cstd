/*
*	Widget Geometrics
*	Nana C++ Library(http://www.nanapro.org)
*	Copyright(C) 2003-2016 Jinhao(cnjinhao@hotmail.com)
*
*	Distributed under the Boost Software License, Version 1.0.
*	(See accompanying file LICENSE_1_0.txt or copy at
*	http://www.boost.org/LICENSE_1_0.txt)
*
*	@file: nana/gui/widget_geometrics.hpp
*	@description:
*/
#ifndef NANA_DETAIL_WIDGET_GEOMETRICS_HPP
#define NANA_DETAIL_WIDGET_GEOMETRICS_HPP



namespace nana
{
	struct color_proxy
	{
#if 0
	public:
		color_proxy(const color_proxy&);
		color_proxy(color_rgb);
		color_proxy(colors);
		color_proxy& operator=(const color_proxy&);
		color_proxy& operator=(const color&);
		color_proxy& operator=(color_rgb);
		color_proxy& operator=(colors);
		color get_color() const;
		operator color() const;
	private:
		shared_ptr<color> color_;
#endif
    unsigned color_;
	};//end namespace color_proxy

	struct widget_geometrics
	{
    widget_geometrics() {
      activated.color_ = 0x60C8FD;
      background.color_ = button_face;
      foreground.color_ = black;
    }
    virtual ~widget_geometrics() {};

		color_proxy activated;//{ static_cast<color_rgb>(0x60C8FD) };
		color_proxy background;//{colors::button_face};
		color_proxy foreground;//{colors::black};
	};
}

#endif