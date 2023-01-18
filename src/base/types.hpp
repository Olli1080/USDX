#pragma once

template<typename T = double>
struct Position2D
{
	T X = 0.0;
	T Y = 0.0;
};

template<typename T = double>
struct Size2D
{
	T W = 0.0;
	T H = 0.0;
};

template<typename T = double>
struct Rect : Position2D<T>, Size2D<T>
{};