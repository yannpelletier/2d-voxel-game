#pragma once

namespace gengine{
	struct coords{
		float x;
		float y;
	};

	struct color{
		color() { r = 0; g = 0; b = 0; a = 0; }
		color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a){
			r = _r; g = _g; b = _b; a = _a; 
		}

		bool compareColor(color& colorToCompare){
			if (r == colorToCompare.r && g == colorToCompare.g && b == colorToCompare.b && a == colorToCompare.a) {
				return true;
			}
			return false;
		}

		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

	struct UV{
		float u;
		float v;
	};

	struct vertex{
		coords position;

		color color;

		UV uv;

		void setPosition(float x, float y){
			position.x = x;
			position.y = y;
		}

		void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a){
			color.r = r;
			color.g = g;
			color.b = b;
			color.a = a;
		}

		void setUv(float u, float v){
			uv.u = u;
			uv.v = v;
		}
	};
}