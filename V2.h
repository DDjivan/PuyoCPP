#pragma once

#include <cmath>

struct V2 
{
	float x, y;

	V2(float _x, float _y) { x = _x; y = _y; }
	V2() {} 

	float norm()          { return sqrt(x*x + y * y); }
	void  normalize()     { float n = norm();  x /= n;    y /= n;    }
	V2    GetNormalized() { float n = norm();  return V2(x/n,y/n); }

	/// NEW ///
	operator std::string() const {
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}

	bool isInside(const V2 vRectStart, const V2 vRectSize) const {
		return (
			x >= vRectStart.x
			&& x <= vRectStart.x + vRectSize.x
			&& y >= vRectStart.y
			&& y <= vRectStart.y + vRectSize.y
		);
	}
	/// NEW ///
};

// comparaison sur des flottants... traitement spécial

bool operator == (const V2 & a, const V2 & b);

// redéfinition des opérateurs standards

V2 operator + (const V2 & a, const V2 & b);
V2 operator - (const V2 & a, const V2 & b);
V2 operator * (float      a, const V2 & b);
V2 operator * (const V2 & a, float      b);
V2 operator / (const V2 & a, float      b);
V2 operator - (const V2 & a);  // - unaire

// produit scalaire
float prodScal(const V2 & a, const V2 & b);
float prodVect(const V2 & a, const V2 & b);

// affichage 
std::ostream & operator << (std::ostream & os, V2 & t);

