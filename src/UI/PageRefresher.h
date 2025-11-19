#ifndef PAGEREFRESHER_H
#define PAGEREFRESHER_H

#include <QWidget>
#include <functional>
#include <QPushButton>

class PageRefresher
{
public:
    // Clear all search/filter input fields within the given container widget
    static void clearSearchFields(QWidget* container);
    
    // @brief Refresh page with standardized behavior
    static QPushButton* createRefreshButton(QWidget* parent, 
                                           const QString& text = "🔄 Tải lại tất cả");
    // @brief Execute a standardized refresh operation
    static void executeRefresh(std::function<void()> callback);

private:
    PageRefresher() = delete; // Static utility class
};

#endif // PAGEREFRESHER_H