#include "GameState.hpp"

GameState::GameState(ah::StateManager& manager)
: ah::State(manager)
{
    NWorld::registerActor<Entity>();
    NWorld::registerActor<Building>();
    NWorld::registerActor<Unit>();
    NWorld::registerActor<Map>();
    NWorld::registerActor<Forest>();

    NWorld::createActor<Map>();

    auto house = createActor<Building>();
    house->setCoords(2,3);
    house->addSprite(0,0,sf::IntRect(0,0,256,256));
    house->addSprite(0,-1,sf::IntRect(0,0,256,256));
    house->addSprite(1,-2,sf::IntRect(0,0,256,256));
    house->addSprite(1,-1,sf::IntRect(0,0,256,256));
    house->addSprite(0,1,sf::IntRect(0,0,256,256));
}

bool GameState::handleEvent(sf::Event const& event)
{
    NWorld::addEvent(event);

    sf::Vector2i c = NMapUtility::Isometric::worldToCoords(NWorld::getPointerPositionView());
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
    {
        createActor<Unit>()->setPosition(NWorld::getPointerPositionView());
    }
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        createActor<Forest>(c.x,c.y);
    }

    // Zoom
    if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
    {
        if (event.mouseWheelScroll.delta < 1)
        {
            NWorld::getCameraManager().getView().zoom(1.2f);
        }
        else
        {
            NWorld::getCameraManager().getView().zoom(0.8f);
        }
    }

    return true;
}

bool GameState::update(sf::Time dt)
{
    NWorld::tick(dt);
    NWorld::update();

    sf::Vector2f mvt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
    {
        mvt.y--;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        mvt.x--;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        mvt.y++;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        mvt.x++;
    }
    NWorld::getCameraManager().getView().move(dt.asSeconds() * 400.f * mvt);

    NVector m = NWorld::getPointerPositionView();
    m.x = (int)m.x;
    m.y = (int)m.y;
    NVector t = NVector::SFML2IToN(NMapUtility::Isometric::worldToCoords(m));
    t.x = (int)t.x;
    t.y = (int)t.y;

    NWorld::getWindow().setDebugInfo("mouse",NString::toString(m));
    NWorld::getWindow().setDebugInfo("tile",NString::toString(t));

    NWorld::getWindow().setDebugInfo("actors",std::to_string(NWorld::getActorCount()));
    NWorld::getWindow().setDebugInfo("tickables",std::to_string(NWorld::getTickableCount()));
    NWorld::getWindow().setDebugInfo("renderables",std::to_string(NWorld::getRenderableCount()));

    return true;
}

void GameState::render(sf::RenderTarget& target, sf::RenderStates states)
{
    NWorld::render(target);
}

void GameState::removeActor(std::string const& id)
{
    for(auto it=mBuildings.begin();it!=mBuildings.end();it++)
    {
        if((*it)->getId() == id)
            mBuildings.erase(it);
    }
    for(auto it=mUnits.begin();it!=mUnits.end();it++)
    {
        if((*it)->getId() == id)
            mUnits.erase(it);
    }
    NWorld::removeActor(id);
}
