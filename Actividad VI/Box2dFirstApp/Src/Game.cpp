#include "Game.h"
#include "Box2DHelper.h"
#include <iostream>

// Constructor de la clase Game
Game::Game(int ancho, int alto, std::string titulo)
{
    // Inicialización de la ventana y configuración de propiedades
    wnd = new RenderWindow(VideoMode(ancho, alto), titulo);
    wnd->setVisible(true);
    fps = 60;
    wnd->setFramerateLimit(fps);
    frameTime = 1.0f / fps;
    SetZoom(); // Configuración de la vista del juego
    InitPhysics(); // Inicialización del motor de física
    // Inicializar el arreglo de proyectiles
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i] = nullptr;
        projectileActive[i] = false;
    }
}

// Bucle principal del juego
void Game::Loop()
{
    while (wnd->isOpen())
    {
        wnd->clear(clearColor); // Limpiar la ventana
        DoEvents(); // Procesar eventos de entrada
        CheckCollitions(); // Comprobar colisiones
        UpdatePhysics(); // Actualizar la simulación física
        DrawGame(); // Dibujar el juego
        wnd->display(); // Mostrar la ventana
    }
}

// Actualización de la simulación física
void Game::UpdatePhysics()
{
    phyWorld->Step(frameTime, 8, 8); // Simular el mundo físico
    phyWorld->ClearForces(); // Limpiar las fuerzas aplicadas a los cuerpos
    phyWorld->DebugDraw(); // Dibujar el mundo físico para depuración
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectileActive[i]) {
            b2Vec2 pos = projectiles[i]->GetPosition();
            // Suponiendo que la ventana se extiende hasta x = 100 (o el ancho que hayas definido)
            if (pos.x > 800.0f) {
                phyWorld->DestroyBody(projectiles[i]);
                projectiles[i] = nullptr;
                projectileActive[i] = false;
            }
        }
    }
}

// Dibujo de los elementos del juego
void Game::DrawGame()
{
    // Dibujar las paredes
    sf::RectangleShape leftWallShape(sf::Vector2f(10, alto)); // Alto de la ventana
    leftWallShape.setFillColor(sf::Color::Blue);
    leftWallShape.setPosition(100, 0); // X = 100 para que comience donde termina el suelo
    wnd->draw(leftWallShape);

    // Dibujar el cuerpo de control (rectangulo)
    sf::RectangleShape controlShape(sf::Vector2f(20, 10));
    controlShape.setFillColor(sf::Color::Yellow);
    controlShape.setPosition(controlBody->GetPosition().x - 10, controlBody->GetPosition().y - 5);
    wnd->draw(controlShape);

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


    // Dibujar los proyectiles activos
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectileActive[i]) {
            b2Vec2 pos = projectiles[i]->GetPosition();
            float angle = projectiles[i]->GetAngle();
            sf::CircleShape projShape(2);
            projShape.setOrigin(2.0f, 2.0f); // Centrar
            projShape.setPosition(pos.x, pos.y);
            projShape.setRotation(angle * 180.0f / b2_pi);
            projShape.setFillColor(sf::Color::Yellow);
            wnd->draw(projShape);
        }
    }
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
            wnd->close(); // Cerrar la ventana si se presiona el botón de cerrar
            break;
            
        }
    }

    // Controlar el movimiento del cuerpo de control con el teclado
    // Segun la numeracion usada, cuando mas cerca de cero mas 
    // lento es el desplazamiento sobre ese eje
    if (Keyboard::isKeyPressed(Keyboard::Down))
        controlBody->SetLinearVelocity(b2Vec2(0.0f, 30.0f));
    else if (Keyboard::isKeyPressed(Keyboard::Up))
        controlBody->SetLinearVelocity(b2Vec2(0.0f, -30.0f));
    else
        controlBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));

    if (Keyboard::isKeyPressed(Keyboard::P)) {
        // Se busca el primer espacio libre en el arreglo de proyectiles
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (!projectileActive[i]) {
                // Posición de disparo: desde el borde derecho del cuerpo de control.
                b2Vec2 controlPos = controlBody->GetPosition();
                // Suponiendo que el rectángulo de control mide 20 de ancho y está centrado,
                // su borde derecho es controlPos.x + 10.
                float spawnX = controlPos.x + 10.0f;
                float spawnY = controlPos.y;
                b2Body* proj = Box2DHelper::CreateCircularDynamicBody(phyWorld, 2, 2, 1.0f, 0.5f);
                proj->SetTransform(b2Vec2(spawnX, spawnY), 0.0f);
                // Se le asigna una velocidad para que se mueva hacia la derecha
                proj->SetLinearVelocity(b2Vec2(90.0f, 0.0f));
                projectiles[i] = proj;
                projectileActive[i] = true;
                break; 
            }
        }
    }
    controlBody->SetAwake(true);

    b2Vec2 pos = controlBody->GetPosition();
    float topLimit = 10.0f;  // Límite superior (altura del techo)
    float bottomLimit = 90.0f; // Límite inferior (altura del suelo)

    if (Keyboard::isKeyPressed(Keyboard::Down))
    {
        if (pos.y < bottomLimit)  // Verificar si está dentro del límite inferior
            controlBody->SetLinearVelocity(b2Vec2(0.0f, 30.0f));
        else
            controlBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));  // Frenar en el límite
    }
    else if (Keyboard::isKeyPressed(Keyboard::Up))
    {
        if (pos.y > topLimit)  // Verificar si está dentro del límite superior
            controlBody->SetLinearVelocity(b2Vec2(0.0f, -30.0f));
        else
            controlBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));  // Frenar en el límite
    }
    else
    {
        controlBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));  // Sin movimiento
    }
}

// Comprobación de colisiones (a implementar más adelante)
void Game::CheckCollitions()
{
    // Implementación de la comprobación de colisiones
}

// Configuración de la vista del juego
void Game::SetZoom()
{
    View camara;
    // Posicionamiento y tamaño de la vista
    camara.setSize(100.0f, 100.0f);
    camara.setCenter(50.0f, 50.0f);
    wnd->setView(camara); // Asignar la vista a la ventana
}

// Inicialización del motor de física y los cuerpos del mundo físico
void Game::InitPhysics()
{
    // Inicializar el mundo físico con la gravedad por defecto
    phyWorld = new b2World(b2Vec2(0.0f, 0.1f));

    // Crear un renderer de debug para visualizar el mundo físico
    debugRender = new SFMLRenderer(wnd);
    debugRender->SetFlags(UINT_MAX);
    phyWorld->SetDebugDraw(debugRender);

    // Crear el suelo y las paredes estáticas del mundo físico
    b2Body* groundBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    groundBody->SetTransform(b2Vec2(50.0f, 100.0f), 0.0f);
    groundBody->GetFixtureList()->SetFriction(0.1f);  //menos fricción, más rebote

    b2Body* upWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    upWallBody->SetTransform(b2Vec2(250.0f, 50.0f), 0.0f);

    b2Body* leftWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    leftWallBody->SetTransform(b2Vec2(0.0f, 50.0f), 0.0f);

    // Crear el cuerpo de control (rectángulo) como kinemático para que no se mueva por colisiones
    controlBody = Box2DHelper::CreateRectangularKinematicBody(phyWorld, 20, 10);
    controlBody->SetTransform(b2Vec2(0.0f, 50.0f), 0.0f);
}


// Destructor de la clase
Game::~Game(void)
{ }