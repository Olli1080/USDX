#pragma once

struct Position2D
{
	double X = 0.0;
	double Y = 0.0;
};

struct Size2D
{
	double W = 0.0;
	double H = 0.0;
};

struct Rect : Position2D, Size2D
{};