#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <math.h>
using namespace sf;
using namespace std;

# define PI 3.14159265358979323846

const float G = 3.54;
const float c = 30;
const float dt = 0.1;

float getMagnitude(Vector2f v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

class Photon {
public:
	CircleShape photon;
	Vector2f position;
	float angle;
	Vector2f v = Vector2f(-c, 0);

	Photon(Vector2f position) {
		this->position = position;
		photon.setRadius(4);
		photon.setFillColor(Color::Red);
		photon.setPosition(this->position);
		angle = (rand() % 360) * PI / 180;
		v = Vector2f(cos(angle) * c, sin(angle) * c);
	}

	void update() {
		photon.move(v * dt);
	}

	void draw(RenderWindow& window) {
		window.draw(photon);
	}
};

class Blackhole {
public:
	CircleShape hole, disk, orbit;
	VertexArray line[2];
	Vector2f position;
	float mass, rs;

	Blackhole(Vector2f position, float mass) {
		this->position = position;
		this->mass = mass;
		rs = (2 * G * this->mass) / (c * c);
		
		hole.setOrigin(rs * 2, rs * 2);
		hole.setFillColor(Color::Black);
		hole.setPosition(this->position);
		hole.setRadius(this->rs * 2);
		hole.setPointCount(100);

		disk.setOrigin(rs * 6 + 64, rs * 6 + 64);
		disk.setFillColor(Color::Transparent);
		disk.setPosition(this->position);
		disk.setRadius(this->rs * 6 + 64);
		disk.setOutlineThickness(64);
		disk.setOutlineColor(Color(100, 100, 100, 100));
		disk.setPointCount(100);

		orbit.setOrigin(rs * 3 + 32, rs * 3 + 32);
		orbit.setFillColor(Color::Transparent);
		orbit.setPosition(this->position);
		orbit.setRadius(this->rs * 3 + 32);
		orbit.setOutlineThickness(32);
		orbit.setOutlineColor(Color(255, 150, 0));
		orbit.setPointCount(100);

		for (int i = 0; i < 2; i++) {
			line[i].resize(2);
			line[i].setPrimitiveType(Lines);
		}
		line[0][0].position = Vector2f(0, hole.getPosition().y);
		line[0][1].position = Vector2f(1920, hole.getPosition().y);
		line[1][0].position = Vector2f(0, hole.getPosition().y - rs * 2.6);
		line[1][1].position = Vector2f(1920, hole.getPosition().y - rs * 2.6);
		line[0][0].color = Color::Black;
		line[0][1].color = Color::Black;
		line[1][0].color = Color::Black;
		line[1][1].color = Color::Black;
	}

	void pull(Photon& p) {
		Vector2f force = hole.getPosition() - p.photon.getPosition();
		float theta = atan2(hole.getPosition().y - p.photon.getPosition().y, hole.getPosition().x - p.photon.getPosition().x);
		const float r = getMagnitude(force);
		const float fg = G * this->mass / (r * r);
		float deltaTheta = -fg * (dt / c) * sin(p.angle - theta);
		deltaTheta /= abs(1.0 - 2.0 * G * this->mass / (r * pow(c, 2)));
		p.angle += deltaTheta;
		p.v = Vector2f(cos(p.angle), sin(p.angle));
		float pvMag = getMagnitude(p.v);
		p.v = Vector2f(p.v.x * (c / pvMag), p.v.y * (c / pvMag));
	}

	void draw(RenderWindow& window) {
		window.draw(hole);
		window.draw(disk);
		window.draw(orbit);
		for (int i = 0; i < 2; i++) {
			window.draw(line[i]);
		}
	}
};

Blackhole* m87;
vector<Photon*> particles;

int main() {
	ContextSettings settings;
	settings.antialiasingLevel = 8;
	RenderWindow window(VideoMode(1920, 1080), "SFML", Style::Fullscreen, settings);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(120);

	m87 = new Blackhole(Vector2f(960, 540), 3000);

	while (window.isOpen()) {
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}

		if (Mouse::isButtonPressed(Mouse::Left)) {
			particles.push_back(new Photon(Vector2f(Mouse::getPosition())));
		}

		for (int i = 0; i < particles.size(); i++) {
			m87->pull(*particles[i]);
			particles[i]->update();
		}

		for (int i = 0; i < particles.size(); i++) {
			if (particles[i]->photon.getRadius() + m87->hole.getRadius() > getMagnitude(Vector2f(particles[i]->photon.getPosition().x + particles[i]->photon.getRadius() - m87->hole.getPosition().x, particles[i]->photon.getPosition().y + particles[i]->photon.getRadius() - m87->hole.getPosition().y))) {
				particles.erase(particles.begin() + i);
			}
			else if (particles[i]->photon.getPosition().x <= 0 || particles[i]->photon.getPosition().x >= 1920 || particles[i]->photon.getPosition().y <= 0 || particles[i]->photon.getPosition().y >= 1080) {
				particles.erase(particles.begin() + i);
			}
		}

		window.clear(Color::White);
		m87->draw(window);
		for (int i = 0; i < particles.size(); i++) {
			particles[i]->draw(window);
		}
		window.display();
	}

}