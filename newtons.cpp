#include<iostream>
#include<math.h>
#include<SFML\Graphics.hpp>
#include<complex>
#include<thread>
#include<vector>

/*
Eight root function:-------------------------------------------------------------
complex<double> f(complex<double> x) {
	return (pow(x, 8) + (real(15) * pow(x, 4)) - real(16));

}

complex<double> df(complex<double> x) {
	return ((real(8) * pow(x, 7)) + (real(60) * pow(x, 3)));
}

Three root function:-------------------------------------------------------------
complex<double> f(complex<double> x) {
	return (pow(x, 3) - real(1));

}

complex<double> df(complex<double> x) {
	return ((real(3) * pow(x, 2)));
}


Complex exponent: a = 2.1 ---------------------------------------------------------
complex<double> f(complex<double> x) {
	return pow(z, complex<double>(3,4)) - real(1);

}

complex<double> df(complex<double> x) {
	return complex<double>(3,4) * pow(z, complex<double>(3, 3));
}


Probably not 20 roots but maybe: -------------------------------------------------
complex<double> f(complex<double> x) {
	return pow(x, 20) + pow(x, 10) - pow(x, 5) - pow(x,2);

}

complex<double> df(complex<double> x) {
	return (real(20) * pow(x, 19)) + (real(10) * pow(x, 9)) - (real(5) * pow(x, 4)) - (real(2) * x);
}

Edited function from class use negaitve values too-------------------------------
complex<double> f(complex<double> x) {
	return ((pow(x, 4)) - (pow(x, 3)) + (pow(x, 2)) - real(1));

}

complex<double> df(complex<double> x) {
	return ((real(4) * pow(x, 3)) - (real(3) * pow(x, 2)) + (real(2) * x));
}


Supposed mandelbrot function: requires some program restructuring... -------------
complex<double> f(complex<double> x) {
	return ((pow(x, 2) + real(1)) * (x - a));
}

complex<double> df(complex<double> x) {
	return (((real(2) * x) * (x - a)) + (pow(x, 2) + real(1)));
}
*/

using namespace std;

class Pixel {
public:
	Pixel(complex<double> loc, int pos[2]) {
		z = loc;
		position[0] = pos[0];
		position[1] = pos[1];
	}
	~Pixel() {}

	void iterate(sf::Image& image) {
		z = newtonsMethod(100000);
		color = sf::Color(count * 6, count * 8, count * 12);
		image.setPixel(position[0], position[1], color);
	}

private:
	int count = 0;
	int position[2];
	sf::Color color = sf::Color(255, 255, 255);
	complex<double> z;
	complex<double> a = {1,0};
	//complex<double> z = a / real(3);

	complex<double> newtonsMethod(int maxIter) {
		for (int i = 0; i < maxIter; i++) {
			complex<double> dz = a * (f(z) / df(z));
			if (abs(dz) < .00000001) {
				return z;
			}
			count++;
			z -= dz;
		}
		return false;


	}

	complex<double> f(complex<double> x) {
		return ((pow(x, 4)) - (real(3) * pow(x, 3)) - (pow(x, 2)) + real(1));

	}

	complex<double> df(complex<double> x) {
		return ((real(4) * pow(x, 3)) - (real(9) * pow(x, 2)) - (real(2) * x));
	}
};


void fill_array(vector<Pixel*>& pixels, long double scale, int width, int height, sf::Vector2f mouse) { //fills the pixel array with Pixel objects
	long double shift[2] = { 0, 0 };
	long double horizontalStart;
	long double horizontalEnd;
	long double verticalStart;
	long double verticalEnd;
	long double origin[2] = { (width / 2), (height / 2) };
	long double scaleStart = (-100000000000 / scale);
	long double scaleEnd = (100000000000 / scale);
	long double horizontalSize = (width / (abs(scaleStart) + abs(scaleEnd)));
	long double verticalSize = (height / (abs(scaleStart) + abs(scaleEnd)));
	long double horizontalRes = (abs(scaleStart) + abs(scaleEnd)) / width * 1;
	long double verticalRes = (abs(scaleStart) + abs(scaleEnd)) / height * 1;

	shift[0] = origin[0] + ((origin[0] - mouse.x) * scale);
	shift[1] = origin[1] + ((origin[1] - mouse.y) * scale);

	horizontalStart = scaleStart + ((origin[0] - shift[0]) / horizontalSize);
	horizontalEnd = scaleEnd + ((origin[0] - shift[0]) / horizontalSize);
	verticalStart = scaleStart + ((origin[1] - shift[1]) / verticalSize);
	verticalEnd = scaleEnd + ((origin[1] - shift[1]) / verticalSize);

	complex<double> c;
	int position[2] = { 0, 0 };

	for (long double t = horizontalStart; t < horizontalEnd; t += horizontalRes) {

		for (long double m = verticalStart; m < verticalEnd; m += verticalRes) {

			position[0] = (t * horizontalSize) + shift[0];
			position[1] = (m * verticalSize) + shift[1];

			if (position[0] >= 0 and position[0] <= width and position[1] >= 0 and position[1] <= height) {

				c._Val[0] = t;
				c._Val[1] = m;

				pixels.push_back(new Pixel(c, position));
			}
		}
	}
}


void draw(vector<Pixel*>& pixels, sf::Image& image, int index_begin, int index_end) {
	for (int i = index_begin; i < index_end; i++) {
		pixels[i]->iterate(ref(image));
	}
}


vector<thread> create_threads(vector<Pixel*>& pixels, sf::Image& image, int max) {
	vector<thread> threads;
	int section = pixels.size() / max;
	for (int i = 0; i < max; i++) {
		threads.emplace_back(draw, ref(pixels), ref(image), section * i, (section * (i + 1)) - 1);
	}
	return threads;
}


int main() {
	//RENDER SETUP----------------------------------------------------------
	sf::RenderWindow window(sf::VideoMode(1000, 1000), "Newtons Fractal", sf::Style::None);
	window.setPosition(sf::Vector2i(460, 20));
	sf::Vector2f center(window.getSize().x / 2, window.getSize().y / 2);

	//VARIABLES-------------------------------------------------------------
	sf::Color bgColor = sf::Color(0, 0, 0);
	int zooms = 0; //NO ERROR????
	long double scale = pow(2, zooms);

	sf::Event event;
	vector<thread> active_threads;
	int max_threads = 16;
	sf::Image image;
	image.create(window.getSize().x, window.getSize().y, bgColor);
	sf::Texture texture;
	texture.loadFromImage(image);
	sf::Sprite newtons(texture);
	vector<Pixel*> pixels;
	sf::CircleShape dot(1);
	dot.setFillColor(sf::Color::Green);
	dot.setPosition(center.x, center.y);

	//INITIAL SET CREATION---------------------------------------------------
	fill_array(ref(pixels), scale, window.getSize().x, window.getSize().y, center);
	active_threads = create_threads(ref(pixels), ref(image), max_threads);
	for (auto& th : active_threads) {
		if (th.joinable()) {
			th.join();
		}
		th.~thread();
	}
	active_threads.clear();

	//GAME LOOP--------------------------------------------------------------
	while (window.isOpen()) {
		while (window.pollEvent(event))
		{
			// Close window: exit-----------------
			if (event.type == sf::Event::Closed)
				window.close();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
				window.close();
			}

			if (event.type == sf::Event::MouseWheelScrolled) {
				if (event.mouseWheelScroll.delta > 0) {
					scale *= 2;
					zooms += 1;
				}
				else if (event.mouseWheelScroll.delta < 0) {
					scale /= 2;
					zooms -= 1;
				}
				pixels.clear();
				//fill_array(ref(pixels), scale, window.getSize().x, window.getSize().y, sf::Vector2f(event.mouseWheelScroll.x, event.mouseWheelScroll.y));
				fill_array(ref(pixels), scale, window.getSize().x, window.getSize().y, center);
				//event.mouseWheelScroll.x, event.mouseWheelScroll.y //mouse postiion on scroll
				active_threads = create_threads(ref(pixels), ref(image), max_threads);
				for (auto& th : active_threads) {
					if (th.joinable()) {
						th.join();
					}
					th.~thread();
				}
				active_threads.clear();
			}
		}
		//RENDER--------------------------------------------------------------
		window.clear(bgColor);


		texture.loadFromImage(image);
		window.draw(newtons);
		//cout << sizeof(texture) << "\n";

		window.draw(dot);
		window.display();
	}
}