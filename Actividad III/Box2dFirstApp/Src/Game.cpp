#include "Game.h"
#include "Box2DHelper.h"
#include <iostream>

// Constructor de la clase Game
Game::Game(int ancho, int alto, std::string titulo)
{
    // Inicializaci�n de la ventana y configuraci�n de propiedades
    wnd = new RenderWindow(VideoMode(ancho, alto), titulo);
    wnd->setVisible(true);
    fps = 60;
    wnd->setFramerateLimit(fps);
    frameTime = 1.0f / fps;
    SetZoom(); // Configuraci�n de la vista del juego
    InitPhysics(); // Inicializaci�n del motor de f�sica
}

// Bucle principal del juego
void Game::Loop()
{
    while (wnd->isOpen())
    {
        wnd->clear(clearColor); // Limpiar la ventana
        DoEvents(); // Procesar eventos de entrada
        CheckCollitions(); // Comprobar colisiones
        UpdatePhysics(); // Actualizar la simulaci�n f�sica
        DrawGame(); // Dibujar el juego
        wnd->display(); // Mostrar la ventana
    }
}

// Actualizaci�n de la simulaci�n f�sica
void Game::UpdatePhysics()
{
    phyWorld->Step(frameTime, 8, 8); // Simular el mundo f�sico
    phyWorld->ClearForces(); // Limpiar las fuerzas aplicadas a los cuerpos
    phyWorld->DebugDraw(); // Dibujar el mundo f�sico para depuraci�n
}

// Dibujo de los elementos del juego
void Game::DrawGame()
{
    // Dibujar el suelo
    sf::RectangleShape groundShape(sf::Vector2f(500, 5));
    groundShape.setFillColor(sf::Color::Red);
    groundShape.setPosition(0, 95);
    wnd->draw(groundShape);

    //Dibujar el techo
    sf::RectangleShape upWallShape(sf::Vector2f(500, 5));
    upWallShape.setFillColor(sf::Color::Red);
    upWallShape.setPosition(0, 0);
    wnd->draw(upWallShape);

    // Dibujar las paredes
    sf::RectangleShape leftWallShape(sf::Vector2f(10, alto)); // Alto de la ventana
    leftWallShape.setFillColor(sf::Color::Blue);
    leftWallShape.setPosition(100, 0); // X = 100 para que comience donde termina el suelo
    wnd->draw(leftWallShape);

    sf::RectangleShape rightWallShape(sf::Vector2f(10, alto)); // Alto de la ventana
    rightWallShape.setFillColor(sf::Color::Cyan);
    rightWallShape.setPosition(90, 0); // X = 90 para que comience donde termina el suelo
    wnd->draw(rightWallShape);

    // Dibujar el cuerpo de control (c�rculo)
    sf::CircleShape controlShape(5);
    controlShape.setFillColor(sf::Color::Blue);
    controlShape.setPosition(controlBody->GetPosition().x - 5, controlBody->GetPosition().y - 5);
    wnd->draw(controlShape);

    //dibujamos los obstaculos
    sf::RectangleShape ObstacleShape1(sf::Vector2f(10, 10));
    ObstacleShape1.setFillColor(sf::Color::Cyan);
    ObstacleShape1.setPosition(15,55);
    wnd->draw(ObstacleShape1);

    sf::RectangleShape ObstacleShape2(sf::Vector2f(10, 10));
    ObstacleShape2.setFillColor(sf::Color::Cyan);
    ObstacleShape2.setPosition(45, 65);
    wnd->draw(ObstacleShape2);

    sf::RectangleShape ObstacleShape3(sf::Vector2f(10, 10));
    ObstacleShape3.setFillColor(sf::Color::Cyan);
    ObstacleShape3.setPosition(70, 30);
    wnd->draw(ObstacleShape3);
}


// Procesamiento de eventos de entrada
void Game::DoEvents()
{
    Event evt;
    while (wnd->pollEvent(evt))
    {
        switch (evt.type)
        {
        case Event::Closed:
            wnd->close(); // Cerrar la ventana si se presiona el bot�n de cerrar
            break;
        }
    }
}

// Comprobaci�n de colisiones (a implementar m�s adelante)
void Game::CheckCollitions()
{
    // Implementaci�n de la comprobaci�n de colisiones
}

// Configuraci�n de la vista del juego
void Game::SetZoom()
{
    View camara;
    // Posicionamiento y tama�o de la vista
    camara.setSize(100.0f, 100.0f);
    camara.setCenter(50.0f, 50.0f);
    wnd->setView(camara); // Asignar la vista a la ventana
}

// Inicializaci�n del motor de f�sica y los cuerpos del mundo f�sico
void Game::InitPhysics()
{
    // Inicializar el mundo f�sico con la gravedad por defecto
    phyWorld = new b2World(b2Vec2(0.0f, 9.8f));

    // Crear un renderer de debug para visualizar el mundo f�sico
    debugRender = new SFMLRenderer(wnd);
    debugRender->SetFlags(UINT_MAX);
    phyWorld->SetDebugDraw(debugRender);

    // Crear el suelo y las paredes est�ticas del mundo f�sico
    b2Body* groundBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    groundBody->SetTransform(b2Vec2(50.0f, 100.0f), 0.0f);
    groundBody->GetFixtureList()->SetFriction(0.1f);  //menos fricci�n, m�s rebote

    b2Body* upWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    upWallBody->SetTransform(b2Vec2(250.0f, 50.0f), 0.0f);

    b2Body* leftWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    leftWallBody->SetTransform(b2Vec2(0.0f, 50.0f), 0.0f);
    leftWallBody->GetFixtureList()->SetFriction(0.1f);

    b2Body* rightWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    rightWallBody->SetTransform(b2Vec2(100.0f, 50.0f), 0.0f);
    rightWallBody->GetFixtureList()->SetFriction(0.1f);

    // Crear el circulo 
    controlBody = Box2DHelper::CreateCircularDynamicBody(phyWorld, 5, 1.0f, 0.5, 1.0f);
    controlBody->SetTransform(b2Vec2(10.0f, 50.0f), 0.0f);
    controlBody->SetLinearVelocity(b2Vec2(10.0f, -10.0f)); //movimiento inicial en diagonal

    b2Body* ObstacleBody1 = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 10);
    ObstacleBody1->SetTransform(b2Vec2(20.0f, 60.0f), 0.0f);
    ObstacleBody1->GetFixtureList()->SetFriction(0.1f);

    b2Body* ObstacleBody2 = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 10);
    ObstacleBody2->SetTransform(b2Vec2(50.0f, 70.0f), 0.0f);
    ObstacleBody2->GetFixtureList()->SetFriction(0.1f);

    b2Body* ObstacleBody3 = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 10);
    ObstacleBody3->SetTransform(b2Vec2(75.0f, 35.0f), 0.0f);
    ObstacleBody3->GetFixtureList()->SetFriction(0.1f);
}
// Destructor de la clase

Game::~Game(void)
{ }