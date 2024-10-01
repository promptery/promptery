#pragma once

#include <QBrush>
#include <QColor>

constexpr const int cTextIndent = 4;
constexpr const int cChatIndent = 100;

constexpr const int cHeaderHeight = 24;

struct Globals {
    static const QBrush cLightBlue;
    static const QBrush cLightGreen;
    static const QBrush cLightPink;
    static const QBrush cLightRed;
    static const QBrush cLightYellow;
};

constexpr auto cTypeRole  = std::numeric_limits<std::underlying_type_t<Qt::ItemDataRole>>::max();
constexpr auto cUuidRole  = cTypeRole - 1;
constexpr auto cDataRole  = cTypeRole - 2;
constexpr auto cCheckRole = cTypeRole - 3;
