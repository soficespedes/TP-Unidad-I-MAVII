#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "SFMLRenderer.h"
#include <list>
#include <vector>
using namespace sf;
class Game
{
private:
	// Propiedades de la ventana
	int alto;
	int ancho;
	RenderWindow *wnd;
	Color clearColor;

	// Objetos de box2d
	b2World *phyWorld;
	SFMLRenderer *debugRender;

	//tiempo de frame
	float frameTime;
	int fps;

	// Cuerpo de box2d
	b2Body* controlBody;

	

public:

	// Constructores, destructores e inicializadores
	Game(int ancho, int alto,std::string titulo);
	void CheckCollitions();
	~Game(void);
	void InitPhysics();
	#define MAX_PROJECTILES 500  // M�ximo n�mero de proyectiles permitidos
	b2Body* projectiles[MAX_PROJECTILES];
	bool projectileActive[MAX_PROJECTILES];
	// Main game loop
	void Loop();
	void DrawGame();
	void UpdatePhysics();
	void DoEvents();
	void SetZoom();
};

