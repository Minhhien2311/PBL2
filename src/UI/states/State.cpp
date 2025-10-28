// ============================================================
// UI/states/State.cpp
// Triển khai phần mặc định cho UI::State.
// ============================================================

#include "UI/states/State.h"

namespace UI
{

    State::State(::App &app)
        : mApp(app)
    {
    }

    State::~State() = default;

    void State::onAttach()
    {
        // Mặc định: không làm gì. State con override nếu cần.
    }

    void State::relayout(sf::Vector2u /*windowSize*/)
    {
        // Mặc định: không làm gì. State con override nếu cần.
    }

} // namespace UI
