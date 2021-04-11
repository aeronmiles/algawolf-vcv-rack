#pragma once

#include <rack.hpp>
#include <functional>

using namespace rack;

namespace algaw0lf
{

    struct LambdaMenuItem : MenuItem
    {
        std::function<void()> runOnAction = []() {};
        std::function<void()> runOnEnter = []() {};
        std::function<void()> runOnDragDrop = []() {};
        std::function<void()> runOnDraw = []() {};
        std::function<bool()> isActive = []() { return false; };
        std::function<bool()> isDisabled = []() { return false; };

        LambdaMenuItem(
            const char *label, std::function<void()> onAction, std::function<bool()> isActive, std::function<bool()> isDisabled = []() { return false; })
            : runOnAction(onAction), isActive(isActive), isDisabled(isDisabled)
        {
            this->text = label;
        }

        void onAction(const event::Action &e) override
        {
            this->runOnAction();
        }

        void onEnter(const event::Enter &e) override
        {
            MenuItem::onEnter(e);
            this->runOnEnter();
        }

        void draw(const DrawArgs &args) override
        {
            MenuItem::draw(args);
            this->runOnDraw();
        }

        void step() override
        {
            MenuItem::step();
            this->active = isActive();
            this->disabled = isDisabled();
        }

        void onDragDrop(const event::DragDrop &e) override
        {
            MenuItem::onDragDrop(e);
            this->runOnDragDrop();
        }
    };

} // namespace w0lf
