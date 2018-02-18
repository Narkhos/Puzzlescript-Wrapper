/* ***********************************************************************************
 * Copyright (C) 2015-2018 Xavier Direz - http://www.LaFaceObscureDuPixel.fr         *
 *                                                                                   *
 * This file is part of ObscureGameEngine.                                           *
 *                                                                                   *
 * ObscureGameEngine is free software; you can redistribute it and/or modify         *
 * it under the terms of the GNU Lesser General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or                 *
 * (at your option) any later version.                                               *
 *                                                                                   *
 * ObscureGameEngine is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                    *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                     *
 * GNU Lesser General Public License for more details.                               *
 *                                                                                   *
 * You should have received a copy of the GNU Lesser General Public License          *
 * along with  this program; If not, see <http://www.gnu.org/licenses/>.             *
 *************************************************************************************/
 
template<typename T>
class position2d
{
	public:
	T X;
	T Y;
	position2d();
	position2d(T _x, T _y);

	position2d(const position2d<T>& _p);
	~position2d();
	position2d operator + (const position2d& _p) const;
	position2d  operator - () const;
	position2d  operator - (const position2d& p) const;
	position2d  operator * (T k) const;
	position2d  operator / (T k) const;
	bool operator ==(const position2d& p) const;
	T getLength();
};

/********** POSITION2D *************/
template <typename T>
T position2d<T>::getLength()
{
	return sqrt(X*X + Y*Y);
}

template <typename T>
position2d<T>::position2d()
	:X(0),Y(0)
{
}

template <typename T>
position2d<T>::position2d(T _x, T _y)
	:X(_x),Y(_y)
{
}

template <typename T>
position2d<T>::position2d(const position2d<T>& _p)
{
	this->X = _p.X;
	this->Y = _p.Y;
}
template <typename T>
position2d<T>::~position2d()
{}
template <typename T>
position2d<T> position2d<T>::operator + (const position2d<T>& _p) const
{
	return position2d(X+_p.X,Y+_p.Y);
}

template <typename T>
position2d<T>  position2d<T>::operator - () const
{
	return position2d(-X,-Y);
}

template <typename T>
position2d<T>  position2d<T>::operator - (const position2d& _p) const
{
	return position2d(X+_p.X,Y+_p.Y);
}

template <typename T>
position2d<T>  position2d<T>::operator * (T k) const
{
	return position2d(X*k,Y*k);
}

template <typename T>
position2d<T>  position2d<T>::operator / (T k) const
{
	return position2d(X/k,Y/k);
}

template <typename T>
bool position2d<T>::operator ==(const position2d& p) const
{
	if(this->X==p.X && this->Y==p.Y) return true;

	return false;
}

