/////////////////////////////////////////////////////////////////////////////////
//
// Thor C++ Library
// Copyright (c) 2011-2022 Jan Haller
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////

#include <lib/thor/include/Thor/Graphics/ToString.hpp>

#include <SFML/Graphics/Color.hpp>


namespace thor
{

std::string toString(const sf::Color& color)
{
	std::ostringstream stream;
	stream							 << '('
		<< static_cast<int>(color.r) << ','
		<< static_cast<int>(color.g) << ','
		<< static_cast<int>(color.b) << ','
		<< static_cast<int>(color.a) << ')';
	return stream.str();
}

} // namespace thor
