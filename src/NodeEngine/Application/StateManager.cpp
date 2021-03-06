#include "StateManager.hpp"
#include "State.hpp"

namespace ah
{

StateManager::StateManager()
{
}

StateManager::PendingChange::PendingChange(Action action, std::string const& id)
: action(action)
, id(id)
{
}

void StateManager::handleEvent(sf::Event const& event)
{
	for(auto itr = mStates.rbegin(); itr != mStates.rend(); itr++)
	{
		if(!(*itr)->handleEvent(event))
			break;
	}

    applyPendingChanges();
}

void StateManager::update(sf::Time dt)
{
	for(auto itr = mStates.rbegin(); itr != mStates.rend(); itr++)
	{
		if(!(*itr)->update(dt))
			break;
	}

    applyPendingChanges();
}

void StateManager::render(sf::RenderTarget& target, sf::RenderStates states)
{
	for (auto itr = mStates.begin(); itr != mStates.end(); itr++)
    {
        (*itr)->render(target,states);
    }
}

void StateManager::pushState(std::string const& id)
{
    mPendingList.push_back(PendingChange(Push,id));
}

void StateManager::popState()
{
	mPendingList.push_back(PendingChange(Pop));
}

void StateManager::clearStates()
{
	mPendingList.push_back(PendingChange(Clear));
}

bool StateManager::empty() const
{
	return mStates.empty();
}

std::size_t StateManager::size() const
{
	return mStates.size();
}

void StateManager::applyPendingChanges()
{
    for(PendingChange change : mPendingList)
	{
		switch(change.action)
		{
			case Action::Push:
                if (!mStates.empty())
                {
                    mStates.back()->onDeactivate();
                }
				mStates.push_back(createState(change.id));
				mStates.back()->onActivate();
				break;

			case Action::Pop:
                if (!mStates.empty())
                {
                    mStates.back()->onDeactivate();
                }
				mStates.pop_back();
				if (!mStates.empty())
                {
                    mStates.back()->onActivate();
                }
				break;

			case Action::Clear:
                if (!mStates.empty())
                {
                    mStates.back()->onDeactivate();
                }
                mStates.clear();
                break;
		}
	}
	mPendingList.clear();
}

State::Ptr StateManager::createState(std::string const& id)
{
	auto found = mFactories.find(id);
	if (found == mFactories.end())
    {
        std::cerr << "State : " << id << " : cant be loaded" << std::endl;
        assert(false);
    }
	return found->second();
}

} // namespace ah
